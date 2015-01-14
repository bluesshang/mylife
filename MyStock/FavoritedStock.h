#pragma once

#include "utils\lstr.h"

#include "ui\lwin.h"
#include "ui\LWndFrame.h"
#include "ui\LWndView.h"
#include "ui\LTable.h"

class FavoritedStock :
    public LWndFrame<FavoritedStock>

{
    LWndDrawView *wnd0, *wnd1;
    LTable<LSTR> tblStocks;
public:
    FavoritedStock() 
    {
        SetFrameBorderWidth(7);
        SetFrameBorderColor(RGB(128, 0, 0));
    };
    ~FavoritedStock() 
    {
        delete wnd0;
        delete wnd1;
    };

    LRESULT OnCreate(LPCREATESTRUCT lpcs)
    {
        //LWndCell *pc;
        LCell<LFrameCell> *pc;
        // return 0;
        //float H[] = {40.0, 1.0, 30.0};
        //float W[] = {1};
        // cells(MKCELLPOS(0,0),CELLPOS_END).Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
        //cell.Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
        //return 0;
        float H0[] = {1}; /* rows */
        float W0[] = {0.6f, 0.4f};
        // cell.cells->cell(1, 0).Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);
        pc = &cell;//.CellAt(MKCELLPOS(1,0),CELLPOS_END);
        pc->Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);
#if 0
        float H1[] = {0.6, 0.3, 0.1};
        float W1[] = {1};
        // cell.cells->cell(1, 0).cells->cell(0, 0).Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);
        // pc = &cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0, 0),CELLPOS_END);
        pc = &cell.CellAt(0, CELLPOS_END);
        pc->Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);
#endif
        LRect rc(0, 0, 100, 200);
        // cells(1,0)->cells(0,0)->cells(0,0); cells({1,0},{0,0},{0,0})
        // LWndCell &cell0 = cell.cells->cell(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        // LWndCell &cell0 = cells(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        // LWndCell &cell0 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,0), MKCELLPOS(0, 0), CELLPOS_END);
        wnd0 = new LWndDrawView;
        LFrameCell &cell0 = cell.cell(0, 0);
        wnd0->CreateChild(this, _T("FavoritedStock left"), rc, 1027, 0);
        cell0.AddWnd(wnd0);

        // LWndCell &cell1 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,1), CELLPOS_END);
        LFrameCell &cell1 = cell.cell(0, 1);
        wnd1 = new LWndDrawView;
        wnd1->CreateChild(this, _T("FavoritedStock right"), rc, 1029, 0);
        cell1.AddWnd(wnd1);

        // tblStocks.CreateChild(*this, _T("String in Table"), rc, 1030, 0, LVS_NO_STRETCH_CANVAS);
        tblStocks.Create(this, 10, 2, rc, _T("Realtime trade informations"));
        cell1.AddWnd(&tblStocks);
        return 0;
    }
};

