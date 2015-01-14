
/*
* Thunk-based and member function-based Message Handler Wrap Utility
* By JERKII.SHANG(JERKII@HOTMAIL.COM)
* Compiled successfully under VS 2003 and VS 2005.
* MAR. 10ST, 2006
*/

#ifndef __LWIN_H__
#define __LWIN_H__

//#pragma message("cl version is " _MSC_VER)

#pragma warning(disable: 4996 4311 4312)

#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>
#include <stdio.h> /* sprintf */
#include <typeinfo.h> /* typeid(...).name() */
#include <tchar.h>

#include "..\utils\lstr.h"
#include "..\utils\lrect.h"
#include "..\utils\larray.h"

#include "lgdi.h"

#define _RECT_W(rc)         ((rc).right -(rc).left)
#define _RECT_H(rc)         ((rc).bottom -(rc).top)
#ifndef _T
#define _T TEXT
#endif

#define MEMZERO_THIS() memset(this, 0, sizeof(*this))

#ifndef __ASSERT
#define __ASSERT _ASSERT
#endif

#define WM_LWIN_USER        WM_APP
/* wParam: the LWIN_MOUSESYNC_INFO */
#define WM_LWIN_MOUSESYNC   (WM_LWIN_USER + 100)
///* wParam: not used, lParam: the new actived layer(Layer*) */
//#define WM_LWIN_ACTIVE_LAYER_CHANGED (WM_LWIN_USER + 101)

typedef enum {
    MOUSESYNC_MSG_LVIEW_DRAGDROP,
    MOUSESYNC_MSG_LVIEW_ZOOM,
    MOUSESYNC_MSG_LVIEW_TRACKING
} MOUSESYNC_MSG;
class LWnd;
typedef struct LWIN_MOUSESYNC_INFO_t
{
    LWnd *src, *dst;
    MOUSESYNC_MSG msgId;
    VOID *para; /* depend on the msgId */
} LWIN_MOUSESYNC_INFO;

#define __DO_DEFAULT        (LRESULT) -2

#define _USE_THISCALL_CALLINGCONVENTION

#ifdef _USE_THISCALL_CALLINGCONVENTION
#define THUNK_CODE_LENGTH      10 /* For __thiscall calling convention ONLY */
#if _MSC_VER >= 1400 
#define LCALLBACK __thiscall
#else
#define LCALLBACK
#endif
#else
#define THUNK_CODE_LENGTH      16 /* For __stdcall or __cdecl calling convention ONLY */
#define LCALLBACK __stdcall
#endif

