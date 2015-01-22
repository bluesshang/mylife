#pragma once

# include<math.h>

#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#include "lwin.h"
#include "LWndView.h"
#include "line.h"

//typedef struct ScaleInfo_t
//{
//    float xScale, yScale;
//    LPoint origin; /* in view port reion(rcView) */
//
//    ScaleInfo_t()
//    {
//        xScale = 1.0;
//        yScale = 1.0;
//        origin.x = 0;
//        origin.y = 0;
//    }
//} ScaleInfo;

//class LineLayer : public Layer
//{
//    LArray<Line*> _lines;
//public:
//
//};
//
class LAxisRuler : public LWndView<LAxisRuler>
{
    //AxisInfo ai;
protected:
    INT _rulerId;
    //LRect _rcScroll;
    _DV *_dv;
    int _off;
    LPCTSTR _dvName;
    //LRect *preferRcView; /* set by outside */
    Layer *layerAttached;
private:
    LAxisRuler() {}

public:
    enum {TOP, LEFT, RIGHT, BOTTOM};
    INT GetRulerId() { return _rulerId; }

    LAxisRuler(UINT rulerId, LPCTSTR dvName = NULL) : _rulerId(rulerId), layerAttached(NULL), _dvName(dvName) {}

    void AttachLayer(Layer *layer, Layer **lastLayer = NULL)
    {
        if (lastLayer != NULL)
            *lastLayer = layerAttached;
        layerAttached = layer;
        _layers[0]->ai = layer->ai;
        _layers[0]->offset = layer->offset;
        // _dv = layer
        // _rulers[i]->SetData(_curLayer->QueryDV(dvName));
        _dv = layer->QueryDV(_dvName);
        SetRedrawFlags(REDRAW_PAINT | REDRAW_CALCULATE);
    }
    //VOID SetScrollRect(LRect& rc, int off = 0) 
    //{
    //    _off = off;
    //    _rcScroll = rc; 
    //    SetRedrawFlags(REDRAW_PAINT);
    //}

    VOID SetData(_DV *dv)
    {
        _dv = dv;
        SetRedrawFlags(REDRAW_PAINT);
    }

    virtual BOOL DrawLayer_old(Layer *layer, UINT& uDrawFlags)
    {
        LDC& dc = layer->dc;
        if (_dv == NULL)
            return FALSE;


            LFont ft(dc);
            ft.CreateFont(_T("ËÎÌו"), 10);
        if (_rulerId == BOTTOM)
        {

            LRect rc;
            LSTR str;
            GetLayerRect(rc);

            //int oldMM = dc.SetMapMode(MM_ANISOTROPIC);
            //// dc.SetViewportOrg(_rcView.left, _rcView.bottom); // screen
            //dc.SetViewportOrg(_off, 0);
            //dc.SetWindowOrg(0, 0); // logic
            //// dc.SetWindowExt(_curLayer->cx, -_curLayer->cy);
            //dc.SetWindowExt(_dv->Size(), rc.Height());
            //dc.SetViewportExt(rc.Width(), rc.Height());
            // dc.SetViewportExt(_rcView.Width(), _rcView.Height() / 3);

            dc.SetBkMode(TRANSPARENT);

            for (int i = 0; i < _dv->Size(); i++) {
                int h = i % 15 == 0 ? 10 : 5;
                //if (i == 200)
                //    h = 30;
                int x = i * (layer->ai.xwidth + 1) + layer->ai.xwidth / 2;
                dc.MoveTo(x, 0);
                dc.LineTo(x, h);
                if (h == 10) {
                    int dt = (*_dv)[i];
                    str.Format(_T("%d:%d/%d/%d"), i, (dt >> 8) & 0xff, dt & 0xff, dt >> 16);
                    dc.TextOut(x, h, str);
                }
            }
            //dc.TextOut(200, 5, _T("*200*"));

        //dc.SetMapMode(oldMM);
        //dc.SetWindowOrg(0, 0);
        //dc.SetViewportOrg(0, 0);

            return FALSE;
        }


        INT mx = _dv->GetMaxInt();
        INT mi = _dv->GetMinInt();

        //int oldMM = dc.SetMapMode(MM_ANISOTROPIC);
        //// dc.SetViewportOrg(_rcView.left, _rcView.bottom); // screen
        //dc.SetViewportOrg(0, _layers[0]->cy);
        //dc.SetWindowOrg(0, mi); // logic
        ////// dc.SetWindowExt(_curLayer->cx, -_curLayer->cy);
        //dc.SetWindowExt(_layers[0]->cx, _layers[0]->cy * -1);
        //dc.SetViewportExt(_layers[0]->cx, _layers[0]->cy);
        // dc.SetViewportExt(_rcView.Width(), _rcView.Height() / 3);

        dc.SetBkMode(TRANSPARENT);

        //LFont ft(dc);
        //ft.CreateFont(_T("Tahoma"), 12);

        INT t = (mx - mi) / 20;
        t = max(1, t);
        LSTR str;
        double scale = (double)_layers[0]->cy / (double)(mx - mi);
        for (INT i = mi; i <= mx; i += t) {
            str.Format(_T("%d"), i);
            int y = _layers[0]->cy - (int)((double)(i - mi) * scale);
            // y = _layers[0]->cy - y;
            // int y = (int)((float)i  / (float)(mi - 0));
            if (_rulerId == RIGHT) {
                dc.MoveTo(0, y);
                dc.LineTo(20, y);
                dc.TextOut(0, y, str);
            } /*else if (_rulerId == BOTTOM) {
                dc.MoveTo(i, 0);
                dc.LineTo(i, 20);
            }*/

        }
                //dc.MoveTo(0, 270);
                //dc.LineTo(50, 270);
                //dc.TextOut(0, 270, _T("* 270 *"));
        //_lines[0]->Draw(dc);

        //dc.SetMapMode(oldMM);
        //dc.SetWindowOrg(0, 0);
        //dc.SetViewportOrg(0, 0);
        return TRUE;
    }

    BOOL Redrawx(LDC& dc, UINT& uDrawFlags)
    {
        LSTR str;
        LRect rc;
        GetWindowText(str);
        //GetClientRect(rc);
        GetLayerRect(rc);
        //dc.TextOut(0, 0, str);
        LBrush br(dc);
        COLORREF r = rand() * 255;
        COLORREF g = rand() * 255;
        COLORREF b = rand() * 255;
        br.CreateSolidBrush(RGB(r, g, b));
        dc.FillRect(rc, br);
        //dc.DrawText(str, rc);
        /*dc.SetMapMode(MM_ANISOTROPIC);
        dc.SetWindowOrg(0, 0);
        dc.SetViewportOrg(rc.left, rc.bottom);
        dc.SetWindowExt(rc.Width(), rc.Height());
        dc.SetViewportExt(rc.Width(), -rc.Height());*/
        INT h;
        LFont ft(dc);
        ft.CreateFont(_T("Tahoma"), 12);
        dc.SetBkMode(TRANSPARENT);
        if (_rulerId == RIGHT || _rulerId == LEFT) {
            for (int i = 0; i < rc.Height(); i += 3) {
                h = i % 15 == 0 ? 10 : 5;
                dc.MoveTo(_rulerId == LEFT ? rc.right - h : 0, i);
                dc.LineTo(_rulerId == LEFT ? rc.right : h, i);
                if ((i % 30 == 0) && _rulerId == RIGHT) {
                    str.Format(_T("%d"), i);
                    dc.TextOut(10, i, str);
                }
            }
        } else {
            for (int i = 0; i < rc.Width(); i += 3) {
                h = i % 15 == 0 ? 10 : 5;
                dc.MoveTo(i, _rulerId == TOP ? rc.bottom - h : 0);
                dc.LineTo(i, _rulerId == TOP ? rc.bottom : h);
            }
        }
        return FALSE;
    }
    virtual BOOL DrawLayer(Layer *layer, UINT& uDrawFlags)
    {
        LDC& dc = layer->dc;

            LFont ft(dc);
            ft.CreateFont(_T("ËÎÌו"), 10);

        LRect rc(0, 0, layer->cx, layer->cy);
        //dc.Rectangle(rc);
        dc.SetBkMode(TRANSPARENT);

        if (layerAttached != NULL) {
            LSTR str;
            str.Format(_T("layerAttached %p, cx %d, cy %d, viewport(%d,%d)"), layerAttached, layerAttached->cx, layerAttached->cy,
                layer->viewPort.left, layer->viewPort.right);
            //dc.DrawText(str, rc);
        }
        if (layerAttached == NULL || _dv == NULL) {
            dc.DrawText(_T("no attached layer"), rc);
            return TRUE;
        }
        //GV* gv = layerAttached->GetFocusGV();
        //if (gv == NULL) {
        //    dc.DrawText(_T("gv is null"), rc);
        //    return TRUE;
        //}
        // _DV *dv = gv->GetDV(_dvName);
        //_DV *dv = layerAttached->GetDV(_dvName);
        rc = layer->viewPort;
        if (_rulerId == RIGHT || _rulerId == LEFT) {
            //int step = layer->cy / 100;
            //step = max(1, step);
            //for (int i = 0, j = 0; i < layer->cy; i += step, j++) {
            //    dc.MoveTo(0, i);
            //    dc.LineTo(5, i);
            //    if (j % 10 == 0) {
            //        LSTR str;
            //        str.Format(_T("%d"), i);
            //        dc.TextOut(7, i, str);
            //    }
            //}
            INT mx = _dv->GetMaxInt();
            INT mi = _dv->GetMinInt();
            INT t = (mx - mi) / 20;
            t = max(1, t);
            LSTR str;
            //double scale = (double)_layers[0]->cy / (double)(mx - mi);
            for (INT i = mi; i <= mx; i += t) {
                str.Format(_T("%d"), i);
                //int y = _layers[0]->cy - (int)((double)(i - mi) * scale);
                int y = layer->ai.d2y(i);
                // y = _layers[0]->cy - y;
                // int y = (int)((float)i  / (float)(mi - 0));

                if (_rulerId == RIGHT) {
                    dc.MoveTo(0, y);
                    dc.LineTo(20, y);
                    dc.TextOut(0, y, str);
                } /*else if (_rulerId == BOTTOM) {
                    dc.MoveTo(i, 0);
                    dc.LineTo(i, 20);
                }*/
            }
            //for (int i = 1; i < _dv->Size(); i++) {
            //    //int x = i * (ai.xwidth + 1) + ai.xwidth / 2;
            //    //int y = cy - (int)((double)(dv[i] - mi) * scale);
            //    //int x = ai.d2x(i);
            //    //int y = ai.d2y(dv[i]);
            //    int y = layer->ai.d2y((*_dv)[i]);
            //    dc.MoveTo(0, y);
            //    dc.LineTo(20, y);
            //    if (i % 5 == 0) {
            //        LSTR str;
            //        str.Format(_T("%d"), (*_dv)[i]);
            //        dc.TextOut(0, y, str);
            //    }
            //}
        } else {
            //int step = layer->cx / 100;
            //step = max(1, step);
            int step = 1;
            if (layer->ai.xwidth < 0)
                step = layer->cx / layer->ai.rc.Width();
            step = max(10, step);
                // step = 500;
            // for (int i = 0; i < _dv->Size(); i += step) {
            for (int i = _dv->dvs->x0, j = 0; i < _dv->dvs->xn; i += step, j++) {
                int h = j % 15 == 0 ? 10 : 5;
                //if (i == 200)
                //    h = 30;
                //int x = i * (layer->ai.xwidth + 1) + layer->ai.xwidth / 2;
                //dc.MoveTo(x, 0);
                //dc.LineTo(x, h);
                dc.MoveTo(layer->ai.d2x(i), 0);
                dc.LineTo(layer->ai.d2x(i), h);

                if (h == 10) {
                    int dt = (*_dv)[i];
                    LSTR str;
                    str.Format(_T("%d:%d/%d/%d"), i, (dt >> 8) & 0xff, dt & 0xff, dt >> 16);
                    dc.TextOut(layer->ai.d2x(i), h, str);
                }
            }
            //for (int i = 0, j = 0; i < layer->cx; i += step, j++) {
            //    dc.MoveTo(i, 0);
            //    dc.LineTo(i, 5);
            //    if (j % 10 == 0) {
            //        LSTR str;
            //        str.Format(_T("%d"), i);
            //        dc.TextOut(i, 5, str);
            //    }
            //}
        }
        return TRUE;
    }

    VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        LRect rcClient;
        GetClientRect(rcClient);

        if (layerAttached == NULL)
        {
            cy = rcClient.Height();// * (_rulerId == RIGHT || _rulerId == LEFT ? 2 : 1);
            cx = rcClient.Width();// * (_rulerId == TOP || _rulerId == BOTTOM ? 2 : 1);
            return;
        }

        cx = layerAttached->cx;//ai.rc.Width();
        cy = layerAttached->cy;//ai.rc.Height();

        if (_rulerId == RIGHT || _rulerId == LEFT)
            cx = rcClient.Width();
        else cy = cy = rcClient.Height();
        //if (_dv == NULL)
        //    return;
        //if (_rulerId == RIGHT || _rulerId == LEFT)
        //    cy += layer->ai.ydlt; // _rcScroll.Height();
        //else cx = _dv->Size() * (layer->ai.xwidth + 1) - 1; // _rcScroll.Width();
    }

    VOID CalcViewRegion(Layer *layer, LRect& rc)
    {
        if (layerAttached == NULL)
            return __super::CalcViewRegion(layer, rc);

        // if (layer->gv.Count() == 0)
        LRect rcClient;
        GetClientRect(rcClient);

        rc = layerAttached->viewPort;
        layerAttached->wnd->MapWindowPoints(this, &rc.lt, 2);
        //layerAttached->wnd->ClientToScreen(rc.lt);
        //layerAttached->wnd->ClientToScreen(rc.rb);
        //ScreenToClient(rc.lt);
        //ScreenToClient(rc.rb);
        if (_rulerId == RIGHT || _rulerId == LEFT) {
            rc.left = 0;
            rc.right = rcClient.Width();
        } else {
            rc.top = 0;
            rc.bottom = rcClient.Height();
        }
    }

    HWND CreateChild(LWnd *parent, LPCTSTR lpszWndName, RECT& rc, int nCtrlId = 0)
    {
        return __super::CreateChild(parent, lpszWndName, rc, nCtrlId, 0, LVS_NO_SCROLLBAR/*, WS_VISIBLE | WS_CHILD | WS_BORDER*/);
    }

    LRESULT OnMouseMove(DWORD dwFlags, LPoint& pt)
    {
        __super::OnMouseMove(dwFlags, pt);
        InvalidateRect(NULL, FALSE, TRUE);

        LVIEW_MSGSYNC_TRACKING mst;
        mst.mouse = pt;
        ClientToScreen(mst.mouse);
        SyncMessage(MOUSESYNC_MSG_LVIEW_TRACKING, &mst);

        return 0;
    }
    VOID OnMouseSyncTracking(LVIEW_MSGSYNC_TRACKING *mst, LWnd *sender)
    {
        //if (sender != GetParent())
        //    return;

        // /* do nothing now */
        _ptMouseMove = mst->mouse;
        ScreenToClient(_ptMouseMove);
        //if (_rulerId == RIGHT || _rulerId == LEFT) {
        //    _ptMouseMove.x = -1;
        //    _ptMouseMove.y -= mst->rcFocus.top;
        //    //_ptMouseMove.y += _rcView.top;
        //} else {
        //    _ptMouseMove.x -= mst->rcFocus.left;
        //    //_ptMouseMove.x += _rcView.left;
        //    _ptMouseMove.y = -1;
        //}
        InvalidateRect(NULL, FALSE, TRUE);
    }

    VOID OnMouseSyncZoom(LVIEW_MSGSYNC_ZOOM *msz, LWnd *sender)
    {
        // ZoomView(msz);
        if (msz->cy != 0) {
            //_dltHeight += msz->cy; // (cy > 0 ? 50 : -50);
            _curLayer->offset.y += msz->canvasOff.y; // (cy > 0 ? -50 : 50);
        } 
        if (msz->cx != 0) {
            //int oldWidth = _width;
            //_width += msz->cx; // (cx > 0 ? 1 : -1);
            //_width = max(1, _width);
            //_width = min(20, _width);
            //if (_width == oldWidth)
            //    return;
            int old = _curLayer->ai.xwidth;
            _curLayer->ai.xwidth += msz->cx;
            _curLayer->ai.xwidth = max(0, _curLayer->ai.xwidth);
            _curLayer->ai.xwidth = min(20, _curLayer->ai.xwidth);
            if (_curLayer->ai.xwidth == old)
                return;
#if 0
                LPoint pos = msz->mouse;
                layer->ClientToLayer(pos);
                // pos = rc.left + data * (xwidth + 1) + xwidth / 2;
                // 
                // if (layer->ai.xwidth > 0) {
                //int off = pos.x - layer->ai.rc.left - xwidth_old / 2;
                //off /= (xwidth_old + 1);
                int xpos = layer->ai.x2d(pos.x);
                layer->offset.x += xpos * (msz->cx > 0 ? 1 : -1);
                // }
                //int t0 = layer->ai.d2x(xpos);
                int t = layer->ai.d2x(xpos);
                layer->offset.x += (t - pos.x);//t1 / layer->ai.xwidth;
#endif

            // _ptDragDlt.x = zDelta > 0 ? -__hoverX : __hoverX;
            //if (nFlags & 0x8000)
            //    __hoverX = pt.x;
            _curLayer->offset.x = msz->canvasOff.x; // (cx > 0 ? -canvasOffX : canvasOffX);
        }
        SetRedrawFlags(REDRAW_PAINT | REDRAW_CALCULATE, TRUE);
    }

    VOID DrawTrackingLayer(LDC& dc)
    {
        LRect rc = _curLayer->viewPort;
        if (_ptMouseMove.x >= rc.left && _ptMouseMove.x <= rc.right
            || _ptMouseMove.y >= rc.top && _ptMouseMove.y <= rc.bottom) {
            LPen pen(dc);
            pen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
            if (_rulerId == RIGHT || _rulerId == LEFT) {
                dc.MoveTo(0, _ptMouseMove.y);
                dc.LineTo(_rcClient.right, _ptMouseMove.y);
            } else {
                dc.MoveTo(_ptMouseMove.x, 0);
                dc.LineTo(_ptMouseMove.x, _rcClient.bottom);
            }
        }
    }
};

class LDateAxisRuler : public LAxisRuler
{
public:
    virtual BOOL DrawLayer(Layer *layer, UINT& uDrawFlags)
    {
        if (_dv == NULL)
            return FALSE;

        return TRUE;
    }

};

//class AxisLine
//{
//public:
//    Line *line;
//    Layer *layer;
//
//    AxisLine() : AxisLine(NULL, NULL) {}
//    AxisLine(Line *line, Layer *layer)
//        :line(line), layer(layer) {}
//
//    AxisLine(AxisLine& al)
//    {
//        line = al.line;
//        layer = al.layer;
//    }
//};
//LRect _rcLastZoomed;
class LAxisView : public LWndView<LAxisView>
{
    LArray<LAxisRuler*> _rulers;
    //LArray<AxisLine> _lines;

    //int _width;
    //ScaleInfo _scaleInfo;

    //Layer *_testLayer;
public:
    LAxisView() 
    {
        //_width = VERTEX_W;
        //_scaleInfo.xScale = 10.0f;
        _dltHeight = 0;
    }

