
#pragma once

#include "..\stdafx.h"
//#include "../stdafx.h"

class LPoint : public POINT
{
public:
    LPoint() {x = y = 0;}
    LPoint(int xpos, int ypos) {x = xpos; y = ypos;}
    operator LPoint() {return *this;}
    friend LPoint operator +(const LPoint& pt0, const LPoint& pt1)
    {
        LPoint pt;
        pt.x = pt0.x + pt1.x;
        pt.y = pt0.y + pt1.y;
        return pt;
    }
    friend LPoint operator -(const LPoint& pt0, const LPoint& pt1)
    {
        LPoint pt;
        pt.x = pt0.x - pt1.x;
        pt.y = pt0.y - pt1.y;
        return pt;
    }
    LPoint& operator += (const LPoint& pt)
    {
        x += pt.x;
        y += pt.y;
        return *this;
    }
    LPoint& operator -= (const LPoint& pt)
    {
        x -= pt.x;
        y -= pt.y;
        return *this;
    }
    LPoint& operator = (const LPoint& pt)
    {
        x = pt.x;
        y = pt.y;
        return *this;
    }
    BOOL operator != (const LPoint& pt) {
        return x != pt.x || y != pt.y;
    }
    BOOL operator == (const LPoint& pt) {
        return x == pt.x && y == pt.y;
    }
    //BOOL InRect(LRect& rc)
    //{
    //    return rc.
    //}
};

class LRect
{
public:
    union
    {
        RECT _rc;
        struct {LONG left, top, right, bottom;};
        struct {LPoint lt, rb;};
    };
    LRect() {left = top = right = bottom = 0;}
    LRect(int l, int t, int r, int b) {
        SetLTRB(l, t, r, b);
    }
    LRect(const LPoint& lt, const LPoint& rb)
        : LRect(lt.x, lt.y, rb.x, rb.y)
    {}
    VOID SetLTRB(int l, int t, int r, int b) {
        left = min(l, r); 
        top = min(t, b); 
        right = max(l, r); 
        bottom = max(t, b);
    }
    LRect& operator = (LRect& rc)
    {
        SetLTRB(rc.left, rc.top, rc.right, rc.bottom);
        return *this;
    }
    //LRect(const LRect& rc) {memcpy(this, &rc, sizeof(LRect));}
    //inline operator LPCTSTR() {_stprintf(str, _T("LRect{%d,%d,%d,%d}"), left, top, right, bottom); return str;}
    inline operator RECT&() {return _rc; }
    inline operator RECT*() {return &_rc; }
    inline LONG Width() {return right - left;}
    inline LONG Height() {return bottom - top;}
    inline void Offset(int dx, int dy) {OffsetRect(&_rc, dx, dy);}
    BOOL InflateRect(int dx, int dy) {return ::InflateRect(&_rc, dx, dy);}
    BOOL PtInRect(const LPoint& pt) {
        int l = min(left, right);
        int t = min(top, bottom);
        int r = max(left, right);
        int b = max(top, bottom);
        return(pt.x >= l && pt.x <= r) && (pt.y >= t && pt.y <= b);
    }
    BOOL PtInRect(LPoint& pt, LRect& rcBoundary) {
        LRect rc(max(left, rcBoundary.left), max(top, rcBoundary.top),
            min(right, rcBoundary.right), min(bottom, rcBoundary.bottom));
        return rc.PtInRect(pt);
    }
    HRGN CreateRoundRectRgn(int nWidthEllipse, int nHeightEllipse) {
        return ::CreateRoundRectRgn(left, top, right, bottom, nWidthEllipse, nHeightEllipse);
    }
    LRect& Intersection(LRect& rc) {
        left = max(left, rc.left);
        top = max(top, rc.top);
        right = min(right, rc.right);
        bottom = min(bottom, rc.bottom);
        return *this;
    }
    VOID Zero() {
        left = top = right = bottom = 0;
    }
    int Size() { return Width() * Height(); }
    void Transpose()
    {
        int width = Width();
        right = left + Height();
        bottom = top + width;
    }
    void MoveTo(LPoint& pt)
    {
        int x = pt.x - left;
        int y = pt.y - top;
        Offset(x, y);
    }
    /* center rectangle on specifed region if the region larger than the rectange */
    VOID Center(int cx, int cy)
    {
        int x = 0, y = 0, dltX, dltY;

        if ((dltX = cx - Width()) > 0) {
            // right = left + cx;
            x = dltX / 2;
        }

        if ((dltY = cy - Height()) > 0) {
            //bottom = top + cy;
            y = dltY / 2;
        }

        Offset(x, y);
    }
    HRGN CreateRgn()
    {
        return CreateRectRgnIndirect(&_rc);
    }
private:
    //TCHAR str[80];
};