typedef struct STD_THUNKED_DATA_t
{
    // unsigned char pThunkCode[THUNK_CODE_LENGTH];
    unsigned char *pThunkCode;

    STD_THUNKED_DATA_t()
    {
        /* PAGE_EXECUTE_READWRITE: for support DEP checking */
        pThunkCode = (unsigned char *) VirtualAlloc(NULL, THUNK_CODE_LENGTH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    }
    ~STD_THUNKED_DATA_t()
    {
        VirtualFree(pThunkCode, 0, MEM_RELEASE);
    }
    /* for __stdcall or __cdecl calling convention ONLY */
    void Init(DWORD_PTR pThis, DWORD_PTR dwProcPtr)
    {
        /* 
           THIS IS CORE IMPLEMENTATION TO SUPPORT MEMBER-FUNCTION BASED MESSAGE HANDLER

           March 19th, 2006
        */

        DWORD dwDistance = (DWORD) dwProcPtr -(DWORD) &pThunkCode[0] - THUNK_CODE_LENGTH;

#ifdef _USE_THISCALL_CALLINGCONVENTION
        /*
          For __thiscall, the default calling convention used by Microsoft VC++, The thing needed is
          just set ECX with the value of 'this pointer'

         (ONLY 10 byes machine code needed in this case)

          Encoded machine instruction   Equivalent assembly languate notation
          ---------------------------   -------------------------------------
          B9 ?? ?? ?? ??                mov    ecx, pThis ; Load ecx with this pointer
          E9 ?? ?? ?? ??                jmp    dwProcPtr  ; Jump to target message handler
        */
        pThunkCode[0] = 0xB9;
        pThunkCode[5] = 0xE9;

        *((DWORD *) &pThunkCode[1]) = (DWORD) pThis;
        *((DWORD *) &pThunkCode[6]) = dwDistance;
#else
        /* 
           For __stdcall or __cdecl calling convention, insert 'this pointer' into stack
           between parameter HWND and 'Return Addr'.

           Stack frame before inserting 			Stack frame after inserting

           :      ...      :            			:       ...      :
           |---------------|            			|----------------|
           |     LPARAM    |            			|     LPARAM     |
           |---------------|            			|----------------|
           |     WPARAM    |            			|     WPARAM     |
           |---------------|            			|----------------|
           |   UINT(msg)  |            			|   UINT(msg)   |
           |---------------|            			|----------------|
           |      HWND     |            			|      HWND      |
           |---------------|            			|----------------|
           |(Return Addr) | <- ESP     			| <this pointer> | <- New item inserted by this thunk code
           |---------------|            			|----------------|
           :      ...      :            			|(Return Addr)  | <- ESP
                                        			|----------------|
                                        			:       ...      :

         (16 bytes machine code needed in this case)

          Encoded machine instruction   Equivalent assembly languate notation
          ---------------------------   -------------------------------------
          FF 34 24                      push  dword ptr [esp]          ; Save(or duplicate) the Return Addr into stack
          C7 44 24 04 ?? ?? ?? ??       mov   dword ptr [esp+4], pThis ; Overwite the old Return Addr with 'this pointer'
          E9 ?? ?? ?? ??                jmp   dwProcPtr                ; Jump to target message handler
        */

        pThunkCode[11] = 0xE9; 

        *((DWORD *) &pThunkCode[ 0]) = 0x002434FF;
        *((DWORD *) &pThunkCode[ 3]) = 0x042444C7;
        *((DWORD *) &pThunkCode[ 7]) = (DWORD) pThis;
        *((DWORD *) &pThunkCode[12]) = dwDistance;
#endif
    }

    inline void* GetThunkedCodePtr() { return &pThunkCode[0]; }

} STD_THUNKED_DATA;

typedef struct THUNKED_SUBCLASS_DATA_t
{   
    STD_THUNKED_DATA_t _thunk;
    union {
        WNDPROC fnOldWndProc;
        DLGPROC fnOldDlgProc;
    } oldproc;

    THUNKED_SUBCLASS_DATA_t(DWORD_PTR pThis, DWORD_PTR dwProcPtr)
    {
        _thunk.Init(pThis, dwProcPtr);
    }

} THUNKED_SUBCLASS_DATA;

#define LWS_FRAME_WND 1

#define INVALID_COLOR 0xFFFFFFFF

class /*__declspec(novtable)*/ LWnd;

class LSyncMsgObj
{
    LArray<LWnd*> _wnds;
public:
    void Add(LWnd *wnd);
    VOID SyncMessage(LWnd *from, UINT msg, WPARAM wPara, LPARAM lPara);
    //{
    //    _wnds.Add(wnd);
    //    //wnd->SetMsgSync(this);
    //}
};

class /*__declspec(novtable)*/ LWnd
{
    //STD_THUNKED_DATA _thunk;
//private:
//    UINT _ctrlStyles;
    LSyncMsgObj *_syncMsg;
//protected:
//    inline DWORD_PTR GetThunkedProcPtr() { return(DWORD_PTR) _thunk.GetThunkedCodePtr(); }
protected:
    //BOOL _bIsFrameWnd;
    UINT _uLWndStyle; /* common styles of all derived controls */
    UINT _uCtrlStyle; /* private styles of each derived control */
    COLORREF _clrBkgrnd; /* the background color */

    LWnd *_parent;
    // LArray<LWnd*> _syncs;

public:
    LArray<LWnd*> childs;
public:
    HWND m_hWnd;
    HINSTANCE m_hInstance;

    LWnd() : m_hWnd(NULL), m_hInstance(GetModuleHandle(NULL)), 
        _uLWndStyle(0), _uCtrlStyle(0), _clrBkgrnd(GetSysColor(COLOR_MENUBAR)) {}
    ~LWnd()
    {
        m_hWnd = NULL;
        _syncMsg = NULL;
    }

    VOID SetMsgSync(LSyncMsgObj *syncMsg) 
    {
        _syncMsg = syncMsg; 
    }
    VOID AddMsgSync(LWnd *wnd)
    {
        if (_syncMsg != NULL)
            _syncMsg->Add(wnd);
    }
    VOID SyncMessage(MOUSESYNC_MSG msg, VOID *para, LWnd *dst = NULL)
    {
        if (_syncMsg == NULL)
            return;

        LWIN_MOUSESYNC_INFO msi;
        msi.src = this;
        msi.dst = dst;
        msi.msgId = msg;
        msi.para = para;

        if (dst != NULL) {
            (VOID)::SendMessage(*dst, WM_LWIN_MOUSESYNC, (WPARAM)&msi, NULL);
            return;
        }
        _syncMsg->SyncMessage(this, WM_LWIN_MOUSESYNC, (WPARAM)&msi, NULL);
    }

    UINT32 GetWndStyle() { return _uLWndStyle; }
    UINT32 GetCtrlStyle() { return _uCtrlStyle; }
    VOID SetBackgroundColor(COLORREF uBgColor, COLORREF *pOldBgColor = NULL) {
        if (NULL != pOldBgColor)
            *pOldBgColor = _clrBkgrnd;
        _clrBkgrnd = uBgColor;
    }
    inline operator HWND() {return m_hWnd; }
    //BOOL IsFrameWnd() { return _bIsFrameWnd; }

    /* Window Helper function */
    void GetWindowRect(RECT& rc) {
        ::GetWindowRect(m_hWnd, &rc); 
    }
    void GetClientRect(RECT& rc) {
        ::GetClientRect(m_hWnd, &rc); 
    }
    void GetChildRect(LWnd *child, LRect& rc) {
        child->GetWindowRect(rc);
        ScreenToClient(rc.lt);
        ScreenToClient(rc.rb);
    }
    VOID ClipChildRegions(LDC& dc)
    {
        LRect rc;
        for (int i = 0; i < childs.Count(); i++) {
            //if (!childs[i]->IsWindowVisible())
            //    continue;
            GetChildRect(childs[i], rc);
            dc.ExcludeClipRect(rc);
        }
    }
    HDC GetDC() { return ::GetDC(m_hWnd); }
    int ReleaseDC(HDC hdc) { return ::ReleaseDC(m_hWnd, hdc); }

    LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam) {
        return ::SendMessage(m_hWnd, Msg, wParam, lParam);
    }
    BOOL DestroyWindow() {
        return ::DestroyWindow(m_hWnd); 
    }
    BOOL MoveWindow(int X, int Y, int nWidth, int nHeight, BOOL bRepaint) {
        return ::MoveWindow(m_hWnd, X, Y, nWidth, nHeight, bRepaint); 
    }
    BOOL MoveWindow(RECT& rc, BOOL bRepaint = TRUE) {
        return ::MoveWindow(m_hWnd, rc.left, rc.top, _RECT_W(rc), _RECT_H(rc), bRepaint); 
    }
    int GetWindowText(LSTR& str) {
        LSTR _str;
       (VOID)::GetWindowText(m_hWnd,(LPTSTR)(LPCTSTR)_str, _str.BufSize()); 
        str = (LPCTSTR)_str;
        return str.Len();
    }
    LWnd* GetParent() {
        return _parent;
        // return ::GetParent(m_hWnd);
    }
    HWND GetParentHwnd() {
        return ::GetParent(m_hWnd);
    }
    VOID AddChild(LWnd *child) {
        childs.Add(child);
    }
    BOOL ShowWindow(int nCmdShow) {
        return ::ShowWindow(m_hWnd, nCmdShow);
    }
    BOOL InvalidateRect(RECT *lpRect, BOOL bErase = TRUE, BOOL redrawNow = FALSE) {
        if (redrawNow == FALSE)
            return ::InvalidateRect(m_hWnd, lpRect, bErase);

        return RedrawWindow(m_hWnd, NULL, NULL, RDW_NOINTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW
            | (bErase == TRUE ? RDW_ERASE : 0));
    }
    BOOL ClientToScreen(LPoint& pt) {
        return ::ClientToScreen(m_hWnd, &pt);
    }
    BOOL ScreenToClient(LPoint& pt) {
        return ::ScreenToClient(m_hWnd, &pt);
    }
    int MapWindowPoints(LWnd *to, LPoint *points, int num) {
        return ::MapWindowPoints(m_hWnd, *to, points, num);
    }
    HWND SetCapture() {
        return ::SetCapture(m_hWnd);
    }
    BOOL IsWindowVisible() {
        return ::IsWindowVisible(m_hWnd);
    }
    int MessageBox(LPCTSTR lpText, LPCTSTR lpCaption = _T("MessageBox"), UINT uType = MB_OK | MB_ICONINFORMATION) {
        return ::MessageBox(m_hWnd, lpText, lpCaption, uType); 
    }
    HWND SetFocus() {
        return ::SetFocus(m_hWnd);
    }
    int SetWindowRgn(HRGN hRgn, BOOL bRedraw = TRUE) {
        return ::SetWindowRgn(m_hWnd, hRgn, bRedraw);
    }

    virtual VOID Layout() {}
    virtual VOID GetWndTabColor(COLORREF& clrSelected)
    {
        clrSelected = INVALID_COLOR == _clrBkgrnd ? RGB(0, 0, 0) : _clrBkgrnd;
    }
    virtual UINT GetFixedLWndStyle() { return 0; }
};

