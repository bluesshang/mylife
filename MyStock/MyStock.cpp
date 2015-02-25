// MyStock.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MyStock.h"
#include "ui\lwin.h"
#include "ui\LWndFrame.h"
#include "ui\LWndView.h"
#include "ui\lmenu.h"

#include "StockHome.h"
#include "FavoritedStock.h"

#include "db\db_ora.h"

// #define MAX_LOADSTRING 100

//// Global Variables:
//HINSTANCE hInst;								// current instance
//TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
//TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
//
//// Forward declarations of functions included in this code module:
//ATOM				MyRegisterClass(HINSTANCE hInstance);
//BOOL				InitInstance(HINSTANCE, int);
//LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

class LDlgViewAbout : public LDialogImpl<LDlgViewAbout>
{
public:
    enum {IDD = IDD_ABOUTBOX};

    LRESULT OnCommand(WORD wNotifyCode, WORD wId, HWND hWndCtrl)
    {
        LSTR str;
        if (IDOK == wId)
        {
           (VOID)EndDialog(wId);
        }
        return 0;
    }

};

class MyStockApp : public LWndFrame<MyStockApp>
{
    StockHome *m_stkHome, stkHome2, stkHome3, stkHomeSH, stkHome4, stkHome5, stkHome6;
    FavoritedStock *m_stkFav;
    LNativeMenu *m_menuMain;
    LWndDrawView *m_viewTest0;
    LWndTodo wndTodo;

    LDB *_db;
public:
    MyStockApp() // : LWndFrame<MyStockApp>(_T("MyStockApp"))
    {
        //uFrameBdrWidth = 6;

        //float H[] = {40.0, 1.0, 30};
        //float W[] = {1};
        //cell.Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);

        //float H0[] = {1};
        //float W0[] = {0.8, 0.2};
        //cell.cells->cell(1, 0).Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);

        //float H1[] = {0.6, 0.3, 0.1};
        //float W1[] = {1};
        //cell.cells->cell(1, 0).cells->cell(0, 0).Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);

        //LWndDrawView *wnd0 = new LWndDrawView;
        //LRect rc;
        //LWndCell &cell0 = cell.cells->cell(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        ////GetCellClientRect(cell0, rc);
        //wnd0->Create(*this, WS_EX_WINDOWEDGE, WS_VISIBLE | WS_BORDER | WS_CHILD, _T("LWndDrawView1"), rc, 1027, 0);
        //cell0.AddWnd(wnd0);
        m_stkHome = NULL;
        m_stkFav = NULL;
        m_menuMain = NULL;
        m_viewTest0 = NULL;
        //{
        //    struct trk t;
        //    AAA a0;
        //    t.a = &a0;
        //    t.fn0 = &AAA::foo;
        //    void(AAA::*fn1)(VOID)= &AAA::foo;
        //       (a0.*fn1)();
        //   (a0.*(t.fn0))();
        //    //(t.a->*fn0)();
        //   (t.a->*(t.fn0))();
        //}
        //_clrBkgrnd = RGB(255, 255, 255);
    }

    ~MyStockApp()
    {
        delete m_stkHome;
        delete m_stkFav;
        delete m_menuMain;
        delete m_viewTest0;
    }

    LRESULT OnCommand(WORD wNotifyCode, WORD wId, HWND hWndCtrl)
    {
        LSTR str;
        if (IDM_EXIT == wId)
        {
            return OnDestroy();
        }
        if (ID_HELP_ABOUT == wId)
        {
            LDlgViewAbout dlg;
            return dlg.DoModal(*this);
        }
        if (IDOK != wId)
        {
            str.Format(_T("OnCommand %u"), wId);
            MessageBox(str);
        }
        return 0;
    }

