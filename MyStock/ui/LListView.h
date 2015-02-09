#pragma once

#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#include "lwin.h"
#include "LWndView.h"

#define LLVS_NOBORDER 1
typedef struct tagLISTVIEW_COLINFO
{
    //enum {LEFT, CENTER, RIGHT} align;
    UINT align;
    INT width, maxWidth;
    COLORREF bgColor;
    LSTR name;
    UINT flags;
    tagLISTVIEW_COLINFO()
    {
        align = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
        width = maxWidth = 0;
        bgColor = RGB(0, 0, 0);
        flags = 0;
        //name = _T("");
    }
} LISTVIEW_COLINFO;

#define LLVS_ROWHITTEST    (LVS_MAX_STYLE << 0)
#define LLVS_COLHITTEST    (LVS_MAX_STYLE << 1)
#define LLVS_CELLHITTEST   (LVS_MAX_STYLE << 2)
#define LLVS_HEADERHITTEST (LVS_MAX_STYLE << 3)
#define LLVS_HEADER        (LVS_MAX_STYLE << 4)
#define LLVS_DOTLINE       (LVS_MAX_STYLE << 5)
//#define LLVS_ROWNUMBER     (LVS_MAX_STYLE << 6)
#define LLVS_NOVLINE       (LVS_MAX_STYLE << 7)
#define LLVS_NOHLINE       (LVS_MAX_STYLE << 8)
#define LLVS_TITLE         (LVS_MAX_STYLE << 9)
#define LLVS_ROW_HITTEST   (LVS_MAX_STYLE << 10)
#define LLVS_COL_HITTEST   (LVS_MAX_STYLE << 11)
#define LLVS_COL_SORT      (LVS_MAX_STYLE << 12)

#define LLV_HIT_TITLE      1
#define LLV_HIT_HEADER     2
#define LLV_HIT_LINENUMBER 3
#define LLV_HIT_DATA       4

class LListView : public LWndView<LListView>
{
protected:
    INT _rows, _cols, _fixedRows, _fixedCols;
    //INT *_rowids;
    float _width; /* 0:autosize, 1:100%, <1:x%, n:fixed} */
    COLORREF _rowcolors[2];
    COLORREF _lineColor;
    INT *_widths/*[cols]*/, *_heights/*[rows]*/;
    LISTVIEW_COLINFO *_colinfos;
    struct {
        INT row, col;
        COLORREF bgColor, bdrColor, textColor;
    } _selected;
    LSTR _title;
    LRect rcTitle, rcFixedRows, rcFixedCols, rcFixedRowCols, rcUnfixed; /* in client coordinate */

    VOID GetCellRect(INT row, INT col, LRect& rc)
    {
        LWIN_ASSERT(row < _rows);
        LWIN_ASSERT(col < _cols);
        rc.left = _widths[col];
        rc.right = _widths[col + 1] + 1;
        rc.top = _heights[row];
        rc.bottom = _heights[row + 1] + 1;
    }
    VOID GetRowRect(INT row, LRect& rc)
    {
        LWIN_ASSERT(row < _rows);
        rc.left = _widths[0];
        rc.right = _widths[_cols] + 1;
        rc.top = _heights[row];
        rc.bottom = _heights[row + 1] + 1;
    }
    VOID GetLineNumberRect(INT row, LRect& rc)
    {
        //if (!(LLVS_ROWNUMBER & _uCtrlStyle)) {
        //    rc.Zero();
        //    return;
        //}
        GetRowRect(row, rc);
        rc.right = rc.left + 1;
        rc.left = 0;
    }
    VOID GetCellTextRect(INT row, INT col, LRect& rc)
    {
        GetCellRect(row, col, rc);
        rc.InflateRect(-2, -2);
            //(LLVS_NOHLINE & _uCtrlStyle) ? 0 : -1, 
            //(LLVS_NOVLINE & _uCtrlStyle) ? 0 : -1);
    }

public:
    LListView()
    {
        //MEMZERO_THIS();
        _width = 1.0;
        _rows = _cols = 1;
        _widths = _heights = NULL;
        _colinfos = NULL;
        _fixedRows = _fixedCols = 0;
        _selected.row = -1;
        _selected.col = -1;
        _selected.bgColor = RGB(0, 0, 0);
        _selected.bdrColor = RGB(0, 0, 0);
        _selected.textColor = RGB(255, 255, 255);
        _lineColor = RGB(0, 0, 0);
        _rowcolors[0] = RGB(255, 255, 255);
        _rowcolors[1] = RGB(255, 255, 255);
    }

    ~LListView()
    {
        //if (_rowids) delete[] _rowids;
        if (_widths) delete[] _widths;
        if (_heights) delete[] _heights;
        if (_colinfos) delete[] _colinfos;
    }

