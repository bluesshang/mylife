#pragma once

#include "..\stdafx.h"
#include "..\utils\lutil.h"

#define LGDI_CHECK_HGDIOBJ() {if (NULL == m_hGdi) return FALSE;}

class LGdiObject
{
    HGDIOBJ _hOldGdi;
    DWORD threadid;

protected:
    HDC _hdc;
public:
    HGDIOBJ m_hGdi;

    //LGdiObject() :_hdc(NULL), m_hGdi(NULL), _hOldGdi(NULL) {}
    LGdiObject(HDC hdc = NULL) : m_hGdi(NULL), _hOldGdi(NULL)
    {
        _hdc = hdc;
        threadid = 0;
    }

    operator HGDIOBJ() {return m_hGdi;}
    operator HBRUSH() {return(HBRUSH) m_hGdi;}
    operator HPEN() {return(HPEN) m_hGdi;}
    operator HFONT() {return(HFONT) m_hGdi;}

    void Attach()
    {
        LWIN_ASSERT(threadid == 0);
        threadid = GetCurrentThreadId();
        if (NULL != _hdc && NULL != m_hGdi) {
            _hOldGdi = ::SelectObject(_hdc, m_hGdi);
        }
    }

    void Detach()
    {
        LWIN_ASSERT(threadid == GetCurrentThreadId());
        if (NULL != _hdc && NULL != _hOldGdi) {
            HGDIOBJ ret = ::SelectObject(_hdc, _hOldGdi);
            LWIN_ASSERT(ret != NULL);
            _hOldGdi = NULL;
        }
    }

    BOOL CreateStockObject(INT nIndex)
    {
        m_hGdi = GetStockObject(nIndex);
        LGDI_CHECK_HGDIOBJ();
        Attach();
        return TRUE;
    }
    ~LGdiObject()
    {
        Detach();
        if (NULL != m_hGdi) {
           (VOID)DeleteObject(m_hGdi);
        }
    }
};

class LBrush : public LGdiObject
{
public:
    LBrush(HDC hdc = NULL) : LGdiObject(hdc) {}
    BOOL CreateSolidBrush(COLORREF clr)
    {
        m_hGdi = ::CreateSolidBrush(clr);
        LGDI_CHECK_HGDIOBJ();
        Attach();
        return TRUE;
    }
};

class LPen : public LGdiObject
{
public:
    LPen(HDC hdc = NULL) : LGdiObject(hdc) {}
    BOOL CreatePen(int fnPenStyle, int nWidth, COLORREF crColor)
    {
        m_hGdi = (HGDIOBJ) ::CreatePen(fnPenStyle, nWidth, crColor);
        LGDI_CHECK_HGDIOBJ();
        Attach();
        return TRUE;
    }
};

class LFont : public LGdiObject
{
    COLORREF _clrOldText;
public:
    static const COLORREF CLR_NOT_SPECIFIED = 0xFFFFFFFF;
    LFont(HDC hdc = NULL) : LGdiObject(hdc) { _clrOldText = CLR_NOT_SPECIFIED; }
    ~LFont()
    {
        if (CLR_NOT_SPECIFIED != _clrOldText)
        {
            SetTextColor(_hdc, _clrOldText);
        }
    }
    BOOL CreateFont(LPCTSTR lpszFace, int nHeight, int fnWeight = FW_DONTCARE, COLORREF clrText = CLR_NOT_SPECIFIED, int nEscapement  = 0)
    {
        m_hGdi = (HGDIOBJ) ::CreateFont(nHeight, 0, nEscapement, 0, fnWeight, 
            FALSE, FALSE, FALSE, 
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
            DEFAULT_QUALITY, DEFAULT_PITCH, lpszFace);
        LGDI_CHECK_HGDIOBJ();
        if (CLR_NOT_SPECIFIED != clrText)
        {
            _clrOldText = SetTextColor(_hdc, clrText);
        }
        {
            HGDIOBJ hlast = GetCurrentObject(_hdc, OBJ_FONT);
            LOGFONT lf;
            if (hlast != NULL)
            {
                GetObject(hlast, sizeof(lf), &lf);
            }
        }
        Attach();
        return TRUE;
    }
};

class LDC
{
    HDC _hdc;
    CRITICAL_SECTION _cs;
    BOOL _withLock;

    void init()
    {
        InitializeCriticalSection(&_cs);
    }
public:
    LDC(BOOL withLock = FALSE):_hdc(NULL),_withLock(withLock) {
        init();
    }
    LDC(HDC hdc, BOOL withLock = FALSE):_hdc(hdc),_withLock(withLock) {
        LWIN_ASSERT(NULL != _hdc);
        init();
    }
    ~LDC()
    {
        DeleteCriticalSection(&_cs);
    }

