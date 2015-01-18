#pragma once

#include "..\utils\lstr.h"
#include "lwin.h"
#include "LWndView.h"

class ScrollBar
{
public:
    LRect _rc;
    LRect _rcThumb;
    INT _minPos, _maxPos;
    INT _size; /* the width of vertical scroll bar or the height of horizontal scroll bar */
    // INT _thumb;
    LDC _dc;
    float _scale;
    UINT _dir;
    UINT _flags;
    BOOL enabled;
public:
    enum {VERTICAL = 1, HORIZONTAL = 2, LEFT = 4, RIGHT = 8, TOP = 0x10, BOTTOM = 0x20};
    ScrollBar()
    {
        _dir = VERTICAL | RIGHT;
        _size = 10;
        enabled = FALSE;
    }
    ScrollBar(UINT dir) : _dir(dir), _size(10), enabled(FALSE) {}
    VOID Init(UINT dir)
    {
        _dir = dir;
    }
    VOID SetScrollInfo(LRect& rcWnd, INT minPos, INT maxPos, BOOL bRedraw = TRUE)
    {
        INT R, W, H, t, t0, t1;
        _rc = rcWnd;
        _rcThumb = rcWnd;
        _minPos = min(minPos, maxPos);
        _maxPos = max(minPos, maxPos);
        R = _maxPos - _minPos;
        if (VERTICAL & _dir) {
            H = _rc.Height();
            t0 = H - R;
            t1 = H / 5;
            // t1 = (int)((float)H((float)R /(float)H));
            t = max(t0, t1);
            t = max(t, _size);
            _scale = (float)R /(float)(H - t);
            _rcThumb.bottom = _rcThumb.top + t;
            if (RIGHT & _dir) {
                _rcThumb.right = _rcThumb.left + _size;
                _rcThumb.right = min(_rcThumb.right, _rc.right);
                _rc.right = _rcThumb.right;
            } else {
                _rcThumb.left = _rcThumb.right - _size;
                _rcThumb.left = max(_rcThumb.left, _rc.left);
                _rc.left = _rcThumb.left;
            }
        }
        else {
            W = _rc.Width();
            t0 = W - R;
            t1 = W / 5;
            // t1 = (int)((float)H((float)R /(float)H));
            t = max(t0, t1);
            t = max(t, _size);
            _scale = (float)R /(float)(W - t);
            _rcThumb.right = _rcThumb.left + t;
            if (TOP & _dir) {
                _rcThumb.bottom = _rcThumb.top + _size;
                _rcThumb.bottom = min(_rcThumb.bottom, _rc.bottom);
                _rc.bottom = _rcThumb.bottom;
            } else {
                _rcThumb.top = _rcThumb.bottom - _size;
                _rcThumb.top = max(_rcThumb.top, _rc.top);
                _rc.top = _rcThumb.top;
            }
        }
    }
    VOID SetPos(INT pos, BOOL bRedraw = TRUE)
    {
        INT t;
        _ASSERT(pos >= _minPos);
        _ASSERT(pos <= _maxPos);
        if (VERTICAL & _dir) {
            t = _rcThumb.Height();
            if (pos == _minPos)
                _rcThumb.top = _rc.top;
            else if (pos == _maxPos)
                _rcThumb.top = _rc.bottom - t;
            else
                _rcThumb.top = (INT)((float)pos / _scale) + _rc.top;
            _rcThumb.bottom = _rcThumb.top + t;
        } else {
            t = _rcThumb.Width();
            if (pos == _minPos)
                _rcThumb.left = _rc.left;
            else if (pos == _maxPos)
                _rcThumb.left = _rc.right - t;
            else
                _rcThumb.left = (INT)((float)pos / _scale) + _rc.left;
            _rcThumb.right = _rcThumb.left + t;
        }
    }
    INT Scale(INT val)
    {
        return(INT)((float)val * _scale);
    }
    INT Scroll(INT amount, BOOL& bChanged, BOOL bRedraw = TRUE)
    {
        INT t;
        if (VERTICAL & _dir) {
            t = _rcThumb.Height();
            if (0 == amount
                || _rcThumb.top == _rc.top && amount < 0
                || _rcThumb.bottom == _rc.bottom && amount > 0) {
                bChanged = FALSE;
            } else {
                _rcThumb.top += amount;
                //_rcThumb.top = max(_rcThumb.top, _rc.top);
                _rcThumb.top = min(_rcThumb.top, _rc.bottom - t);
                _rcThumb.bottom = _rcThumb.top + t;
                //DrawLayer(bRedraw);
                bChanged = TRUE;
                if (_rcThumb.top == _rc.top)
                    return _minPos;
                if (_rcThumb.bottom == _rc.bottom)
                    return _maxPos;
            }
            return(INT)((float)_rcThumb.top * _scale);
        }
        /* else: the horizontal bar */
        if (0 == amount
            || _rcThumb.left == _rc.left && amount < 0
            || _rcThumb.right == _rc.right && amount > 0) {
            bChanged = FALSE;
        } else {
            _rcThumb.left += amount;
            _rcThumb.left = max(_rcThumb.left, _rc.left);
            _rcThumb.left = min(_rcThumb.left, _rc.right);
            //DrawLayer(bRedraw);
            bChanged = TRUE;
            if (_rcThumb.left == _rc.left)
                return _minPos;
            if (_rcThumb.right == _rc.right)
                return _maxPos;
        }
        return(INT)((float)_rcThumb.left * _scale);
    }