    BOOL Create(LWnd *parent, INT rows, INT cols, LRect& rc, LPCTSTR lpName, UINT style, float width = 1.0)
    {
        if (NULL == CreateChild(parent, lpName, rc, 0, 0, LVS_NO_STRETCH_CANVAS | style))
        {
            return FALSE;
        }
        _fixedRows = (LLVS_HEADER & style ? 1 : 0);
        _rows = max(rows, 1) + _fixedRows;
        _cols = max(cols, 1);
        _width = width;
        //_rowids = new INT[_rows];
        _widths = new INT[_cols + 1];
        _heights = new INT[_rows + 1];
        _colinfos = new LISTVIEW_COLINFO[_cols];
        _title = lpName ? lpName : _T("NO TITLE");

                    //// =======
                    //_colinfos[0].width = 30;
                    //_colinfos[0].flags = LLVS_NOBORDER;
                    //_colinfos[0].align = LISTVIEW_COLINFO::RIGHT;
                    ////_uCtrlStyle |= LLVS_ROWNUMBER;
                    //_uCtrlStyle |= LLVS_NOVLINE;
                    //_uCtrlStyle |= LLVS_NOHLINE;
                    ////_uCtrlStyle |= LLVS_HEADER;
                    ////_selected.bdrColor = RGB(255, 0, 0);
                    ////_selected.bgColor = RGB(255, 255, 255);
                    //_lineColor = RGB(0, 0, 0);
                    //_selected.bdrColor = _lineColor;
                    //_selected.bgColor = RGB(0, 0, 0);
                    //_selected.textColor = RGB(255, 0, 0);
                    //_rowcolors[0] = RGB(240, 240, 240);
                    ////_fixedRows = 0;
                    //_fixedRows += 1;
                    //_fixedCols = 2;

                    //for (int i = 0; i < _cols; i++) {
                    //    _colinfos[i].name.Format(_T("header#%d"), i);
                    //}
                    //for (int i = 1; i < _cols; i++) 
                    //    _colinfos[i].width = 0;
                    //_colinfos[1].width = 100;
        return TRUE;
    }

    void SetSize(INT rows, INT cols, LISTVIEW_COLINFO *colInfos = NULL, BOOL redrawNow = TRUE)
    {
        if (_widths) delete[] _widths;
        if (_heights) delete[] _heights;
        if (_colinfos) delete[] _colinfos;

        //_fixedRows = (LLVS_HEADER & style ? 1 : 0);
        SetFixedRows(0, FALSE);
        _rows = max(rows, 1) + _fixedRows;
        _cols = max(cols, 1);

        _widths = new INT[_cols + 1];
        _heights = new INT[_rows + 1];
        _colinfos = new LISTVIEW_COLINFO[_cols];

        for (int i = 0; i < cols; i++)
            _colinfos[i] = colInfos[i];
        if (redrawNow) InvalidateRect(NULL, FALSE, TRUE);
    }

    void SetFixedRows(int rows, BOOL redrawNow = TRUE)
    {
        _fixedRows = (LLVS_HEADER & _uCtrlStyle ? 1 : 0);
        _fixedRows += rows;
        if (redrawNow) InvalidateRect(NULL, FALSE, TRUE);
    }

    void SetFixedCols(int cols, BOOL redrawNow = TRUE)
    {
        _fixedCols = cols;
        if (redrawNow) InvalidateRect(NULL, FALSE, TRUE);
    }

    void GetLayoutRect(LRect& rc)
    {
        GetClientRect(rc);
        //rc.InflateRect(-10, -10);
    }

