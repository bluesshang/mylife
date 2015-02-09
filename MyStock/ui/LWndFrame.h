#pragma once

#include "..\utils\lrect.h"
#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#include "lwin.h"
#include "lgdi.h"
#include "LWndView.h"
#include "LWndTab.h"
#include "LTable.h"


#if 0
class LWndCells;
class LWndCell
{
public:
    enum {WNDS = 0, CELLS = 1};
    LRect rc;
    //LWndCell *parent;
    INT type;
    //union
    //{
    LArray<LWnd*> wnds;
    LWndTab *wndTabs;

    LWndCells *cells;
    //};

    LWndCell()
    {
        type = WNDS;
        //parent = NULL;
        cells = NULL;
        wndTabs = NULL;
    }
    ~LWndCell();
    BOOL Split(int rows, int cols, float *widths = NULL, float *heights = NULL);
    LWndCell& CellAt(DWORD pos, ...);
    // LWndCell& CellAt(UINT row, UINT col, ...);

    VOID AddWnd(LWnd* wnd, LPCTSTR lpTabName = NULL, COLORREF clrSelected = RGB(0, 0, 0)) 
    {
        if (NULL == wndTabs && wnds.Count() > 0)
        {   /* create a tab automatically */
            CreateTabWnd(wnd->GetParent());
            LWIN_ASSERT(NULL != wndTabs);
            wndTabs->AddTab(wnds[0]);
        }

        wnds.Add(wnd);
        if (NULL != wndTabs)
        {
           (VOID)wndTabs->AddTab(wnd, lpTabName, clrSelected);
        }
    }
    
    VOID CreateTabWnd(HWND hParent, UINT uCtrlStyle = LTS_BOTTOM, DWORD dwStyle = WS_VISIBLE | WS_CHILD)
    {
        LRect rc(0, 0, 100, 100);
        wndTabs = new LWndTab;
        wndTabs->CreateChild(hParent, _T("LWndTab"), rc, 0, 0, uCtrlStyle, dwStyle);
    }

    VOID SetActiveTabWnd(int pos)
    {
        wndTabs->SetActiveTab(pos);
    }
};

class LWndCells
{
    UINT _rows, _cols;
    LArray<LWndCell> *_cells;
    float *_widths, *_heights;
    UINT _xfixed, _yfixed;
public:
    UINT N;
    LWndCells() {MEMZERO_THIS();}
    //LWndCells(int rows, int cols, LWndCell *parent = NULL, float *widths = NULL, float *heights = NULL)
    //{
    //   (VOID)SetCellsInfo(rows, cols, parent, widths, heights);
    //}
    ~LWndCells()
    {
        if (_cells) delete _cells;
        if (_widths) delete[] _widths;
        if (_heights) delete[] _heights;
    }

    LWndCell& cell(UINT row, UINT col)
    {
        LWIN_ASSERT(row < _rows);
        LWIN_ASSERT(col < _cols);
        return(*_cells)[row * _cols + col];
    }
    LWndCell& operator[](int pos) {return(*_cells)[pos];}

    BOOL SetCellsInfo(UINT rows, UINT cols, LWndCell *parent = NULL, float *widths = NULL, float *heights = NULL)
    {
        UINT i;

        _cells = new LArray<LWndCell>(rows * cols);
        _widths = new float[cols];
        _heights = new float[rows];
        _rows = rows;
        _cols = cols;
        N = _rows * _cols;

        for (i = 0; i < cols; i++)
        {
            _widths[i] = widths ? widths[i] :(float)1.0 /(float)cols;
            if ((UINT)_widths[i] > 1)
            {
                _xfixed += (UINT)_widths[i];
            }
        }
        for (i = 0; i < rows; i++)
        {
            _heights[i] = heights ? heights[i] :(float)1.0 /(float)rows;
            if ((UINT)_heights[i] > 1)
            {
                _yfixed += (UINT)_heights[i];
            }
        }
        for (i = 0; i < N; i++)
        {
            LWndCell& c = (*_cells)[i];
            c.type = LWndCell::WNDS;
            //c.parent = parent;
        }
        return TRUE;
    }

