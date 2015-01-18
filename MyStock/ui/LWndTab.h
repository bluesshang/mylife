#pragma once

#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#include "lwin.h"
#include "LWndView.h"

#define LTI_STATUS_SELECTED 1
#define LTI_MAX_NAME_LEN 79

#define LTS_BOTTOM  1
#define LTS_TOP     2
#define LTS_LEFT    4
#define LTS_RIGHT   8
#define LTS_CENTER  0x10

typedef struct tagLTABITEM : public LRectStr
{
    //LRect rc;
    //TCHAR szName[LTI_MAX_NAME_LEN + 1];
    LWnd *wnd;
    UINT nStatus;
    COLORREF clrSelected, clrSelectedText;
    UINT msgOnActive; /* message sent to wnd before wnd was shown visible */
    WPARAM wMsgPara; /* additional informations of msgOnActive */
    LPARAM lMsgPara; /* additional informations of msgOnActive */
    // LTABITEM() {memset(this, 0, sizeof(*this));}
    tagLTABITEM& operator = (tagLTABITEM& ti)
    {
        LRectStr::Copy(ti);
        wnd = ti.wnd;
        clrSelected = ti.clrSelected;
        clrSelectedText = ti.clrSelectedText;
        return *this;
    }
    tagLTABITEM() : LRectStr(), wnd(NULL), nStatus(0), msgOnActive(0) {}
    tagLTABITEM(LPCTSTR lpText, LWnd *wndTab) : LRectStr(lpText), wnd(wndTab), nStatus(0) {}

} LTABITEM;

// template <class T>
class LWndTab : public LWndView<LWndTab>
{
    LArray<LTABITEM> _tabs;
    // INT _idHover;
    LTABITEM *_tiHover;
private:
    VOID MarkDefaultActiveTab()
    {
        int i;
        for (i = 0; i < _tabs.Count(); i++)
        {
            if (_tabs[i].nStatus & LTI_STATUS_SELECTED)
                return;
        }
        if (i > 0)
        {
            SetActiveTab(0);
        }
    }

public:

    LWndTab()
    {
        _tiHover = NULL;
    }

    ~LWndTab()
    {
    }

    VOID UpdateTrackingRngs(Layer *layer)
    {
        // __super::UpdateTrackingRngs();
        AddTrackingRng<LTABITEM>(layer, _tabs);
    }