    virtual int CalcCellWidth(int col, LDC& dc)
    {
        LRect rc;
        GetLayoutRect(rc);
        return rc.Width() / _cols;
    }
    virtual int CalcCellHeight(int row, LDC& dc)
    {
        TEXTMETRIC tm;
        dc.GetTextMetrics(&tm);
                    tm.tmHeight += 4;
                    /*if (row & 1)
                        tm.tmHeight += 10;*/
                    if (row == 0)
                        tm.tmHeight = 30;
                    if (row == 20)
                        tm.tmHeight = 60;
        return tm.tmHeight;
    }
    virtual int CalcTitleHeight(LDC& dc)
    {
        if (!(LLVS_TITLE & _uCtrlStyle))
            return 0;
        return 50;
    }
    virtual BOOL Layout(LDC& dc)
    {
        int i, fixedWidth, varWidth, varCols, t, lastVarCol;
        _widths[0] = 0;
        //if (LLVS_ROWNUMBER & _uCtrlStyle) {
        //    LSTR str; SIZE sz;
        //    // str = _rows;
        //    str.Format(_T("0%d"), _rows);
        //    dc.GetTextExtentPoint32(str, &sz);
        //    _widths[0] = sz.cx;
        //}
        if (0.0 == _width) {
            for (i = 1; i <= _cols; i++) {
                _widths[i] = _colinfos[i - 1].width;
                if (0 == _widths[i]) {
                    _widths[i] = CalcCellWidth(i - 1, dc);
                    if (_colinfos[i - 1].maxWidth > 0)
                        _widths[i] = min(_widths[i], _colinfos[i - 1].maxWidth);
                }
            }
        } else {
            LRect rc;
            GetLayoutRect(rc);
            int cx = (int)(_width <= 1.0 ? (float)(rc.Width() - 1) * _width : _width);
            ////cx -= (LLVS_ROWNUMBER & _uCtrlStyle ? _widths[0] : 0);
            fixedWidth = varWidth = 0;
            for (i = 1; i <= _cols; i++) {
                if (0 != _colinfos[i - 1].width) {
                    _widths[i] = _colinfos[i - 1].width;
                    fixedWidth += _colinfos[i - 1].width;
                }
            }
            cx = max(cx, fixedWidth); /* expand the width of table if any*/
            cx -= fixedWidth;
            varCols = 0;
            lastVarCol = _cols;
            /* calculate _widths[i] for variable-width columns; */
            for (i = 1; i <= _cols; i++) {
                if (0 == _colinfos[i - 1].width) {
                    _widths[i] = CalcCellWidth(i - 1, dc);
                    _widths[i] = (INT)(((float)_widths[i] /(float)cx) *(float)cx);
                    if (_colinfos[i - 1].maxWidth > 0)
                        _widths[i] = min(_widths[i], _colinfos[i - 1].maxWidth);
                    varWidth += _widths[i];
                    varCols += 1;
                    lastVarCol = i;
                }
            }
            if (varCols > 0) {
                cx -= varWidth;
                t = cx / varCols;
                for (i = 1; i <= _cols; i++) {
                    if (0 == _colinfos[i - 1].width) {
                        _widths[i] += t;
                    }
                }
                _widths[lastVarCol] += (cx % varCols);
            }
        }
        /* translate the width of column to position on layer */
        for (i = 1; i <= _cols; i++)
            _widths[i] += _widths[i - 1];

        _heights[0] = CalcTitleHeight(dc);
        for (i = 1; i <= _rows; i++) {
            _heights[i] = CalcCellHeight(i - 1, dc);
        }
        //TEXTMETRIC tm;
        //dc.GetTextMetrics(&tm);
        //tm.tmHeight += 4;
        for (i = 1; i <= _rows; i++)
            _heights[i] += _heights[i - 1];
            // _heights[i] = i * tm.tmHeight;

        return TRUE;
    }
    VOID CalcScrollRegion(Layer *layer, LRect& rcScroll, SIZE& dltSize)
    {
        // CalcAxisArea(rcScroll);
        //__super::CalcScrollRegion(layer, rcScroll);

        //LRect rcFix;
        //GetFixedRect(rcFix);
        //rcScroll.top = rcFix.bottom;
        GetUnfixedRect(rcScroll);

        dltSize.cx = -rcFixedCols.Width();
        dltSize.cy = -(rcTitle.Height() + rcFixedRows.Height());
    }

    virtual VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
                layer->Lock();
                LFont font(layer->dc);
                font.CreateFont(_T("Times New Roman"), 15);
                //layer->dc.SelectObject(font);