void LSyncMsgObj::Add(LWnd *wnd)
{
    _wnds.Add(wnd);
    wnd->SetMsgSync(this);
}

VOID LSyncMsgObj::SyncMessage(LWnd *from, UINT msg, WPARAM wPara, LPARAM lPara)
{
    // for (int i = 0; i < _wnds.Count(); i++)
    for (int i = _wnds.Count() - 1; i >= 0; i--)
    {
        //if (i == 1)
        //    continue;
        LWnd *wnd = _wnds[i];
        if (from != wnd) {
            // (VOID)::PostMessage(*wnd, msg, wPara, lPara);
            (VOID)::SendMessage(*wnd, msg, wPara, lPara);
        }
    }
}

//#define __DO_DEFAULT return static_cast<T *>(this)->DoDefault(msg, wParam, lParam);
template <class T>
class /*__declspec(novtable)*/ LWINDOW_ROOT_IMPL : public LWnd
{
    STD_THUNKED_DATA _thunk;

protected:
    inline DWORD_PTR GetThunkedProcPtr() { return(DWORD_PTR) _thunk.GetThunkedCodePtr(); }

public:

    ///*
    // * there is must be at least one virtual function decleared here
    // * to support overriding in descendants through "T* pThis = static_cast<T *>(this)" */
    //virtual void __place_holder__() {/* do nothing */}