    operator HDC() {return _hdc;}
    void operator = (HDC hdc) { _hdc = hdc; }
    void EnableLock(BOOL enable = TRUE) { _withLock = enable; }
    void Lock()
    {
        if (_withLock)
            EnterCriticalSection(&_cs);
    }

    void Unlock()
    {
        if (_withLock)
            LeaveCriticalSection(&_cs);
    }

    COLORREF SetTextColor(COLORREF clrText) {
        return ::SetTextColor(_hdc, clrText);
    }
    COLORREF SetBkColor(COLORREF clrBk) {
        return ::SetBkColor(_hdc, clrBk);
    }
    BOOL TextOut(int x, int y, LPCTSTR lpString) {
        return ::TextOut(_hdc, x, y, lpString, _tcslen(lpString));
    }
    BOOL TextOut(LPoint& pt, LPCTSTR lpString) {
        return TextOut(pt.x, pt.y, lpString);
    }
    int DrawText(LPCTSTR lpString, RECT& rc, UINT uFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE) {
        return ::DrawText(_hdc, lpString, _tcslen(lpString), &rc, uFormat);
    }
    int SetBkMode(int mode) {
        return ::SetBkMode(_hdc, mode);
    }
    BOOL Rectangle(RECT& rc) {
        return ::Rectangle(_hdc, rc.left, rc.top, rc.right, rc.bottom);
    }
    BOOL Rectangle(int left, int top, int right, int bottom) {
        return ::Rectangle(_hdc, left, top, right, bottom);
    }
    BOOL Rectangle(LPoint& ptLT, LPoint& ptRB) {
        return ::Rectangle(_hdc, ptLT.x, ptLT.y, ptRB.x, ptRB.y);
    }
    BOOL RoundRect(LRect& rc, int nWidth, int nHeight, int transparency = 100) {
        if (transparency == 100)
            return ::RoundRect(_hdc, rc.left, rc.top, rc.right, rc.bottom, nWidth, nHeight);

        LDC hdc0(CreateCompatibleDC(NULL));
        LRect rc0 = {0, 0, rc.right - rc.left, rc.bottom - rc.top};
        HBITMAP hbm0 = CreateCompatibleBitmap(_hdc, rc0.right, rc0.bottom);
        BOOL ret;
        ::SelectObject(hdc0, hbm0);
        {
        LPoint pt0(0, 0);
        LBrush br0(hdc0);
        LPen pen(hdc0);
        COLORREF clr0 = RGB(200, 200, 200);
        pen.CreatePen(PS_SOLID, 1, clr0);
        br0.CreateSolidBrush(clr0);
        ::RoundRect(hdc0, 0, 0, rc0.right, rc0.bottom, nWidth, nHeight);
        //BOOL ret = TransparentBlt(rc, hdc0, pt0, RGB(0, 0, 0));
        ret = AlphaBlend(rc, hdc0, pt0, (int)(((float)transparency / 100.0) * 255.0), RGB(0, 0, 0));
        //BOOL ret = BitBlt(rc, hdc0, pt0);
        }
        ::DeleteObject(hdc0);
        ::DeleteObject(hbm0);
        return ret;
    }
    int FrameRect(RECT& rc, HBRUSH hbr) {
        return ::FrameRect(_hdc, &rc, hbr);
    }
    BOOL Ellipse(RECT& rc) {
        return ::Ellipse(_hdc, rc.left, rc.top, rc.right, rc.bottom);
    }
    void CalcTextRect(LPCTSTR lpchText, LRect& rc) {
       (VOID)::DrawText(_hdc,(LPTSTR)lpchText, _tcslen(lpchText), rc, DT_CALCRECT);
    }
    BOOL BitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop = SRCCOPY) {
        return ::BitBlt(_hdc, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
    }
    BOOL BitBlt(LRect& rc, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop = SRCCOPY) {
        return ::BitBlt(_hdc, rc.left, rc.top, rc.Width(), rc.Height(), hdcSrc, nXSrc, nYSrc, dwRop);
    }
    BOOL BitBlt(LRect& rc, HDC hdcSrc, const LPoint& pos, DWORD dwRop = SRCCOPY) {
        return ::BitBlt(_hdc, rc.left, rc.top, rc.Width(), rc.Height(), hdcSrc, pos.x, pos.y, dwRop);
    }
    BOOL TransparentBlt(LRect& rc, HDC hdcSrc, int x, int y, COLORREF clrTransparent) {
        return ::TransparentBlt(_hdc, rc.left, rc.top, rc.Width(), rc.Height(), hdcSrc, 
            x, y, rc.Width(), rc.Height(), clrTransparent);
    }
    BOOL TransparentBlt(LRect& rc, HDC hdcSrc, const LPoint& pos, COLORREF clrTransparent) {
        return TransparentBlt(rc, hdcSrc, pos.x, pos.y, clrTransparent);
    }
    BOOL AlphaBlend(LRect& rc, HDC hdcSrc, const LPoint& pos, INT alphaVal) {
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.AlphaFormat = 0;//AC_SRC_ALPHA ;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alphaVal;
        return ::AlphaBlend(_hdc, rc.left, rc.top, rc.Width(), rc.Height(), 
            hdcSrc, pos.x, pos.y, rc.Width(), rc.Height(), bf);
    }
    /* TOO SLOW!!! use CreateDIBSection to speed up */
    BOOL AlphaBlend(LRect& rc, HDC hdcSrc, const LPoint& pos, INT alphaVal, COLORREF clrMask) {
        //int a0 = GetDeviceCaps(_hdc, SHADEBLENDCAPS);
        //int a1 = GetDeviceCaps(hdcSrc, SHADEBLENDCAPS);
        double f = (double)alphaVal / 255.0;
        for (int x = rc.left; x < rc.right; x++) {
            for (int y = rc.top; y < rc.bottom; y++) {
                COLORREF clrSrc = GetPixel(hdcSrc, pos.x + x - rc.left, pos.y + y - rc.top);
                if (clrSrc != clrMask) {
                    COLORREF clrDst = GetPixel(_hdc, x, y);
                    int r0, g0, b0, r1, g1, b1, R, G, B;
                    r0 = GetRValue(clrSrc);
                    g0 = GetGValue(clrSrc);
                    b0 = GetBValue(clrSrc);
                    r1 = GetRValue(clrDst);
                    g1 = GetGValue(clrDst);
                    b1 = GetBValue(clrDst);
                    R = (int)((double)r0 * f + (1 - f) * (double)r1);
                    G = (int)((double)g0 * f + (1 - f) * (double)g1);
                    B = (int)((double)b0 * f + (1 - f) * (double)b1);
                    SetPixel(x, y, RGB(R, G, B));
                }
            }
        }

        return TRUE;
    }

    //void gdiRectangleAlpha(HDC hdc,const RECT *rect,COLORREF color, unsigned char alpha)
    //{
    //    BYTE * g_pBits;
    //    HDC g_hMemDC;
    //    HBITMAP g_hBmp, g_hOldBmp;

    //    int xMin = rect->left;
    //    int yMin = rect->top;
    //    int xMax = rect->right;
    //    int yMax = rect->bottom;
    //    int x,y;
    //    unsigned char r = GetRValue(color);
    //    unsigned char g = GetGValue(color);
    //    unsigned char b = GetBValue(color);

    //    COLORREF clSrc;
    //    unsigned char   rSrc;
    //    unsigned char gSrc;
    //    unsigned char   bSrc;
    //    g_hMemDC = ::CreateCompatibleDC(hdc);

    //    if (!g_hMemDC)
    //    {
    //        ::DeleteDC(hdc);
    //    }

    //    int iWidth = rect->right - rect->left;
    //    int iHeight = rect->bottom - rect->top;

    //    BYTE bmibuf[sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD)];
    //    memset(bmibuf, 0, sizeof(bmibuf));

    //    BITMAPINFO* pbmi = (BITMAPINFO*)bmibuf;
    //    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //    pbmi->bmiHeader.biWidth = iWidth;
    //    pbmi->bmiHeader.biHeight = iHeight;
    //    pbmi->bmiHeader.biPlanes = 1;
    //    pbmi->bmiHeader.biBitCount = 24;
    //    pbmi->bmiHeader.biCompression = BI_RGB;
    //    g_hBmp = ::CreateDIBSection(g_hMemDC, pbmi, DIB_RGB_COLORS, (void **)&g_pBits, 0, 0);
    //    if (!g_hBmp)
    //    {
    //        ::DeleteDC(g_hMemDC);
    //    }

    //    g_hOldBmp = (HBITMAP)::SelectObject(g_hMemDC, g_hBmp);
    //    ::BitBlt(g_hMemDC,0,0,iWidth,iHeight,hdc,0,0,SRCCOPY);
    //    // offset = y * (width * 24 / 8) + x * (24 / 8)

    //    for (y = 0; y < iHeight; y++)
    //    {
    //      for (x = 0; x < iWidth; x++)
    //      {
    //       rSrc = g_pBits[y * iWidth * 3  + x * 3 + 2];
    //       gSrc = g_pBits[y * iWidth * 3  + x * 3 + 1];
    //       bSrc = g_pBits[y * iWidth * 3  + x * 3];
 
    //       rSrc = (rSrc * alpha + r * (255 - alpha)) >>8;
    //       gSrc = (gSrc * alpha + g * (255 - alpha)) >>8;
    //       bSrc = (bSrc * alpha + b * (255 - alpha)) >>8;

    //       g_pBits[y * iWidth * 3  + x * 3 + 2] = rSrc;
    //       g_pBits[y * iWidth * 3  + x * 3 + 1] = gSrc;
    //       g_pBits[y * iWidth * 3  + x * 3]     = bSrc;
    //      }
    //    }

    //    ::BitBlt(hdc, 0, 0, iWidth, iHeight, g_hMemDC, 0, 0, SRCCOPY);
    //    ::SelectObject(g_hMemDC, g_hOldBmp);

    //    DeleteObject(g_hBmp); 
    //    DeleteDC(g_hMemDC); 
    //    ReleaseDC(NULL, hdc); 
    //}


    HGDIOBJ SelectObject(HGDIOBJ hGdiObj) {
        return ::SelectObject(_hdc, hGdiObj);
    }
    int FillRect(RECT& rc, HBRUSH hbr) {
        return ::FillRect(_hdc, &rc, hbr);
    }
    BOOL MoveTo(int x, int y, LPPOINT lpPoint = NULL) {
        return ::MoveToEx(_hdc, x, y, lpPoint);
    }
    BOOL LineTo(int x, int y) {
        return ::LineTo(_hdc, x, y);
    }
    int IntersectClipRect(LRect &rc) {
        return ::IntersectClipRect(_hdc, rc.left, rc.top, rc.right, rc.bottom);
    }
    int ExcludeClipRect(LRect &rc) {
        return ::ExcludeClipRect(_hdc, rc.left, rc.top, rc.right, rc.bottom);
    }
    int SelectClipRgn(HRGN rgn) {
        return ::SelectClipRgn(_hdc, rgn);
    }
    COLORREF SetPixel(int x, int y, COLORREF clr) {
        return ::SetPixel(_hdc, x, y, clr);
    }
    int Save() {
        return SaveDC(_hdc);
    }
    BOOL Restore(int nSavedDC) {
        return RestoreDC(_hdc, nSavedDC);
    }
    BOOL GetTextExtentPoint32(LPCTSTR lpString, LPSIZE lpSize) {
        return ::GetTextExtentPoint32(_hdc, lpString, _tcslen(lpString), lpSize);
    }
    BOOL GetTextMetrics(LPTEXTMETRIC lptm) {
        return ::GetTextMetrics(_hdc, lptm);
    }
    int SetMapMode(int mapMode) {
        return ::SetMapMode(_hdc, mapMode);
    }
    BOOL SetViewportOrg(int x, int y, LPPOINT lpPoint = NULL) {
        return ::SetViewportOrgEx(_hdc, x, y, lpPoint);
    }
    BOOL SetViewportOrg(LPoint pt, LPPOINT lpPoint = NULL) {
        return ::SetViewportOrgEx(_hdc, pt.x, pt.y, lpPoint);
    }
    BOOL SetWindowOrg(int x, int y, LPPOINT lpPoint = NULL) {
        return ::SetWindowOrgEx(_hdc, x, y, lpPoint);
    }
    BOOL SetWindowExt(int cx, int cy, LPSIZE lpSize = NULL) {
        return ::SetWindowExtEx(_hdc, cx, cy, lpSize);
    }
    BOOL SetViewportExt(int cx, int cy, LPSIZE lpSize = NULL) {
        return ::SetViewportExtEx(_hdc, cx, cy, lpSize);
    }
    BOOL GetWindowExtEx(LPSIZE lpSize) {
        return ::GetWindowExtEx(_hdc, lpSize);
    }
    BOOL GetViewportExtEx(LPSIZE lpSize) {
        return ::GetViewportExtEx(_hdc, lpSize);
    }

    /* layout strings using the current selected GDI objects */
    template <typename T>
    VOID LayoutStrings(
        LArray<T>& strings, LRect& rcLimitBound, LRect& rcInit,
        INT nGap = 10, BOOL bLimittedInBound = TRUE,
        LRect* rcMaxBound = NULL, INT nPreferHeight = 0, BOOL vertLayout = FALSE)
    {
        int N = strings.Count();
        int i, wBound;
        LArray<float> widths(N + 1);
        LArray<float> heights(N + 1);
        INT lenTotal = 0, t;
        LRect rcLimitBound0 = rcLimitBound, rcInit0 = rcInit;

        if (vertLayout == TRUE) {
            rcLimitBound0.right = rcLimitBound.left + rcLimitBound.Height();
            rcLimitBound0.bottom = rcLimitBound.top + rcLimitBound.Width();
            rcInit0.right = rcInit.left + rcInit.Height();
            rcInit0.bottom = rcInit.top + rcInit.Width();
        }

        /* get the orignal width and height of each string */
        for (i = 0; i < N; i++)
        {
            LRect rc(0, 0, 100, 100);
            CalcTextRect(strings[i], rc);
            rc.right += nGap;
            widths[i] = (float)rc.Width();
            heights[i] = (float)rc.Height();
            lenTotal += rc.Width();
        }

        wBound = rcLimitBound0.Width();
        if (TRUE == bLimittedInBound && lenTotal > wBound)
        {   /* re-adjust the size of tab item */
            for (i = 0; i < N; i++)
            {
                widths[i] = wBound *(widths[i] /(float)lenTotal);
            }
        }

        t = rcInit0.left;
        for (i = 0; i < N; i++)
        {
            LRect& rc = strings[i]; //_tabs[i].rc;
            rc.top = rcInit0.top;
            rc.bottom = rc.top + max(nPreferHeight,(INT)heights[i]);// rcMaxBound.bottom;
            rc.left = t;
            rc.right = rc.left +(INT)widths[i];
            t += (INT) widths[i];
        }

        if (TRUE == bLimittedInBound)
        {
            for (i = 0; i < N; i++)
            {
                LRect& rc = strings[i];
                rc.right = min(rc.right, rcLimitBound0.right);
            }
            if (lenTotal > wBound && i > 0)
                strings[i - 1].right = rcLimitBound0.right;
        }

        if (vertLayout == TRUE) {
            for (i = 0; i < N; i++) {
                LRect& rc = strings[i];
                LRect rcTmp = rc;
                rc.left = rcInit.left;
                rc.right = rc.left + rcInit.Width();
                rc.top = rcTmp.left;
                rc.bottom = rc.top + rcTmp.Width();
            }
        }

        if (NULL != rcMaxBound)
        {
            rcMaxBound->left = strings[0].left;
            rcMaxBound->top = strings[0].top;
            rcMaxBound->right = strings[N - 1].right;
            rcMaxBound->bottom = strings[N - 1].bottom;
        }
    }
};