    VOID CalcAxisArea(LRect& rcAxis)
    {
        LRect rc;
        INT off[4] = {0};

        for (INT i = 0; i < _rulers.Count(); i++) {
            _rulers[i]->GetWindowRect(rc);
            INT rulerId = _rulers[i]->GetRulerId();
            if (rulerId == LAxisRuler::RIGHT || rulerId == LAxisRuler::LEFT)
                off[rulerId] += rc.Width();
            else off[rulerId] += rc.Height();
        }

        GetClientRect(rcAxis);
        rcAxis.top += off[LAxisRuler::TOP];
        rcAxis.bottom -= off[LAxisRuler::BOTTOM];
        rcAxis.left += off[LAxisRuler::LEFT];
        rcAxis.right -= off[LAxisRuler::RIGHT];
        rcAxis.bottom = max(rcAxis.bottom, rcAxis.top);
        rcAxis.right = max(rcAxis.right, rcAxis.left);
    }

    VOID CalcViewRegion(Layer *layer, LRect& rc)
    {
        //int x = 0, y = 0, dltX, dltY;

        if (layer->gv.Count() == 0)
            return __super::CalcViewRegion(layer, rc);

        CalcAxisArea(rc);
        LRect rc0 = rc;
        // CenterCanvas(rc, layer->cx, layer->cy);
        //rc.Center(layer->cx, layer->cy);
        //rc.Center(_rcClient.Width(), _rcClient.Height());

        LRect rcLayer = {0, 0, layer->cx, layer->cy};
        // rc.Intersection(rcLayer);
        if (rc.Height() > layer->cy)
            rc.bottom = rc.top + layer->cy;
        rc.Center(0, rc0.Height());

            if (_tcsstr(layer->name, _T("floating"))) {
                CalcAxisArea(rc);
                rc.InflateRect(-10, -10);
                if (0) {
                    rc.left = rc.right - 500;
                    rc.bottom = 300;//rc.top + 200;
                } else {
                    rc.left = rc.right - layer->cx;
                    rc.bottom = rc.top + layer->cy;
                }
            }
        if (layer->id == Layer::EAGLE_EYE) {
            CalcAxisArea(rc);
            rc.top = rc.bottom - layer->cy;
            //rc.Offset(0, -20);
        }
        //if ((dltX = rc.Width() - layer.cx) > 0) {
        //    rc.right = rc.left + layer.cx;
        //    x = dltX / 2;
        //}

        //if ((dltY = rc.Height() - layer.cy) > 0) {
        //    rc.bottom = rc.top + layer.cy;
        //    y = dltY / 2;
        //}

        //rc.Offset(x, y);

        //INT mx = _lines[0]->_dv->GetMaxInt();
        //INT mi = _lines[0]->_dv->GetMinInt();
        //_scaleInfo.yScale = (float)_curLayer->cy / (float)(mx - mi);
        //_scaleInfo.origin.x = (int)(_scaleInfo.xScale / 2);
        //_scaleInfo.origin.y = _curLayer->cy;
    }

    VOID CalcScrollRegion(Layer *layer, LRect& rcScroll, SIZE& dltSize)
    {
        CalcAxisArea(rcScroll);
    }

    int _dltHeight;
    //VOID CalcLayerSize(int& cx, int& cy)
    //{
    //    LRect rc;
    //    CalcAxisArea(rc);
    //    cx = _lines[0]->Size() * (int)_scaleInfo.xScale;
    //    cy = rc.Height() /** (int)_scaleInfo.yScale*/ + _dltHeight;
    //}

    VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        if (layer->gv.Count() == 0)
            return __super::CalcLayerSize(layer, drawFlags, cx, cy);

        LRect rc;
        CalcAxisArea(rc);

        int /*t = 0, */dmin = INT_MAX, dmax = INT_MIN, xmin = INT_MAX, xmax = INT_MIN;
        int extraMax = 0;
        for (int i = 0; i < layer->gv.Count(); i++)
        {
            //t = max(t, layer->gv[i]->Size(layer->id == Layer::EAGLE_EYE));
            dmin = min(dmin, layer->gv[i]->GetMinInt(layer->id == Layer::EAGLE_EYE));
            dmax = max(dmax, layer->gv[i]->GetMaxInt(layer->id == Layer::EAGLE_EYE));

            if (layer->id == Layer::EAGLE_EYE) {
                xmin = min(xmin, 0);
                xmax = max(xmax, layer->gv[i]->Size(TRUE));
                FilledLine *fl = (FilledLine *)layer->gv[i];
                extraMax = max(extraMax, fl->extraBottom);
            } else {
                _DV* dv = layer->gv[i]->GetDV(NULL);
                xmin = min(xmin, dv->dvs->x0);
                xmax = max(xmax, dv->dvs->xn);
            }
        }

        cx = rc.Width();
        cy = rc.Height();

        if (layer->id == Layer::EAGLE_EYE) {
            cy /= 3;
            cy += extraMax;
            layer->ai.xwidth = -1; /* auto-stretch to fit cx */
        } else {
            cx = (xmax - xmin) * (layer->ai.xwidth + 1) - 1;
            cy = rc.Height() + layer->ai.ydlt;
            //if (cy < 0)
            //    cy = -layer->ai.ydlt;
        }

            // if (layer->transparency != 0 && layer->transparency != 100) {
            if (_tcsstr(layer->name, _T("floating"))) {
                cy = 200;
                cx = 500;
                layer->ai.xwidth = -1;
            }

        // set the AxisInfo here
        layer->ai.rc = LRect(0, 0, cx, cy - extraMax);
        //layer->ai.rc.bottom -= extraMax;
        //layer->ai.cy = cy;
        layer->ai.y.dmin = dmin;
        layer->ai.y.dmax = dmax/* + extraMax*/;
        layer->ai.x.dmin = xmin;
        layer->ai.x.dmax = xmax;