    void LayoutCells(LRect& rc, INT n)
    {
        UINT i, j;
        LONG w, h, t;
        LArray<UINT> xpos(_cols + 1), ypos(_rows + 1);

        w = rc.Width() - _xfixed;
        w = max(w, 0);
        t = rc.left;
        for (i = 0; i < _cols; i++)
        {
            xpos[i] = min(t, rc.right);
            t += (INT)_widths[i] > 1 ?(INT)_widths[i] :(INT)((float)w * _widths[i]);
        }
        xpos[i] = rc.right;

        h = rc.Height() - _yfixed;
        h = max(h, 0);
        t = rc.top;
        for (i = 0; i < _rows; i++)
        {
            ypos[i] = min(t, rc.bottom);
            t += (INT)_heights[i] > 1 ?(INT)_heights[i] :(INT)((float)h * _heights[i]);
        }
        ypos[i] = rc.bottom;

        for (i = 0; i < _rows; i ++)
        {
            for (j = 0; j < _cols; j++)
            {
                LRect& _rc = cell(i, j).rc;
                _rc.left = xpos[j];
                _rc.top = ypos[i];
                _rc.right = xpos[j + 1] + n; /* +n: using rectangle in closed interval(the right and bottom was on border) */
                _rc.bottom = ypos[i + 1] + n;
                _rc.right = min(_rc.right, rc.right);
                _rc.bottom = min(_rc.bottom, rc.bottom);
            }
        }
    }
};

LWndCell::~LWndCell()
{
    if (cells) delete cells;
    if (wndTabs) delete wndTabs;
}

BOOL LWndCell::Split(int rows, int cols, float *widths/* = NULL*/, float *heights/* = NULL*/)
{
    type = CELLS;
    cells = new LWndCells();
    cells->SetCellsInfo(rows, cols, this, widths, heights);
    return TRUE;
}

LWndCell& LWndCell::CellAt(DWORD pos, ...)
{
    LWndCell *c0 = this;//&cell;
    int row, col;
    va_list vp;

    LWIN_ASSERT(CELLPOS_END != pos);
    va_start(vp, pos);
    do {
        //if (LWndCell::WNDS == c0->type) { break; }
        LWIN_ASSERT(LWndCell::CELLS == c0->type);
        row = HIWORD(pos);
        col = LOWORD(pos);
        c0 = &c0->cells->cell(row, col);
        pos = va_arg(vp, DWORD);
    } while(CELLPOS_END != pos);
    return *c0;
}
#endif

#define MKCELLPOS(row, col) MAKELONG(col, row)
#define CELLPOS_END         MKCELLPOS(0xffff, 0xffff)
#define LFS_OVERLAP_FRAME_BORDER 1

class LFrameCell
{
public:
    LArray<LWnd*> wnds;
    LWndTab *wndTabs;

    LFrameCell()
    {
        wndTabs = NULL;
    }
    ~LFrameCell()
    {
        delete wndTabs;
    }

    VOID AddWnd(LWnd* wnd, LPCTSTR lpTabName = NULL, COLORREF clrSelected = RGB(0, 0, 0)) 
    {
        if (NULL == wndTabs && wnds.Count() > 0)
        {   /* create a tab automatically */
            CreateTabWnd(wnd->GetParent());
            LWIN_ASSERT(NULL != wndTabs);
            wndTabs->AddTab(wnds[0]);
        }

        wnds.Add(wnd);
        if (NULL != wndTabs)
        {
           (VOID)wndTabs->AddTab(wnd, lpTabName, clrSelected);
        }
    }
    
    VOID CreateTabWnd(LWnd *parent, UINT uCtrlStyle = LTS_BOTTOM, DWORD dwStyle = WS_VISIBLE | WS_CHILD)
    {
        LRect rc(0, 0, 100, 100);
        wndTabs = new LWndTab;
        wndTabs->CreateChild(parent, _T("LWndTab"), rc, 0, 0, uCtrlStyle, dwStyle);
    }