    typedef LRESULT(LCALLBACK T::*WNDPROC_PTR)(HWND, UINT, WPARAM, LPARAM);
    typedef INT_PTR(LCALLBACK T::*DLGPROC_PTR)(HWND, UINT, WPARAM, LPARAM);

    LWINDOW_ROOT_IMPL() // : m_hWnd(NULL), m_hInstance(GetModuleHandle(NULL)) 
    {
        T* pThis = static_cast<T *>(this);
        _thunk.Init((DWORD_PTR) pThis, pThis->GetMessageProcPtr());
    }

    //inline operator HWND() {return m_hWnd; }

    /* Window Helper function */
    //void GetWindowRect(RECT& rc) { ::GetWindowRect(m_hWnd, &rc); }
    //void GetClientRect(RECT& rc) { ::GetClientRect(m_hWnd, &rc); }
    //BOOL DestroyWindow() { return ::DestroyWindow(m_hWnd); }
    //int MessageBox(LPCTSTR lpText, LPCTSTR lpCaption = _T("MessageBox"), UINT uType = MB_OK | MB_ICONINFORMATION) { return ::MessageBox(m_hWnd, lpText, lpCaption, uType); }
    // More helper function can be declared here ...

    /* Common message handler fro window and dialog box */
    LRESULT ProcessBaseMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        T* pThis = static_cast<T *>(this);
        LRESULT r = __DO_DEFAULT;

        LPoint pt;
        pt.x = (SHORT)LOWORD(lParam);
        pt.y = (SHORT)HIWORD(lParam);