        //if (layer->id == Layer::EAGLE_EYE) {
        //    layer->ai.rc.bottom -= 30;
        //    //layer->ai.rc.top = layer->ai.rc.bottom / 3;
        //    //cy = layer->ai.rc.Height();
        //}
        //else
        //    CalcLayerSize(cx, cy);
        //if (layer->id == 555) {
        //    cy = 300;
        //    cx = 500;
        //}
    }
    //LRect __lastRcView;
    //VOID AdjustLayerDrawOffset(Layer *layer, LPoint& pt)
    //{
    //    // __super::AdjustLayerDrawOffset(layer, pt);
    //    // pt.y = 0;
    //    if (pt.x < 0) {
    //        _rcView.left = -pt.x + __lastRcView.left;
    //        pt.x = 0;
    //    }
    //    if (pt.y < 0) {
    //        _rcView.top = -pt.y;
    //        pt.y = 0;
    //    }
    //}

    VOID AddRuler(LAxisRuler *ruler)
    {
        //_ASSERT(rulerId < 4);
        _ASSERT(ruler != NULL);
        _rulers.Add(ruler);
        AddMsgSync(ruler);
        //AddMouseSync(ruler);

        //ruler->_wndDragSync = _wndDragSync; // XXX
    }
    VOID AddLine(Line *line, Layer *layer = NULL)
    {
        if (layer == NULL) {
            _layers[0]->gv.Add(line);
            return;
        }
        layer->gv.Add(line);
        // _lines.Add(AxisLine(line, layer ? layer : _layers[0]));
    }
    Layer* FindLayer(LPCTSTR layerName)
    {
        return NULL;
    }
    Layer* FindLayer(int layerId)
    {
        for (int i = 0; i < _layers.Count(); i++)
            if (_layers[i]->id == layerId)
                return _layers[i];
        return NULL;
    }
    Layer* CreateLayer(LPCTSTR layerName, COLORREF clrBkgrnd = RGB(1, 1, 1), INT transparency = 100)
    {
        Layer *layer = new Layer(Layer::USER + _layers.Count(), clrBkgrnd, layerName, transparency);
        _layers.Add(layer);
        //layer->offset.x = LONG_MAX; /* show the most right item */
        return layer;

    }
    void AddLine(Line *line, LPCTSTR layerName)
    {
        Layer *layer = FindLayer(layerName);
        if (layer == NULL) {
            layer = CreateLayer(layerName);
        }
        AddLine(line, layer);
    }
    void AddLine(Line *line, int layerId)
    {
        AddLine(line, FindLayer(layerId));
    }
    //void addtestlayer()
    //{
    //    _testLayer = new Layer(555, RGB(255, 0, 0), _T("Eagle-Eye"));
    //    _layers.Add(_testLayer);

    //    _testLayer = new Layer(666);
    //    _layers.Add(_testLayer);
    //}
    void SetActiveLayer(Layer *layer)
    {
        //_curLayer = layer;
        //InvalidateRect(NULL, FALSE);
        __super::SetActiveLayer(layer);
        // SetActiveLayer(_layers[i]);
        //if (_lines.Count() > 2) {
        //    _rulers[3]->SetData(_layers[i]->id == 666 ? _lines[2]->_dv : _lines[0]->_dv);
        //    UpdateRulerScrollRect();
        //}
        //::SendMessage(*GetParent(), WM_LWIN_ACTIVE_LAYER_CHANGED, 0, (LPARAM)_curLayer);
        for (INT i = 0; i < _rulers.Count(); i++) {
            //_rulers[i]->SetData(_curLayer->QueryDV(dvName));
            _rulers[i]->AttachLayer(_curLayer);
        }

    }
    LRESULT OnLButtonDblClk (DWORD dwFlags, LPoint& pt)
    {
        static int i = 0;
        i++;
        i %= _layers.Count();
        SetActiveLayer(_layers[i]);

        return 0;
    }
    int _MouseMove = 0;
    Layer* LayerAtPt(LPoint& pt)
    {
        // todo: 
        return _curLayer;
    }
    LRESULT OnMouseMove(DWORD dwFlags, LPoint& pt)
    {
        _MouseMove++;
        __super::OnMouseMove(dwFlags, pt);

        // if (!_rcView.PtInRect(pt) || _eDragMode == LVIEW_DRAG_SELECTION)
        if (!LayerAtPt(pt) || _eDragMode == LVIEW_DRAG_SELECTION)
            return 0;


        //if (!(dwFlags & 0x80000000))  {
        //    pt.x -= _rcView.left;
        //    pt.y -= _rcView.top;
        //
        ////for (INT i = 0; i < _rulers.Count(); i++) {
        //    InvalidateRect(NULL, FALSE); //  if mouse tracking enable
        //
        //    SyncMessage(WM_MOUSEMOVE, dwFlags | 0x80000000, MAKELPARAM(pt.x, pt.y));
        ////}
        //} else {
            //_ptMouseMove.x += _rcView.left;
            //_ptMouseMove.y += _rcView.top;
        InvalidateRect(NULL, FALSE, TRUE); //  if mouse tracking enable
        // RedrawWindow (m_hWnd, NULL, NULL, /*RDW_INTERNALPAINT |  */RDW_INVALIDATE |RDW_UPDATENOW);

        //}
        //return 0;

        LVIEW_MSGSYNC_TRACKING mst;
        mst.mouse = pt;
        ClientToScreen(mst.mouse);
        //mst.rcFocus = _rcView;
        SyncMessage(MOUSESYNC_MSG_LVIEW_TRACKING, &mst);

        //for (INT i = 0; i < _rulers.Count(); i++)
        //    SyncMessage(MOUSESYNC_MSG_LVIEW_TRACKING, &mst, _rulers[i]);

        return 0;
    }
    int __hoverX;
    VOID DrawTrackingLayer(LDC& dc/*, const LRect& rc*/)
    {
        LSTR str;
        LRect rc = _rcClient;//_layers[0]->viewPort;
        LFont ft(dc);
        CalcAxisArea(rc);
        rc.InflateRect(-20, -20);
        ft.CreateFont(_T("ËÎÌו"), 13);
        str.Format(_T("_MouseMove %d(%d,%d), _SyncTracking %d"), _MouseMove, _ptMouseMove.x, _ptMouseMove.y, _SyncTracking);
        dc.DrawText(str, rc, DT_LEFT | DT_TOP);
        for (int i = 0; i < _layers.Count(); i++)
        {
                LPoint pos = _ptMouseMove;
                _layers[i]->ClientToLayer(pos);
                // pos = rc.left + data * (xwidth + 1) + xwidth / 2;
                // 
                // if (layer->ai.xwidth > 0) {
                int xPos = _layers[i]->ai.x2d(pos.x);
                //int xPos = pos.x - _layers[i]->ai.rc.left - _layers[i]->ai.xwidth / 2;
                //xPos /= (_layers[i]->ai.xwidth + 1);
                // layer->offset.x += off * (msz->cx > 0 ? 1 : -1);
            int yvalue = -1;
            if (_layers[i]->gv.Count() > 0 && _layers[i]->gv[0] != NULL) {
                _DV *dv = _layers[i]->gv[0]->GetDV(NULL);
                if (i != 1)
                    xPos += dv->dvs->x0;
                if (xPos >= 0 && xPos < dv->Size(TRUE))
                    yvalue = dv->GetInt(xPos);
            }
            str.Format(_T("\r\n[%s]#%d: %s (id %d, lines %d, offset(%d,%d),size(%d X %d), xwidth %d), xpos %d -> %d"), 
                _curLayer == _layers[i] ? "X" : "-", i, 
                (LPCTSTR)_layers[i]->name, _layers[i]->id, _layers[i]->gv.Count(),
                _layers[i]->offset.x, _layers[i]->offset.y, _layers[i]->cx, _layers[i]->cy, _layers[i]->ai.xwidth,
                xPos, yvalue);
                // _layers[i]->ai.d2x(xPos), pos.x);
            dc.DrawText(str, rc, DT_LEFT | DT_TOP);
            rc.Offset(0, 20);
        }
        
        LPoint pt = _ptMouseMove;
        //if (_rcView.PtInRect(_ptMouseMove)) {

        rc = _curLayer->viewPort;
        if (_eDragMode != LVIEW_DRAG_SELECTION) {
            LPen pen(dc);
            pen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));

            if (pt.x >= rc.left && pt.x <= rc.right) {
                dc.MoveTo(_ptMouseMove.x, 0);
                dc.LineTo(_ptMouseMove.x, _rcClient.bottom);
            }
            if (pt.y >= rc.top && pt.y <= rc.bottom) {
                dc.MoveTo(0, _ptMouseMove.y);
                dc.LineTo(_rcClient.right, _ptMouseMove.y);
            }
        } else {
            LBrush br(dc);
            LRect rc(_ptMouseDown, _ptMouseMove);
            br.CreateStockObject(NULL_BRUSH);
            dc.Rectangle(rc);

            LSTR str;
            str.Format(_T("%d,%d %d,%d"), rc.left, rc.top, rc.right, rc.bottom);
            ///dc.SetBkMode(OPAQUE);
            dc.SetTextColor(RGB(255, 0, 0));
            COLORREF clrBk = dc.SetBkColor(RGB(255, 255, 250));
            dc.DrawText(str, rc);
            dc.SetTextColor(RGB(0, 0, 0));
            dc.SetBkColor(clrBk);

        }

        {
            LBrush br(dc);
            LPen pen(dc);
            pen.CreatePen(PS_SOLID, 1, clrZoom);
            br.CreateStockObject(NULL_BRUSH);

            if (rcZoom.Width() > 0)
                dc.Rectangle(rcZoom);
        }
#ifdef xxxx
        //}
        //return;
        if (_lines.Count() == 0)
            return;
        if (1)
        {
            //Line lineOverview(_lines[0]->_dv);
        INT mx = _lines[0]->_dv->GetMaxInt();
        INT mi = _lines[0]->_dv->GetMinInt();

        int oldMM = dc.SetMapMode(MM_ANISOTROPIC);
        dc.SetViewportOrg(_rcView.left, _rcView.bottom); // screen
        dc.SetWindowOrg(0, mi - 0); // logic
        // dc.SetWindowExt(_curLayer->cx, -_curLayer->cy);
        // dc.SetWindowExt(_lines[0]->_dv->Size(), -(mx - mi));
        dc.SetWindowExt(_layers[0]->cx, -(mx - mi));
        dc.SetViewportExt(_rcView.Width(), _rcView.Height() / 3);

        float scaleX = (float)_layers[0]->cx / (float)_lines[0]->_dv->Size();
        float scaleY = (float)(_rcView.Height() + _dltHeight) / (float)(mx - mi);
            LPoint pt = _ptMouseMove;
            ClientToLayer(_layers[0], pt);

            __hoverX = (int)((float)(pt.x/* - _width / 2*/) / scaleX);
            __hoverX = min(__hoverX, _lines[0]->_dv->Size() - 1);
            //int y = (int)((float)(_curLayer->cy - pt.y) / scaleY);
            //__hoverX = x;

        // Line *ll = 
        Line ll(_lines[0]->_dv, RGB(200, 200, 200));
        // _lines[0]->Draw(dc);
        ll.Draw(dc);
        int x = __hoverX * (int)_scaleInfo.xScale;
        int y = _lines[0]->_dv->GetInt(__hoverX);
        //dc.MoveTo(x, y + (int)((float)50.0 / scaleY));
        //dc.LineTo(x, y);
        dc.TextOut(x, y, _T("*"));
        //dc.LineTo(x + (int)((float)25.0 / scaleY), y + (int)((float)25.0 / scaleY));
        // dc.TextOut(__hoverX * _width, _lines[0]->_dv->GetInt(__hoverX), _T("#"));

        dc.SetMapMode(oldMM);
        dc.SetWindowOrg(0, 0);
        dc.SetViewportOrg(0, 0);
        }
#endif
        {
            LPoint pos = _ptMouseMove;
            Layer *eagle = _layers[1];
            _layers[0]->ClientToLayer(pos);
            pos.x = max(0, pos.x);
            pos.x = min(_layers[0]->ai.rc.right, pos.x);
            if (eagle->gv.Count() > 0/* && pos.x >= 0*/) {
                // pos = rc.left + data * (xwidth + 1) + xwidth / 2;
                // 
                // if (layer->ai.xwidth > 0) {
                int xPos = _layers[0]->ai.x2d(pos.x);
                _DV *dv = eagle->gv[0]->GetDV(NULL);
                LPoint pos2;
                xPos += dv->dvs->x0;
                pos2.x = eagle->ai.d2x(xPos);
                // eagle->ClientToLayer(pos2);
                //pos2.x -= eagle->viewPort.left;
                pos2.x += eagle->viewPort.left;
                //pos2.x += 1;
                //dc.MoveTo(pos2.x, /*_rcClient.bottom - */30);
                int ypos = eagle->ai.d2y((*dv)[xPos]);
                ypos += eagle->viewPort.top;
                ypos += 1; /* move down one pixel */
                LPen pen(dc);
                pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                dc.MoveTo(pos2.x, ypos);
                dc.LineTo(pos2.x, _rcClient.bottom);
            }
        }
#if 0
        if (1/* && _rcView.PtInRect(_ptMouseMove)*/)
        {
            INT mx = _lines[0]->_dv->GetMaxInt();
            INT mi = _lines[0]->_dv->GetMinInt();

            float scaleX = (float)_curLayer->cx / (float)_lines[0]->_dv->Size();
            float scaleY = (float)(_curLayer->cy) / (float)(mx - mi);
                LSTR str;
            LPoint pt = _ptMouseMove;
            _curLayer->ClientToLayer(pt);

            int x = __hoverX;// (int)((float)(pt.x/* + _width / 2*/) / scaleX);
            int y = (int)((float)(_curLayer->cy - pt.y) / scaleY);
            //__hoverX = x;
            y += mi;
            //pt.x -= _rcView.left;
            //pt.y -= _rcView.top;
            str.Format(_T("scale(%.2f,%.2f) mouse(%d,%d of %d,%d) x %d, y %d, value %d, wheel %d, points %d, layer %s, _SyncTracking %d, _MouseMove %d"), 
                scaleX, scaleY, 
                pt.x, pt.y, _ptMouseMove.x, _ptMouseMove.y,
                x, y, _lines[0]->_dv->GetInt(x), xxxxxx, _lines[0]->_dv->Size(), (LPCTSTR)_curLayer->name, _SyncTracking, _MouseMove);
            dc.TextOut(20, 50, str);
        }
