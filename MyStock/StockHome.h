#pragma once

#include "stdafx.h"
#include "utils\larray.h"
#include "ui\lwin.h"
#include "ui\LWndFrame.h"
#include "ui\LWndView.h"

#include "LWndTodo.h"
#include "FavoritedStock.h"
#include "ui\LListView.h"
#include "ui\LAxisView.h"

class LDlgViewStockOptions : public LDialogImpl<LDlgViewStockOptions>
{
    LWndDrawView _lvew;
public:
    enum {IDD = IDD_DLG_STOCKOPTIONS};

    INT_PTR OnInitDialog(HWND hWndFocus, LPARAM lParam)
    {
        LRect rc;
        HWND hw = GetDlgItem(IDC_LVIEW0);
        ::GetWindowRect(hw, rc);
        ScreenToClient(rc.lt);
        ScreenToClient(rc.rb);
        //::GetClientRect(hw, rc);
        _lvew.CreateChild(this, _T("IDC_LVIEW0"), rc, 9999);
        return TRUE;
    }

    LRESULT OnCommand(WORD wNotifyCode, WORD wId, HWND hWndCtrl)
    {
        LSTR str;
        if (IDOK == wId)
        {
            GetDlgItemText(IDC_EDIT1, str);
            // str.Format(_T("OnCommand %u"), wId);
            MessageBox(str);
        }
        return 0;
    }

};
FilledLine *_fline = NULL;
class StockHome :
    public LWndFrame<StockHome>
{
    //LArray<LWnd *> _wnds;
    LWndDrawView *wnd0, *wnd1, *wnd2, *wnd3, *wnd4, *wnd5;
    // LArray<LWnd*> wndSync;
    LDlgViewStockOptions *dlg0, dlg1;
    LWndTodo *wndTodo;
    FavoritedStock *wndFav;
    LDVTable tblRltvStocks;
    LListView lvTest, lvTest2;
    LAxisView avKLine, avVol, avMcad;

    LSyncMsgObj msgSync;
    DVS *_ds;

    LSTR stockId;
    //LAxisRuler *avRulers[10];
    //LAxisRuler *rulerDate;
public:
    StockHome()
    {
        SetFrameBorderWidth(7);
        SetFrameBorderColor(RGB(54, 78, 111));
        _ds = NULL;
    }
    ~StockHome()
    {
        //for (int i = 0; i < _wnds.Count(); i++)
        //{
        //    delete _wnds[i];
        //}
        delete wnd0;
        delete wnd1;
        delete wnd2;
        delete wnd3;
        delete wnd4;
        delete wnd5;
        delete dlg0;
        delete wndTodo;
        delete wndFav;
    }

    LRESULT OnCreate(LPCREATESTRUCT lpcs)
    {
        //LWndCell *pc;
        LCell<LFrameCell> *pc;
        //SetFrameBorderWidth(7);
        //SetFrameBorderColor(RGB(54, 78, 111));
        // return 0;
        //float H[] = {40.0, 1.0, 30.0};
        //float W[] = {1};
        // cells(MKCELLPOS(0,0),CELLPOS_END).Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
        //cell.Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
        //return 0;
        float H0[] = {1};
        float W0[] = {0.7f, 0.3f};
        // cell.cells->cell(1, 0).Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);
        pc = &cell;//.CellAt(MKCELLPOS(1,0),CELLPOS_END);
        pc->Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);

        float H1[] = {0.6f, 0.25f, 0.15f};
        float W1[] = {1};
        // cell.cells->cell(1, 0).cells->cell(0, 0).Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);
        // pc = &cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0, 0),CELLPOS_END);
        pc = &cell.cell(0, 0);
        pc->Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);
        //return 0;
        LRect rc(0, 0, 100, 200);

        // cells(1,0)->cells(0,0)->cells(0,0); cells({1,0},{0,0},{0,0})
        // LWndCell &cell0 = cell.cells->cell(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        // LWndCell &cell0 = cells(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        // LWndCell &cell0 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,0), MKCELLPOS(0, 0), CELLPOS_END);

        LFrameCell &cell0 = cell.cell(0, 0, CELLPOS_END);
        cell0.CreateTabWnd(this);
        //cell0.wndTabs = new LWndTab;
        //cell0.wndTabs->Create(*this, WS_EX_WINDOWEDGE, WS_VISIBLE | /*WS_BORDER |*/ WS_CHILD, _T("LWndTab"), rc, 0, 0);
        wndFav = new FavoritedStock;
        wndFav->SetFrameBorderWidth(3);
        wndFav->SetFrameBorderColor(RGB(0, 0, 128));
        wndFav->CreateChild(this, _T("favorited"), rc, 1028, 0);
        cell0.AddWnd(wndFav, _T("Nested Favorited Stocks"), RGB(255, 255, 255));

        //GetCellClientRect(cell0, rc);
        wnd0 = new LWndDrawView;
        wnd0->SetBackgroundColor(RGB(255, 200, 200));
        wnd0->SetDragMode(LVIEW_DRAG_SELECTION);
        wnd0->CreateChild(this, _T("Price indicators"), rc, 1027, 0, LVS_NO_STRETCH_CANVAS);
        cell0.AddWnd(wnd0);
        //_wnds.Add(wnd0);

        wnd1 = new LWndDrawView;
        wnd1->SetBackgroundColor(RGB(214, 219, 233));
        wnd1->CreateChild(this, _T("K lines"), rc, 1028, 0, LVS_NO_STRETCH_CANVAS);
        cell0.AddWnd(wnd1);
        //_wnds.Add(wnd0);

        wnd2 = new LWndDrawView;
        wnd2->CreateChild(this, _T("MCAD lines"), rc, 1028, 0);
        cell0.AddWnd(wnd2);

        msgSync.Add(&avVol);
        msgSync.Add(&avKLine);
        msgSync.Add(&avMcad);

        avVol.CreateChild(this, _T("Volume"), rc, 0, 0, LVS_NO_STRETCH_CANVAS);
        avMcad.SetBackgroundColor(RGB(255, 255, 255));
        avMcad.CreateChild(this, _T("MCAD"), rc, 0, 0, LVS_NO_STRETCH_CANVAS);

        //avKLine._wndDragSync = &wndSync;
        avKLine.SetBackgroundColor(RGB(214, 219, 233));
        // avKLine.SetBackgroundColor(RGB(0, 0, 233));
        avKLine.CreateChild(this, _T("Axis view"), rc, 1030, 0, LVS_NO_STRETCH_CANVAS);
        
        //avKLine.addtestlayer();

        // avKLine.ad
        //{
        //    Layer *layer = new Layer(0x1234, RGB(255, 255, 255), _T("open price"));
        //    avKLine._layers.Add(layer);
        //}

        {
            LAxisRuler *ruler = new LAxisRuler(LAxisRuler::BOTTOM, _T("Date"));
            LRect rcRuler = {0, 0, 0, 0};
            rcRuler.bottom = 30;
            ruler->CreateChild(&avKLine, _T("LAxisRuler BOTTOM"), rcRuler);
            avKLine.AddRuler(ruler);
            //rulerDate = ruler;
            //ruler->SetData(&(*_ds)[_T("Date")]);

            ruler = new LAxisRuler(LAxisRuler::BOTTOM);
            rcRuler.bottom = 20;
            ruler->CreateChild(&avKLine, _T("LAxisRuler BOTTOM 22"), rcRuler);
            avKLine.AddRuler(ruler);

            ruler = new LAxisRuler(LAxisRuler::TOP);
            rcRuler.bottom = 20;
            ruler->CreateChild(&avKLine, _T("LAxisRuler TOP"), rcRuler);
            avKLine.AddRuler(ruler);

            ruler = new LAxisRuler(LAxisRuler::RIGHT);
            rcRuler.right = 100;
            ruler->CreateChild(&avKLine, _T("LAxisRuler RIGHT"), rcRuler);
            avKLine.AddRuler(ruler);
            //ruler->SetData(&(*_ds)[_T("Close")]);

            ruler = new LAxisRuler(LAxisRuler::LEFT);
            rcRuler.right = 20;
            ruler->CreateChild(&avKLine, _T("LAxisRuler LEFT"), rcRuler);
            avKLine.AddRuler(ruler);

            ruler = new LAxisRuler(LAxisRuler::RIGHT);
            rcRuler.right = 100;
            ruler->CreateChild(&avVol, _T("LAxisRuler RIGHT"), rcRuler);
            avVol.AddRuler(ruler);

            ruler = new LAxisRuler(LAxisRuler::LEFT);
            rcRuler.right = 20;
            ruler->CreateChild(&avVol, _T("LAxisRuler LEFT"), rcRuler);
            //ruler->SetData(&(*_ds)[_T("Volume")]);
            avVol.AddRuler(ruler);

            ruler = new LAxisRuler(LAxisRuler::RIGHT);
            rcRuler.right = 100;
            ruler->CreateChild(&avMcad, _T("LAxisRuler RIGHT"), rcRuler);
            avMcad.AddRuler(ruler);

            ruler = new LAxisRuler(LAxisRuler::LEFT);
            rcRuler.right = 20;
            ruler->CreateChild(&avMcad, _T("LAxisRuler LEFT"), rcRuler);
            //ruler->SetData(&(*_ds)[_T("Volume")]);
            avMcad.AddRuler(ruler);
        }

        cell0.AddWnd(&avKLine);

        dlg0 = new LDlgViewStockOptions;
        // dlg0->SetBackgroundColor(RGB(255, 255, 255));
        dlg0->Create(*this);
        cell0.AddWnd(dlg0, _T("Options"));

        dlg1.SetBackgroundColor(RGB(209, 230, 248));
        dlg1.Create(*this);
        cell0.AddWnd(&dlg1, _T("Options[Custom background color]"));

        LFrameCell &cell1 = cell.cell(0, 0x10000, CELLPOS_END);
        // avVol._wndDragSync = &wndSync;
        //avVol.CreateChild(this, _T("Volume"), rc, 0, 0, LVS_NO_STRETCH_CANVAS);
        // avVol.AddMouseSync()

        cell1.AddWnd(&avVol);
        wnd3 = new LWndDrawView;
        msgSync.Add(wnd3);
        wnd3->CreateChild(this, _T("RSI"), rc, 1028, 0);
        // cell1.AddWnd(wnd3);
        cell1.AddWnd(&avMcad, _T("MCAD"));
        cell1.AddWnd(wnd3, _T("KDJ"));
        cell1.AddWnd(wnd3, _T("EMA"));
        cell1.AddWnd(wnd3, _T("More Indicators ..."));

        tblRltvStocks.Create(this, rc, NULL, _T("Relative stocks"), 1);
        cell1.AddWnd(&tblRltvStocks);
        if (1)
        {
            DVS *ds = new DVS(30);
            ds->Add(new DV<LSTR>(0, _T("Descriptions"), NULL, _T("%s")));
            ds->Add(new DV<int>(0, _T("id")));
            ds->Add(new DV<float>(0, _T("Open"), NULL, _T("%.2f")));
            ds->Add(new DV<float>(0, _T("Close"), NULL, _T("%.2f")));
            ds->Add(new DV<float>(0, _T("High"), _T("High Price"), _T("%.2f")));
            ds->Add(new DV<float>(0, _T("Low"), NULL, _T("%.2f")));
            //DV<LSTR> *dv = new DV<LSTR>(0, _T("Descriptions"), NULL, _T("%s"));
            //(*dv)[1] = _T("hello world!");
            //ds->Add(dv);
            DV<LSTR> &dv = *(DV<LSTR> *)&(*ds)[0];
            dv[0] = _T("hello world!");
            dv[5] = _T("天津磁卡");
            dv[6] = _T("三一重工");
            dv[7] = _T("中联重科");
            tblRltvStocks.Load(ds);
        }

        lvTest.Create(this, 20, 5, rc, _T("ListView Test: Realtime trade information"), LLVS_HEADER | LLVS_TITLE, 500);
        cell1.AddWnd(&lvTest);

        LFrameCell &cell2 = cell.cell(0, 0x20000, CELLPOS_END);
        wnd4 = new LWndDrawView;
        wnd4->CreateChild(this, _T("KDJ lines"), rc, 1028, 0);
        cell2.AddWnd(wnd4);

        //wnd0->__wndDragSync = wnd3;
        //wnd3->__wndDragSync = wnd0;
        //LArray<LWnd*> wndSync;
        //wndSync.Add(wnd0);
        //wndSync.Add(wnd1);
        //wndSync.Add(wnd2);
        //wndSync.Add(wnd3);
        //wndSync.Add(wnd4);
        //wnd0->_wndDragSync = &wndSync;
        //wnd1->_wndDragSync = &wndSync;
        //wnd2->_wndDragSync = &wndSync;
        //wnd3->_wndDragSync = &wndSync;
        //wnd4->_wndDragSync = &wndSync;
        //for (i = 0; i < cell.wnds.Count(); i++)
        //{
        //    cell.wndTabs->AddTab(cell.wnds[i]);
        //}

        cell0.SetActiveTabWnd(4);
        //_wnds.Add(wnd0);

        // LWndCell &cell1 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,1), CELLPOS_END);
        LFrameCell &cell3 = cell.cell(0, 1);
        cell3.CreateTabWnd(this, LTS_BOTTOM, WS_VISIBLE | WS_CHILD/* | WS_BORDER*/);
        wndTodo = new LWndTodo;
        wndTodo->CreateChild(this, _T("To do list"), rc, 1029, 0, LVS_NO_STRETCH_CANVAS);

        lvTest2.Create(this, 200, 5, rc, _T("ListView Test: Realtime trade information 222"), LLVS_HEADER | LLVS_TITLE, 1500);

        cell3.AddWnd(wndTodo, _T("TODO"));
        cell3.AddWnd(wndTodo, _T("基本资料"), RGB(255, 0, 0));
        cell3.AddWnd(&lvTest2, _T("Notices"));
        // cell3.SetActiveTabWnd(0);
        // cell3.AddWnd(wnd4);
        //wndTodo = new LWndTodo;
        //wndTodo->CreateChild(*this, _T("To do list"), rc, 1029, 0);
        //cell0.AddWnd(wndTodo);
        
        //cell3.AddWnd(wndTodo);
        //_wnds.Add(wnd0);

        if (_ds != NULL)
        {
            Line *line = new KLine(&(*_ds)[_T("Open")], &(*_ds)[_T("Close")], &(*_ds)[_T("High")], &(*_ds)[_T("Low")]);
            avKLine.AddLine(line);
            //avKLine.AddLine(new Line(&(*_ds)[_T("Close")]));
            _DV *ma5 = (*_ds)[_T("Close")].MA(5);
            avKLine.AddLine(new Line(ma5, RGB(255, 0, 0), _T("MA5")));

            _DV *ma20 = (*_ds)[_T("Close")].MA(20);
            avKLine.AddLine(new Line(ma20, RGB(255, 255, 0), _T("MA20")));

            _fline = new FilledLine(&(*_ds)[_T("Close")], RGB(50, 50, 142), RGB(125, 125, 179), 20);
            avKLine.AddLine(_fline, Layer::EAGLE_EYE);
            avKLine.AddLine(new Line(&(*_ds)[_T("Amount")], RGB(0, 255, 0)), _T("Amount Layer(green)"));
            //avKLine.AddLine(new Line(&(*_ds)[_T("Volume")], RGB(255, 0, 0)), _T("Volume (red)"));
            //avKLine.AddLine(new VolLine(&(*_ds)[_T("Volume")]), _T("Volume layer"));
            //avKLine.AddLine(new VolLine(&(*_ds)[_T("Volume")]));
            // avKLine.AddLine()
            if (stockId != _T("ZHDC"))
            {
                /* REFs */
                DVS *dsRef = LoadStockData(_T("ZHDC"));
                if (dsRef != NULL) {
                    Layer *refLayer = avKLine.CreateLayer(_T("REF:ZHDC(red line) -- floating"), RGB(0, 0, 0), 50);
                    avKLine.AddLine(new Line(&(*dsRef)[_T("Close")], RGB(162, 195, 241)), refLayer);

                    Layer *refLayer2 = avKLine.CreateLayer(_T("REF:ZHDC(blue line)"), RGB(54, 78, 111), 100);
                    avKLine.AddLine(new Line(&(*dsRef)[_T("Close")], RGB(162, 195, 241)), refLayer2);
                }
                //_fline->SetFilledRange(100, -1);
            }

            if (stockId != _T("SZZZ"))
            {
                /* REF SZZZ */
                DVS *dsRefSH = LoadStockData(_T("SZZZ"));
                if (dsRefSH != NULL) {
                    Layer *refLayer2 = avKLine.CreateLayer(_T("REF:SH INDEX"), RGB(54, 78, 111), 100);
                    avKLine.AddLine(new Line(&(*dsRefSH)[_T("Close")], RGB(162, 195, 241)), refLayer2);
                }
            }

            avVol.AddLine(new VolLine(&(*_ds)[_T("Volume")]));

            _DV *ma30 = (*_ds)[_T("Close")].MA(30);
            avMcad.AddLine(new Line(ma30, RGB(0, 0, 200), _T("MA30")));

        }
        return 0;
    }

    //LRESULT LCALLBACK LWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    //{
    //    //T* pThis = static_cast<T *>(this); /* for 'overiding' */
    //    //LVIEW_MOUSESYNC_INFO *pmsi;

    //    switch (msg)
    //    {
    //    //case WM_LVIEW_MOUSESYNC:
    //    //    pmsi = (LVIEW_MOUSESYNC_INFO *) lParam;
    //    //    if ((LVIEW_MOUSESYNC_DRAGEND | LVIEW_MOUSESYNC_DRAGGING) & wParam)
    //    //    {
    //    //        _pThis->OnMouseDragDrop(wParam, pmsi->ptMouse, pmsi->ptDragDlt);
    //    //    }
    //    //    return 0;
    //    case WM_LWIN_ACTIVE_LAYER_CHANGED:
    //        // return 0;
    //        //printf("\r\nWM_LWIN_ACTIVE_LAYER_CHANGED");
    //        Layer *layer = (Layer *)lParam;
    //        //if (ruler == rulerDate)
    //        rulerDate->SetData(layer->QueryDV(_T("Date")));
    //        // rulerDate->SetData(&(*_ds)[_T("Date")]);
    //        break;
    //    }
    //    return __super::LWndProc(hWnd, msg, wParam, lParam);
    //}

    BOOL LoadData(LPCTSTR stockId)
    {

        //_dv = new DV<float>(882);
        //DV<float> &l0 = *(DV<float>*)_dv;
        //fmin = 400000; fmax = 0;
        //{
        //    LSTR str;
        //    str.Format(_T("C:\\dzh365\\%s.%s"), stockId, _T("txt"));
        //    FILE *f = _tfopen(str, _T("r"));
        //    if (f == NULL)
        //        return FALSE;

        //    TCHAR ch;
        //    int rows = 0;
        //    while (!feof(f)) {
        //        if ((ch = _gettc(f)) == '\n')
        //            rows++;
        //    }

        //    fseek(f, 0, SEEK_SET );
        //    //TCHAR id[80], name[80], type[80];
        //    //_ftscanf(f, _T("%s %s %s\r\n"), id, name, type);
        //    //_ftscanf(f, _T("%s\r\n"), title);
        //    //int price;
        //    while ((ch = _gettc(f)) != '\n'); /* skip the header */
        //    int i = 0;
        //    int m, d, y, v;
        //    float o, h, l, c, p;

        //    _ds = new DVS(rows - 1);
        //    // ds->Add(new DV<LSTR>(0, _T("Descriptions"), NULL, _T("%s")));
        //    _ds->Add(new DV<int>(0, _T("Date")));
        //    _ds->Add(new DV<float>(0, _T("Open"), NULL, _T("%.2f")));
        //    _ds->Add(new DV<float>(0, _T("Close"), NULL, _T("%.2f")));
        //    _ds->Add(new DV<float>(0, _T("High"), _T("High Price"), _T("%.2f")));
        //    _ds->Add(new DV<float>(0, _T("Low"), NULL, _T("%.2f")));
        //    _ds->Add(new DV<int>(0, _T("Volume")));
        //    _ds->Add(new DV<float>(0, _T("Amount"), NULL, _T("%.2f")));
        //    DV<float> &dvO = *(DV<float>*)&(*_ds)[1], 
        //        &dvC = *(DV<float>*)&(*_ds)[2], 
        //        &dvH = *(DV<float>*)&(*_ds)[3], 
        //        &dvL = *(DV<float>*)&(*_ds)[4], 
        //        &dvAmount = *(DV<float>*)&(*_ds)[6];
        //    DV<int> &dvDate = *(DV<int>*)&(*_ds)[0], 
        //        &dvVol = *(DV<int>*)&(*_ds)[5];

        //    while(!feof(f) && i < dvO.Size())
        //    {
        //        // price = 0.0;
        //        //y = 0;
        //        _ftscanf(f, _T("%d-%d-%d %f %f %f %f %d %f\r\n"), &y, &m, &d, &o, &h, &l, &c, &v, &p);
        //        //price /= 15;
        //        //l0[i] = price / 1;
        //        //fmin = min(fmin, l0[i]);
        //        //fmax = max(fmax, l0[i]);
        //        dvDate[i] = y << 16 | m << 8 | d;
        //        dvO[i] = o * 100;
        //        dvH[i] = h * 100;
        //        dvL[i] = l * 100;
        //        dvC[i] = c * 100;
        //        dvVol[i] = v;
        //        dvAmount[i] = p;
        //        i += 1;
        //    }
        //    fclose(f);

        //    //this->avKLine.scrolltonew();
        //}
        this->stockId = stockId;
        if ((_ds = LoadStockData(stockId)) != NULL) {
            //_ds->x0 = _ds->xn - 1000;
            //_ds->x0 = 10;
            //_ds->xn = 500;

            _ds->x0 = _ds->xn - _ds->Rows() / 2;
            //_ds->xn = 500;
        }
        return _ds ? TRUE : FALSE;
    }

    DVS* LoadStockData(LPCTSTR stockId)
    {

        //_dv = new DV<float>(882);
        //DV<float> &l0 = *(DV<float>*)_dv;
        //fmin = 400000; fmax = 0;
        LSTR str;
        str.Format(_T("data\\%s.%s"), stockId, _T("txt"));
        FILE *f = _tfopen(str, _T("r"));
        if (f == NULL)
            return NULL;

        TCHAR ch;
        int rows = 0;
        while (!feof(f)) {
            if ((ch = _gettc(f)) == '\n')
                rows++;
        }

        fseek(f, 0, SEEK_SET );
        while ((ch = _gettc(f)) != '\n'); /* skip the header */
        int i = 0;
        int m, d, y, v;
        float o, h, l, c;
        float p;

        DVS *ds = new DVS(rows - 1);
        // ds->Add(new DV<LSTR>(0, _T("Descriptions"), NULL, _T("%s")));
        ds->Add(new DV<int>(0, _T("Date")));
        ds->Add(new DV<float>(0, _T("Open"), NULL, _T("%.2f")));
        ds->Add(new DV<float>(0, _T("Close"), NULL, _T("%.2f")));
        ds->Add(new DV<float>(0, _T("High"), _T("High Price"), _T("%.2f")));
        ds->Add(new DV<float>(0, _T("Low"), NULL, _T("%.2f")));
        ds->Add(new DV<int>(0, _T("Volume")));
        ds->Add(new DV<double>(0, _T("Amount"), NULL, _T("%.2f")));
        DV<float> &dvO = *(DV<float>*)&(*ds)[1], 
            &dvC = *(DV<float>*)&(*ds)[2], 
            &dvH = *(DV<float>*)&(*ds)[3], 
            &dvL = *(DV<float>*)&(*ds)[4];
        DV<double> &dvAmount = *(DV<double>*)&(*ds)[6];
        DV<int> &dvDate = *(DV<int>*)&(*ds)[0], 
            &dvVol = *(DV<int>*)&(*ds)[5];

        while(!feof(f) && i < dvO.Size())
        {
            _ftscanf(f, _T("%d-%d-%d %f %f %f %f %d %e\r\n"), &y, &m, &d, &o, &h, &l, &c, &v, &p);
            dvDate[i] = y << 16 | m << 8 | d;
            dvO[i] = o * 100;
            dvH[i] = h * 100;
            dvL[i] = l * 100;
            dvC[i] = c * 100;
            dvVol[i] = v;
            dvAmount[i] = p / 1000.0;
            i += 1;
        }
        fclose(f);

        return ds;
    }

};