    LRESULT OnCreate(LPCREATESTRUCT lpcs)
    {
        //LWndCell *pc;
        //LCell<LFrameCell> *pc;
        LFrameCell *pc;
        SetFrameBorderWidth(5);
        //SetFrameBorderColor(RGB(128, 128, 128));
        //bKeepBoundary = 1;
        // return 0;
        float H[] = {40.0f, 1.0f, 30.0f}; /* rows */
        float W[] = {0.5};
        LRect rc(0, 0, 100, 200);

        // cells(MKCELLPOS(0,0),CELLPOS_END).Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
        cell.Split(sizeof(H)/sizeof(H[0]), sizeof(W)/sizeof(W[0]), W, H);
        //return 0;

        //pc = cell.cell(MKCELLPOS(0,0),CELLPOS_END);
        pc = cell.cell(0, 0);
        m_menuMain = new LNativeMenu;
        //m_menuMain->SetBackgroundColor(RGB(209, 230, 248));
        m_menuMain->CreateChild(this, _T("MainMenu"), rc, 1027, 0);
        m_menuMain->LoadMenu(IDC_MYSTOCK);
        pc->AddWnd(m_menuMain);
        //return 0;

        // pc = cell.cell(MKCELLPOS(1,0),CELLPOS_END);
        pc = cell.cell(1, 0);
        pc->CreateTabWnd(this, LTS_TOP);

        // wndTodo = new LWndTodo;
        wndTodo.SetBackgroundColor(RGB(130, 121, 79));
        wndTodo.CreateChild(this, _T("TODOs"), rc, 1029, 0, LVS_NO_STRETCH_CANVAS);
        pc->AddWnd(&wndTodo);

        m_viewTest0 = new LWndDrawView;
        m_viewTest0->SetBackgroundColor(RGB(209, 230, 248));
        m_viewTest0->CreateChild(this, _T("View:Test:A long window tab name"), rc, 1027, 0, 0, /*WS_BORDER | */WS_VISIBLE | WS_CHILD);
        pc->AddWnd(m_viewTest0, NULL, RGB(0, 255, 0));
        pc->AddWnd(m_viewTest0);

        m_stkFav = new FavoritedStock;
        m_stkFav->CreateChild(this, _T("Frame:FavoritedStock"), rc, 1027, 0, LFS_OVERLAP_FRAME_BORDER);
        pc->AddWnd(m_stkFav, NULL);

        //m_stkHome = new StockHome;
        stkHomeSH.LoadData(_T("SZZZ"));
        stkHomeSH.CreateChild(this, _T("Frame:Home(SH)"), rc, 1027, 0, LFS_OVERLAP_FRAME_BORDER);
        pc->AddWnd(&stkHomeSH, NULL);

        m_stkHome = new StockHome;
        m_stkHome->LoadData(_T("HBGT"));
        m_stkHome->CreateChild(this, _T("Frame:Home(QCDL)"), rc, 1027, 0, LFS_OVERLAP_FRAME_BORDER);
        pc->AddWnd(m_stkHome, NULL);

        stkHome2.LoadData(_T("SQZY"));
        stkHome2.CreateChild(this, _T("Frame:Home(SQZY)"), rc, 1027, 0, LFS_OVERLAP_FRAME_BORDER);
        pc->AddWnd(&stkHome2, NULL);

        stkHome3.LoadData(_T("ZHDC"));
        stkHome3.SetFrameBorderColor(RGB(0, 0, 0));
        // stkHome3.SetFrameBorderColor(RGB(182, 133, 195));
        stkHome3.SetFrameBorderWidth(1);
        // stkHome3.CreateChild(this, _T("Frame:Home(SZ000043)"), rc, 1027, 0, LFS_OVERLAP_FRAME_BORDER);
        stkHome3.CreateChild(this, _T("Frame:Home(SZ000043)"), rc, 1027, 0);
        pc->AddWnd(&stkHome3, NULL, RGB(255, 255, 255));

        stkHome4.LoadData(_T("HYTX"));
        stkHome4.SetFrameBorderColor(RGB(0, 0, 0));
        stkHome4.SetFrameBorderWidth(1);
        stkHome4.CreateChild(this, _T("HYTX"), rc, 1027, 0);
        pc->AddWnd(&stkHome4, NULL, RGB(255, 255, 255));

        stkHome5.LoadData(_T("SYZG"));
        stkHome5.SetFrameBorderColor(RGB(0, 0, 0));
        stkHome5.SetFrameBorderWidth(1);
        stkHome5.CreateChild(this, _T("Sanyi"), rc, 1027, 0);
        pc->AddWnd(&stkHome5, NULL, RGB(255, 255, 255));

        pc->SetActiveTabWnd(4);

        _db = new OraDB(_T("Provider=OraOLEDB.Oracle.1;Password=3189271;User ID=SYSTEM;Data Source=MYSTOCK;Persist Security Info=True"));
        DVS *dvs = _db->Fetch(_T("select * from stock_info where code like '%66'"));
        delete dvs;
#if 0
        //return 0;
        float H0[] = {1};
        float W0[] = {0.8, 0.2};
        // cell.cells->cell(1, 0).Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);
        pc = &cell.CellAt(MKCELLPOS(1,0),CELLPOS_END);
        pc->Split(sizeof(H0)/sizeof(H0[0]), sizeof(W0)/sizeof(W0[0]), W0, H0);

        float H1[] = {0.6, 0.3, 0.1};
        float W1[] = {1};
        // cell.cells->cell(1, 0).cells->cell(0, 0).Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);
        pc = &cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0, 0),CELLPOS_END);
        pc->Split(sizeof(H1)/sizeof(H1[0]), sizeof(W1)/sizeof(W1[0]), W1, H1);

        LWndDrawView *wnd0 = new LWndDrawView;
        LRect rc(0, 0, 100, 200);
        // cells(1,0)->cells(0,0)->cells(0,0); cells({1,0},{0,0},{0,0})
        // LWndCell &cell0 = cell.cells->cell(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        // LWndCell &cell0 = cells(1, 0).cells->cell(0, 0).cells->cell(0, 0);
        // LWndCell &cell0 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,0), MKCELLPOS(0, 0), CELLPOS_END);
        LWndCell &cell0 = cell.CellAt(0x10000, 0, 0, CELLPOS_END);
        //GetCellClientRect(cell0, rc);
        wnd0->Create(*this, WS_EX_WINDOWEDGE, WS_VISIBLE | /*WS_BORDER |*/ WS_CHILD, _T("LWndDrawView ÊÓÍ¼"), rc, 1027, 0);
        cell0.AddWnd(wnd0);

        wnd0 = new LWndDrawView;
        wnd0->Create(*this, WS_EX_WINDOWEDGE, /*WS_VISIBLE |*/ WS_BORDER | WS_CHILD, _T("K lines"), rc, 1028, 0);
        cell0.AddWnd(wnd0);

        // LWndCell &cell1 = cell.CellAt(MKCELLPOS(1,0), MKCELLPOS(0,1), CELLPOS_END);
        LWndCell &cell1 = cell.CellAt(0x10000, 1, CELLPOS_END);
        wnd0 = new LWndDrawView;
        wnd0->Create(*this, WS_EX_WINDOWEDGE, WS_VISIBLE | WS_BORDER | WS_CHILD, _T("Realtime trade informations"), rc, 1029, 0);
        cell1.AddWnd(wnd0);
#endif
        return 0;
    }

};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetBreakAlloc(1);

    HRESULT hr = CoInitialize(NULL);
    LWIN_ASSERT(hr == S_OK);
    //__asm int 3;
    //int *p = new int[20];
    //int *t = (int*)malloc(40);
    MyStockApp app;
    //app.CreateOverlappedWindow(_T("MyStock"));
    RECT rc;// = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    app.Create(NULL, 0, /*WS_CLIPCHILDREN|WS_CLIPSIBLINGS|*/WS_POPUP | WS_VISIBLE /*| WS_MAXIMIZE*/, _T("My Stock"), rc);

    //LRect rc;
    //app.GetClientRect(rc);
    //rc.InflateRect(50, 50);
    //HRGN rgn = rc.CreateRoundRectRgn(50, 50);
    //app.SetWindowRgn(rgn);
    LRESULT ret = app.Run();
    CoUninitialize();
    return ret;
}