#endif
    }

    LRESULT OnCreate(LPCREATESTRUCT lpcs) 
    {
        __super::OnCreate(lpcs);
        //LAxisRuler *ruler = new LAxisRuler();
        //RECT rc = {0, 0, 100, 100};
        //ruler->CreateChild(this, _T("LAxisRuler BOTTOM"), rc, 0, 0, LVS_NO_SCROLLBAR);
        //rules[BOTTOM].Add(ruler);
        //
        //if (_wndDragSync != NULL) {
        //    _wndDragSync->Add(this);
        //    _wndDragSync->Add(rules[BOTTOM][0]);
        //}
        //AddMouseSync(this);

        //HDC hdc = GetDC();
        Layer *layer = new Layer(Layer::EAGLE_EYE, RGB(255, 255, 255), _T("Eagle Eye"), 100);
        layer->Create(this, lpcs->cx, lpcs->cy);
        _layers.Add(layer);
        //ReleaseDC(hdc);

        //_uDragFlags |= LVIEW_DRAGDROP_ENABLE;
        return 0; 
    }
    //VOID UpdateRulerScrollRect()
    //{
    //    int cx, cy;
    //    LRect rcCanvas;

    //    CalcLayerSize(_curLayer, 0, cx, cy);
    //    rcCanvas.right = cx;
    //    rcCanvas.bottom = cy;
    //    for (INT i = 0; i < _rulers.Count(); i++) {
    //        _rulers[i]->SetScrollRect(rcCanvas);
    //    }
    //}
    VOID UpdateRulerPosition(LRect& rcAxis)
    {
        LRect rc, rcRuler;// = _rcDraw;//_rcClient;
        INT off[4] = {0};
        //CalcAxisArea(rcAxis);

        //int cx0, cy0;
        //CalcLayerSize(cx0, cy0);
        //rcCanvas.right = cx0;
        //rcCanvas.bottom = cy0;
        // CalcViewRegion
        //rc.bottom += 22;
        //rc.top = rc.bottom - 20;
        //rules[BOTTOM][0]->MoveWindow(rc);
        //off.top = 0;
        //off.left = 0;
        off[LAxisRuler::BOTTOM] = rcAxis.bottom;
        off[LAxisRuler::RIGHT] = rcAxis.right;

        // CalcLayerSize()
        //for (INT rulerId = 0; rulerId < 4; rulerId++) {
        for (INT i = 0; i < _rulers.Count(); i++) {
            rc = rcAxis;
            _rulers[i]->GetWindowRect(rcRuler);
            INT rulerId = _rulers[i]->GetRulerId();
            switch (rulerId) {
            case LAxisRuler::TOP:
            case LAxisRuler::BOTTOM:
                rc.top = off[rulerId];
                rc.bottom = rc.top + rcRuler.Height();
                off[rulerId] += rcRuler.Height();
                break;
            case LAxisRuler::RIGHT:
            case LAxisRuler::LEFT:
                rc.left = off[rulerId];
                rc.right = rc.left + rcRuler.Width();
                off[rulerId] += rcRuler.Width();
                break;
            }
            // rc.InflateRect(-2, -2);
            // rc.top += 1;
            //_rulers[i]->SetScrollRect(rcCanvas);
            _rulers[i]->MoveWindow(rc);
        }
    }
    LRESULT OnSize(UINT nType, INT cx, INT cy)
    {
        __super::OnSize(nType, cx, cy);
        LRect rcAxis;// = _rcDraw;//_rcClient;
        //INT off[4] = {0};
        CalcAxisArea(rcAxis);

        //int cx0, cy0;
        //CalcLayerSize(cx0, cy0);
        //rcCanvas.right = cx0;
        //rcCanvas.bottom = cy0;
        // CalcViewRegion
        //rc.bottom += 22;
        //rc.top = rc.bottom - 20;
        //rules[BOTTOM][0]->MoveWindow(rc);
        //off.top = 0;
        //off.left = 0;
        //off[LAxisRuler::BOTTOM] = rcAxis.bottom;
        //off[LAxisRuler::RIGHT] = rcAxis.right;

        //// CalcLayerSize()
        ////for (INT rulerId = 0; rulerId < 4; rulerId++) {
        //for (INT i = 0; i < _rulers.Count(); i++) {
        //    rc = rcAxis;
        //    _rulers[i]->GetWindowRect(rcRuler);
        //    INT rulerId = _rulers[i]->GetRulerId();
        //    switch (rulerId) {
        //    case LAxisRuler::TOP:
        //    case LAxisRuler::BOTTOM:
        //        rc.top = off[rulerId];
        //        rc.bottom = rc.top + rcRuler.Height();
        //        off[rulerId] += rcRuler.Height();
        //        break;
        //    case LAxisRuler::RIGHT:
        //    case LAxisRuler::LEFT:
        //        rc.left = off[rulerId];
        //        rc.right = rc.left + rcRuler.Width();
        //        off[rulerId] += rcRuler.Width();
        //        break;
        //    }
        //    // rc.InflateRect(-2, -2);
        //    // rc.top += 1;
        //    //_rulers[i]->SetScrollRect(rcCanvas);
        //    _rulers[i]->MoveWindow(rc);
        //}
        //}
        UpdateRulerPosition(rcAxis);
        //UpdateRulerScrollRect();
        return 0;
    }

    BOOL DrawLayer(Layer *layer, UINT& uDrawFlags)
    {
        LDC& dc = layer->dc;

        if (layer->id == 555) {
            //dc.TextOut(0, 0, _T("Lyer 555"));
            LRect rc = layer->viewPort;
            int y = rc.top + rc.Height() / 2;
            LPen pen(dc);
            pen.CreatePen(PS_DOT, 1, RGB(255, 0, 0));
            dc.MoveTo(rc.left, y);
            dc.LineTo(rc.right, y);
            int N = 10;
            int x = rc.left;
            int t = rc.Width() / N;
            for (int i = 1; i < N; i++) {
                dc.MoveTo(x + i * t, rc.top);
                dc.LineTo(x + i * t, rc.bottom);
            }
            return TRUE;
        }
#if 0
        if (layer->id == 666 && _lines[2] != NULL) {
            INT mx = _lines[2]->_dv->GetMaxInt();
            INT mi = _lines[2]->_dv->GetMinInt();
            int oldMM = dc.SetMapMode(MM_ANISOTROPIC);
            dc.SetViewportOrg(0, layer->cy);
            dc.SetWindowOrg(0, mi - 0);
            // dc.SetWindowExt(_curLayer->cx, -_curLayer->cy);
            dc.SetWindowExt(layer->cx, -(mx - mi));
            // dc.SetWindowExt(_lines[0]->_dv->Size(), -(mx - mi));
            // dc.SetViewportExt(_curLayer->cx, _rcView.Height()/* + _dltHeight*/);
            dc.SetViewportExt(layer->cx, layer->cy);
            _lines[2]->Draw(dc);
            dc.SetMapMode(oldMM);
            dc.SetWindowOrg(0, 0);
            dc.SetViewportOrg(0, 0);
            return TRUE;
        }
#endif
        if (layer->gv.Count() == 0) {
            //LSTR str;
            //str.Format(_T("No line on layer %s %s"), (LPCTSTR)layer->name, "s");
            //dc.TextOut(0, 0, str);
            return TRUE;
        }

        // layer->ai.cy = layer->cy; // XXXX
        for (int i = 0; i < layer->gv.Count(); i++)
        {
            //AxisLine& al = _lines[i];
            //if (al.layer != layer)
            //    continue;
            GV *gv = layer->gv[i];

#if 0
            INT mx = gv->GetMaxInt();
            INT mi = gv->GetMinInt();
            int oldMM = dc.SetMapMode(MM_ANISOTROPIC);
            // dc.SetViewportOrg((int)(_scaleInfo.xScale / 2), _curLayer->cy/* - (int)(_scaleInfo.yScale / 2)*/);

            if (layer->id == Layer::EAGLE_EYE) {
                dc.SetViewportOrg(0, layer->cy); // screen
                dc.SetWindowOrg(0, mi - 0); // logic
                // dc.SetWindowExt(_curLayer->cx, -_curLayer->cy);
                // dc.SetWindowExt(_lines[0]->_dv->Size(), -(mx - mi));
                dc.SetWindowExt(gv->Size(), -(mx - mi));
                dc.SetViewportExt(layer->viewPort.Width(), layer->viewPort.Height() / 3);
            } else {
                _scaleInfo.origin.x = (int)(_scaleInfo.xScale / 2);
                _scaleInfo.origin.y = layer->cy;

                dc.SetViewportOrg(_scaleInfo.origin);
                dc.SetWindowOrg(0, mi);
                // dc.SetWindowExt(_curLayer->cx, -_curLayer->cy);
                // _scaleInfo.yScale = _curLayer->cy / (mx - mi);

                //dc.SetWindowExt(1, -1);
                //dc.SetViewportExt((int)_scaleInfo.xScale, (int)_scaleInfo.yScale);

                dc.SetWindowExt(gv->Size(), -(mx - mi));
                dc.SetViewportExt(layer->cx, layer->cy);
            }
#endif
            //for (int i = 0; i < _lines.Count(); i++)
            gv->Draw(dc, layer->ai);

            //dc.SetMapMode(oldMM);
            //dc.SetWindowOrg(0, 0);
            //dc.SetViewportOrg(0, 0);
        }

        //if (layer->id == Layer::EAGLE_EYE) {
        //    LBrush br(dc);
        //    br.CreateSolidBrush(RGB(255, 0, 0));
        //    dc.FrameRect(layer->ai.rc, br);
        //}

        return TRUE;
    }
    int xxxxxx = 0;
    LRESULT OnMouseWheel(UINT nFlags, SHORT zDelta, LPoint& pt) 
    {
        LVIEW_MSGSYNC_ZOOM msz = {0};
#if 0
        xxxxxx++;
        //INT cx = 0, cy = 0;
        //LPoint canvasOff;

        if (nFlags & MK_CONTROL) {
            msz.cy = (zDelta > 0 ? 50 : -50);
            //msz.canvasOff.y = (zDelta > 0 ? -31 : 31);
            //msz.canvasOff.y /= 2;
            _curLayer->ClientToLayer(pt);
            // msz.canvasOff.y = (int)((float)msz.cy * ((float)pt.y / (float)_curLayer->cy));
            {
                INT mx = _lines[0]->_dv->GetMaxInt();
                INT mi = _lines[0]->_dv->GetMinInt();
                float scaleY = (float)_curLayer->cy / (float)(mx - mi);
                // int y = _lines[0]->_dv->GetInt(__hoverX);

                /* get y-value at mouse */
                LPoint ptMouse = _ptMouseMove;
                _curLayer->ClientToLayer(ptMouse);
                int y = (int)((float)(_curLayer->cy - ptMouse.y) / scaleY) + mi;

                /* get y-position within layer at value-y */
                float scaleY0 = (float)_curLayer->cy / (float)(mx - mi);
                int yPos0 = _curLayer->cy - (int)((float)(y - mi)* scaleY0);

                float scaleY1 = (float)(_curLayer->cy + msz.cy) / (float)(mx - mi);
                int yPos1 = (_curLayer->cy + msz.cy) - (int)((float)(y - mi)* scaleY1);

                msz.canvasOff.y = yPos1 - yPos0;
                 // y - mi = (int)((float)(_curLayer->cy - yPos) / scaleY);
                
            //__hoverX = x;

                //LPoint pt0 = pt;
                //ClientToLayer(&_curLayer, pt0);

                // int y = (int)((float)(_curLayer->cy - pty) / scaleY);
                // canvasto
                //int pty = (_curLayer->cy + msz.cy) - (int)((float)y * scaleY);
                //msz.canvasOff.y = pt0.y - yPos;

            }
            //msz.canvasOff.y *= -1;
            ZoomView(&msz);
            if (!(nFlags & MK_SHIFT)) {
                for (INT i = 0; i < _rulers.Count(); i++)
                    SyncMessage(MOUSESYNC_MSG_LVIEW_ZOOM, &msz, _rulers[i]);
                return 0;
            }
            /* else: zoom on width also */
        }

#endif
        if (nFlags & MK_CONTROL)
            msz.cy = (zDelta > 0 ? 50 : -50);
        else msz.cx = (zDelta > 0 ? 1 : -1);

        msz.mouse = pt;//_ptMouseMove;
        ScreenToClient(msz.mouse);
        if (ZoomView(&msz) == TRUE) {
            msz.canvasOff.x = _curLayer->offset.x;
            msz.cy = 0; /* dont synchronize height zooming */
            msz.canvasOff.y = 0;
            SyncMessage(MOUSESYNC_MSG_LVIEW_ZOOM, &msz);
        }
        return 0;
    }

    BOOL ZoomView(LVIEW_MSGSYNC_ZOOM *msz)
    {
        if (msz->cy != 0) {
            //_dltHeight += msz->cy; // (cy > 0 ? 50 : -50);
            //_curLayer->offset.y += msz->canvasOff.y; // (cy > 0 ? -50 : 50);
            LRect rc;
            CalcAxisArea(rc);
            if (msz->cy < 0 && rc.Height() < -(_curLayer->ai.ydlt + msz->cy))
                return FALSE;


            LPoint pos = msz->mouse;
            _curLayer->ClientToLayer(pos);
            int ydata = _curLayer->ai.y2d(pos.y);
            _curLayer->ai.ydlt += msz->cy;
            _curLayer->PlaceDataOnY(ydata, msz->mouse.y, msz->cy);
            //{
            //    int t = _curLayer->ai.d2y(ydata, msz->cy);
            //    _curLayer->offset.y = t - (msz->mouse.y - _curLayer->viewPort.top);
            //}
        } 
        if (msz->cx != 0) {
            //float oldXScale = _scaleInfo.xScale;
            //_scaleInfo.xScale += msz->cx; // (cx > 0 ? 1 : -1);
            //_scaleInfo.xScale = max(1.0f, _scaleInfo.xScale);
            //_scaleInfo.xScale = min(20.0f, _scaleInfo.xScale);
            //if (_scaleInfo.xScale == oldXScale)
            //    return FALSE;
            for (int i = 0; i < _layers.Count(); i++) {
                // int old = _curLayer->ai.xwidth;

                Layer *layer = _layers[i];
                int xwidth_old = layer->ai.xwidth;
                int xwidth_new = xwidth_old + msz->cx;

                xwidth_new = max(0, xwidth_new);
                xwidth_new = min(20, xwidth_new);
                //if (xwidth_new > 0)
                //    xwidth_new |= 1;
                if (xwidth_new == xwidth_old || xwidth_old < 0)
                    continue;

                LPoint pos = msz->mouse;
                layer->ClientToLayer(pos);
                int xdata = layer->ai.x2d(pos.x);

                if (layer->gv.Count() > 0)
                    xdata += layer->gv[0]->GetDV(NULL)->dvs->x0; // TODO

                // pos = rc.left + data * (xwidth + 1) + xwidth / 2;
                // 
                // if (layer->ai.xwidth > 0) {
                //int off = pos.x - layer->ai.rc.left - xwidth_old / 2;
                //off /= (xwidth_old + 1);
                //layer->offset.x += xpos * (msz->cx > 0 ? 1 : -1);
                //// }
                ////int t0 = layer->ai.d2x(xpos);

                //int t = layer->ai.d2x(xpos);
                //layer->offset.x += (t - pos.x);//t1 / layer->ai.xwidth;
                /* move the offset of layer to make sure the position of data unched after zooming */
                layer->PlaceDataOnX(xdata, msz->mouse.x, msz->cx); /* mapping in new coordinate */
                layer->ai.xwidth = xwidth_new;
                //int t = layer->ai.d2x(xpos); 
                //layer->offset.x = t - (msz->mouse.x - layer->viewPort.left);
                //layer->offset.x += (pos.x - t1);
                //if (_curLayer->ai.xwidth == old)
                //    return FALSE;

                // _ptDragDlt.x = zDelta > 0 ? -__hoverX : __hoverX;
                //if (nFlags & 0x8000)
                //    __hoverX = pt.x;
                //layer->offset.x -= off; // msz->canvasOff.x; // (cx > 0 ? -canvasOffX : canvasOffX);
            }
        }
        SetRedrawFlags(REDRAW_PAINT | REDRAW_CALCULATE, TRUE);
        
        //LRect rcAxis;
        //CalcAxisArea(rcAxis);
        //UpdateRulerPosition(rcAxis);

        //UpdateRulerScrollRect();
        return TRUE;
    }

    VOID OnMouseSyncZoom(LVIEW_MSGSYNC_ZOOM *msz, LWnd *sender)
    {
        ZoomView(msz);
    }
    int _SyncTracking = 0;

    VOID OnMouseSyncTracking(LVIEW_MSGSYNC_TRACKING *mst, LWnd *sender)
    {
        // /* do nothing now */
        _ptMouseMove = mst->mouse;
        ScreenToClient(_ptMouseMove);
        //_ptMouseMove.y = -1; /* don't care about the Y position */
        InvalidateRect(NULL, FALSE, TRUE);
        // RedrawWindow (m_hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
        _SyncTracking++;

        //{
        //        PAINTSTRUCT ps;
        //        // HDC hdc;
        //        LDC dc(BeginPaint(m_hWnd, &ps));
        //        //TCHAR szInfo[80];
        //        // _stprintf(szInfo, _T("hello world: this %p, hwnd %p"), this, m_hWnd);
        //        // hdc = BeginPaint(hWnd, &ps);
        //        // TextOut(hdc, 0, 0, szInfo, _tcsclen(szInfo));
        //        //dc.SetBkMode(TRANSPARENT);
        //        OnPaint(dc);
        //        //OnPaint(dc);
        //        EndPaint(m_hWnd, &ps);
        //}
        //LVIEW_MSGSYNC_TRACKING mst;
        //mst.mouse = pt;
        //mst.rcFocus = _rcView;
        //SyncMessage(MOUSESYNC_MSG_LVIEW_TRACKING, &mst);

        ////mst->rcFocus = _rcView;
        //for (INT i = 0; i < _rulers.Count(); i++) {
        //    if (_rulers[i] != sender) /* don't send back to sender */
        //        SyncMessage(MOUSESYNC_MSG_LVIEW_TRACKING, mst, _rulers[i]);
        //}
    }

    //VOID OnMouseSyncTracking(LVIEW_MSGSYNC_TRACKING *mst)
    //{
    //    /* do nothing now */
    //}

    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
        __super::OnLButtonDown(dwFlags, pt);
        //_nMouseStatus |= LVIEW_LBUTTONDOWN;
        //return OnButtonDown(dwFlags, pt);
        if (GetKeyState(VK_LCONTROL) & 0x8000) {
            _eDragMode = LVIEW_DRAG_SELECTION; /* statistic in selected range */
            return 0;
        }
        SetFocus();
        return 0;
    }

    LRESULT OnLButtonUp(DWORD dwFlags, LPoint& pt)
    {
        __super::OnLButtonUp(dwFlags, pt);
        if (GetKeyState(VK_LCONTROL) & 0x8000) {
            _eDragMode = LVIEW_DRAG_DROP;
            InvalidateRect(NULL, FALSE, TRUE);
        }
        return 0;
    }

    LRESULT OnRButtonDown(DWORD dwFlags, LPoint& pt)
    {
        __super::OnRButtonDown(dwFlags, pt);
        _eDragMode = LVIEW_DRAG_SELECTION;
        return 0;
    }

    LRect rcZoom;
    COLORREF clrZoom;
    LRESULT OnRButtonUp(DWORD dwFlags, LPoint& pt)
    {
        __super::OnRButtonUp(dwFlags, pt);

#if 0
        if (_eDragMode == LVIEW_DRAG_SELECTION) {
            LRect rc(_ptMouseDown, _ptMouseMove);
            //CalcAxisArea(rcAxis);
            rcZoom = rc;
            //rcZoom.Intersection(rcZoom);
            clrZoom = 0;
            if (rcZoom.Width() > 0) {
                SetTimer(*this, 1111, 10, NULL);
                
                {
                    //memcpy(&_rcLastZoomed, &rcZoom, sizeof(LRect));
                    rcZoom.left += 1;
                    rcZoom.right += 1;
                    Layer *eagle = _layers[1];
                    eagle->ClientToLayer(rcZoom);
                    int x1 = eagle->ai.x2d(rcZoom.left);
                    int x2 = eagle->ai.x2d(rcZoom.right);
                    if (eagle->gv.Count() > 0) {
                        _DV *dv = eagle->gv[0]->GetDV(NULL);
                        x1 = max(0, x1);
                        x2 = min(x2, dv->Size(TRUE) - 1);
                        dv->dvs->x0 = x1;
                        dv->dvs->xn = x2;
                    }
                }
            }
        }
#endif
        _eDragMode = LVIEW_DRAG_DROP;

        return 0;
    }

    LRESULT OnTimer(UINT tmrId)
    {
        if (tmrId == 1111) {
            LRect rc;
            static int n = 1;
            CalcAxisArea(rc);
            
            // rc.InflateRect(-5, -5);
            if (rcZoom.top == rc.top && rcZoom.bottom == rc.bottom
                && rcZoom.left == rc.left && rcZoom.right == rc.right)
            {
                KillTimer(*this, 1111);
                memset(&rcZoom, 0, sizeof(rcZoom));
                SetRedrawFlags(REDRAW_PAINT | REDRAW_CALCULATE);
                InvalidateRect(NULL, FALSE, TRUE);
                n = 1;
                //SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGEND, _ptDragDlt);
                LVIEW_MSGSYNC_ZOOM msz = {0};
                SyncMessage(MOUSESYNC_MSG_LVIEW_ZOOM, &msz);
                //{
                //            Layer *eagle = _layers[1];
                //            // eagle->
                //            // _uRedrawFlags = 1;
                //            //SetRedrawFlags(REDRAW_PAINT);
                //            //GetParent()->InvalidateRect(NULL, FALSE, TRUE);
                //}
                //{
                //    extern FilledLine *_fline;
                //    // SetFilledRange
                //    if (_fline != NULL) {
                //        _fline->SetFilledRange(20, 200);
                //        InvalidateRect(NULL, FALSE, TRUE);
                //    }
                //}
                return 0;
            }

            n *= 2;
            // n += 1;
            //if (rcZoom.top > rc.top) {
                rcZoom.top -= n;
                rcZoom.top = max(rcZoom.top, rc.top);
            //}
            //if (rcZoom.bottom < rc.bottom) {
                rcZoom.bottom += n;
                rcZoom.bottom = min(rcZoom.bottom, rc.bottom);
            //}
            //if (rcZoom.left > rc.left) {
                rcZoom.left -= n;
                rcZoom.left = max(rcZoom.left, rc.left);
            //}
            //if (rcZoom.right < rc.right) {
                rcZoom.right += n;
                rcZoom.right = min(rcZoom.right, rc.right);
            //}
            clrZoom += RGB(20, 20, 20);
            InvalidateRect(NULL, FALSE, TRUE);
        }
        return 0;
    }
    //LRESULT OnLButtonUp(DWORD dwFlags, LPoint& pt)
    //{
    //    __super::OnLButtonUp(dwFlags, pt);
    //    __lastRcView = _rcView;

    //    return 0;
    //}

    VOID OnMouseDragSelectionDone(DWORD dwFlags, LRect& rc)
    {
        //float scaleX_canvas = (float)_width; // (float)_curLayer->cx / (float)_lines[0]->_dv->Size();
        //// rc.Intersection(_rcView);
        //rc.left = max(rc.left, 0);
        //rc.right = min(rc.right, _curLayer->cx);
        //int xBegin = (int)((float)rc.left / scaleX_canvas);
        //int xEnd = (int)((float)rc.right / scaleX_canvas);
        //LSTR str;
        //str.Format(_T("selected range{%d,%d,%d,%d}: %d - %d"), rc.left, rc.top, rc.right, rc.bottom, xBegin, xEnd);
        //_trackingLayer->dc.DrawText(str, rc);
        //MessageBox(str);

            LRect rc0(_ptMouseDown, _ptMouseMove);
            //CalcAxisArea(rcAxis);
            rcZoom = rc0;
            //rcZoom.Intersection(rcZoom);
            clrZoom = 0;
            if (rc.Width() > 0) {
                SetTimer(*this, 1111, 10, NULL);
            }

        if (GetKeyState(VK_LCONTROL) & 0x8000) {
            int x1 = _curLayer->ai.x2d(rc.left);
            int x2 = _curLayer->ai.x2d(rc.right);
            
            //LSTR str;
            //str.Format(_T("%d to %d"), x1, x2);
            //MessageBox(str);
        } else {
            //LRect rc0(_ptMouseDown, _ptMouseMove);
            ////CalcAxisArea(rcAxis);
            //rcZoom = rc0;
            ////rcZoom.Intersection(rcZoom);
            //clrZoom = 0;
            if (rc.Width() > 0) {
                //SetTimer(*this, 1111, 10, NULL);
                if (_curLayer == _layers[1]) /* the eagle layer */
                {
                    int x1 = _curLayer->ai.x2d(rc.left);
                    int x2 = _curLayer->ai.x2d(rc.right);
                    if (_curLayer->gv.Count() > 0) {
                        _DV *dv = _curLayer->gv[0]->GetDV(NULL);
                        x1 = max(0, x1);
                        x2 = min(x2, dv->Size(TRUE) - 1);
                        dv->dvs->x0 = x1;
                        dv->dvs->xn = x2;
                    }
                }
            }
        }
    }

    //VOID DrawDragTracking(LDC& dc)
    //{
    //    //if (LVIEW_DRAG_SELECTION == _eDragMode && NULL == _sbHover)
    //    //{
    //    //    LBrush br(dc);
    //    //    br.CreateStockObject(NULL_BRUSH);
    //    //    dc.Rectangle(_ptMouseDown, _ptMouseMove);
    //    //    {
    //    //        float scaleX_canvas = (float)_width; // (float)_curLayer->cx / (float)_lines[0]->_dv->Size();
    //    //        // rc.Intersection(_rcView);
    //    //        LRect rc(_ptMouseDown.x, _ptMouseDown.y, _ptMouseMove.x, _ptMouseMove.y);
    //    //        ClientToLayer(_curLayer, rc);
    //    //        rc.left = max(rc.left, 0);
    //    //        rc.right = min(rc.right, _curLayer->cx);
    //    //        int xBegin = (int)((float)rc.left / scaleX_canvas);
    //    //        int xEnd = (int)((float)rc.right / scaleX_canvas);
    //    //        LSTR str;
    //    //        str.Format(_T("selected range{%d,%d,%d,%d}: %d - %d"), rc.left, rc.top, rc.right, rc.bottom, xBegin, xEnd);
    //    //        _trackingLayer->dc.DrawText(str, rc);

    //    //    }
    //    //    return;
    //    //}
    //    return __super::DrawDragTracking(dc);
    //}

    LRESULT OnMButtonDown(DWORD dwFlags, LPoint& pt)
    {
#if 0
        LPoint dltDrag;

            //LPoint pt = _ptMouseMove;
            //ClientToLayer(&_curLayer, pt);
        LPoint ptCanvas = pt;
        _curLayer->ClientToLayer(ptCanvas);

        //pt.x -= _rcView.left;
        LRect rc = _curLayer->viewPort;

        float scaleX_client = (float)_lines[0]->_dv->Size() / (float)rc.Width();
        int hoverX_client = (int)((float)(pt.x - rc.left) * scaleX_client);

        //float scaleX_canvas = _scaleInfo.xScale; // (float)_curLayer->cx / (float)_lines[0]->_dv->Size();
        int hoverX_canvas = (int)((float)ptCanvas.x / _scaleInfo.xScale);

        dltDrag.x = (hoverX_canvas - hoverX_client) * (int)_scaleInfo.xScale;
            //int x = (int)((float)pt.x * scaleX);
            //int y = (int)((float)(_curLayer->cy - pt.y) / scaleY);
            //__hoverX = x;

        //dltDrag.x = x;
        _curLayer->offset -= dltDrag;
        //_ptMouseMove = pt;
        InvalidateRect(NULL, FALSE);
        // _curLayer->offset.x = 500;
        SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGEND, dltDrag);
