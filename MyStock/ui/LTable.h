#pragma once

#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#include "lwin.h"
#include "LWndView.h"

#define CELLPOS_MAX 0xFFFFFFFF

template<typename T>
class LCell
{
public:
    enum {DATA = 0, SUBCELL = 1} type;
    LRect rc; /* the location of cell on window client region */

    //union
    //{
    T _data; /* type is DATA, the stored data */
    class LSubCell
    {
    public: // FIXME!!!
        UINT rows, cols;
        float *widths, *heights; /* >1: fixed size, <=1: auto-scale size,  x% */
        UINT xfixed, yfixed;
        LCell *cells; /* cells[rows, cols]: the sub-cells */
        LSubCell() { MEMZERO_THIS(); }
    public:
        ~LSubCell()
        {
            if (cells) delete[] cells;
            if (widths) delete[] widths;
            if (heights) delete[] heights;
        }
        void Layout(LRect& rc, BOOL bOverlap = TRUE)
        {
            UINT i, j;
            LONG w, h, t;
            LArray<UINT> xpos(cols + 1), ypos(rows + 1);

            w = rc.Width() - xfixed;
            w = max(w, 0);
            t = rc.left;
            for (i = 0; i < cols; i++)
            {
                xpos[i] = min(t, rc.right);
                t += (INT)widths[i] > 1 ?(INT)widths[i] :(INT)((float)w * widths[i]);
            }
            xpos[i] = rc.right;

            h = rc.Height() - yfixed;
            h = max(h, 0);
            t = rc.top;
            for (i = 0; i < rows; i++)
            {
                ypos[i] = min(t, rc.bottom);
                t += (INT)heights[i] > 1 ?(INT)heights[i] :(INT)((float)h * heights[i]);
            }
            ypos[i] = rc.bottom;

            for (i = 0; i < rows; i ++)
            {
                for (j = 0; j < cols; j++)
                {
                    LRect& _rc = cell(i, j).rc;
                    _rc.left = xpos[j];
                    _rc.top = ypos[i];
                    _rc.right = xpos[j + 1] +(bOverlap ? 1 : 0);
                    _rc.bottom = ypos[i + 1] +(bOverlap ? 1 : 0);
                    _rc.right = min(_rc.right, rc.right);
                    _rc.bottom = min(_rc.bottom, rc.bottom);
                }
            }
        }

        LCell& cell(UINT row, UINT col) /* reference the subcell */
        {
            //_ASSERT(SUBCELL == type);
            _ASSERT(row < rows);
            _ASSERT(col < cols);
            return cells[row * cols + col];
        }

    } subcell; /* type is SUBCELL */
    //}
    T& operator = (T data) {
        _ASSERT(DATA == type);
        _data = data;
        return _data;
    }
    T& operator = (LPCTSTR lpText) /* used for string ONLY */
    {
        _ASSERT(DATA == type);
        _data = lpText;
        return _data;
    }
    operator RECT&() { return rc; }
    operator T&() { return _data; }
    operator T*() { return &_data; }
    //operator LPCTSTR() { return _data; }
    //T& operator = (T& d) { _data = d; return _data; }

    LCell() : type(DATA) {/*MEMZERO_THIS();*/}
    BOOL Split(UINT rows, UINT cols, float widths[/*cols*/] = NULL, float heights[/*rows*/] = NULL)
    {
        UINT i;
        /* delete previous subcells if any */
        if (NULL != subcell.cells && subcell.rows * subcell.cols != rows * cols)
            delete[] subcell.cells;
        if (NULL != subcell.widths && subcell.cols != cols)
            delete[] subcell.widths;
        if (NULL != subcell.heights && subcell.rows != rows)
            delete[] subcell.heights;
        type = SUBCELL;
        subcell.cells = new LCell[rows * cols];
        subcell.widths = new float[cols];
        subcell.heights = new float[rows];
        subcell.rows = rows;
        subcell.cols = cols;
        //N = _rows * _cols;

        for (i = 0; i < cols; i++)
        {
            subcell.widths[i] = widths ? widths[i] :(float)1.0 /(float)cols;
            if ((UINT)subcell.widths[i] > 1)
            {
                subcell.xfixed += (UINT)subcell.widths[i];
            }
        }
        for (i = 0; i < rows; i++)
        {
            subcell.heights[i] = heights ? heights[i] :(float)1.0 /(float)rows;
            if ((UINT)subcell.heights[i] > 1)
            {
                subcell.yfixed += (UINT)subcell.heights[i];
            }
        }
        return TRUE;
    }