    VOID SetActiveTabWnd(int pos)
    {
        wndTabs->SetActiveTab(pos);
    }
};

template<class T>
class LWndFrame :// public LWndView<T>
    public LWindowImpl<T>
{
    // LWndCells cells;
    INT _nFrameBdrWidth;
    COLORREF _clrFrameBdr;
protected:
    //LArray<LWnd *> _wnds;
public:
    // LWndCell cell;
    LCell<LFrameCell> cell;
public:
    //BOOL bKeepBoundary;
    VOID SetFrameBorderWidth(UINT uWidth, BOOL bRedraw = FALSE, INT *plOldWidth = NULL) 
    {
        if (NULL != plOldWidth) *plOldWidth = _nFrameBdrWidth;
        _nFrameBdrWidth = uWidth;
        if (uWidth > 0)
            _nFrameBdrWidth |= 1; /* the width of frame border should be odd */
    }

    VOID SetFrameBorderColor(COLORREF clr, BOOL bRedraw = FALSE, COLORREF *plOldColor = NULL) 
    {
        if (NULL != plOldColor) *plOldColor = _clrFrameBdr;
        _clrFrameBdr = clr;
        _clrBkgrnd = clr;
    }
    COLORREF GetFrameBorderColor() { return _clrFrameBdr; }

    VOID GetWndTabColor(COLORREF& clrSelected)
    {
        clrSelected = _clrFrameBdr;
    }

    //VOID ClipChildRegions(LDC& dc)
    //{
    //    LRect rc;
    //    for (int i = 0; i < childs.Count(); i++) {
    //        if (!childs[i]->IsWindowVisible())
    //            continue;
    //        GetChildRect(childs[i], rc);
    //        //rc.left += 20;
    //        //{
    //        //    dc.Rectangle(rc);
    //        //    LSTR txt;
    //        //    childs[i]->GetWindowText(txt);
    //        //    dc.DrawText(txt, rc);
    //        //}
    //        dc.ExcludeClipRect(rc);
    //    }
    //}

    LWndFrame() : LWindowImpl<T>(_T("LWndFrame"))
    {
        _nFrameBdrWidth = 1;
        _clrFrameBdr = RGB(0, 0, 0);
        //_bIsFrameWnd = TRUE;
        // _uLWndStyle |= LWS_FRAME_WND;

        //bKeepBoundary = FALSE;
    }

    ~LWndFrame()
    {
    }
    UINT GetFixedLWndStyle() { return LWS_FRAME_WND; }
    LRESULT OnSize(UINT nType, INT cx, INT cy)
    {
        if (cx > 0 && cy > 0)
        {
            LRect rc(0, 0, cx - 0, cy - 0);
            //if (TRUE == bKeepBoundary)
            //{
            //INT x = _nFrameBdrWidth / 2;
            //rc.InflateRect(-x, -x);
            ////}
            //cell.rc = rc;
            //LayoutCells(cell, rc);
            cell.Layout(rc, _nFrameBdrWidth, 0, 0, _nFrameBdrWidth ? TRUE : FALSE);
            Layout(cell);
        }
       (VOID)__super::OnSize(nType, cx, cy);
        return 0;
    }

    //void LayoutCells(LWndCell& cell, LRect& rc)
    //{
    //    if (LWndCell::CELLS == cell.type)
    //    {
    //        cell.cells->LayoutCells(rc, _nFrameBdrWidth ? 1 : 0);
    //        for (UINT i = 0; i < cell.cells->N; i++)
    //        {
    //            LWndCell& ci = (*cell.cells)[i];
    //            LayoutCells(ci, ci.rc);
    //        }
    //    }
    //    else // if (cell.wnds.Count() > 0)
    //    {
    //        int i;
    //        LRect rcWnd, rcFrame, rcTabs, rcFrameInner;
    //        GetCellClientRect(cell, rcWnd, rcTabs, rcFrame, rcFrameInner);
    //        for (i = 0; i < cell.wnds.Count(); i++)
    //        {
    //            LWnd* wnd = cell.wnds[i];
    //            LRect& rc = (LWS_FRAME_WND & wnd->GetWndStyle()) ? 
    //               ((LFS_OVERLAP_FRAME_BORDER & wnd->GetCtrlStyle()) ? rcFrame : rcFrameInner): rcWnd;
    //            cell.wnds[i]->MoveWindow(rc, TRUE);
    //        }
    //        // if (i > 1 && NULL != cell.wndTabs)
    //        if (/*i > 1 && */NULL != cell.wndTabs)
    //        {
    //            cell.wndTabs->MoveWindow(rcTabs, TRUE);
    //        }
    //    }
    //}

    // rcFrame : the border of frame window will be overlaped on it's container cell
    //void GetCellClientRect(LWndCell& cell, LRect &rcWnd, LRect& rcTab, LRect& rcFrame, LRect& rcFrameInner)
    //{
    //    int nTabHeight = 20;
    //    int t0 = _nFrameBdrWidth / 2;
    //    int t = t0 +(_nFrameBdrWidth ? 1 : 0);
    //    // t = max(t, 1);
    //    rcWnd = cell.rc;
    //    //if (FALSE == bOverlapped)
    //    rcWnd.InflateRect(-t, -t);
    //    if (LWndCell::WNDS == cell.type && cell.wnds.Count() > 1)
    //    {
    //        //if (NULL != lrcTab)
    //        //{
    //        rcTab = rcWnd;
    //        rcFrameInner = rcWnd;
    //        rcFrame = cell.rc;
    //        rcFrame.InflateRect(t0, t0);

    //        if (LTS_BOTTOM & cell.wndTabs->GetCtrlStyle())
    //        {
    //            rcTab.top = rcTab.bottom - nTabHeight;
    //            rcTab.top = max(rcTab.top, 0);
    //            rcTab.top = max(rcTab.top, rcWnd.top);
    //            rcWnd.bottom -= nTabHeight;
    //            rcWnd.bottom = max(rcWnd.bottom, 0);
    //            rcWnd.bottom = max(rcWnd.bottom, rcWnd.top);
    //            rcFrame.bottom = rcWnd.bottom;
    //            rcFrameInner.bottom = rcWnd.bottom;
    //        }
    //        else
    //        {
    //            rcTab.bottom = rcTab.top + nTabHeight;
    //            rcTab.bottom = min(rcTab.bottom, rcWnd.bottom);
    //            rcWnd.top += nTabHeight;
    //            rcWnd.top = min(rcWnd.top, rcWnd.bottom);
    //            rcFrame.top = rcWnd.top;
    //            rcFrameInner.top = rcWnd.top;
    //        }
    //    }
    //}

    void GetCellClientRect(LCell<LFrameCell>& fc, LRect &rcWnd, LRect& rcTab, LRect& rcFrame, LRect& rcFrameInner)
    {
        int nTabHeight = 20;
        int t0 = _nFrameBdrWidth / 2;
        int t = t0 +(_nFrameBdrWidth ? 1 : 0);
        LFrameCell& c = fc;
        // t = max(t, 1);
        rcWnd = fc.rc;
        //if (FALSE == bOverlapped)
        rcWnd.InflateRect(-t, -t);
        if (LCell<LFrameCell>::DATA == fc.type && c.wnds.Count() > 1)
        {
            //if (NULL != lrcTab)
            //{
            rcTab = rcWnd;
            rcFrameInner = rcWnd;
            rcFrame = fc.rc;
            rcFrame.InflateRect(t0, t0);

            UINT32 ctrlStyle = c.wndTabs->GetCtrlStyle();
            if (ctrlStyle & LTS_BOTTOM)
            {
                rcTab.top = rcTab.bottom - nTabHeight;
                rcTab.top = max(rcTab.top, 0);
                rcTab.top = max(rcTab.top, rcWnd.top);
                rcWnd.bottom -= nTabHeight;
                rcWnd.bottom = max(rcWnd.bottom, 0);
                rcWnd.bottom = max(rcWnd.bottom, rcWnd.top);
                rcFrame.bottom = rcWnd.bottom;
                rcFrameInner.bottom = rcWnd.bottom;
            }
            else if (ctrlStyle & LTS_TOP)
            {
                rcTab.bottom = rcTab.top + nTabHeight;
                rcTab.bottom = min(rcTab.bottom, rcWnd.bottom);
                rcWnd.top += nTabHeight;
                rcWnd.top = min(rcWnd.top, rcWnd.bottom);
                rcFrame.top = rcWnd.top;
                rcFrameInner.top = rcWnd.top;
            }
            else if (ctrlStyle & LTS_LEFT)
            {
                rcTab.right = rcTab.left + nTabHeight; /* TODO: use nTabWidth instead? */
                rcTab.right = min(rcTab.right, rcWnd.right);
                rcWnd.left += nTabHeight;
                rcWnd.left = min(rcWnd.left, rcWnd.right);
                rcFrame.left = rcWnd.left;
                rcFrameInner.left = rcWnd.left;
            }
        }
    }

    /* layout all child windows */
    void Layout(LCell<LFrameCell>& fc)
    {
        if (LCell<LFrameCell>::SUBCELL == fc.type)
        {
            int N = fc.subcell.rows * fc.subcell.cols;
            for (INT i = 0; i < N; i++)
            {
                Layout(fc.subcell.cells[i]);
            }
            return;
        }

        // LCell<LFrameCell>& ci = cell.subcell.cells[i];
        LFrameCell& c = fc; //cell.subcell.cells[i];
        // DrawCells(ci, dc);
        int i;
        LRect rcWnd, rcFrame, rcTabs, rcFrameInner;
        GetCellClientRect(fc, rcWnd, rcTabs, rcFrame, rcFrameInner);
        for (i = 0; i < c.wnds.Count(); i++)
        {
            LWnd* wnd = c.wnds[i];
            LRect& rc = (LWS_FRAME_WND & wnd->GetWndStyle()) ? 
               ((LFS_OVERLAP_FRAME_BORDER & wnd->GetCtrlStyle()) ? rcFrame : rcFrameInner): rcWnd;
            //{
            //    HRGN rgn = rc.CreateRoundRectRgn(1, 1);
            //    DWORD err;
            //    if (0 == c.wnds[i]->SetWindowRgn(rgn))
            //    {
            //        err = GetLastError();
            //    }
            //}
            c.wnds[i]->MoveWindow(rc, TRUE);
        }
        // if (i > 1 && NULL != cell.wndTabs)
        if (/*i > 1 && */NULL != c.wndTabs)
        {
            c.wndTabs->MoveWindow(rcTabs, TRUE);
        }
    }

    //void SetCellWnd(LWndCell& cell, LWnd *wnd)
    //{
    //    cell.AddWnd(wnd);
    //    //_wnds.Add(wnd);
    //}
    /* layout all child windows */
    void DrawFrame(LCell<LFrameCell>& fc, LDC& dc)
    {
        //dc.Rectangle(fc);

        if (LCell<LFrameCell>::SUBCELL == fc.type)
        {
            int N = fc.subcell.rows * fc.subcell.cols;
            for (INT i = 0; i < N; i++)
            {
                DrawFrame(fc.subcell.cells[i], dc);
            }
            return;
        }
        if (1)
        {
            LRect rc, rcTab, rcFrame, rcFrameInner;
            // HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
            LPen pen(dc);
            //_rc.InflateRect((int)uFrameBdrWidth/2 * -1,(int)uFrameBdrWidth/2 * -1);
            GetCellClientRect(fc, rc, rcTab, rcFrame, rcFrameInner);
            // HGDIOBJ hOld = SelectObject(dc, pen);
            pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
            //rc.bottom = rc.top + 20;
            //rc.right = rc.left + 40;
            dc.Rectangle(rc);
            //SelectObject(dc, hOld);
            //DeleteObject(pen);
            //dc.DrawText(cell.rc, cell.rc);
            
            //HBRUSH hbr = CreateSolidBrush(RGB(0, 255, 0));
            //dc.FrameRect(_rc, hbr);
            //DeleteObject(hbr);
        }
    }
#if 0
    void DrawFrame(LWndCell& cell, LDC& dc)
    {
        //LSTR str;
        //str.Format(_T("%s"),(LPCTSTR)cell);
        dc.Rectangle(cell.rc);
        if (0)
        {
            LPen pen(dc);
            pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
            dc.Rectangle(cell.rc);
        }
        if (1)
        {
            LRect rc, rcTab, rcFrame, rcFrameInner;
            // HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
            LPen pen(dc);
            //_rc.InflateRect((int)uFrameBdrWidth/2 * -1,(int)uFrameBdrWidth/2 * -1);
            GetCellClientRect(cell, rc, rcTab, rcFrame, rcFrameInner);
            // HGDIOBJ hOld = SelectObject(dc, pen);
            pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
            //rc.bottom = rc.top + 20;
            //rc.right = rc.left + 40;
            dc.Rectangle(rc);
            //SelectObject(dc, hOld);
            //DeleteObject(pen);
            //dc.DrawText(cell.rc, cell.rc);
            
            //HBRUSH hbr = CreateSolidBrush(RGB(0, 255, 0));
            //dc.FrameRect(_rc, hbr);
            //DeleteObject(hbr);
        }
        if (LWndCell::CELLS == cell.type)
        {
            for (UINT i = 0; i < cell.cells->N; i++)
            {
                // LWndCell& ci = (*cell.cells)[i];
                DrawFrame((*cell.cells)[i], dc);
                //break;
            }
        }
#if 0
        else if (cell.wnds.Count() > 1)
        {
            /* draw tabs */
            LRect rc, rcTabs;
            GetCellClientRect(cell, rc, &rcTabs);
            //{
                LBrush br0;//(dc);
                br0.CreateSolidBrush(RGB(0, 0, 255));
                dc.FrameRect(rcTabs, br0);

                LBrush br;//(dc);
                br.CreateSolidBrush(RGB(0, 255, 0));
                // DeleteObject(hbr);
            //}

            rcTabs.right = rcTabs.left + 140;
            for (INT i = 0; i < cell.wnds.Count(); i++)
            {
                LSTR str;
                cell.wnds[i]->GetWindowText(str);
                dc.DrawText(str, rcTabs);
                dc.FrameRect(rcTabs, br);
                rcTabs.left = rcTabs.right;
                rcTabs.right = rcTabs.left + 140;
            }

            //DeleteObject(hbr);
        }
#endif
    }
#endif
    VOID OnPaint(LDC& dc)
    // BOOL DrawLayer(LDC& dc, UINT& uFlags)
    {
        //HPEN pen = CreatePen(PS_SOLID, uFrameBdrWidth, RGB(255, 0, 0));
        //HGDIOBJ hOld = SelectObject(dc, pen);
        return;
        LPen pen(dc);
        LBrush br(dc);

        //LRect rc;
        //GetClientRect(rc);
        //LayoutCells(cell, rc);

        pen.CreatePen(PS_SOLID, _nFrameBdrWidth, _clrFrameBdr);
        br.CreateStockObject(NULL_BRUSH);
        //SelectObject(dc, GetStockBrush(NULL_BRUSH));
        dc.SetBkMode(TRANSPARENT);
        // ClipChildRegions(dc);
        DrawFrame(cell, dc);
        //dc.SelectClipRgn(NULL);
        //SelectObject(dc, hOld);
        //DeleteObject(pen);
        //uFlags = 0;
        //return TRUE;
    }

private:
    //void DestroyCells(LWndCell& cell)
    //{
    //    if (LWndCell::CELLS == cell.type)
    //    {
    //        for (UINT i = 0; i < cell.cells->N; i++)
    //        {
    //            // LWndCell& ci = (*cell.cells)[i];
    //            DestroyCells((*cell.cells)[i]);
    //        }
    //        delete cell.cells;
    //    }
    //    else // if (cell.wnds.Count() > 0)
    //    {
    //        for (int i = 0; i < cell.wnds.Count(); i++)
    //        {
    //            // delete cell.wnds[i];
    //        }
    //    }
    //}
};

