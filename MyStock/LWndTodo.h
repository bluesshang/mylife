#pragma once

#include "utils\larray.h"
#include "utils\lstr.h"

#include "ui\lwin.h"
#include "ui\LWndView.h"

class LWndTodo : public LWndView<LWndTodo>
{
    LPCTSTR _strTodo;
public:

    LWndTodo()
    {
        _strTodo = //_T("\r\n\r\n\r\n\r\nTODOs:\r\n")
            _T("[ ] tip window, it better be a transparent style\r\n")
            _T("[X] set the color of selected tab item same as associated frame\r\n")
            _T("[D] auto save all child windows in a LArray<LWnd*>\r\n")
            _T("[X] tabs on top\r\n")
            _T("[X] layer smaller than client region\r\n")
            _T("[P] LWndTab can get the color from virtual LWnd::GetTabColor\r\n")
            _T("[ ] change name to TWnd, TWndTab, ...\r\n")
            _T("[ ] draw string with hyper link\r\n")
            _T("[X] merge CalcLayerSize & DrawLayer ??? \r\n")
            _T("[X] create window with custome styles(LVS_AUTO_SCROLL, LVS_BESTFIT_CANVAS, ...) ??? \r\n")
            _T("[X] LWndCell.wndTabs.style {TOP, BOTTOM, LEFT?, RIGHT?}\r\n")
            _T("[ ] LTable, LTabledText, LListView, LRtfString?\r\n")
            _T("[ ] support LWndTab::LTS_CENTER\r\n")
            _T("[X] support layer color, background/client color recpectively\r\n")
            _T("[ ] LWnd::ShowWindow(bShow, showParas) 'showParas' can get from LWndTab in case of multiple tabs are associated with on View\r\n")
            _T("[X] REDRAW_CALCULATE to REDRAW_CALCLAYOUT, seperate the Layout and the DrawLayer\r\n")
            _T("[ ] pass control style via constructor, new LWndView(LVS_NO_STRETCH_CANVAS)\r\n")
            _T("[X] specified Layer with scrollbar or not when being created (NO_SCROLLBAR)\r\n")
            _T("[X] draw scroll bar on the top most layer\r\n")
            _T("[D] LAxisVew::OnMouseMove cause extra useless WM_PAINT message per second\r\n")
            _T("    caused by InvalidateRect(NULL, FALSE, TRUE)?\r\n")
            _T("[ ] display VOL by historical ratio (n%)\r\n")
            _T("[ ] refactore Size(BOOL all = FALSE), GetMaxInt(BOOL all = FALSE)\r\n")
            _T("[ ] layer->ai.x2d() should add DVS::x0\r\n")
            _T("[ ] -- add more here --")
            ;
    }

    ~LWndTodo()
    {
    }

    VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        //cx = 1000;
        //cy = 500;
        LRect rc;
        LFont ft(layer->dc);
        ft.CreateFont(_T("ËÎÌו"), 18, FW_BOLD);
        layer->dc.CalcTextRect(_strTodo, rc);
        cx = rc.Width() + 10;
        cy = rc.Height() + 10;
    }

    BOOL DrawLayer(Layer *layer, UINT& uFlags)
    {
        LDC& dc = layer->dc;

        if (REDRAW_CALCULATE & uFlags)
        {
            LRect rc;
            //GetClientRect(rc);
            GetLayerRect(rc);
            rc.InflateRect(-5, -5);
            LFont ft(dc);
            dc.SetBkMode(TRANSPARENT);
            ft.CreateFont(_T("ËÎÌו"), 18, FW_BOLD);
            dc.DrawText(_strTodo, rc, DT_LEFT);
        }
        //uFlags = 0;
        return TRUE;
    }

    LRESULT OnCreate(LPCREATESTRUCT lpcs) 
    {
        __super::OnCreate(lpcs);

        // _cachedLayer->clrBkgrnd = RGB(255, 255, 255);
        _layers[0]->clrBkgrnd = RGB(239, 228, 176);
        _layers[0]->transparency = 0;
        return 0;
    }
};