    LCell& cell(UINT row, UINT col) /* reference the subcell */
    {
        return subcell.cell(row, col);
    }

    LCell& cell(DWORD pos, ...)
    {
        LCell *c0 = this; //&_cell;//&cell;
        int row, col;
        va_list vp;

        _ASSERT(CELLPOS_MAX != pos);
        va_start(vp, pos);
        do {
            //if (LWndCell::WNDS == c0->type) { break; }
            //_ASSERT(LWndCell::CELLS == c0->type);
            row = HIWORD(pos);
            col = LOWORD(pos);
            c0 = &c0->cell(row, col);
            pos = va_arg(vp, DWORD);
        } while(CELLPOS_MAX != pos);
        return *c0;
    }

    /* layout cell and it's subcells */
    void LayoutCells(LCell& cell, LRect& rc, BOOL bOverlap = TRUE)
    {
        if (LCell::SUBCELL == cell.type)
        {
            cell.subcell.Layout(rc, bOverlap);
            int N = cell.subcell.rows * cell.subcell.cols;
            for (INT i = 0; i < N; i++)
            {
                LCell& ci = cell.subcell.cells[i];
                LayoutCells(ci, ci.rc, bOverlap);
            }
        }
    }

    // nBdrWidth, nLineWidth, nCellPadding
    // VOID Layout(LRect& rcBound, INT nBdrWidth = 1, BOOL bOverlap = TRUE)
    VOID Layout(LRect& rcBound, INT nLineWidth = 1, INT nCellPadding = 0, INT nBdrWidth = 0, BOOL bOverlap = TRUE)
    {
        //INT x = (nLineWidth + nCellPadding * 2) / 2 + nBdrWidth;
        //x -= (nCellPadding + nBdrWidth);
        INT x = nLineWidth / 2/* + nCellPadding*/;
        rc = rcBound;
        rc.InflateRect(-x, -x);
        LayoutCells(*this, rc, bOverlap);
    }

    LCell* HitTest(LPoint& pt, INT* row = NULL, INT* col = NULL)
    {
        return HitTestCells(*this, pt, row, col);
    }
    //void GetCellTextRect(LRect &rcText, INT nBdrWidth = 1)
    //{
    //    int t = nBdrWidth / 2 +(nBdrWidth ? 1 : 0);
    //    rcText = rc;
    //    rcText.InflateRect(-t, -t);
    //}
private:
    LCell* HitTestCells(LCell& c, LPoint& pt, INT* row, INT* col)
    {
        LCell* hit = NULL;
        UINT i, j;
        if (LCell::SUBCELL == c.type)
        {
            for (i = 0; i < c.subcell.rows; i ++)
            {
                for (j = 0; j < c.subcell.cols; j++)
                {
                    if (NULL != (hit = HitTestCells(c.cell(i, j), pt, row, col))) {
                        if (NULL != row) *row = i;
                        if (NULL != col) *col = j;
                        return hit;
                    }
                }
            }
            return NULL;
        }
        if (c.rc.PtInRect(pt))
            return &c;
        return NULL;
    }
};

template<typename T>
class LTable : public LWndView<LTable<T>>
{
    // LArray<LCell> cells;
protected:
    LCell<T> _cell;
    INT _nBdrWidth;
    INT _nLineWidth; /* must be odd number */
    INT _nCellPadding;
    INT _nActiveRow;
public:

    LTable()
    {
        _nBdrWidth = 0;
        _nLineWidth = 1;
        _nCellPadding = 0;
        _nActiveRow = -1;

        //_cell.Split(10, 3);
        //_cell.cell(1, 1).Split(2, 2);
        //_cell.cell(1, 1).cell(1, 1) = _T("123");
        //_cell.cell(2, 1) = _T("hello world!");
        //_cell.cell(4, 1) = _T("hello world! 4,1");
        //_cell.cell(0x10001, 0x10000, CELLPOS_MAX) = _T("(1,1).(3,0)");
        //_uBgColor = RGB(214, 219, 233);
        //LSTR str = _T("hello world!");
        //_cell.cell(2, 1) = str;
        //_cell.cell(2, 1).operator= (_T("hello world!"));
    }

