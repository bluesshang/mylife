#pragma once

#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#include "lwin.h"
#include "LWndView.h"

#define LMI_FLAG_RIGHTALIGN 1

typedef struct tagLMENUITEM : public LRectStr
{
    INT id; /* command ID */
    UINT nFlags;
    tagLMENUITEM& operator = (tagLMENUITEM& mi)
    {
        LRectStr::Copy(mi);
        id = mi.id;
        return *this;
    }
    tagLMENUITEM() : LRectStr(), id(-1) {}
    tagLMENUITEM(LPCTSTR lpText, UINT cmdid, UINT nItemFlags = 0) 
        : LRectStr(lpText), id(cmdid), nFlags(nItemFlags) {}

} LMENUITEM;

class LMenu : public LWndView<LMenu>
{
protected:
    LArray<LMENUITEM> _items;
    LRect _rcBound;
    LMENUITEM *_miHover;
    //INT _idHover;
public:

    LMenu()
    {
        SetDragMode(LVIEW_DRAP_DISABLE);
        //_items.Add(LMENUITEM(_T("FILE"), 1));
        //_items.Add(LMENUITEM(_T("EDIT"), 2));
        //_items.Add(LMENUITEM(_T("PROJECT"), 3));
        //_items.Add(LMENUITEM(_T("ARCHITECTURE"), 4));
        _miHover = NULL;
    }

    ~LMenu()
    {
    }