        Layout(layer->dc);
        cx = _widths[_cols] + 1;
        cy = _heights[_rows] + 1;
                layer->Unlock();

    }

    virtual VOID CalcViewRegion(Layer *layer, LRect& rc)
    {
        __super::CalcViewRegion(layer, rc);

        GetTitleRect(rcTitle); /* rcTitle in canvas coordination */
        rcTitle.MoveTo(_curLayer->viewPort.lt);

        GetUnfixedRect(rcUnfixed);
        rcFixedRows.top = rcTitle.bottom;
        rcFixedRows.right = rcTitle.right;
        rcFixedRows.left = rcUnfixed.left;
        rcFixedRows.bottom = rcUnfixed.top;

        rcFixedCols.left = rcTitle.left;
        rcFixedCols.bottom = rcUnfixed.bottom;
        rcFixedCols.right = rcUnfixed.left;
        rcFixedCols.top = rcUnfixed.top;

        rcFixedRowCols.left = rcTitle.left;
        rcFixedRowCols.top = rcTitle.bottom;
        rcFixedRowCols.bottom = rcUnfixed.top;
        rcFixedRowCols.right = rcUnfixed.left;
    }

    VOID GetTitleRect(LRect& rc)
    {
        if (!(LLVS_TITLE & _uCtrlStyle)) {
            rc.Zero();
            return;
        }
        GetLayerRect(rc);
        rc.left = _widths[0];
        rc.bottom = _heights[0];
        int viewCx = _curLayer->viewPort.Width();
        rc.right = rc.left + min(_curLayer->cx, viewCx);
        //rc.Intersection(_curLayer->viewPort);
        //rc = _curLayer->viewPort;
        //rc.bottom = _heights[1];
    }
    virtual VOID DrawHeader(LDC& dc)
    {
        INT i;
        LRect rc;
        LBrush br(dc);
        LPen pen(dc);
        br.CreateSolidBrush(RGB(214, 219, 233));
        pen.CreatePen(PS_SOLID, 1, _lineColor);
        //dc.SetTextColor(RGB(0, 0, 0));

        LFont ft(dc);
        ft.CreateFont(_T("Tahoma"), 13, FW_BOLD, RGB(0, 0, 0));

        //if (LLVS_ROWNUMBER & _uCtrlStyle) {
        //    //GetRowRect(0, rc);
        //    //rc.right = rc.left + 1;
        //    //rc.left = 0;
        //    GetLineNumberRect(0, rc);
        //    //dc.Rectangle(rc);
        //    //LSTR str;
        //    //str = row;// -(LLVS_HEADER & _uCtrlStyle ? 1 : 0);
        //    //LFont fNbr(dc);
        //    //fNbr.CreateFont(_T("Times New Roman"), 10, 0, RGB(128, 128, 128));
        //    //rc.right -= 4;
        //    dc.DrawText(_T("# "), rc/*, DT_SINGLELINE | DT_RIGHT | DT_VCENTER*/);
        //}

        for (i = 0; i < _cols; i++) {
            GetCellRect(0, i, rc);
            dc.Rectangle(rc);
            GetCellTextRect(0, i, rc);
            dc.DrawText(_colinfos[i].name, rc);
        }
    }
    virtual VOID DrawTitle(LDC& dc, LRect& rc)
    {
        //LSTR str;
        //str.Format(_T("NO TITLE. table info: rows %d, cols %d"), _rows, _cols);
        LFont ft(dc);
        ft.CreateFont(_T("Times New Roman"), rc.Height() - 3, FW_BOLD);
        dc.DrawText(_title, rc);
            //LFont ft0(dc);
            //ft0.CreateFont(_T("Times New Roman"), 40, FW_BOLD, RGB(0, 0, 255));
            //dc.DrawText(_T("_title"), rc);
        //ft.CreateFont(_T("Tahoma"), 11, FW_NORMAL, RGB(0, 0, 255));
        //dc.DrawText(_T("18/01/2015"), rc, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE );
    }
    virtual VOID DrawItemText(LDC &dc, int row, int col, LRect &rc, LRect &rcText)
    {
            if (col == 2)
            {
                float f = (float)rand() /(float)RAND_MAX;
                LRect rc0 = rcText;
                rc0.right = rc0.left +(int)((float)rcText.Width() * f);
                LBrush br(dc);
                br.CreateSolidBrush(RGB(255, 255, 0));
                dc.Rectangle(rc0);
                LSTR str;
                str.Format(_T("%.2f%%"),(float)(rc0.right - rc0.left) / 100.0);
                dc.DrawText(str, rc0);
            }
            else if (col == 0)
            {
                LSTR str;
                str.Format(_T("%d"), row);
                dc.DrawText(str, rcText);
            }
            else
            {
                LSTR str;
                str.Format(_T("[%d, %d] = %d"), row, col, rand());
                dc.DrawText(str, rcText);
            }
    }
    virtual VOID DrawRow(INT row, LDC& dc, BOOL bChkSelected = TRUE)
    {
        INT col;
        LRect rc, rcText;

        if (0 == row && (LLVS_HEADER & _uCtrlStyle)) {
            DrawHeader(dc);
            return;
        }

        LBrush br(dc);
        LPen pen(dc);
        if (bChkSelected) {
            br.CreateSolidBrush(_selected.bgColor);
            pen.CreatePen(PS_SOLID, 1, _selected.bdrColor);
            //dc.SetTextColor(_selected.textColor);
        } else {
            br.CreateSolidBrush(_rowcolors[row & 1]);
            pen.CreatePen(PS_SOLID, 1, _lineColor);
            //dc.SetTextColor(RGB(0, 0, 0));
        }
        LFont ft(dc);
        ft.CreateFont(_T("Tahoma"), 13, FW_NORMAL, bChkSelected ? _selected.textColor : RGB(0, 0, 0));

        GetRowRect(row, rc);
        //{
        //    LBrush br0(dc);
        //    br0.CreateSolidBrush(bChkSelected ? RGB(0, 255, 0) : _lineColor);
        //    dc.FrameRect(rc, br0);
        //}
        //rc.left += 50;
        //rc.right -= 200;
        //dc.Rectangle(rc);
#if 1
        //if (LLVS_ROWNUMBER & _uCtrlStyle) {
        //    //rc.right = rc.left - 3;
        //    //rc.left = 0;
        //    LSTR str;
        //    //str = row;// -(LLVS_HEADER & _uCtrlStyle ? 1 : 0);
        //    //LFont fNbr(dc);
        //    //fNbr.CreateFont(_T("Times New Roman"), 10, 0, RGB(128, 128, 128));
        //    GetLineNumberRect(row, rc);
        //    str.Format(_T("%d "), row);
        //    dc.DrawText(str, rc/*, DT_SINGLELINE | DT_RIGHT | DT_VCENTER*/);
        //}
        //LPen pen(dc);
        //pen.CreatePen(PS_SOLID, 1, _lineColor);
        for (col = 0; col < _cols; col++) {
            GetCellRect(row, col, rc);
            GetCellTextRect(row, col, rcText);
            if (!(_colinfos[col].flags & LLVS_NOBORDER))
                dc.Rectangle(rc);
            DrawItemText(dc, 
                row - (LLVS_HEADER & _uCtrlStyle ? 1 : 0), 
                col, rc, rcText);
            //// dc.Rectangle(rc);
            //LSTR str;
            ////str = rand(); 通过GetCellDrawText来获取单元格输出内容，可以为字符串，可以为一个对象，以支持复杂单元格内容输出
            //str.Format(_T("[%d, %d] = %d"), row, col, rand());
            //GetCellTextRect(row, col, rcText);
            //dc.DrawText(str, rc);

            if (0)
            {
                LPen pen(dc);
                pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
                GetCellTextRect(row, col, rc);
                dc.Rectangle(rc);
            }
        }
        //GetRowRect(row, rc);
        //{
        //    LBrush br0(dc);
        //    br0.CreateSolidBrush(bChkSelected ? RGB(255, 0, 0) : _lineColor);
        //    dc.FrameRect(rc, br0);
        //}

#endif
    }
    virtual BOOL DrawLayer(Layer *layer, UINT& uFlags)
    {
        LDC& dc = layer->dc;
        LRect rc;//, rcInit(0, 0, 100, 20);
        //INT w = _nLineWidth + _nCellPadding * 2;
        // GetClientRect(rc);
        //INT w;// = _nBdrWidth - _nLineWidth;
        //w = max(w, 0);
        //GetLayerRect(rc);
        layer->Lock();
        dc.SetBkMode(TRANSPARENT);
        //if (REDRAW_CALCULATE & uFlags)
        //{
        //    //w = _nBdrWidth - _nLineWidth;
        //    //w = max(w, 0);
        //    //GetLayerRect(rc);
        //    //rc.InflateRect(-w, -w);
        //    //_cell.Layout(rc, _nLineWidth, _nCellPadding, _nBdrWidth, _nLineWidth ? TRUE : FALSE);
        //    ////_cell.rc = rcClient;
        //    ////_cell.rc.InflateRect(-5, -5);
        //    ////_cell.LayoutCells(_cell, _cell.rc, TRUE);
        //}

                    //LFont font(dc);
                    //font.CreateFont(_T("Tahoma"), 14);

        INT row/*, col*/;
        // GetLayerRect(rc);
        /* draw horizontal line */
        //for (row = 0; /*!(LLVS_NOHLINE & _uCtrlStyle) &&*/ row <= _rows; row++)
        //{
        //    dc.MoveTo(_widths[0], _heights[row]);
        //    dc.LineTo(rc.right, _heights[row]);
        //}
        ///* draw vertical line */
        //for (col = 0; /*!(LLVS_NOVLINE & _uCtrlStyle) &&*/ col <= _cols; col++)
        //{
        //    dc.MoveTo(_widths[col], _heights[0]);
        //    dc.LineTo(_widths[col], rc.bottom);
        //}

        LPen pen(dc);
        pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
        LBrush br(dc);
        br.CreateStockObject(NULL_BRUSH);

        // GetLayerRect(rc);
        if (LLVS_TITLE & _uCtrlStyle) {
            GetTitleRect(rc);
            DrawTitle(dc, rc);
        }
        layer->Unlock();
        //row = 0;
        //if (LLVS_HEADER & _uCtrlStyle) {
        //    DrawHeader(dc);
        //    row = 1;
        //}
        for (row = 0; row < _rows; row++) {
            //if (row > 200)
            //    continue;
            layer->Lock();
            DrawRow(row, dc, FALSE);
            layer->Unlock();
        }
        if (-1 != _selected.row) {
            layer->Lock();
            DrawRow(_selected.row, dc, TRUE);
            layer->Unlock();
        }
        //w = _nBdrWidth;
        //if (w > 0)
        //{
        //    if (0 == (w & 1))
        //    {
        //        w -= 1;
        //    }
        //    LPen penBdr(dc);
        //    penBdr.CreatePen(PS_SOLID, w, RGB(0, 0, 0));
        //    w /= 2;
        //    GetLayerRect(rc);
        //    rc.InflateRect(-w, -w);
        //    dc.Rectangle(rc);
        //    // dc.FrameRect(rc, penBdr);
        //}
        //// dc.SelectObject(GetStockObject(NULL_BRUSH));
        //LPen pen(dc);
        //LFont font(dc);
        //font.CreateFont(_T("Tahoma"), 14);
        //pen.CreatePen(PS_SOLID, _nLineWidth, RGB(0, 0, 0));
        //DrawCells(_cell, 0, 0, dc);
        //uFlags = 0;
        return TRUE;
    }

    /* pt must be in client coordinate */
    //enum {HIT_TITLE, HIT_HEADER, HIT_CELL, HIT_NONE};
    BOOL HitTest(LPoint& pt, INT& row, INT& col, UINT *hitFlags = NULL)
    {
        LPoint pt0 = pt;
        //LRect rcUnfix;
        UINT32 hitFlags0;

        if (!_curLayer->viewPort.PtInRect(pt))
            return FALSE;
        //GetUnfixedRect(rcUnfix);
        //if (rcUnfix.PtInRect(pt0))
        //    _curLayer->ClientToLayer(pt0);
        //else pt0 -= _curLayer->viewPort.lt;
        _curLayer->ClientToLayer(pt0);
        hitFlags0 = LLV_HIT_DATA;
        if (rcFixedRows.PtInRect(pt)) {
            pt0.y = pt.y - _curLayer->viewPort.top;
        } else if (rcFixedCols.PtInRect(pt)) {
            pt0.x = pt.x - _curLayer->viewPort.left;
        } else if (!rcUnfixed.PtInRect(pt)) {
            /* hit on rcFixedRowCols or rcTitle */
            pt0 = pt - _curLayer->viewPort.lt;
            if (rcTitle.PtInRect(pt))
                hitFlags0 = LLV_HIT_TITLE;
            // else if (rcFixedRowCols)
        }
        INT i;
        row = -1;
        // for (i = (LLVS_HEADER & _uCtrlStyle) ? 1 : 0; i < _rows; i++)
        for (i = 0; i < _rows; i++)
        {
            if (pt0.y >= _heights[i] && pt0.y <= _heights[i + 1]) {
                row = i;
                break;
            }
        }
        if ((LLVS_HEADER & _uCtrlStyle) && row == 0)
            hitFlags0 = LLV_HIT_HEADER;

        col = -1;
        for (i = 0; i <= _cols; i++)
        {
            if (pt0.x >= _widths[i] && pt0.x <= _widths[i + 1]) {
                col = i;
                break;
            }
        }

        if (hitFlags != NULL)
            *hitFlags = hitFlags0;
        return -1 != row || -1 != col;
    }

    //VOID GetFixedRect(LRect& rc)
    //{
    //    rc.Zero();
    //    if (_fixedRows > 0) {
    //        rc = _curLayer->viewPort;// _rcDraw;
    //        rc.bottom = _heights[_fixedRows] + 1;
    //    }
    //}

    /* the fixed region include the title, header, top N fixed rows */
    VOID GetUnfixedRect(LRect& rc) /* rc in client coordination */
    {
        rc = _curLayer->viewPort;
        if (_fixedRows > 0/* && _curLayer->sbVert.enabled*/)
            rc.top += (_heights[_fixedRows] + 1);
        if (_fixedCols > 0/* && _curLayer->sbHorz.enabled*/)
            rc.left += (_widths[_fixedCols] + 1);
    }

    virtual VOID OnPaint(LDC& dc)
    {
        // LRect rcFixed;
        if (0 != _uRedrawFlags || _fixedCols == 0 && _fixedRows == 0)
            __super::OnPaint(dc);

        if (_fixedCols > 0 || _fixedRows > 0) {
            //LRect rcUnfixed;
            //GetUnfixedRect(rcUnfixed);
            // dc.IntersectClipRect(rcUnfixed); /* draw the unfixed region (scrollable) only */
            HRGN rgnClient = _rcClient.CreateRgn();//_curLayer->viewPort.CreateRgn();
            HRGN rgnUnfixed = rcUnfixed.CreateRgn();
            HRGN rgnView = _curLayer->viewPort.CreateRgn();
            int r = CombineRgn(rgnClient, rgnClient, rgnView, RGN_DIFF);
            LWIN_ASSERT(r != ERROR);
            r = CombineRgn(rgnClient, rgnClient, rgnUnfixed, RGN_OR);
            LWIN_ASSERT(r != ERROR);
            dc.SelectClipRgn(rgnClient);
            __super::OnPaint(dc);
            DeleteObject(rgnClient);
            DeleteObject(rgnUnfixed);
            DeleteObject(rgnView);

            //LBrush br(dc);
            //br.CreateSolidBrush(RGB(255, 0, 0));
            //FillRgn(dc, rgnClient, br);

            //return;
            //GetTitleRect(rcTitle); /* rcTitle in canvas coordination */
            //rcTitle.MoveTo(_curLayer->viewPort.lt);
            dc.BitBlt(rcTitle, _curLayer->dc, 0, 0);
            //dc.Rectangle(rcTitle);

            // HRGN hrgn = CreateRectRgn()
            //dc.Rectangle(rcUnfixed);
            LPoint pt = _curLayer->offset - _ptDragDlt;
            _curLayer->AdjustLayerDrawOffset(pt);

            //LRect rc = _curLayer->viewPort;
            /* the fixed rows */
            dc.BitBlt(rcFixedRows, _curLayer->dc, pt.x + (rcUnfixed.left - rcTitle.left), rcTitle.Height());

            /* the fixed cols */
            //rcFixedCols.left = rcTitle.left;
            //rcFixedCols.bottom = rcUnfixed.bottom;
            //rcFixedCols.right = rcUnfixed.left;
            //rcFixedCols.top = rcUnfixed.top;
            dc.BitBlt(rcFixedCols, _curLayer->dc, 0, pt.y + (rcUnfixed.top - rcTitle.top));

            /* the left-top corner */
            //rcFixedRowCols.left = rcTitle.left;
            //rcFixedRowCols.top = rcTitle.bottom;
            //rcFixedRowCols.bottom = rcUnfixed.top;
            //rcFixedRowCols.right = rcUnfixed.left;
            dc.BitBlt(rcFixedRowCols, _curLayer->dc, 0, rcTitle.Height());
        }

        ///* repaint the fixed rows region if any */
        //GetFixedRect(rcFixed);
        //// if (_fixedRows > 0) {
        //if (rcFixed.Size() > 0) {
        //    //INT t = dc.Save();
        //    LRect rc = _curLayer->viewPort;
        //    //GetClientRect(rc);
        //    rc.top = rcFixed.bottom;// _heights[_fixedRows] + 1;
        //        rc.left = _widths[_fixedCols];
        //    dc.IntersectClipRect(rc);
        //    __super::OnPaint(dc);
        //    //dc.Restore(t);

        //    //rc = _rcDraw;
        //    //rc.bottom = _heights[_fixedRows] + 1;
        //    LPoint pt = _curLayer->offset - _ptDragDlt;
        //    // _pThis->AdjustLayerDrawOffset(_curLayer, pt);
        //    _curLayer->AdjustLayerDrawOffset(pt);
        //    int yyy = pt.y;
        //    pt.y = 0;
        //    //GetFixedRect(rc);
        //        rcFixed.left = _widths[_fixedCols];
        //        pt.x += _widths[_fixedCols];
        //    dc.BitBlt(rcFixed, _curLayer->dc, pt);

        //    LRect rcFixedCol = _curLayer->viewPort;
        //    rcFixedCol.right = _widths[_fixedCols] + 1;
        //    pt.y = yyy + rcFixed.bottom;
        //    pt.x = 0;
        //    rcFixedCol.top = rcFixed.bottom;
        //    dc.BitBlt(rcFixedCol, _curLayer->dc, pt);
        //    // dc.BitBlt(_rcDraw.left, _rcDraw.top, )
        //    //DrawScrollbar(dc);

        //    LRect rcTl = _curLayer->viewPort;
        //    rcTl.right = _widths[_fixedCols] + 1;
        //    rcTl.bottom = rcFixed.bottom;
        //    pt.x = 0; pt.y = 0;
        //    dc.BitBlt(rcTl, _curLayer->dc, pt);
        //}
        //else {
        //    __super::OnPaint(dc);
        //}
        
    }

    virtual void OnHitTest(UINT hitFlags, int row, int col)
    {
    }

    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
       (VOID)__super::OnLButtonDown(dwFlags, pt);
       if (this->drawThread != 0xFFFFFFFF)
           return 0;
        //LPoint pt0 = pt;
        //LRect rcFix;
        //GetFixedRect(rcFix);
        //if (!rcFix.PtInRect(pt0))
        //    _curLayer->ClientToLayer(pt0);
        if (_curLayer->PtInScrollBar(pt))
            return 0;

        SetFocus();

        INT row = _selected.row;
        INT col = _selected.col;
        UINT hitFlags = 0;
        // _nActiveRow = -1;
        if (HitTest(pt, row, col, &hitFlags) && hitFlags != LLV_HIT_TITLE)
        {
            if (!(hitFlags & LLV_HIT_HEADER) && (row != _selected.row || col != _selected.col)) {
                //_nActiveRow = t;
                //SetRedrawFlags(REDRAW_PAINT);
                //InvalidateRect(NULL, FALSE);
                //_curLayer->dc
                //_curLayer->Lock();
                if (-1 != _selected.row)
                    DrawRow(_selected.row, _curLayer->dc, FALSE);
                DrawRow(row, _curLayer->dc, TRUE);
                _selected.row = row;
                _selected.col = col;

                LRect rc;
                GetCellRect(row, col, rc);
                LPen pen(_curLayer->dc);
                pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                LBrush br(_curLayer->dc);
                br.CreateStockObject(NULL_BRUSH);
                //_curLayer->dc.FrameRect(rc, br);
                _curLayer->dc.Rectangle(rc);
                //_curLayer->Unlock();
            }
            OnHitTest(hitFlags, row, col);
            return 0;
        }

        //_nActiveRow = -1;
        //_nMouseStatus |= LVIEW_LBUTTONDOWN;
        //return OnButtonDown(dwFlags, pt);
        return 0;
    }

    //BOOL wheeling = 0;
    #define LLV_MOUSEWHEEL_END 1
    LRESULT OnMouseWheel(UINT nFlags, SHORT zDelta, LPoint& pt) 
    {
        //if (wheeling == TRUE)
        //    return 0;

        LPoint pt0;
        pt0.x = _widths[_fixedCols];
        pt0.y = _heights[_fixedRows] + 1 + 1;
        pt0.x += _curLayer->viewPort.left + 10; // TODO
        pt0.y += _curLayer->viewPort.top;
        // _curLayer->LayerToClient(pt0);
        
        int row = -1, col = -1, amount;
        amount = zDelta > 0 ? -1 : 1;
        if (GetKeyState(VK_LCONTROL) & 0x8000)
            amount *= 10;

        HitTest(pt0, row, col);
        row += amount;
        row = max(row, _fixedRows);
        int maxRow = _rows - _fixedRows + ((LLVS_HEADER & _uCtrlStyle) ? 1 : 0);
        row = min(row, maxRow);
        //if (row < _fixedRows || row >= _rows)
        //    return 0;

        pt0.y = _heights[row];
        _curLayer->LayerToClient(pt0);
        int dltY = pt0.y - _heights[_fixedRows];
        if (dltY == 0)
            return 0;
        _curLayer->offset.y += dltY;
        _curLayer->flags |= LAYER_SHOW_SCROLLBAR_ALWAYS;
        InvalidateRect(NULL, FALSE, TRUE);
        SetTimer(*this, LLV_MOUSEWHEEL_END, 100, NULL);
        return 0;
    }

    LRESULT OnTimer(UINT tmrId)
    {
        if (tmrId == LLV_MOUSEWHEEL_END) {
            KillTimer(*this, tmrId);
            _curLayer->flags &= ~LAYER_SHOW_SCROLLBAR_ALWAYS;
            InvalidateRect(NULL, FALSE, TRUE);
        }

        return 0;
    }

    //virtual LRESULT OnLButtonDblClk (DWORD dwFlags, LPoint& pt) {return 0;}

};