class AxisInfo
{
    //double xscale, yscale;
public:
    //LPoint origin;
    int xwidth;
    int ydlt;

    // int cx, cy; 
    LRect rc; /* the drawable region */
    struct { int dmin, dmax; } x,y; /* data min and max */
    AxisInfo()
        : xwidth(5), ydlt(0)
    { }

    int d2x(int xdata, int dlt = 0)
    {
        if (xwidth < 0)
            return rc.left + (int)((double)xdata * (double)rc.Width() / (double)(x.dmax - x.dmin));
        return rc.left + (xdata - x.dmin) * (xwidth + dlt + 1) + (xwidth + dlt) / 2;
    }

    int d2y(int ydata, int dlt = 0)
    {
        double yscale = (double)(rc.Height() + dlt) / (double)(y.dmax - y.dmin);
        int y0 = rc.bottom + dlt - (int)((double)(ydata - y.dmin) * yscale) - 1; /* -1: to show the bottom pixel */
        return max(y0, rc.top);
    }

    /* data on position pos is the dv[x2d(pos)] */
    int x2d(int xpos)
    {
        if (xwidth < 0)
            return (int)((double)(xpos - rc.left) / ((double)rc.Width() / (double)(x.dmax - x.dmin)));

        int xdata = xpos - rc.left/* - xwidth / 2*/;
        return xdata / (xwidth + 1);
    }

    int y2d(int ypos)
    {
        double yscale = (double)rc.Height() / (double)(y.dmax - y.dmin);

        int ydata = (int)((double)(rc.bottom - ypos) / yscale) + y.dmin;

        return ydata;
        //return (int)((double)(ypos - rc.left) / ((double)rc.Width() / (double)(x.dmax - x.dmin)));
    }
};

class GV
{
public:
    /* draw on ai.rc */
    virtual void Draw(LDC& dc, AxisInfo& ai) = 0;
    virtual INT Size(BOOL part = TRUE) = 0;
    virtual INT GetMaxInt(BOOL part = TRUE) = 0;
    virtual INT GetMinInt(BOOL part = TRUE) = 0;
    virtual _DV* GetDV(LPCTSTR dvName) = 0;
};

class LMemDC
{

};