    virtual LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
       (VOID)__super::OnLButtonDown(dwFlags, pt);
        if (NULL != _miHover)
        {
           (VOID)SetFocus();
            ::SendMessage(GetParentHwnd(), WM_COMMAND, _miHover->id,(LPARAM)this);
        }
        //(VOID)__super::OnLButtonDown(dwFlags, pt);
        return 0;
    }
    VOID OnMouseHoverRect(Layer *layer, LVIEW_TRACKING_RNG* trIn, LVIEW_TRACKING_RNG* trOut, LPoint& pt)
    {
        _miHover = (LMENUITEM *)trIn->pv;
        SetCursor(LoadCursor(NULL, IDC_HAND));
        SetRedrawFlags(REDRAW_PAINT, TRUE);
    }
    VOID OnMouseLeaveRect(Layer *layer, LVIEW_TRACKING_RNG* trOut) 
    {
        _miHover = NULL;
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        SetRedrawFlags(REDRAW_PAINT, TRUE);
    }

    VOID UpdateTrackingRngs(Layer *layer)
    {
        // __super::UpdateTrackingRngs();
        AddTrackingRng<LMENUITEM>(layer, _items);
    }

    virtual VOID OnLayoutTabs() {}
    virtual BOOL DrawLayer(Layer *layer, UINT& uFlags)
    {
        LDC& dc = layer->dc;
        // dc.TextOut(0, 0, _T("LMenu::DrawLayer"));
        dc.SetBkMode(TRANSPARENT);
        LFont ft(dc);
        ft.CreateFont(_T("Times New Roman"), 19, FW_BOLD);

        if ((REDRAW_CALCULATE & uFlags) && _items.Count() > 0)
        {
            INT nBdrGap = 2;
            LRect rcClient/*, rcInit(nBdrGap, nBdrGap, 100, 20)*/;
            GetClientRect(rcClient);
            rcClient.InflateRect(-nBdrGap, -nBdrGap);
            dc.LayoutStrings<LMENUITEM>(_items, rcClient, rcClient, 20, TRUE, &_rcBound, 25);
            /* make the last item on right-align if any */
            LMENUITEM& mi = _items[_items.Count() - 1];
            if ((LMI_FLAG_RIGHTALIGN & mi.nFlags) && mi.right < rcClient.right)
            {
                mi.Offset(rcClient.right - mi.right, 0);
            }
            // LPCTSTR lpText = _items[_items.Count() - 1];
            OnLayoutTabs(); /* give the derived the change to modify the layouted regions */
            //_rcBound.InflateRect(2, 2);

            //ResetTrackingRngs();
            //AddTrackingRng<LMENUITEM>(_items);
            // uFlags &= ~REDRAW_CALCULATE;
        }
        //dc.Rectangle(_rcBound);

        for (int i = 0; i < _items.Count(); i++)
        {
            //dc.Rectangle(_items[i]);
            dc.SetTextColor(_miHover == &_items[i] ? RGB(0, 0, 255) : 0);
            dc.DrawText((LPCTSTR)_items[i],(RECT)_items[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }

        //uFlags = 0;
        return TRUE;
    }

};

class LNativeMenu : public LMenu
{
    BOOL _bTrackingPopMenu;
public:
    HMENU m_hMenu;
    LNativeMenu() : m_hMenu(NULL),_bTrackingPopMenu(FALSE) {}
    ~LNativeMenu()
    {
        if (NULL != m_hMenu)
            DestroyMenu(m_hMenu);
    }
    BOOL LoadMenu(UINT idMenu)
    {
        if (NULL == (m_hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(idMenu))))
        {
            return FALSE;
        }

        INT N = GetMenuItemCount(m_hMenu);
        MENUITEMINFO mii;
        TCHAR name[256];
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.dwTypeData = name;
        mii.cch = sizeof(name);
        mii.fMask = MIIM_STRING | MIIM_ID;
        mii.fType = MFT_STRING | MIIM_ID;
        for (int i = 0; i < N; i++)
        {
            mii.cch = sizeof(name);
            GetMenuItemInfo(m_hMenu, i, TRUE, &mii);
            _items.Add(LMENUITEM(name, mii.wID, i && i == N - 1 ? LMI_FLAG_RIGHTALIGN : 0));
        }

        return TRUE;
    }

    BOOL DrawLayer(Layer *layer, UINT& uFlags)
    {
        __super::DrawLayer(layer, uFlags);

        if (NULL != _miHover && TRUE == _bTrackingPopMenu)
        {
            LDC& dc = layer->dc;
            LBrush br(dc);
            br.CreateSolidBrush(RGB(151, 151, 151));
            dc.FrameRect(*_miHover, br);
        }
        return TRUE;
    }

    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
        //(VOID)__super::OnLButtonDown(dwFlags, pt);
        if (NULL != _miHover)
        {
            MENUITEMINFO mii;
            mii.cbSize = sizeof(MENUITEMINFO);
            mii.fMask = MIIM_SUBMENU;
            mii.fType = MIIM_SUBMENU;
            mii.hSubMenu = NULL;
            GetMenuItemInfo(m_hMenu, _miHover->id, FALSE, &mii);
            if (NULL == mii.hSubMenu)
                ::SendMessage(GetParentHwnd(), WM_COMMAND, _miHover->id,(LPARAM)this);
            else
            {
                TPMPARAMS para = {sizeof(TPMPARAMS)};
                LPoint pt(_miHover->left, _miHover->bottom);
                LRect rc = *_miHover;
                ClientToScreen(pt);
                ClientToScreen(rc.lt);
                ClientToScreen(rc.rb);
                rc.bottom -= 1;
                para.rcExclude = (RECT)rc;
                _bTrackingPopMenu = TRUE;
                SetRedrawFlags(REDRAW_PAINT, TRUE);
               (VOID)TrackPopupMenuEx(mii.hSubMenu, TPM_LEFTALIGN | TPM_VERTICAL, pt.x, pt.y - 1, GetParentHwnd(), &para);
                _bTrackingPopMenu = FALSE;
                SetRedrawFlags(REDRAW_PAINT, TRUE);
            }
            /* check the position of mouse, update the current hovering item if any */
            SetCapture();
            LPoint ptCursor;
            GetCursorPos(&ptCursor);
            ScreenToClient(ptCursor);
            TestTrackingRngs(ptCursor);
        }
        //(VOID)__super::OnLButtonDown(dwFlags, pt);
        return 0;
    }
};