class LDvsView : public LListView
{
    DVS *_ds;
public:
    void SetData(DVS *ds)
    {
        int cols = ds->Cols() + 1;
        LISTVIEW_COLINFO *colInfos = new LISTVIEW_COLINFO[cols];
        LSTR t;
        for (int i = 1; i < cols; i++)
        {
            colInfos[i].name = (*ds)[i - 1].GetDispName();
            colInfos[i].maxWidth = 200;
            t.Format(_T("0%d"), (*ds)[i - 1].GetMaxInt(FALSE));
            int lenTitle = colInfos[i].name.Len() * 12;
            int lenMaxCell = t.Len() * 12;
            colInfos[i].width = max(lenTitle, lenMaxCell);
            colInfos[i].align = DT_RIGHT | DT_SINGLELINE | DT_VCENTER;
        }
        colInfos[0].width = 40;
        colInfos[0].align = DT_RIGHT | DT_SINGLELINE | DT_VCENTER;
        colInfos[0].flags = LLVS_NOBORDER;

        colInfos[1].align = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        SetSize(ds->Rows(), cols, colInfos);
        //SetSize(200, cols, colInfos);
        _ds = ds;
        SetFixedRows(3, FALSE);
        delete[] colInfos;
    }

    //virtual VOID DrawTitle(LDC& dc, LRect& rc)
    //{
    //    LSTR str;
    //    str.Format(_T("offset.y = %d"), _curLayer->offset.y);
    //    //str.Format(_T("NO TITLE. table info: rows %d, cols %d"), _rows, _cols);
    //    //LFont ft(dc);
    //    //ft.CreateFont(_T("Times New Roman"), rc.Height() - 3, FW_BOLD);
    //    dc.DrawText(str, rc);
    //    //    //LFont ft0(dc);
    //    //    //ft0.CreateFont(_T("Times New Roman"), 40, FW_BOLD, RGB(0, 0, 255));
    //    //    //dc.DrawText(_T("_title"), rc);
    //    //ft.CreateFont(_T("Tahoma"), 11, FW_NORMAL, RGB(0, 0, 255));
    //    //dc.DrawText(_T("18/01/2015"), rc, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE );
    //}