    ~LTable()
    {
    }
    void GetCellTextRect(LCell<T>& cell, LRect &rcText)
    {
        int w = _nLineWidth + _nCellPadding * 1;
        int t = w / 2 +(w ? 1 : 0);
        rcText = cell.rc;
        rcText.InflateRect(-t, -t);
    }
    virtual void DrawCell(LCell<T>& cell, UINT row, UINT col, LDC& dc)
    {
        /* must be overided in derived */
        //LRect rc;
        //GetCellTextRect(cell, rc);
        //dc.DrawText(cell, rc);
    }
    void DrawCells(LCell<T>& cell, UINT row, UINT col, LDC& dc)
    {
        //LSTR str;
        //str.Format(_T("%s"),(LPCTSTR)cell);
        if (_nLineWidth)
            dc.Rectangle(cell.rc);
        if (LCell<T>::DATA == cell.type)
        {
            DrawCell(cell, row, col, dc);
            //LRect rc;
            //cell.GetCellRect(rc, _nBdrWidth);
            //dc.DrawText(cell, rc);
        }
        if (0)
        {
            LRect rc;
            LPen pen(dc);
            LBrush br(dc);
            br.CreateStockObject(NULL_BRUSH);
            GetCellTextRect(cell, rc);
            pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
            dc.Rectangle(rc);
        }

        if (LCell<T>::SUBCELL == cell.type)
        {
            //int N = cell.subcell.rows * cell.subcell.cols;
            for (UINT i = 0; i < cell.subcell.rows; i++)
            {
                for (UINT j = 0; j < cell.subcell.cols; j++)
                {
                    // LCell<T>& ci = cell.subcell.cells[i];
                    DrawCells(cell.cell(i, j), i, j, dc);
                }
            }
        }
    }

    BOOL Create(LWnd *parent, UINT32 rows, UINT32 cols, LRect& rc, LPCTSTR lpszName = NULL, UINT32 lineWidth = 1)
    {
        _nBdrWidth = 0;
        _nLineWidth = lineWidth;
        _nCellPadding = 0;
        _cell.Split(rows, cols);
        return NULL != CreateChild(parent, lpszName, rc, 0, 0, LVS_NO_STRETCH_CANVAS | LVS_HALIGN_LEFT | LVS_VALIGN_TOP);
    }

    virtual VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        LRect rc;
        GetClientRect(rc);
        cx = rc.Width();// 800;
        cy = 200;
    }

    BOOL DrawLayer(Layer *layer, UINT& uFlags)
    {
        LRect rc;//, rcInit(0, 0, 100, 20);
        //INT w = _nLineWidth + _nCellPadding * 2;
        // GetClientRect(rc);
        INT w;// = _nBdrWidth - _nLineWidth;
        LDC& dc = layer->dc;
        //w = max(w, 0);
        //GetLayerRect(rc);
        if (REDRAW_CALCULATE & uFlags)
        {
            w = _nBdrWidth - _nLineWidth;
            w = max(w, 0);
            GetLayerRect(rc);
            rc.InflateRect(-w, -w);
            _cell.Layout(rc, _nLineWidth, _nCellPadding, _nBdrWidth, _nLineWidth ? TRUE : FALSE);
            //_cell.rc = rcClient;
            //_cell.rc.InflateRect(-5, -5);
            //_cell.LayoutCells(_cell, _cell.rc, TRUE);
        }

        w = _nBdrWidth;
        if (w > 0)
        {
            if (0 == (w & 1))
            {
                w -= 1;
            }
            LPen penBdr(dc);
            penBdr.CreatePen(PS_SOLID, w, RGB(0, 0, 0));
            w /= 2;
            GetLayerRect(rc);
            rc.InflateRect(-w, -w);
            dc.Rectangle(rc);
            // dc.FrameRect(rc, penBdr);
        }
        // dc.SelectObject(GetStockObject(NULL_BRUSH));
        LPen pen(dc);
        LFont font(dc);
        font.CreateFont(_T("Tahoma"), 14);
        pen.CreatePen(PS_SOLID, _nLineWidth, RGB(0, 0, 0));
        DrawCells(_cell, 0, 0, dc);
        //uFlags = 0;
        return TRUE;
    }

    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
       (VOID)__super::OnLButtonDown(dwFlags, pt);
        LPoint pt0 = pt;
        _curLayer->ClientToLayer(pt0);

        INT t = -1;
        // _nActiveRow = -1;
        if (NULL != _cell.HitTest(pt0, &t, NULL))
        {
            if (_nActiveRow != t) {
                _nActiveRow = t;
                SetRedrawFlags(REDRAW_PAINT);
                InvalidateRect(NULL, FALSE);
            }
            return 0;
        }
        _nActiveRow = -1;
        //_nMouseStatus |= LVIEW_LBUTTONDOWN;
        //return OnButtonDown(dwFlags, pt);
        return 0;
    }
    VOID OnPaint(LDC& dc)
    {
        int t = dc.Save();
        LRect rc;
        GetClientRect(rc);
        rc.top += 20;
        dc.IntersectClipRect(rc);
        __super::OnPaint(dc);
        dc.Restore(t);
        
        rc.top -= 20;
        dc.BitBlt(0, 0, rc.Width(), 20, _curLayer->dc, 0, 0);
    }
    //LCell<T>& cell(DWORD pos, ...)
    //{
    //    LCell *c0 = &_cell;//&cell;
    //    int row, col;
    //    va_list vp;

    //    _ASSERT(CELLPOS_MAX != pos);
    //    va_start(vp, pos);
    //    do {
    //        //if (LWndCell::WNDS == c0->type) { break; }
    //        //_ASSERT(LWndCell::CELLS == c0->type);
    //        row = HIWORD(pos);
    //        col = LOWORD(pos);
    //        c0 = &c0->cell(row, col);
    //        pos = va_arg(vp, DWORD);
    //    } while(CELLPOS_MAX != pos);
    //    return *c0;
    //}

};