    //INT Scroll(INT amount)
    //{
    //    INT t;
    //    LRect rc = _rcThumb;
    //    if (VERTICAL & _dir) {
    //        t = rc.Height();
    //        if (0 == amount
    //            || rc.top == _rc.top && amount < 0
    //            || rc.bottom == _rc.bottom && amount > 0) {
    //            return 0;
    //        } else {
    //            rc.top += amount;
    //            //_rcThumb.top = max(_rcThumb.top, _rc.top);
    //            rc.top = min(rc.top, _rc.bottom - t);
    //            rc.bottom = rc.top + t;
    //            //DrawLayer(bRedraw);
    //            if (rc.top == _rc.top)
    //                return _minPos;
    //            if (rc.bottom == _rc.bottom)
    //                return _maxPos;
    //        }
    //        return(INT)((float)rc.top * _scale);
    //    }
    //    /* else: the horizontal bar */
    //    if (0 == amount
    //        || rc.left == _rc.left && amount < 0
    //        || rc.right == _rc.right && amount > 0) {
    //    } else {
    //        rc.left += amount;
    //        rc.left = max(rc.left, _rc.left);
    //        rc.left = min(rc.left, _rc.right);
    //        if (rc.left == _rc.left)
    //            return _minPos;
    //        if (rc.right == _rc.right)
    //            return _maxPos;
    //    }
    //    return(INT)((float)rc.left * _scale);
    //}

    VOID Draw(LDC& dc/*, BOOL bRedraw = TRUE*/)
    {
        //LDC& dc = layer->dc;

        //if (FALSE == bRedraw)
        //    return;
        // dc.re
        // dc.Rectangle(_rcThumb);
        LPen pen(dc);
        pen.CreatePen(PS_SOLID, 1, _flags ? RGB(208, 209, 215) : RGB(128, 0, 0));
        LBrush br(dc);
        //br.CreateStockObject(NULL_BRUSH);
        br.CreateSolidBrush(_flags ? RGB(208, 209, 215) : RGB(200, 200, 200));
        if (!_flags) {
            LRect rc = _rcThumb;
            if (_dir & HORIZONTAL) 
                rc.top = rc.bottom - 2;
            else rc.left = rc.right - 2;
            dc.Rectangle(rc);
        } else {
            dc.RoundRect(_rcThumb, 5, 5, 60);
        }
    }
};

#define LSBS_VERTICAL    1
#define LSBS_HORIZONTAL  2

#if 0
template <class T>
class LWndView;

class LScrollBar : public LWndView<LScrollBar>
{
public:

    LScrollBar()
    {
    }

    ~LScrollBar()
    {
    }
};

#endif
