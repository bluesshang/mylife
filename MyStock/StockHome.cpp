#include "stdafx.h"
#include "StockHome.h"

StockHome::StockHome()
{
}


StockHome::~StockHome()
{
}

LRESULT StockHome::OnCreate(LPCREATESTRUCT lpcs)
{
    LWndCell *pc;
    SetFrameBorderWidth(2);
    // return 0;
    //float H[] = {40.0, 1.0, 30.0};
    //float W[] = {1};
    // cells(MKCELLPOS(0,0),CELLPOS_END).Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
    //cell.Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
    //return 0;
    float H0[] = {1};
    float W0[] = {0.8, 0.2};
    // cell.cells->cell(1, 0).Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);
    pc = &cell;//.CellAt(MKCELLPOS(1,0),CELLPOS_END);
    pc->Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);

    float H1[] = {0.6, 0.3, 0.1};
    float W1[] = {1};
    // cell.cells->cell(1, 0).cells->cell(0, 0).Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);
    // pc = &cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0, 0),CELLPOS_END);
    pc = &cell.CellAt(0, CELLPOS_END);
    pc->Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);

    LWndDrawView *wnd0 = new LWndDrawView;
    LRect rc(0, 0, 100, 200);
    // cells(1,0)->cells(0,0)->cells(0,0); cells({1,0},{0,0},{0,0})
    // LWndCell &cell0 = cell.cells->cell(1, 0).cells->cell(0, 0).cells->cell(0, 0);
    // LWndCell &cell0 = cells(1, 0).cells->cell(0, 0).cells->cell(0, 0);
    // LWndCell &cell0 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,0), MKCELLPOS(0, 0), CELLPOS_END);
    LWndCell &cell0 = cell.CellAt(0, 0, CELLPOS_END);
    //GetCellClientRect(cell0, rc);
    wnd0->CreateChild(*this, _T("LWndDrawView йсм╪"), rc, 1027, 0);
    cell0.AddWnd(wnd0);

    wnd0 = new LWndDrawView;
    wnd0->CreateChild(*this, _T("K lines"), rc, 1028, 0);
    cell0.AddWnd(wnd0);

    // LWndCell &cell1 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,1), CELLPOS_END);
    LWndCell &cell1 = cell.CellAt(1, CELLPOS_END);
    wnd0 = new LWndDrawView;
    wnd0->CreateChild(*this, _T("Realtime trade informations"), rc, 1029, 0);
    cell1.AddWnd(wnd0);

    return 0;
}