    // VOID OnPaint(LDC& dc)
    BOOL DrawLayer(Layer *layer, UINT& uFlags)
    {
        int i;
        //LSTR str;
        //TCHAR info[256];
        //GetWindowText(str);
        //_stprintf(info, _T("LWndTab(%s)::OnPaint %d"),(LPCTSTR)str, GetDlgCtrlID(m_hWnd));
        //dc.TextOut(0, 0, info);

        //SetCanvasSize(1, 1);
        //LFont ft(dc);
        //ft.CreateFont(_T("Tahoma"), 12);
        LDC& dc = layer->dc;
        dc.SetBkMode(TRANSPARENT);

        //if (REDRAW_CALCULATE & uFlags)
        //{
        //    LayoutTabs(dc);
        //    MarkDefaultActiveTab();
        //}
        LRect rcClient;//, rcInit(0, 0, 100, 20);
        GetClientRect(rcClient);
        if (REDRAW_CALCULATE & uFlags)
        {
            LFont ft(dc);
            ft.CreateFont(_T("Tahoma"), 13, FW_BOLD);
            //rcClient.left += 10;
            dc.LayoutStrings<LTABITEM>(_tabs, rcClient, rcClient, 10, TRUE, NULL, 
                rcClient.Height(), (_uCtrlStyle & (LTS_LEFT | LTS_RIGHT)) ? TRUE : FALSE);
            //_rcBound.InflateRect(2, 2);

            // ResetTrackingRngs();
            // AddTrackingRng<LTABITEM>(_tabs);
            MarkDefaultActiveTab();
            // uFlags &= ~REDRAW_CALCULATE;
        }

        for (i = 0; i < _tabs.Count(); i++)
        {
            LTABITEM& ti = _tabs[i];
            LBrush br(dc);
            LFont ft(dc);
            LPen pen(dc);
            ft.CreateFont(_T("Tahoma"), 13, 
                LTI_STATUS_SELECTED & ti.nStatus ? FW_BOLD : FW_NORMAL, LFont::CLR_NOT_SPECIFIED,
                (_uCtrlStyle & (LTS_LEFT | LTS_RIGHT)) ? 900 : 0);
            COLORREF clrFill = _clrBkgrnd;
            //clrFill = RGB(230, 230, 230);
            br.CreateSolidBrush((LTI_STATUS_SELECTED & ti.nStatus) ? ti.clrSelected : clrFill);
            // br.CreateSolidBrush(RGB(255, 255, 255));
            //clrFill = RGB(255, 0, 0);
            pen.CreatePen(PS_SOLID, 1, LTI_STATUS_SELECTED & ti.nStatus ? ti.clrSelected : clrFill);
            dc.Rectangle(ti);
            // AddTrackingRng(ti.rc, i); // move to LayoutTabs ?

            dc.SetTextColor(LTI_STATUS_SELECTED & ti.nStatus ? ti.clrSelectedText/*RGB(255, 255, 255)*/ : 
               (_tiHover == &ti ? RGB(0, 0, 255) : 0));
            if (_uCtrlStyle & (LTS_LEFT | LTS_RIGHT))
            {
                LRect rc0 = ti;
                //rc0.right = rc0.left;//rc0.Height();
                // dc.TextOut(rc0.right - 2, rc0.top + 5, (LPCTSTR)ti);
                //LSTR strTmp = ti;
                TCHAR buf[256];
                _tcscpy(buf, ti);
                LRect rc1 = ti;
                rc1.Transpose(); /* vertical to horizontal */
                //rc1.left = 0; rc1.top = 0;
                //rc1.right = rc0.Height();
                //rc1.bottom = rc0.Width();
                rc1.right -= 5;
                int h = dc.DrawText(buf, rc1, DT_MODIFYSTRING | DT_END_ELLIPSIS | DT_CALCRECT);
                int xoff = 0;
                if (rc0.Width() > h)
                    xoff = (rc0.Width() - h) / 2;
                // dc.SelectClipRgn()
                dc.TextOut(rc0.left + xoff, rc0.bottom - 5, buf);
                //rc0.InflateRect(20, 20);
                //dc.DrawText((LPCTSTR)ti, rc0, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            else dc.DrawText((LPCTSTR)ti, (RECT)ti, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            //dc.TextOut(ti._rc.left, ti._rc.top, (LPCTSTR)ti);

            //LBrush br0(dc);
            //br0.CreateSolidBrush(RGB(255, 0, 0));
            //dc.FrameRect(ti.rc, br0);
        }

            //LRect rcClient;//, rcInit(0, 0, 100, 20);
            //GetClientRect(rcClient);
        //LPen pen(dc);
        //pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        //dc.MoveTo(rcClient.left, rcClient.top);
        //dc.LineTo(rcClient.right, rcClient.top);
            // dc.Rectangle(rcClient);

        //uFlags = 0;
        return TRUE;
    }

    VOID OnMouseHoverRect(Layer *layer, LVIEW_TRACKING_RNG* trIn, LVIEW_TRACKING_RNG* trOut, LPoint& pt) 
    {
        _tiHover = (LTABITEM *)trIn->pv;
        SetCursor(LoadCursor(NULL, IDC_HAND));
        SetRedrawFlags(REDRAW_PAINT, TRUE);
    }
    VOID OnMouseLeaveRect(Layer *layer, LVIEW_TRACKING_RNG* trOut) 
    {
        _tiHover = NULL;
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        SetRedrawFlags(REDRAW_PAINT, TRUE);
    }

    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
        //LTABITEM *pti;
        // if (NULL != (pti = HitTest(pt)))
        if (NULL != _tiHover)
        {
            if (!(_tiHover->nStatus & LTI_STATUS_SELECTED))
            {
                SetActiveTab(_tiHover);
            }
        }
        //__super::OnLButtonDown(dwFlags, pt);
        return 0;
    }

    VOID SetActiveTab(int pos)
    {
        _ASSERT(pos < _tabs.Count());
        LTABITEM& ti = _tabs[pos];
        SetActiveTab(&ti);
    }

    VOID SetActiveTab(LTABITEM *pti)
    {
        for (int i = 0; i < _tabs.Count(); i++)
        {
            LTABITEM& ti = _tabs[i];
            ti.nStatus &= ~LTI_STATUS_SELECTED;
            ti.wnd->ShowWindow(SW_HIDE);
        }
        pti->nStatus |= LTI_STATUS_SELECTED;
        if (0 != pti->msgOnActive)
            pti->wnd->SendMessage(pti->msgOnActive, pti->wMsgPara, pti->lMsgPara);
        pti->wnd->ShowWindow(SW_SHOW);
        // AnimateWindow(*pti->wnd, 500, AW_SLIDE | AW_ACTIVATE);
        SetRedrawFlags(REDRAW_PAINT);
       (VOID)InvalidateRect(NULL, FALSE);
    }
#if 0
    LTABITEM* HitTest(LPoint pt)
    {
        for (int i = 0; i < _tabs.Count(); i++)
        {
            LTABITEM& ti = _tabs[i];
            if (PtInRect(ti.rc, pt))
            {
                return &ti;
            }
        }
        return FALSE;
    }
#endif
    BOOL AddTab(LTABITEM& ti)
    {
        _tabs.Add(ti);
        return TRUE;
    }

    BOOL AddTab(LWnd* wnd, LPCTSTR lpName = NULL, COLORREF clrSelectedText = RGB(0, 0, 0))
    {
        LTABITEM ti;
        LSTR& str = ti;

        if (NULL == lpName)
        {
            wnd->GetWindowText(str);
        }
        else
        {
            str = lpName;
        }
        if (0 == str.Len())
        {
            str = _T("NO-NAME"); 
        }
        // ti. = str;
        //_tcsncpy(ti.szName, str, LTI_MAX_NAME_LEN);
        ti.wnd = wnd;
        ti.clrSelectedText = clrSelectedText;
        wnd->GetWndTabColor(ti.clrSelected);
        if (ti.clrSelected == _clrBkgrnd)
            ti.clrSelected = RGB(255, 255, 255);
        return AddTab(ti);
    }
};