//class LTabledText : public LWndView<LTabledText>
//{
//    LTable<LSTR> _tData;
//public:
//
//    LTabledText()
//    {
//    }
//
//    ~LTabledText()
//    {
//    }
//};

///* a 2-dimensions list view */
//class LListView : public LTable<UINT>
//{
//};

class LDVTable : public LTable<UINT>
{
    DVS *_ds;
public:
    void DrawCell(LCell<UINT>& cell, UINT row, UINT col, LDC& dc)
    {
        LRect rc;
        LSTR str;
        if (NULL == _ds)
            return;
        GetCellTextRect(cell, rc);
        if (0 == row) {
            _ds->GetName(col, str);
            LBrush br;
            br.CreateSolidBrush(RGB(0, 64, 0));
            // dc.Rectangle(cell);
            dc.FillRect(rc, br);
        } else {
            _ds->GetText(row - 1, col, str);
            if (_nActiveRow == row)
            {
                LBrush br;
                br.CreateSolidBrush(RGB(0, 0, 128));
                // dc.Rectangle(cell);
                dc.FillRect(rc, br);
            }
            dc.SetTextColor(_nActiveRow == row ? RGB(255, 255, 255) : 0);
        }
        dc.SetBkMode(TRANSPARENT);
        dc.DrawText(str, rc);
    }

    BOOL Load(DVS* ds)
    {
        INT rows, cols;
        rows = ds->Rows() + 1; /* +1: the table header */
        cols = ds->Cols();
        _cell.Split(rows, cols);
        //for (int i = 0; i < rows; i++)
        //{
        //    for (int j = 0; j < cols; j++)
        //    {
        //        _cell.cell(i, j) = ds->GenCellId(i, j);
        //    }
        //}
        _ds = ds;
        return TRUE;
    }

    BOOL Create(LWnd *parent, LRect& rc, DVS* ds = NULL, LPCTSTR lpszName = NULL, UINT32 lineWidth = 1)
    {
        _nBdrWidth = 0;
        _nLineWidth = lineWidth;
        _nCellPadding = 0;
        if (NULL != ds)
            Load(ds);
        return NULL != CreateChild(parent, lpszName, rc, 0, 0, LVS_NO_STRETCH_CANVAS | LVS_HALIGN_LEFT | LVS_VALIGN_TOP);
    }

    VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        LRect rc;
        GetClientRect(rc);
        cx = max(rc.Width(), 500);// 800;
        cy = _ds ? 20 * _ds->Rows() : rc.Height();
    }
};