#endif
        Layer *eagle = _layers[1];
        if (eagle->id != Layer::EAGLE_EYE || eagle->gv.Count() == 0)
            return 0;

            LPoint ptCanvas = pt;
            eagle->ClientToLayer(ptCanvas);
            int xdata = eagle->ai.x2d(ptCanvas.x); // data on pt.x is dv[xdata]

            LPoint dltDrag;
           int oldx = _layers[0]->offset.x; // the BOTTOM layer
       for (int i = 0; i < _layers.Count(); i++) {
            //LPoint ptCanvas = pt;
            //_layers[i]->ClientToLayer(ptCanvas);
            //int t = _layers[i]->ai.d2x(xpos);
            //_layers[i]->offset.x = t - (pt.x - _layers[i]->viewPort.left);
           _layers[i]->PlaceDataOnX(xdata, pt.x);
        }
        dltDrag.x = oldx - _layers[0]->offset.x;
        SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGEND, dltDrag);
        InvalidateRect(NULL, FALSE);

        return 0;
    }
    //void scrolltonew()
    //{
    //    LPoint dltDrag;
    //    dltDrag.x = -1000000;
    //    _curLayer->offset -= dltDrag;
    //    //_ptMouseMove = pt;
    //    InvalidateRect(NULL, FALSE);
    //    // _curLayer->offset.x = 500;
    //    SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGEND, dltDrag);

    //}
    ~LAxisView()
    {
        //delete _rules[BOTTOM][0];
    }
};