    //virtual VOID OnPaint(LDC& dc)
    //{
    //    __super::OnPaint(dc);

    //    LRect rc;
    //    LSTR str;
    //    GetTitleRect(rc);
    //    int row, h = 0;
    //    {
    //        row = 1;
    //        LRect rc0;
    //        GetCellRect(row, 0, rc0);
    //        h = rc0.Height();
    //    }
    //    str.Format(_T("offset.y = %d, row %d"), _curLayer->offset.y, _curLayer->offset.y / h);
    //    dc.DrawText(str, rc);
    //}
    virtual VOID DrawItemText(LDC &dc, int row, int col, LRect &rc, LRect &rcText)
    {
        //if (row > 200)
            //return;
        LSTR text;

        if (col == 0) {
            text.Format(_T("%d"), row);
            dc.DrawText(text, rcText, _colinfos[col].align);
            return;
        }
        //(*_ds)[col].GetText(row, text);
        _ds->GetText(row, col - 1, text);
                    //if (col == 0) {
                    //    LSTR ttt;
                    //    ttt.Format(_T("%d:%s"), row, (LPCTSTR)text);
                    //    dc.DrawText(ttt, rcText);
                    //    return;
                    //}
        dc.DrawText(text, rcText, _colinfos[col].align);
    }

    virtual void OnHitTest(UINT hitFlags, int row, int col)
    {
        if (hitFlags & LLV_HIT_HEADER) {
            if (col == 0)
                return;
            static int f = 0;
            f++;
            _ds->Sort(col - 1, f & 1);
            SetRedrawFlags(REDRAW_PAINT);
            InvalidateRect(NULL, FALSE, 0);
            return;
        }
    }

    //LRESULT OnLButtonDblClk (DWORD dwFlags, LPoint& pt)
    //{
    //    _ds->reverse();
    //    SetRedrawFlags(REDRAW_PAINT);
    //    InvalidateRect(NULL, FALSE, 0);
    //    return 0;
    //}

};