        switch (msg)
        {
        case WM_COMMAND:
            r = pThis->OnCommand(HIWORD(wParam), LOWORD(wParam),(HWND) lParam);
            break;

        case WM_NOTIFY:
            r = pThis->OnNotify((int) wParam,(LPNMHDR) lParam);
            break;

        case WM_NCCREATE:
            r = pThis->OnNcCreate((LPCREATESTRUCT) lParam);
            break;

        case WM_CLOSE:
            r = pThis->OnClose();
            break;

        case WM_DESTROY :
            r = pThis->OnDestroy();
            break;

        case WM_CREATE:
            r = pThis->OnCreate((LPCREATESTRUCT) lParam);
            break;

        case WM_DRAWITEM:
            r = pThis->OnDrawItem((int) wParam,(LPDRAWITEMSTRUCT) lParam);
            break;

        case WM_MEASUREITEM :
            r = pThis->OnMeasureItem((int) wParam,(LPMEASUREITEMSTRUCT) lParam);
            break;

        case WM_WINDOWPOSCHANGING :
            r = pThis->OnWindowPosChanging((LPWINDOWPOS) lParam);
            break;

        case WM_MOUSEMOVE:
            r = pThis->OnMouseMove(wParam, pt);
            break;

        case WM_LBUTTONDOWN:
            r = pThis->OnLButtonDown(wParam, pt);
            break;

        case WM_RBUTTONDOWN:
            r = pThis->OnRButtonDown(wParam, pt);
            break;

        case WM_LBUTTONUP:
            r = pThis->OnLButtonUp(wParam, pt);
            break;

        case WM_RBUTTONUP:
            r = pThis->OnRButtonUp(wParam, pt);
            break;

        case WM_MBUTTONDOWN:
            r = pThis->OnMButtonDown(wParam, pt);
            break;

        case WM_MOUSEWHEEL:
            /* 'pt' is in screen coordinate */
            r = pThis->OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam), pt);
            break;

        case WM_LBUTTONDBLCLK:
            r = pThis->OnLButtonDblClk(wParam, pt);
            break;

        case WM_SIZE:
            r = pThis->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
            break;
        
        case WM_SETFOCUS:
            r = pThis->OnSetFocus((HWND)wParam);
            break;

        case WM_KILLFOCUS:
            r = pThis->OnKillFocus((HWND)wParam);
            break;

        case WM_ERASEBKGND:
            r = pThis->OnEraseBkgnd(LDC((HDC)wParam));
            break;

        case WM_LWIN_MOUSESYNC:
            r = pThis->OnMouseSync((LWIN_MOUSESYNC_INFO *)wParam);
            break;

        case WM_TIMER:
            r = pThis->OnTimer(wParam);
            break;

        default: break;
        }

        return r == __DO_DEFAULT ? DoDefault(msg, wParam, lParam) : r;
    }

    LRESULT DoDefault(UINT msg, WPARAM wParam, LPARAM lParam) { return static_cast<T *>(this)->DoDefault(msg, wParam, lParam); }

    /* Default message handler for window and dialog box */
    LRESULT OnCommand     (WORD wNotifyCode, WORD wId, HWND hWndCtrl) { return __DO_DEFAULT; }
    LRESULT OnNotify      (int idCtrl, LPNMHDR pnmh) { return __DO_DEFAULT; }

    LRESULT OnEraseBkgnd  (LDC& dc) {
        //return __DO_DEFAULT;
        if (INVALID_COLOR == _clrBkgrnd)
            return __DO_DEFAULT;
        LRect rc;
        LBrush br(dc);
        br.CreateSolidBrush(_clrBkgrnd);
        // br.CreateSolidBrush(RGB(255, 0, 0));
        GetClientRect(rc);
        ClipChildRegions(dc);
        dc.FillRect(rc, br);
        dc.SelectClipRgn(NULL);
        return 1; /* the background has been erased, no further erasing is required */
    }

    LRESULT OnCreate      (LPCREATESTRUCT lpcs) { return __DO_DEFAULT; }
    LRESULT OnNcCreate    (LPCREATESTRUCT lpcs) { return __DO_DEFAULT; }
    LRESULT OnClose       () { return __DO_DEFAULT; }
    LRESULT OnDestroy     () { return __DO_DEFAULT; }
    LRESULT OnDrawItem    (int idCtrl, LPDRAWITEMSTRUCT lpdis) { return __DO_DEFAULT; }
    LRESULT OnMeasureItem (int idCtrl, LPMEASUREITEMSTRUCT lpmis) { return __DO_DEFAULT; }
    LRESULT OnWindowPosChanging(LPWINDOWPOS lppos) { return __DO_DEFAULT; }
    LRESULT OnSize        (UINT nType, INT cx, INT cy) { return __DO_DEFAULT; }
    LRESULT OnSetFocus    (HWND hWndLostFocus) {return __DO_DEFAULT; }
    LRESULT OnKillFocus   (HWND hWndGetFocus) {return __DO_DEFAULT; }

    /* mouse events */
    LRESULT OnLButtonUp  (DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnRButtonUp  (DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnRButtonDown(DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnMButtonDown(DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnMouseMove  (DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnMouseWheel (UINT nFlags, SHORT zDelta, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnLButtonDblClk (DWORD dwFlags, LPoint& pt) {return __DO_DEFAULT;}
    LRESULT OnMouseSync  (LWIN_MOUSESYNC_INFO *msi) { return __DO_DEFAULT; }
    LRESULT OnTimer      (UINT tmrId) { return __DO_DEFAULT; }

    /* Subclass helper function */
    THUNKED_SUBCLASS_DATA* SubclassWindowHelper(HWND hWnd, WNDPROC_PTR pfnHook)
    {
        DWORD dwProcPtr = 0;
        __asm
        {
            mov eax, pfnHook
            mov dword ptr [dwProcPtr], eax
        }

        THUNKED_SUBCLASS_DATA* tsd = new THUNKED_SUBCLASS_DATA(
           (DWORD_PTR) static_cast<T *>(this), 
            dwProcPtr);
        tsd->oldproc.fnOldWndProc = (WNDPROC) SetWindowLongPtr(hWnd, GWL_WNDPROC,(LONG) tsd->_thunk.GetThunkedCodePtr());
    
        return tsd;
    }

    THUNKED_SUBCLASS_DATA* SubclassDialogHelper(HWND hDlg, DLGPROC_PTR pfnHook)
    {
        DWORD dwProcPtr = 0;
        __asm
        {
            mov eax, pfnHook
            mov dword ptr [dwProcPtr], eax
        }

        THUNKED_SUBCLASS_DATA* tsd = new THUNKED_SUBCLASS_DATA(
           (DWORD_PTR) static_cast<T *>(this), 
            dwProcPtr);
        tsd->oldproc.fnOldWndProc = (WNDPROC) SetWindowLongPtr(hWnd, DWLP_DLGPROC,(LONG) tsd->_thunk.GetThunkedCodePtr());
    
        return tsd;
    }

};

template <class T>
class LDialogImpl // LDialogImpl
    : public LWINDOW_ROOT_IMPL <T>
{
public:
    LDialogImpl() : LWINDOW_ROOT_IMPL<T>() {}
    inline DWORD GetMessageProcPtr() 
    {
        DWORD dwProcAddr = 0;
        __asm
        {   
            mov eax, offset T::LDialogProc /* Use the prefix 'T::' to enable the overload of LDialogProc in derived class */
            mov dword ptr [dwProcAddr], eax
        }
        return dwProcAddr;
    }


    INT_PTR OnInitDialog(HWND hWndFocus, LPARAM lParam) { return __DO_DEFAULT; }
    INT_PTR OnGetDlgCode(LPMSG lpMsg) { return __DO_DEFAULT; }

    LRESULT DoDefault(UINT msg, WPARAM wParam, LPARAM lParam) { return 0; }
    INT_PTR LCALLBACK LDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (m_hWnd == NULL)
        {   m_hWnd = hWnd;
        }

        __ASSERT(m_hWnd == hWnd);

        //char text[256];
        //sprintf(text, "In LDialogProc [%s], msg is %04X ...\n", typeid(&*this).name(), msg);
        //OutputDebugStringA(text);

        T* pThis = static_cast<T *>(this);
        INT_PTR fReturn = 0;

        switch (msg)
        {
        case WM_INITDIALOG:
            fReturn = pThis->OnInitDialog((HWND) wParam, lParam);
            break;

        case WM_GETDLGCODE:
            fReturn = pThis->OnGetDlgCode((LPMSG) lParam);
            break;

        default:
            fReturn = __super::ProcessBaseMessage(msg, wParam, lParam);
        }

        if (fReturn == __DO_DEFAULT) return DoDefault(msg, wParam, lParam);

        if (fReturn)
        {
            switch (msg)
            {
            case WM_COMPAREITEM         :
            case WM_VKEYTOITEM          :
            case WM_CHARTOITEM          :
            case WM_INITDIALOG          :
            case WM_QUERYDRAGICON       :
            case WM_CTLCOLORMSGBOX      :
            case WM_CTLCOLOREDIT        :
            case WM_CTLCOLORLISTBOX     :
            case WM_CTLCOLORBTN         :
            case WM_CTLCOLORDLG         :
            case WM_CTLCOLORSCROLLBAR   :
            case WM_CTLCOLORSTATIC      :
	            break;

            default:
	            ::SetWindowLongPtr(pThis->m_hWnd, DWLP_MSGRESULT,(LONG) fReturn);
	            break;
            }
        }

        return fReturn;
    }

    INT_PTR DoModal(
        HWND hWndParent = ::GetActiveWindow( ),
        LPARAM lpInitParam = NULL)
    {
        return DialogBoxParam(
            m_hInstance, MAKEINTRESOURCE(T::IDD), 
            hWndParent, 
           (DLGPROC) GetThunkedProcPtr(), lpInitParam);
    }

    HWND Create(
        HWND hWndParent,
        LPARAM lpInitParam = NULL)
    {
        //__ASSERT(m_hWnd == NULL);

        return CreateDialogParam(
            m_hInstance, MAKEINTRESOURCE(T::IDD), 
            hWndParent, 
           (DLGPROC) GetThunkedProcPtr(), lpInitParam);
    }

    VOID GetWndTabColor(COLORREF& clrSelected)
    {
        clrSelected = INVALID_COLOR == _clrBkgrnd ?
            GetSysColor(COLOR_3DFACE) : _clrBkgrnd;
    }

    inline BOOL SetDlgItemInt      (int nIDDlgItem, UINT uValue, BOOL bSigned = TRUE) { return ::SetDlgItemInt(m_hWnd, nIDDlgItem, uValue, bSigned); }
    inline UINT GetDlgItemInt      (int nIDDlgItem, BOOL *lpTranslated = NULL, BOOL bSigned = TRUE) { return ::GetDlgItemInt(m_hWnd, nIDDlgItem, lpTranslated, bSigned); }
    inline BOOL SetDlgItemText     (int nIDDlgItem, LPCTSTR lpString ) { return ::SetDlgItemText(m_hWnd, nIDDlgItem, lpString); }
    inline UINT GetDlgItemText     (int nIDDlgItem, LSTR& str) {
        LSTR _str;
       (VOID)::GetDlgItemText(m_hWnd, nIDDlgItem,(LPTSTR)(LPCTSTR)_str, _str.BufSize());
        str = (LPCTSTR)_str;
        return str.Len();
    }
    inline HWND GetDlgItem         (int nIDDlgItem) { return ::GetDlgItem(m_hWnd, nIDDlgItem); }
    inline BOOL CheckDlgButton     (int nIDButton, UINT uCheck = BST_CHECKED) { return ::CheckDlgButton(m_hWnd, nIDButton, uCheck); }
    inline UINT IsDlgButtonChecked (int nIDButton) { return ::IsDlgButtonChecked(m_hWnd, nIDButton); }
    BOOL EndDialog(INT nResult) { return ::EndDialog(m_hWnd, nResult); }
};

    LArray<LPCTSTR> clsNames; /* FIXME: the stored pointer of a string of window class is not safe */
    LArray<HWND> createdHwnds;

template <class T>
class LWindowImpl
    : public LWINDOW_ROOT_IMPL <T>
{
    //UINT _msg;
    //WPARAM _wParam;
    //LPARAM _lParam;

    LPTSTR m_lpszClassName;

    static HWND _hWndLastCreated; /* work as sentinel */
    static CRITICAL_SECTION _cs; /* Enable multi-thread safe support */
    static BOOL _fInitilized;

public:
    //typedef LRESULT(__stdcall T::*MSGPROC_PTR)(HWND, UINT, WPARAM, LPARAM);
    inline DWORD GetMessageProcPtr() 
    {
        DWORD dwProcAddr = 0;
        __asm
        {   
            mov eax, offset T::LWndProc /* Use the prefix 'T::' to enable the overload of LWndProc in derived class */
            mov dword ptr [dwProcAddr], eax
        }
        return dwProcAddr;
    }

    LWindowImpl() { __ASSERT(FALSE); }
    LWindowImpl(LPCTSTR lpszClassName)
    {   
        m_lpszClassName = new TCHAR [_tcslen(lpszClassName) + _tcslen(_T("LWindowImpl")) + 8];
        _stprintf(m_lpszClassName, _T("%s:%s"), _T("LWindowImpl"), lpszClassName);
        
        if (!_fInitilized)
        {
            ::InitializeCriticalSection(&_cs);
            _fInitilized = TRUE;
        }
        // BOOL r = JKRegisterClass();
        //_ASSERT(FALSE != r);
    }

    ~LWindowImpl()
    {   if (m_lpszClassName) delete[] m_lpszClassName;
    }

    BOOL OnDestroy(INT nExitCode = 0) { PostQuitMessage(nExitCode); return TRUE; }
    /*virtual*/ VOID OnPaint(LDC& dc) {}


    LRESULT DoDefault(UINT msg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(m_hWnd, msg, wParam, lParam); }

    LRESULT LCALLBACK LWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        //_msg = msg;
        //_wParam = wParam;
        //_lParam = lParam;
        T* pThis = static_cast<T *>(this);
        LRESULT r = __DO_DEFAULT;

#if 1
        if (m_hWnd == NULL)
        {   m_hWnd = hWnd;
            _hWndLastCreated = m_hWnd;

            /* When T::LWndProc was called first time, then leave the critical section */
            //::LeaveCriticalSection(&_cs);
        }
#endif
        __ASSERT(m_hWnd == hWnd);

        switch (msg)
        {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                // HDC hdc;
                LDC dc(BeginPaint(hWnd, &ps));
                //TCHAR szInfo[80];
                // _stprintf(szInfo, _T("hello world: this %p, hwnd %p"), this, m_hWnd);
                // hdc = BeginPaint(hWnd, &ps);
                // TextOut(hdc, 0, 0, szInfo, _tcsclen(szInfo));
                //dc.SetBkMode(TRANSPARENT);
                pThis->OnPaint(dc);
                //OnPaint(dc);
                EndPaint(hWnd, &ps);
            }
            return 0;

        }

        /* Do some default message process */
        return __super::ProcessBaseMessage(msg, wParam, lParam);
    }

    //BOOL JKRegisterClass()
    //{
    //    WNDCLASSEX wcx = {sizeof WNDCLASSEX};

    //    /* Enable multi-thread safe, for SetClassLongPtr use ONLY, call here for convenience */
    //    //::EnterCriticalSection(&_cs);

    //    if (GetClassInfoEx(m_hInstance, m_lpszClassName, &wcx))
    //    {   // ::EnterCriticalSection(&_cs);
    //        // _hWndLastCreated : 通过_hWndLastCreated找到CLASSINFO,并修改其中的GCL_WNDPROC
    //        //SetClassLongPtr(_hWndLastCreated, GCL_WNDPROC,(LONG) GetThunkedProcPtr()); 必须要在CreateWindow之前设置
    //        // 创建后续窗体时，要设置正确的this指针
    //        return TRUE;
    //    }

    //    wcx.cbClsExtra       = 0;
    //    wcx.cbWndExtra       = 0;
    //    wcx.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);//m_hbrBackground;
    //    wcx.hCursor          = LoadCursor(NULL, IDC_ARROW);
    //    wcx.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
    //    wcx.hInstance        = m_hInstance;
    //    wcx.lpfnWndProc      = (WNDPROC) GetThunkedProcPtr(); // 第一个窗体的this指针是OK的
    //    wcx.lpszClassName    = m_lpszClassName;
    //    wcx.lpszMenuName     = NULL;
    //    wcx.style            = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    //    return RegisterClassEx(&wcx);
    //}

    HWND CreateOverlappedWindow(LPCTSTR lpszWndName, RECT* lprc = NULL, LWnd *parent = NULL, BOOL fShowNow = TRUE)
    {
        RECT rc = {CW_USEDEFAULT, CW_USEDEFAULT, 0, 0};
        if (lprc) CopyRect(&rc, lprc);

        return Create(parent, NULL, WS_OVERLAPPEDWINDOW |(fShowNow ? WS_VISIBLE : 0) , lpszWndName, rc);
    }

    LRESULT Run()
    {
        MSG msg;
        while(GetMessage(&msg, 0, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return msg.wParam;
    }

    HWND Create(/*HWND hWndParent*/LWnd *parent, DWORD dwExStyle, DWORD dwStyle, LPCTSTR lpszWndName, RECT& rc, 
        int nCtrlId = 0, LPVOID lpParam = NULL, UINT uLWndStyle = 0, UINT uCtrlStyle = 0)
    {
        __ASSERT(m_hWnd == NULL);
        WNDCLASSEX wcx = {0};

        if (0 == GetClassInfoEx(m_hInstance, m_lpszClassName, &wcx))
        {
            // if (!JKRegisterClass()) return NULL;
            wcx.cbSize          = sizeof(WNDCLASSEX);
            wcx.cbClsExtra      = 0;
            wcx.cbWndExtra      = 0;
            wcx.hbrBackground   = (HBRUSH)(COLOR_MENU + 1);// m_hbrBackground;
            wcx.hCursor         = LoadCursor(NULL, IDC_ARROW);
            wcx.hIcon           = NULL; // LoadIcon(m_hInstance, IDI_APPLICATION);
            wcx.hInstance       = m_hInstance;
            wcx.lpfnWndProc     = (WNDPROC)GetThunkedProcPtr(); // 第一个窗体的this指针是OK的
            wcx.lpszClassName   = m_lpszClassName;
            wcx.lpszMenuName    = NULL;
            wcx.style           = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

            if (0 == RegisterClassEx(&wcx))
            {
                return NULL;
            }
        }
        else
        {
            HWND hWndLastCreated = _hWndLastCreated;
            if (NULL != hWndLastCreated 
                || NULL != (hWndLastCreated = FindWindow(m_lpszClassName, NULL))
                || NULL != (hWndLastCreated = FindLocalWindow(m_lpszClassName)))
            {
                /* the windows class has already registered, change the GCL_WNDPROC of registered class
                 * so the subsequently created windows can get the correct 'this' pointer */
                // ::EnterCriticalSection(&_cs);
                // _hWndLastCreated : 通过_hWndLastCreated找到CLASSINFO,并修改其中的GCL_WNDPROC
               (VOID)SetClassLongPtr(hWndLastCreated, GCL_WNDPROC,(LONG) GetThunkedProcPtr()); // 必须要在CreateWindow之前设置
            }
            _ASSERT(NULL != hWndLastCreated);
        }

        // 创建后续窗体时，要设置正确的this指针

        //RECT rc = {CW_USEDEFAULT, CW_USEDEFAULT, 0, 0};
        _uCtrlStyle = uCtrlStyle;
        _uLWndStyle = uLWndStyle | GetFixedLWndStyle();
        m_hWnd = ::CreateWindowEx(
            dwExStyle, 
            m_lpszClassName,//_T("JKTL::_JkLightWindow"), 
            lpszWndName, dwStyle, 
            rc.left, rc.top, 
            rc.left == CW_USEDEFAULT ? CW_USEDEFAULT : _RECT_W(rc) , 
            rc.top  == CW_USEDEFAULT ? CW_USEDEFAULT : _RECT_H(rc) ,
            parent ? (HWND)*parent : NULL,(HMENU) nCtrlId, m_hInstance, lpParam);
        clsNames.Add((LPCTSTR)m_lpszClassName);
        createdHwnds.Add(m_hWnd);

        return m_hWnd;
    }

    HWND CreateChild(LWnd *parent, LPCTSTR lpszWndName, RECT& rc, 
        int nCtrlId = 0, UINT uLwndStyle = 0, UINT uCtrlStyle = 0,
        DWORD dwStyle = WS_VISIBLE | WS_CHILD, DWORD dwExStyle = 0)
    {
        _parent = parent;
        HWND hWnd = Create(parent, dwExStyle, dwStyle, lpszWndName, rc, nCtrlId, NULL, uLwndStyle, uCtrlStyle);
        if (hWnd != NULL && parent != NULL)
            parent->AddChild(this);
        return hWnd;
    }

    HWND FindLocalWindow(LPCTSTR clsName)
    {
        for (int i = 0; i < clsNames.Count(); i++)
        {
            if (0 == _tcscmp(clsNames[i], clsName) && ::IsWindow(createdHwnds[i]))
            {
                return createdHwnds[i];
            }
        }
        return NULL;
    }
};

template<class T> HWND LWindowImpl<T>::_hWndLastCreated = NULL;
template<class T> CRITICAL_SECTION LWindowImpl<T>::_cs;
template<class T> BOOL LWindowImpl<T>::_fInitilized = FALSE;

#endif // __LWIN_H__