#if 0
class LAxisViewTest : public LWndView<LAxisViewTest>
{
    // DV<INT> *line;
    _DV* line;
    int w;
    float fmin, fmax;

public:
    LAxisViewTest()
    {
        line = NULL;
        w = 5;
    }
    ~LAxisViewTest()
    {
    }
    VOID CalcLayerSize(LDC& dc, int& cx, int& cy)
    {
        delete line;
        line = new DV<float>(882);
        DV<float> &l0 = *(DV<float>*)line;
        fmin = 400000; fmax = 0;
        {
            FILE *f = fopen("d:\\stock.txt", "r");
            int price;
            int i = 0;
            while(!feof(f))
            {
                price = 0.0;
                fscanf(f, "%d\r\n", &price);
                //price /= 15;
                l0[i] = price;
                fmin = min(fmin, l0[i]);
                fmax = max(fmax, l0[i]);
                i += 1;
            }
        }
        // line->Size
        //for (int i = 0; i < line->Size(); i++)
        //{
        //    l0[i] = sin((float)rand() * 3.14f);// rand();
        //    //l0[i] = sin(3.14f * i);
        //    //l0[i] *= ((float)rand() / 10000.0f);
        //    l0[i] *= 100;
        //    l0[i] += 100;
        //    fmin = min(fmin, l0[i]);
        //    fmax = max(fmax, l0[i]);
        //}

        cx = (line->Size() - 1) * w + 0;
        cy = 700;//(int)(fmax - fmin) + 1;
    }
    VOID CalcViewRegion(LDC& dc, LRect& rc)
    {
        // rc = _rcClient;
        GetClientRect(rc);
        rc.InflateRect(-20, -40);
        rc.left += 20;
        //rc.Offset(20, 0);
    }
    //VOID UpdateTrackingRngs()
    //{
    //    __super::UpdateTrackingRngs();
    //    // AddTrackingRng<LRect>(_rcTracks);
    //    for (int i = 0; i < _rcTracks.Count(); i++)
    //    {
    //        AddTrackingRng(_rcTracks[i], i);
    //    }
    //}
    BOOL DrawLayer(LDC& dc, UINT& uDrawFlags) /* dc is the layer */
    {
        if (REDRAW_CALCULATE & uDrawFlags)
        {
            //delete line;
            //line = new DV<INT>(rand() / 5);
            //DV<INT> &l0 = *(DV<INT>*)line;
            //for (int i = 0; i < line->Size(); i++)
            //{
            //    l0[i] = rand();
            //}
        }
        LRect rc;
        LSTR str;
        int y0;
        GetLayerRect(rc);

        int oldMode = dc.SetMapMode(MM_ANISOTROPIC);
        dc.SetWindowOrg(0, fmin);
        dc.SetViewportOrg(0, rc.bottom);
        dc.SetWindowExt(rc.Width(), -(fmax-fmin));
        dc.SetViewportExt(rc.Width(), rc.Height());
        // dc.SetViewportExt(rc.Width(), 400);

        DV<float> &l0 = *(DV<float>*)line;
        // y0 = rc.Height() -((int)l0[0] -(int)fmin);
        // dc.MoveTo(0, l0[0]);
        for (int i = 0; i < line->Size(); i++)
        {
            // dc.LineTo(i * 10,(int)(l0[i] * 100) + 100);
            // y0 = rc.Height() -((int)l0[i] -(int)fmin);
            // dc.LineTo(i * w, l0[i]);
            LRect rcItem;
            rcItem.left = i * w - w / 2;
            rcItem.right = rcItem.left + w;
            rcItem.top = l0[i];
            int r = rand() % 100;
            rcItem.top -= r;
            rcItem.bottom = rcItem.top + r * 2;
            dc.Rectangle(rcItem);
            // l0[i] = rand();
        }

        //{
        //    //dc.SetWindowOrg(0, fmin);
        //    //dc.SetViewportOrg(0, _rcView.bottom - 100);
        //    LPen pen(dc);
        //    pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        //    dc.SetWindowExt(rc.Width(), -(fmax-fmin));
        //    dc.SetViewportExt(_rcView.Width(), 100);
        //    // dc.SetViewportExt(rc.Width(), 400);

        //    DV<float> &l0 = *(DV<float>*)line;
        //    // y0 = rc.Height() -((int)l0[0] -(int)fmin);
        //    dc.MoveTo(0, l0[0]);
        //    for (int i = 1; i < line->Size(); i++)
        //    {
        //        // dc.LineTo(i * 10,(int)(l0[i] * 100) + 100);
        //        // y0 = rc.Height() -((int)l0[i] -(int)fmin);
        //        dc.LineTo(i * w, l0[i]);
        //        // l0[i] = rand();
        //    }


        //}
        dc.SetMapMode(oldMode);
        dc.SetViewportOrg(0, 0);
        dc.SetWindowOrg(0, 0);
        //rc.top = rc.bottom - 30;
        //str.Format(_T("fmin = %.2f, fmax = %.2f"), fmin, fmax);
        //dc.SetTextColor(RGB(0, 0, 255));
        //dc.DrawText(str, rc, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
        //dc.TextOut(0, 0, _T("function: rand sin."));

        return FALSE;
    }
    VOID DrawTrackingLayer(LDC& dc, const LRect& rc) /* dc is the client */
    {
        // dc.TextOut(_rcDraw.lt, _T("function: rand sin."));
        dc.SetTextColor(RGB(0, 0, 255));
        dc.DrawText(_T("function: rand sin."), _rcDraw, DT_SINGLELINE | DT_CENTER | DT_TOP);

        LSTR str;
        str.Format(_T("fmin = %.2f, fmax = %.2f, total %u"), fmin, fmax, line->Size());
        // dc.SetTextColor(RGB(0, 0, 255));
        dc.DrawText(str, _rcDraw, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);

        {
        int y = _rcDraw.top + _rcDraw.Height() / 2;
        LPen pen(dc);
        pen.CreatePen(PS_DOT, 1, RGB(255, 0, 0));
        dc.MoveTo(_rcDraw.left, y);
        dc.LineTo(_rcDraw.right, y);
        int N = 10;
        int x = _rcDraw.left;
        int t = _rcDraw.Width() / N;
        for (int i = 1; i < N; i++) {
            dc.MoveTo(x + i * t, _rcDraw.top);
            dc.LineTo(x + i * t, _rcDraw.bottom);
        }
        }

        LRect rc0 = _rcDraw;
        rc0.Offset(0, 30);
        rc0.top = rc0.bottom - 20;
        dc.Rectangle(rc0);
        str.Format(_T("%d, %d"), _ptDragDlt.x, _ptDragDlt.y);
        dc.DrawText(str, rc0);
        str.Format(_T("%.2f"),(float)_ptMouseMove.x);
        rc0.left = _ptMouseMove.x + 2;
        rc0.right -= 2;
        LFont ft(dc);
        ft.CreateFont(_T("Tahoma"), 12, 0, RGB(255, 0, 0));
        dc.DrawText(str, rc0, DT_SINGLELINE | DT_VCENTER);
        // dc.TextOut(_ptMouseMove.x, rc0.top + 1, str);
        {
            dc.MoveTo(_ptMouseMove.x, 0);
            dc.LineTo(_ptMouseMove.x, _rcClient.bottom);
            dc.MoveTo(0, _ptMouseMove.y);
            dc.LineTo(_rcClient.right, _ptMouseMove.y);
        }

        {
            //dc.SetWindowOrg(0, fmin);
            //dc.SetViewportOrg(0, _rcView.bottom - 100);
            int oldMode = dc.SetMapMode(MM_ANISOTROPIC);
            LPen pen(dc);
            pen.CreatePen(PS_SOLID, 1, RGB(210, 210, 210));

            dc.SetWindowOrg(0, fmin);
            dc.SetViewportOrg(_rcDraw.left, _rcDraw.bottom);

            dc.SetWindowExt(_curLayer->cx, -(fmax-fmin));
            dc.SetViewportExt(_rcDraw.Width(), _rcDraw.Height() / 2);
            // dc.SetViewportExt(rc.Width(), 400);

            DV<float> &l0 = *(DV<float>*)line;
            // y0 = rc.Height() -((int)l0[0] -(int)fmin);
            dc.MoveTo(0, l0[0]);
            for (int i = 1; i < line->Size(); i++)
            {
                // dc.LineTo(i * 10,(int)(l0[i] * 100) + 100);
                // y0 = rc.Height() -((int)l0[i] -(int)fmin);
                dc.LineTo(i * w, l0[i]);
                // l0[i] = rand();
            }
            dc.SetMapMode(oldMode);
            dc.SetViewportOrg(0, 0);
            dc.SetWindowOrg(0, 0);
        }
    }
    LRESULT OnMouseMove(DWORD dwFlags, LPoint& pt)
    {
        __super::OnMouseMove(dwFlags, pt);
        InvalidateRect(NULL, FALSE);
        return 0;
    }
    //VOID OnPaint(LDC& dc)
    //{
    //    __super::OnPaint(dc);
    //}
};
#endif

