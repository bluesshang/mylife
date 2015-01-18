#pragma once

#include "..\utils\lstr.h"

#include "lwin.h"
#include "LScrollBar.h"

#define REDRAW_CALCULATE    1   /* the size of layer was changed, re-calculate the size and position of childs */
#define REDRAW_PAINT        2   /*  */

/* for MOUSESYNC_MSG_LVIEW_DRAGDROP */
typedef struct tagLVIEW_DRAGDROP_INFO
{
    UINT flags;
    LPoint ptMouse; /* the current position of mouse */
    LPoint ptDragDlt;
} LVIEW_DRAGDROP_INFO;

/* for MOUSESYNC_MSG_LVIEW_ZOOM */
typedef struct LVIEW_MSGSYNC_ZOOM_t
{
    INT cx, cy;
    LPoint canvasOff, mouse;
} LVIEW_MSGSYNC_ZOOM;

/* for MOUSESYNC_MSG_LVIEW_TRACKING */
typedef struct LVIEW_MSGSYNC_TRACKING_t
{
    LPoint mouse;
    //LRect rcFocus; /* focus region */
} LVIEW_MSGSYNC_TRACKING;

#define LVIEW_LBUTTONDOWN           1
#define LVIEW_RBUTTONDOWN           2
#define LVIEW_MBUTTONDOWN           4
#define LVIEW_MOUSEDOWN            (LVIEW_LBUTTONDOWN | LVIEW_RBUTTONDOWN | LVIEW_MBUTTONDOWN)
#define LVIEW_MOUSESYNC_DRAGGING    8
#define LVIEW_MOUSESYNC_DRAGEND     0x10

#define LVIEW_DRAGDROP_VERT     1
#define LVIEW_DRAGDROP_HORZ     2
#define LVIEW_DRAGDROP_ENABLE  (LVIEW_DRAGDROP_VERT | LVIEW_DRAGDROP_HORZ)

#define WM_LVIEW_USER       WM_APP
#define WM_LVIEW_MOUSESYNC (WM_LVIEW_USER + 1)

#define LVS_NO_STRETCH_CANVAS   1 /* dont stretch the layer to client when the region of layer is smaller then client */
#define LVS_VALIGN_TOP          2
#define LVS_VALIGN_BOTTOM       4
#define LVS_HALIGN_LEFT         8
#define LVS_HALIGN_RIGHT        0x10
#define LVS_NO_VERT_SCROLLBAR   0x20
#define LVS_NO_HORZ_SCROLLBAR   0x40
#define LVS_NO_SCROLLBAR        (LVS_NO_VERT_SCROLLBAR | LVS_NO_HORZ_SCROLLBAR)
#define LVS_MAX_STYLE           0x100

typedef enum tagLVIEW_DRAG_MODE
{
    LVIEW_DRAP_DISABLE = 0, LVIEW_DRAG_DROP, LVIEW_DRAG_SELECTION
} LVIEW_DRAG_MODE;

// #define TRACKING_RNG_CANVAS 0
//#define TRACKING_RNG_CLIENT 1

//class Layer;
//class LWndViewRoot
//{
//public:
//    VOID(LWndViewRoot::*pfnOnMouseHoverIn)(Layer *layer, struct tagLVIEW_TRACKING_RNG* trIn, struct tagLVIEW_TRACKING_RNG* trOut, const LPoint& pt);
//    VOID(LWndViewRoot::*pfnOnMouseHoverOut)(Layer *layer, struct tagLVIEW_TRACKING_RNG* trOut);
//    //LVIEW_MOUSETRACKING_HOVER_IN pfnOnMouseHoverIn;
//    //LVIEW_MOUSETRACKING_HOVER_OUT pfnOnMouseHoverOut;
//};

class Layer;
typedef VOID(LWnd::*LVIEW_MOUSETRACKING_HOVER_IN)(Layer *layer, struct tagLVIEW_TRACKING_RNG* trIn, struct tagLVIEW_TRACKING_RNG* trOut, const LPoint& pt);
typedef VOID(LWnd::*LVIEW_MOUSETRACKING_HOVER_OUT)(Layer *layer, struct tagLVIEW_TRACKING_RNG* trOut);

#define TRACKING_COORD_CLIENT 1
#define TRACKING_COORD_SCREEN 2

typedef struct tagLVIEW_TRACKING_RNG
{
    LRect rc;
    UINT flags;
    union
    {
        VOID* pv;
        INT i;
    }; /* paras for callback */
    //LWnd* wnd; /* window receive tracking events */
    //LVIEW_MOUSETRACKING_HOVER_IN pfnOnMouseHoverIn;
    //LVIEW_MOUSETRACKING_HOVER_OUT pfnOnMouseHoverOut;
    // pfnOnMouseDragDrop; ???
    //VOID(LWnd::*pfnOnMouseHoverIn)(struct tagLVIEW_TRACKING_RNG* trIn, struct tagLVIEW_TRACKING_RNG* trOut);
    //VOID(LWnd::*pfnOnMouseHoverOut)(struct tagLVIEW_TRACKING_RNG* trOut);

    tagLVIEW_TRACKING_RNG() {
        MEMZERO_THIS();
    }
} LVIEW_TRACKING_RNG;

// typedef Layer Layer;

#define LAYER_NO_HSCROLLBAR 1
#define LAYER_NO_VSCROLLBAR 2

class Layer
{
    Layer() {}

    GV *gvFocus; /* current selected or focused GV, for example, a selected line */
public:
    enum {BOTTOM = 0, EAGLE_EYE, TRACKING, CACHE, USER};
    //typedef enum {ALPHA_BLEND, TRANSPARENT_BLEND, OPAQUE_BLEND} LayerBlendType;
    //LayerBlendType blendType;
    LSTR name;
    UINT id;
    LPoint offset;
    HBITMAP bm;
    LDC dc;
    INT cx;
    INT cy;
    COLORREF clrBkgrnd;
    UINT flags;
    /* 0: opaque, background is filled with color 'clrBkgrnd'
     * 100: transparent, all color marked with 'clrBkgrnd' is masked out
     * [1 - 99]: do alpha blending */
    INT transparency; 
    //BOOL autoSize;

    LArray<LVIEW_TRACKING_RNG> _rngTrackings;
    LVIEW_TRACKING_RNG *_rngTracking;
    LRect viewPort; /* the draw area in client */
    LArray<GV*> gv;

    AxisInfo ai;
    LWnd *wnd;

    ScrollBar sbHorz, sbVert, *sbHover;

    Layer(UINT id, COLORREF clrBkgrnd = RGB(255, 255, 255), LPCTSTR name = NULL, int transparency = 100, UINT flags = 0) 
        : id(id), clrBkgrnd(clrBkgrnd), name(name), transparency(transparency),
        sbVert(ScrollBar::VERTICAL), sbHorz(ScrollBar::HORIZONTAL), flags(flags)
    {
        //memset(this, 0, sizeof(*this));
        dc = ::CreateCompatibleDC(NULL);
        _ASSERT((HDC)dc != NULL);
        cx = 0;
        cy = 0;
        bm = 0;
        offset.x = 0;
        offset.y = 0;
        sbHover = NULL;
        gvFocus = NULL;
        flags = 0;

        //sbVert.Init(ScrollBar::VERTICAL);
        //sbHorz.Init(ScrollBar::HORIZONTAL);

        if (name == NULL)
            this->name.Format(_T("Layer#%d"), id);
    }
    ~Layer()
    {
        if ((HDC)dc != NULL) {
            DeleteObject((HDC)dc);
        }
        if (NULL != bm) {
           (VOID)DeleteObject(bm);
        }
    }
    GV* GetFocusGV()
    {
        if (gvFocus != NULL)
            return gvFocus;
        if (gv.Count() > 0)
            return gv[0];
        return NULL;
    }
    //GV* LookupGV(LPCTSTR gvName)
    //{
    //    for (int i = 0; i < gv.Count(); i++) {
    //    }
    //    return NULL;
    //}
    _DV* QueryDV(LPCTSTR dvName)
    {
        GV* gv = GetFocusGV();

        if (gv == NULL)
            return NULL;

        return gv->GetDV(dvName);
    }
    BOOL IsValid()
    {
        return(HDC)dc != NULL && NULL != bm;
    }

    // BOOL Create(HDC hdc, INT width, INT height)
    BOOL Create(LWnd *wnd, INT width, INT height)
    {
        //HDC hdc = GetDC(hWnd);
        //_ASSERT(NULL != hdc);
        HDC hdc = wnd->GetDC();
        HBITMAP hbmNew = CreateCompatibleBitmap(hdc, width, height);
        //ReleaseDC(hWnd, hdc);
        if (hbmNew != NULL)
        {
           (VOID)DeleteObject(bm);
            bm = hbmNew;
            dc.SelectObject(hbmNew);
            cx = width;
            cy = height;
        }
        wnd->ReleaseDC(hdc);
        this->wnd = wnd;
        return TRUE;
    }
    VOID EraseBackground()
    {
        LRect rc(0, 0, cx, cy);
        LBrush br(dc);
        br.CreateSolidBrush(clrBkgrnd);
        dc.FillRect(rc, br);
    }
    /* get the layer drawing offset by the sizeof client(_rcClient) and the dragging amount(_ptDragDlt) */
    VOID AdjustLayerDrawOffset(LPoint& pt)
    {
        if (cx > viewPort.Width()) {
            pt.x = max(pt.x, 0);
            pt.x = min(pt.x, cx - viewPort.Width());
        }
        else pt.x = 0;
        if (cy > viewPort.Height()) {
            pt.y = max(pt.y, 0);
            pt.y = min(pt.y, cy - viewPort.Height());
        }
        else pt.y = 0;
    }
    VOID ClientToLayer(LPoint& pt)
    {
        pt += offset;
        //pt -= _rcView.lt;
        pt -= viewPort.lt;
        //pt.x -= _offsetX;
        //pt.y -= _offsetY;
        //pt.x -= _curLayer->offset.x;
        //pt.y -= _curLayer->offset.y;
    }
    VOID ClientToLayer(LRect& rc)
    {
        ClientToLayer(rc.lt);
        ClientToLayer(rc.rb);
    }
    VOID LayerToClient(LPoint& pt)
    {
        pt += viewPort.lt;
        pt -= offset;
    }

    /* xpos in client coordination, not in layer */
    void PlaceDataOnX(int xdata, int xpos, int dlt = 0)
    {
        int t = ai.d2x(xdata, dlt);
        offset.x = t - (xpos - viewPort.left);
    }

    /* move the layer/canvas to make the ydata shown on the ypos */
    void PlaceDataOnY(int ydata, int ypos, int dlt = 0)
    {
        int t = ai.d2y(ydata, dlt);
        offset.y = t - (ypos - viewPort.top);
    }

    void CalcScrollBarInfo(LRect& rcScrollArea)
    {
        INT sbx, sby;

        //UINT _uDragFlags = 0; // &= ~(LVIEW_DRAGDROP_HORZ | LVIEW_DRAGDROP_VERT);

        sbx = cx - rcScrollArea.Width();
            //_uDragFlags |= LVIEW_DRAGDROP_HORZ;

        sby = cy - rcScrollArea.Height();
            //_uDragFlags |= LVIEW_DRAGDROP_VERT;
        sbVert.enabled = FALSE;
        sbHorz.enabled = FALSE;
        if (sbx > 0 && !(flags & LAYER_NO_HSCROLLBAR)) {
            LRect rc = rcScrollArea; // _rcClient;
            rc.right -= (sby > 0 ? 1/*sbVert._size*/ : 0);
            sbHorz.SetScrollInfo(rc, 0, sbx);
            sbHorz.enabled = TRUE;
        }

        if (sby > 0 && !(flags & LAYER_NO_VSCROLLBAR)) {
            LRect rc = rcScrollArea;// _rcClient;
            rc.bottom -= (sbx > 0 ? 1/*sbHorz._size*/ : 0);
            sbVert.SetScrollInfo(rc, 0, sby);
            sbVert.enabled = TRUE;
        }
        //_curLayer = layer;
    }

    //void CalcDrawOffset(const LPoint& dragAmount, LPoint drawOffset)
    //{
    //    if (dragAmount.x || dragAmount.y)
    //    {   /* in drag & drop mode, dont change the offset */
    //        drawOffset = offset - dragAmount;
    //        AdjustLayerDrawOffset(drawOffset);
    //        return;
    //    }

    //    AdjustLayerDrawOffset(offset);
    //    drawOffset = offset;
    //}

    void DrawScrollBar(LDC& dcDst, const LPoint& dragAmount)
    {
        if (dragAmount.x || dragAmount.y || sbHorz._flags || sbVert._flags) {
            if (/*!(wndCtrlStyle & LVS_NO_HORZ_SCROLLBAR) && */sbHorz.enabled) {
                sbHorz.SetPos(drawOffset.x);
                sbHorz.Draw(dcDst);
            }

            if (/*!(wndCtrlStyle & LVS_NO_VERT_SCROLLBAR) && */sbVert.enabled) {
                sbVert.SetPos(drawOffset.y);
                sbVert.Draw(dcDst);
            }
        }
    }
    BOOL PtInScrollBar(LPoint& pt)
    {
        if (sbHover && sbHover->_rc.PtInRect(pt))
            return TRUE;
        return FALSE;
    }
    LPoint drawOffset;
    void BitBlt(LDC& dcDst, const LPoint& dragAmount/*, LDC& dcScrollbar*/)
    {
        BOOL ret;
        //LPoint drawOffset;
        if (dragAmount.x || dragAmount.y)
        {   /* in drag & drop mode, dont change the offset */
            drawOffset = offset - dragAmount;
            AdjustLayerDrawOffset(drawOffset);
        } else {
            AdjustLayerDrawOffset(offset); /* adjust each time */
            drawOffset = offset;
        }

        if (viewPort.Height() == 0 || viewPort.Width() == 0)
            return;

        if (transparency == 0) {
            ret = dcDst.BitBlt(viewPort, dc, drawOffset);
        } else if (transparency == 100) {
            ret = dcDst.TransparentBlt(viewPort, dc, drawOffset, clrBkgrnd);
        } else {
            ret = dcDst.AlphaBlend(viewPort, dc, drawOffset, (int)(((float)transparency / 100.0) * 255.0));
            //ret = dcDst.AlphaBlend(viewPort, dc, drawOffset, (int)(((float)transparency / 100.0) * 255.0), clrBkgrnd);
        }
        if (ret != TRUE) {
            DWORD err = GetLastError();
        }
        _ASSERT(ret == TRUE);

        /* show scroll bar if any */
        /* XXX: no wndCtrlStyle here */
        //if (dragAmount.x || dragAmount.y || sbHorz._flags || sbVert._flags) {
        //    if (/*!(wndCtrlStyle & LVS_NO_HORZ_SCROLLBAR) && */sbHorz.enabled) {
        //        sbHorz.SetPos(drawOffset.x);
        //        sbHorz.Draw(dcScrollbar);
        //    }

        //    if (/*!(wndCtrlStyle & LVS_NO_VERT_SCROLLBAR) && */sbVert.enabled) {
        //        sbVert.SetPos(drawOffset.y);
        //        sbVert.Draw(dcScrollbar);
        //    }
        //}
    }

    void ResetTrackingRngs()
    {
        _rngTrackings.Reset();

        LVIEW_TRACKING_RNG tr;

        tr.flags = TRACKING_COORD_CLIENT;
        if (sbHorz.enabled) {
            tr.rc = sbHorz._rc;
            tr.pv = &sbHorz;
            //ClientToLayer(tr.rc);
            _rngTrackings.Add(tr);
        }

        if (sbVert.enabled) {
            tr.rc = sbVert._rc;
            tr.pv = &sbVert;
            //ClientToLayer(tr.rc);
            _rngTrackings.Add(tr);
        }
    }

    BOOL IsTrackingScrollbar(LVIEW_TRACKING_RNG *tr, BOOL isHover)
    {
        if (tr->pv != &sbHorz && tr->pv != &sbVert)
            return FALSE;

        sbHover = (ScrollBar *)tr->pv;
        sbHover->_flags = isHover ? 1 : 0;
        return TRUE;
    }

    enum {TRACKING_NOTHING, TRACKING_KEEP, TRACKING_START, TRACKINT_END};
    UINT TestTrackingRngs(const LPoint& ptClient, 
        LWnd *wnd,
        LVIEW_MOUSETRACKING_HOVER_IN pfnOnMouseHoverIn,
        LVIEW_MOUSETRACKING_HOVER_OUT pfnOnMouseHoverOut)
    {
        //if (viewPort.PtInRect(ptClient))
        {
            LPoint ptLayer = ptClient;
            ClientToLayer(ptLayer);
            if (NULL != _rngTracking && _rngTracking->rc.PtInRect(
                    _rngTracking->flags & TRACKING_COORD_CLIENT ? ptClient : ptLayer))
            {   /* mouse still in current tracking region */
                wnd->SetCapture(); /* keep capturing */
                return TRACKING_KEEP;
            }
            for (int i = 0; i < _rngTrackings.Count(); i++)
            {
                LVIEW_TRACKING_RNG& tr = _rngTrackings[i];
                if (tr.rc.PtInRect(tr.flags & TRACKING_COORD_CLIENT ? ptClient : ptLayer))
                {
                    wnd->SetCapture();
                    // if (tr.wnd && tr.pfnOnMouseHoverIn)
                    // (tr.wnd->*tr.pfnOnMouseHoverIn)(&tr, _rngTracking, ptClient);
                    //if (tr.pv == &sbVert || tr.pv == &sbHorz) {
                    //    ((ScrollBar*)tr.pv)->_flags = 1;
                    //} else
                    if (_rngTracking)
                        (wnd->*pfnOnMouseHoverOut)(this, _rngTracking);
                    (wnd->*pfnOnMouseHoverIn)(this, &tr, _rngTracking, ptClient);
                    // else _pThis->OnMouseHoverRect(&tr, _rngTracking, ptClient);
                    _rngTracking = &tr;
                    return TRACKING_START;
                }
            }
        }

        if (NULL != _rngTracking)
        {   /* now mouse is not in any regions */
            LVIEW_TRACKING_RNG& tr = *_rngTracking;
            // if (tr.wnd && tr.pfnOnMouseHoverOut)
           // (tr.wnd->*tr.pfnOnMouseHoverOut)(_rngTracking);
            /*if (_rngTracking->pv == &sbVert || _rngTracking->pv == &sbHorz) {
                    ((ScrollBar*)_rngTracking->pv)->_flags = 0;
            } else */
            (wnd->*pfnOnMouseHoverOut)(this, _rngTracking);
            // else _pThis->OnMouseLeaveRect(_rngTracking);
            ReleaseCapture();
            _rngTracking = NULL;
            return TRACKINT_END;
        }

        return TRACKING_NOTHING;
    }
};

template <class T>
class LWndView : public LWindowImpl<T>/*, public LWnd*/
{
    // Layer _layers[10];
public:
    // LDC _dcClient;
    UINT _nMouseStatus;
    UINT _uRedrawFlags;
    LArray<Layer *> _layers;
    Layer *_curLayer;
    Layer *_trackingLayer;
    Layer *_cachedLayer; /* the memory DC */
    //Layer _curLayer;
    //Layer _trackingLayer; // represent the whole visible client area, drawing on fly, for example mouse move tracking
    // Layer *_curCanvas;
    UINT _uDragFlags;
    LRect _rcClient;
    //LRect _rcView;
    //LRect _rcDraw;
    //INT _offsetX, _offsetY;
    //LRect _rcVisibleCanvas;
    //LPoint _ptLastDrag;
    LPoint _ptDragDlt;
    LPoint _ptMouseDown;
    LPoint _ptMouseMove;
    
    //LPoint __ptMove;
    LVIEW_DRAG_MODE _eDragMode;
    T* _pThis;
    //LArray<LVIEW_TRACKING_RNG> _rngTrackings;
    //LVIEW_TRACKING_RNG *_rngTracking;

    int _ysbpos;
    //ScrollBar _sbHorz, _sbVert, *_sbHover;
public:
    
    //LArray<LWnd*> *_wndDragSync;
    //LWnd* __wndDragSync;
    LWndView() : LWindowImpl<T>(_T("LWndView"))
    {
        //_wndDragSync = NULL;
        _uRedrawFlags = 0;
        _nMouseStatus = 0;
        _eDragMode = LVIEW_DRAG_DROP;
        _uDragFlags = 0;//LVIEW_DRAGDROP_VERT | LVIEW_DRAGDROP_HORZ;
        _ptDragDlt.x = 0;
        _ptDragDlt.y = 0;
        _pThis = static_cast<T *>(this); /* for 'overiding' */
        //_rngTracking = NULL;
        //_offsetX = 0;
        //_offsetY = 0;
        //_prcDraw = &_rcClient; /* the drawing area is the client by default */
        //_hbmCanvas = NULL;
        //_ptCanvasOffset.x = 0;
        //_ptCanvasOffset.y = 0;
        //_curLayer->dc = ::CreateCompatibleDC(NULL);
        //_ASSERT((HDC)_dcCanvas != NULL);
        //_sbVert.Init(ScrollBar::VERTICAL);
        //_sbHorz.Init(ScrollBar::HORIZONTAL);
        //_sbHover = NULL;
        _curLayer = NULL;
        //_curLayer = &_layers[0];
    }
    ~LWndView()
    {
        for (int i = 0; i < _layers.Count(); i++)
            delete _layers[i];
        delete _trackingLayer;
    }

    VOID SetRedrawFlags(UINT nFlags, BOOL bRedrawNow = FALSE) {
        _uRedrawFlags |= nFlags; 
        if (TRUE == bRedrawNow) {
            InvalidateRect(NULL, FALSE);
        }
    }
    VOID SetDragMode(LVIEW_DRAG_MODE eDragMode) {_eDragMode = eDragMode;}
    LVIEW_DRAG_MODE GetDragMode() {return _eDragMode;}
    //VOID AddMouseSync(LWnd *wnd)
    //{
    //    if (_wndDragSync != NULL)
    //        _wndDragSync->Add(wnd);
    //}

    BOOL DrawLayer(Layer *layer, UINT& uDrawFlags)
    {
        //{
        //    LSTR str;
        //    TCHAR info[256];
        //    GetWindowText(str);
        //    _stprintf(info, _T("LWndView(%s)::DrawLayer %d"),(LPCTSTR)str, GetDlgCtrlID(m_hWnd));
        //    dc.TextOut(0, 0, info);
        //}
        //uDrawFlags = 0;
        return FALSE;
    }

    //VOID DrawDragTracking(LDC& dc)
    //{
    //    //if ((LVIEW_DRAGDROP_ENABLE & _uDragFlags) && (LVIEW_LBUTTONDOWN & _nMouseStatus))
    //    //{
    //    if (LVIEW_DRAG_DROP == _eDragMode && (LVIEW_DRAGDROP_ENABLE & _uDragFlags))
    //    {
    //        return;
    //    }

    //    if (LVIEW_DRAG_SELECTION == _eDragMode && NULL == _sbHover)
    //    {
    //        LBrush br(dc);
    //        br.CreateStockObject(NULL_BRUSH);
    //        dc.Rectangle(_ptMouseDown, _ptMouseMove);
    //        return;
    //    }
    //}
    //VOID ClientToLayer(Layer *layer, LPoint& pt)
    //{
    //    pt += layer->offset;
    //    //pt -= _rcView.lt;
    //    pt -= layer->viewPort.lt;
    //    //pt.x -= _offsetX;
    //    //pt.y -= _offsetY;
    //    //pt.x -= _curLayer->offset.x;
    //    //pt.y -= _curLayer->offset.y;
    //}
    //VOID ClientToLayer(Layer *layer, LRect& rc)
    //{
    //    ClientToLayer(layer, rc.lt);
    //    ClientToLayer(layer, rc.rb);
    //}
    //BOOL SetCanvasOffset(LPoint& ptDlt, BOOL bInvalidate = TRUE)
    //{
    //    //_curLayer->offset.x += ptDlt.x;
    //    //_curLayer->offset.y += ptDlt.y;

    //    //_curLayer->offset.x = min(_curLayer->offset.x, 0);
    //    //_curLayer->offset.y = min(_curLayer->offset.y, 0);
    //    //LRect rc;
    //    //GetClientRect(rc);
    //    //_curLayer->offset.x = max(_curLayer->offset.x, rc.Width() - _curLayer->cx);
    //    //_curLayer->offset.y = max(_curLayer->offset.y, rc.Height() - _curLayer->cy);

    //    // GetCanvasDrawOffset()
    //    if (TRUE == bInvalidate)
    //    {
    //        InvalidateRect(NULL, FALSE);
    //    }

    //    return TRUE;
    //}
    //VOID UpdateCanvasOffset()
    //{
    //    //LRect rc;
    //    //GetClientRect(rc);
    //    GetCanvasDrawOffset(/*_rcClient, _ptDragDlt, */_curLayer->offset.x, _curLayer->offset.y);
    //}
    /////* get the layer drawing offset by the sizeof client(_rcClient) and the dragging amount(_ptDragDlt) */
    ////VOID AdjustLayerDrawOffset(Layer *layer, LPoint& pt)
    ////{
    ////    if (layer->cx >= _rcView.Width()) {
    ////        pt.x = max(pt.x, 0/*_rcView.left*/);
    ////        pt.x = min(pt.x, layer->cx - _rcView.Width());
    ////    }
    ////    else pt.x = 0;
    ////    if (layer->cy >= _rcView.Height()) {
    ////        pt.y = max(pt.y, 0/*_rcView.top*/);
    ////        pt.y = min(pt.y, layer->cy - _rcView.Height());
    ////    }
    ////    else pt.y = 0;
    ////}
    VOID GetLayerRect(LRect& rc)
    {
        rc.left = 0;
        rc.top = 0;
        rc.right = _curLayer->cx;
        rc.bottom = _curLayer->cy;
    }
    VOID GetViewRect(LRect& rc)
    {
        rc = _rcView;
    }
    /* OnSize must be called in derived class for supporting memory layer */
    LRESULT OnSize(UINT nType, INT cx, INT cy)
    {
        {
            const char *cname = typeid(this).name();
            if (strstr(cname, "LWndTodo"))
            {
                int a = 0;
            }
        }
        _rcClient.right = cx;
        _rcClient.bottom = cy;
        //if (cx > 0 && cy > 0/* && TRUE == _curLayer->autoSize*/)
        //{
        _uRedrawFlags |= REDRAW_CALCULATE;
        //}
        return 0;
    }

    VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        /* the default size of layer is the whole client area */
        cx = _rcClient.Width();
        cy = _rcClient.Height();
    }
    VOID CalcViewRegion(Layer *layer, LRect& rc)
    {
        rc = _rcClient; /* default to the whole client area */

        LRect rcLayer = {0, 0, layer->cx, layer->cy};
        rc.Intersection(rcLayer);
        // CenterCanvas(rc, layer->cx, layer->cy);
        // rc.Center(layer->cx, layer->cy);
        rc.Center(_rcClient.Width(), _rcClient.Height());
        //LRect rcCanvas(0, 0, layer.cx, layer.cy);
        //CenterCanvas(rc, rcCanvas);
        //INT x = 0, y = 0;
        //INT sbx, sby;
        //if ((sbx = layer.cx - rc.Width()) < 0) {
        //    rc.right = rc.left + layer.cx;
        //    x = -sbx / 2;
        //}
        //if ((sby = layer.cy - rc.Height()) < 0) {
        //    // rc = _curLayer->cy
        //    //rc.top = -sby / 2;
        //    rc.bottom = rc.top + layer.cy;
        //    y = -sby / 2;
        //}
        //rc.Offset(x, y);
    }
    VOID CalcScrollRegion(Layer *layer, LRect& rcScroll)
    {
        rcScroll = layer->viewPort;
    }
    //VOID CenterCanvas(LRect& rc, int cx, int cy)
    //{
    //    int x = 0, y = 0, dltX, dltY;

    //    if ((dltX = rc.Width() - cx) > 0) {
    //        rc.right = rc.left + cx;
    //        x = dltX / 2;
    //    }

    //    if ((dltY = rc.Height() - cy) > 0) {
    //        rc.bottom = rc.top + cy;
    //        y = dltY / 2;
    //    }

    //    rc.Offset(x, y);
    //}
    //VOID CenterCanvas(LRect& rc, LRect& rcDest)
    //{
    //    INT offsetX = 0;
    //    INT offsetY = 0;
    //    INT sbx, sby;
    //    if ((sbx = rcDest.Width() - rc.Width()) < 0) {
    //        rc.right = rc.left + rcDest.Width();
    //        offsetX = -sbx / 2;
    //    }
    //    if ((sby = rcDest.Height() - rc.Height()) < 0) {
    //        // rc = _curLayer->cy
    //        //rc.top = -sby / 2;
    //        rc.bottom = rc.top + rcDest.Height();
    //        offsetY = -sby / 2;
    //    }
    //    rc.Offset(offsetX, offsetY);
    //}
    LRESULT OnEraseBkgnd(LDC& dc) {
        return 1; /* dont erase the background */
    }
    VOID DrawTrackingLayer(LDC& dc)
    {
    }
    //VOID AddLayer()
    //{
    //}
    LRESULT OnCreate(LPCREATESTRUCT lpcs) 
    {
        __super::OnCreate(lpcs);

        /* create the bottom and tracking layers */
        //HDC hdc = GetDC();
        UINT layerFlags = 0;
        
        if (_uCtrlStyle & LVS_NO_VERT_SCROLLBAR)
            layerFlags |= LAYER_NO_VSCROLLBAR;
        if (_uCtrlStyle & LVS_NO_HORZ_SCROLLBAR)
            layerFlags |= LAYER_NO_HSCROLLBAR;

        Layer *layer = new Layer(Layer::BOTTOM, RGB(251, 251, 251), _T("BOTTOM"), 100, layerFlags);
        layer->Create(this, lpcs->cx, lpcs->cy);
        _layers.Add(layer);

        _trackingLayer = new Layer(Layer::TRACKING);
        _trackingLayer->Create(this, lpcs->cx, lpcs->cy);

        _cachedLayer = new Layer(Layer::CACHE, _clrBkgrnd);// RGB(64, 0, 128));
        _cachedLayer->Create(this, lpcs->cx, lpcs->cy);

        _curLayer = _layers[0];
        //SetActiveLayer(_layers[0]);
        //ReleaseDC(hdc);
        return 0; 
    }

    UINT _nPaints = 0, _nRedraws = 0;
    VOID OnPaint(LDC& dc)
    {
        {
            const char *cname = typeid(this).name();
            if (strstr(cname, "LWndTodo"))
            {
                int a = 0;
            }
        }

        _nPaints++;
        LRect rc;//(0, 0, _curLayer->cx, _curLayer->cy);
        if (0 != _uRedrawFlags)
        // if (REDRAW_CALCULATE & _uRedrawFlags)
        {
            // _dcCanvas.f
            //FillRect(dc, _rcClient,(HBRUSH)(COLOR_MENU+1));
            int cx, cy;
            LRect rcScroll;
            _nRedraws++;
            for (int i = 0; i < _layers.Count(); i++) {
                Layer *layer = _layers[i];
                if (_uRedrawFlags & REDRAW_CALCULATE) {
                    _pThis->CalcLayerSize(layer, _uRedrawFlags, cx, cy);
                    if (layer->Create(this, cx, cy) == FALSE)
                        continue;

                    /* all layers share the same view port */
                    _pThis->CalcViewRegion(layer, layer->viewPort);
                    /* the view port region should small than the canvas */
                    if (layer->viewPort.Width() > cx)
                        layer->viewPort.right = layer->viewPort.left + cx;
                    if (layer->viewPort.Height() > cy)
                        layer->viewPort.bottom = layer->viewPort.top + cy;

                    _pThis->CalcScrollRegion(layer, rcScroll);
                    layer->CalcScrollBarInfo(rcScroll);
                    // TODO: layer->viewPort should intersect with the whole visible client area (not include rulers etc.)
                    // layer->viewPort.Intersection(_rcClient); /* draw region can't larger then the client region */
                }
                /* else: just redraw the layer */
                layer->EraseBackground();
                _pThis->DrawLayer(layer, _uRedrawFlags);

                if (_uRedrawFlags & REDRAW_CALCULATE) {
                    // layer->_rngTrackings.Reset();
                    layer->ResetTrackingRngs();
                    _pThis->UpdateTrackingRngs(layer);
                }
                // ResetTrackingRngs();
                //layer->_rngTrackings.Reset();
                //_pThis->UpdateTrackingRngs();
            }
            if (_uRedrawFlags & REDRAW_CALCULATE) {
                _cachedLayer->Create(this, _rcClient.Width(), _rcClient.Height());
                _trackingLayer->Create(this, _rcClient.Width(), _rcClient.Height());
                SetActiveLayer(_curLayer); /* reset the current active layer */
            }

            _uRedrawFlags = 0;
            _nPaints = 0;
        }
        /* else: just bitblt all layers */

        //if (NULL == _curLayer->bm)
        //{
        //    dc.SetBkMode(TRANSPARENT);
        //    dc.DrawText(_T("ERROR: The size of layer is too large to draw."), _rcClient);
        //    return;
        //}
        //if (1)
        //        {
        //        //LRect rc(0, 0, min(_rcClient.Width(), _curLayer->cx), min(_rcClient.Height(), _curLayer->cy));
        //        //INT x = 0, y = 0;
        //        //if (_rcClient.Width() > _curLayer->cx)
        //        //    x = _rcClient.Width() - _curLayer->cx;
        //        //if (_rcClient.Height() > _curLayer->cy)
        //        //    y = _rcClient.Height() - _curLayer->cy;
        //        //rc.Offset(x/2, y/2);
        //        //IntersectClipRect(dc, _rcVisibleCanvas.left, _rcVisibleCanvas.top, _rcVisibleCanvas.right, _rcVisibleCanvas.bottom);
        //            }
        //LRect rcDraw = _rcView;
        ////rcDraw.Offset(_offsetX, _offsetY);
        //rcDraw.InflateRect(-_offsetX, -_offsetY);
        ////LRect rcCanvas;
        ////GetLayerRect(rcCanvas);
        ////rcDraw.Intersection(rcCanvas);
        //if (rcDraw.Width() > _curLayer->cx)
        //    rcDraw.right = rcDraw.left + _curLayer->cx;
        //if (rcDraw.Height() > _curLayer->cy)
        //    rcDraw.bottom = rcDraw.top + _curLayer->cy;
        //if (1)
        //{
        //    LBrush br(dc);
        //    LPen pen(dc);
        //    pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
        //    br.CreateStockObject(NULL_BRUSH);
        //    dc.Rectangle(_rcView);
        //    pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
        //    dc.Rectangle(_rcDraw);
        //    //return;
        //}

        //DrawScrollbar(dc);
        //dc.IntersectClipRect(_rcDraw);
            //{
            //    LRect rc2;
            //    GetClientRect(rc2);
            //    HRGN rgn = rc2.CreateRoundRectRgn(3, 3);
            //    SelectClipRgn(dc, rgn);
            //}

        //if (0)
        //{
        //    HBITMAP hbmClient = CreateCompatibleBitmap(dc, rc.Width(), rc.Height());
        //    LDC dcClient = ::CreateCompatibleDC(dc);
        //    dcClient.SelectObject(hbmClient);
        //    dcClient.FillRect(rc,(HBRUSH)(COLOR_BACKGROUND+1));
        //    dc.BitBlt(rc, dcClient, 0, 0);
        //    DeleteObject(hbmClient);
        //}
        //dc.FillRect(rc,(HBRUSH)(COLOR_WINDOW+1));
        // rc.Offset(100, 0);
        //dc.BitBlt(rc, _curLayer->dc, _curLayer->offset.x, _curLayer->offset.y);
        //dc.BitBlt(_curLayer->offset.x, _curLayer->offset.y, rc.Width(), rc.Height(), _curLayer->dc, 0, 0);
        // dc.BitBlt(_curLayer->offset.x, _curLayer->offset.y, _curLayer->cx, _curLayer->cy, _curLayer->dc, 0, 0);
        //{
            //int x =_curLayer->offset.x + _ptDragDlt.x;
            //int y =_curLayer->offset.y + _ptDragDlt.y;
            //x = min(x, 0);
            //y = min(y, 0);
            //x = max(x, rc.Width() - _curLayer->cx);
            //y = max(y, rc.Height() - _curLayer->cy);
        //LPoint dragDraw, *canvasDraw = &_curLayer->offset; /* adjust the offset of layer drawing every time */
        //if (_ptDragDlt.x || _ptDragDlt.y)
        //{   /* in drag & drop mode, dont change the _curLayer->offset */
        //    dragDraw = _curLayer->offset - _ptDragDlt;
        //    canvasDraw = &dragDraw;
        //}
        //// _pThis->AdjustLayerDrawOffset(_curLayer, *canvasDraw);
        //_curLayer->AdjustLayerDrawOffset(*canvasDraw);

        //dc.BitBlt(_rcDraw, _curLayer->dc, *canvasDraw);
        //__super::OnEraseBkgnd(_trackingLayer->dc);
        //{
        //    LBrush br(_trackingLayer->dc);
        //    br.CreateSolidBrush(INVALID_COLOR != _clrBkgrnd ? _clrBkgrnd : GetSysColor(COLOR_MENU));
        //    _trackingLayer->dc.FillRect(_rcClient, br);
        //}

        /* always draw the tracking layer */
        _trackingLayer->EraseBackground();
        _pThis->DrawTrackingLayer(_trackingLayer->dc);

        //if ((LVIEW_MOUSEDOWN & _nMouseStatus) && LVIEW_DRAG_SELECTION == _eDragMode)
        //    _pThis->DrawSelectionBox(_trackingLayer->dc); /* draw on tracking layer only */
        //_trackingLayer->dc.BitBlt(_rcView, _curLayer->dc, *canvasDraw);
        //_trackingLayer->DrawLayer(layer, _uRedrawFlags);
        // dc.BitBlt(_rcView.left + _offsetX, _rcView.top + _offsetY, _rcView.Width(), _rcView.Height(), _curLayer->dc, pos.x, pos.y);
        //}

#if 0
        /* draw anything on the fly when mouse down */
        if (LVIEW_MOUSEDOWN & _nMouseStatus)
        {
            _pThis->DrawDragTracking(_trackingLayer);
        }

        /* draw scroll bar if any */
        if ((LVIEW_DRAGDROP_VERT & _uDragFlags) && !(LVS_NO_VERT_SCROLLBAR & _uCtrlStyle)
            && (_sbHover == &_sbVert || _ptDragDlt.y != 0 || _ptDragDlt.x != 0)) {
            _sbVert.SetPos(canvasDraw->y);
            _sbVert.DrawLayer(_trackingLayer->dc);
        }
        if ((LVIEW_DRAGDROP_HORZ & _uDragFlags) && !(LVS_NO_HORZ_SCROLLBAR & _uCtrlStyle)
            && (_sbHover == &_sbHorz || _ptDragDlt.x != 0 || _ptDragDlt.y != 0)) {
            _sbHorz.SetPos(canvasDraw->x);
            _sbHorz.DrawLayer(_trackingLayer->dc);
        }
#endif

        if (GetKeyState(VK_RCONTROL) & 0x8000)
        {
            LSTR str;
            TCHAR cname_w[256] = {0};
            //TCHAR info[256];
            //GetWindowText(str);
            //_stprintf(info, _T("LWndView(%s)::OnPaint %d"),(LPCTSTR)str, GetDlgCtrlID(m_hWnd));
            LFont ft(_trackingLayer->dc);
            ft.CreateFont(_T("ËÎÌו"), 12, 0, RGB(200, 0, 0));
            const char *cname = typeid(this).name();
            MultiByteToWideChar(CP_ACP, 0, cname, -1, cname_w, strlen(cname));  
            LRect viewPort = _curLayer->viewPort;
            str.Format(_T("%s:_curLayer->offset(%d, %d).size(%d,%d), _nPaints %d, _nRedraws %d, _ptDragDlt(%d, %d), ")
                _T("viewPort(%d,%d,%d,%d), client(%d,%d,%d,%d), childs %d"),
                cname_w,
                _curLayer->offset.x, _curLayer->offset.y, _curLayer->cx, _curLayer->cy,
                _nPaints, _nRedraws, _ptDragDlt.x, _ptDragDlt.y, 
                viewPort.left, viewPort.top, viewPort.right, viewPort.bottom, 
                _rcClient.left, _rcClient.top, _rcClient.right, _rcClient.bottom,
                childs.Count());
            //dc.IntersectClipRect(_rcClient);
            //dc.IntersectClipRect(_rcDraw);
            //dc.TextOut(_rcView.left, _rcView.top + 20, str);
            LRect rc = viewPort;
            //rc.InflateRect(0, -20);
            _trackingLayer->dc.SetBkMode(TRANSPARENT);
            _trackingLayer->dc.DrawText(str, rc, DT_WORDBREAK);
        }

        //_pThis->DrawTrackingLayer(_trackingLayer->dc, _rcClient);
        // dc.BitBlt(_rcClient, _trackingLayer->dc, 0, 0);
        _cachedLayer->EraseBackground();
        //LBrush br(_cachedLayer->dc);
        //br.CreateSolidBrush(RGB(255, 255, 255));
        //_cachedLayer->dc.FillRect(_rcView, br);
        LPoint ptZero(0, 0);
        for (int i = 0; i < _layers.Count(); i++) {
            Layer *layer = _layers[i];
            if (layer == _curLayer)
                continue;
            layer->BitBlt(_cachedLayer->dc, /*layer != _curLayer ? ptZero : */_ptDragDlt);
        }
_curLayer->BitBlt(_cachedLayer->dc, _ptDragDlt); /* draw the selected layer at the most top */
        /* draw tracking layer */
        _cachedLayer->dc.TransparentBlt(_rcClient, _trackingLayer->dc, 0, 0, _trackingLayer->clrBkgrnd);
        //for (int i = 0; i < _layers.Count(); i++)
        //    _layers[i]->DrawScrollBar(_cachedLayer->dc, _layers[i] != _curLayer ? ptZero : _ptDragDlt);
_curLayer->DrawScrollBar(_cachedLayer->dc, _ptDragDlt);
        // _cachedLayer->dc.BitBlt(_rcClient, _trackingLayer->dc, 0, 0);
        // _cachedLayer->dc.AlphaBlend(_rcClient, _trackingLayer->dc, LPoint(0,0), 90);
        // dc.BitBlt(_rcClient, _trackingLayer->dc, 0, 0);

        ClipChildRegions(dc);
        dc.BitBlt(_rcClient, _cachedLayer->dc, 0, 0);
        //{
        //    LBrush br(dc);
        //    br.CreateSolidBrush(RGB(255, 255, 255));
        //    dc.FillRect(_rcClient, br);
        //}
        //dc.AlphaBlend(_rcClient, _cachedLayer->dc, LPoint(0,0), 200);
        dc.SelectClipRgn(NULL);
        //dc.TextOut(0, 60, _T("normal text"));
        //if (1)
        //{
        //    if (childs.Count() > 0)
        //    {
        //        LRect rc;
        //        childs[0]->GetWindowRect(rc);
        //        ScreenToClient(rc.lt);
        //        ScreenToClient(rc.rb);
        //        //MapWindowPoints(*childs[0], *this, &rc.lt, 1);
        //        //MapWindowPoints(*childs[0], *this, &rc.rb, 1);
        //        rc.InflateRect(3, 3);
        //        dc.Rectangle(rc);
        //    }
        //}

        //if (0)
        //{
        //    LBrush br(dc);
        //    LPen pen(dc);
        //    pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        //    br.CreateStockObject(NULL_BRUSH);
        //    dc.Rectangle(_curLayer->viewPort);
        //    //LPen pen2(dc);
        //    //pen2.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
        //    //dc.Rectangle(_rcView);
        //    //return;
        //}
        //if (!(_nMouseStatus & LVIEW_LBUTTONDOWN))
        //    _ptDragDlt.x = 0;
    }

    LRESULT OnMouseSync(LWIN_MOUSESYNC_INFO *msi)
    {
        _ASSERT(msi->src != this);
        
        switch (msi->msgId) {
        case MOUSESYNC_MSG_LVIEW_DRAGDROP:
            _pThis->OnMouseSyncDragDrop((LVIEW_DRAGDROP_INFO *)msi->para, msi->src);
            return 0;
        case MOUSESYNC_MSG_LVIEW_ZOOM:
            _pThis->OnMouseSyncZoom((LVIEW_MSGSYNC_ZOOM *)msi->para, msi->src);
            return 0;
        case MOUSESYNC_MSG_LVIEW_TRACKING:
            _pThis->OnMouseSyncTracking((LVIEW_MSGSYNC_TRACKING *)msi->para, msi->src);
            return 0;
        default:
            return __super::OnMouseSync(msi);
        }
    }
    //LRESULT LCALLBACK LWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    //{
    //    //T* pThis = static_cast<T *>(this); /* for 'overiding' */
    //    LVIEW_MOUSESYNC_INFO *pmsi;

    //    switch (msg)
    //    {
    //    case WM_LVIEW_MOUSESYNC:
    //        pmsi = (LVIEW_MOUSESYNC_INFO *) lParam;
    //        if ((LVIEW_MOUSESYNC_DRAGEND | LVIEW_MOUSESYNC_DRAGGING) & wParam)
    //        {
    //            _pThis->OnMouseDragDrop(wParam, pmsi->ptMouse, pmsi->ptDragDlt);
    //        }
    //        return 0;
    //    }
    //    return __super::LWndProc(hWnd, msg, wParam, lParam);
    //}
    /* the derived must call __super::OnMouseDragDrop to flash screen */
    // VOID OnMouseDragDrop(DWORD dwFlags, LPoint& pt, LPoint& ptDragDlt)
    VOID OnMouseSyncDragDrop(LVIEW_DRAGDROP_INFO *ddi, LWnd *sender)
    {
        //if ((LVIEW_MOUSESYNC_DRAGEND | LVIEW_MOUSESYNC_DRAGGING) & dwFlags)
        {   /* message synchronized from other windows */
            _ptDragDlt = ddi->ptDragDlt;
            // if (LVIEW_MOUSESYNC_DRAGEND & dwFlags)
            if (ddi->flags & LVIEW_MOUSESYNC_DRAGEND)
            {
#if 0
                LRect rc;
                int cx, cy;
                GetLayerRect(rc);
                if (0 == rc.Width() || 0 == rc.Height())
                {   /* if window was never shown, so it's size of client is not set */
                    _pThis->CalcLayerSize(_curLayer->dc, cx, cy); // FIXME!!!
                    SetCanvasSize(cx, cy);
                }
#endif
                //UpdateCanvasOffset();
                //_curLayer->offset -= _ptDragDlt;
                for (int i = 0; i < _layers.Count(); i++) {
                    Layer *layer = _layers[i];
                    layer->offset -= _ptDragDlt;
                }
                _ptDragDlt.x = 0;
                _ptDragDlt.y = 0;
            }
        //_nPaints = 0;

            // InvalidateOnDragDrop(_ptDragDlt);
            InvalidateRect(NULL, FALSE); /* FALSE: prevent flashing */
        }
        //else
        //{
        //    _ptLastDrag = pt;
        //    SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGGING, pt, ptDragDlt);
        //}

        //InvalidateRect(NULL, FALSE); /* FALSE: prevent flashing */
    }
    VOID OnMouseSyncZoom(LVIEW_MSGSYNC_ZOOM *msz, LWnd *sender)
    {
        /* do nothing now */
    }
    VOID OnMouseSyncTracking(LVIEW_MSGSYNC_TRACKING *mst, LWnd *sender)
    {
        _ptMouseMove = mst->mouse;
        InvalidateRect(NULL, FALSE);
    }

    /* dwFlags: MK_LBUTTON, MK_RBUTTON, MK_MBUTTON */
    VOID OnMouseDragSelectionDone(DWORD dwFlags, LRect& rc) { }

    VOID OnMouseHoverRect(Layer *layer, LVIEW_TRACKING_RNG* trIn, LVIEW_TRACKING_RNG* trOut, LPoint& pt) {}
    VOID OnMouseLeaveRect(Layer *layer, LVIEW_TRACKING_RNG* trOut) {}

    //VOID OnMouseHoverScrollbar(LVIEW_TRACKING_RNG* trIn, LVIEW_TRACKING_RNG* trOut, LPoint& pt)
    //{
    //    _sbHoverx = (ScrollBar *) trIn->pv;
    //    _sbHover->_flags = _sbHover->_rcThumb.PtInRect(pt) ? 1 : 0;
    //    //if (_sbHover->_rcThumb.PtInRect(pt))
    //    //    SetCapture();
    //    InvalidateRect(NULL, FALSE);
    //}
    //VOID OnMouseLeaveScrollbar(LVIEW_TRACKING_RNG* trOut)
    //{
    //    if (NULL != _sbHover)
    //        _sbHover->_flags = 0;
    //    _sbHover = NULL;
    //    InvalidateRect(NULL, FALSE);
    //}

    LRESULT OnButtonDown(DWORD dwFlags, LPoint& pt)
    {
        _ptMouseDown = pt;
        if (LVIEW_DRAG_DROP == _eDragMode || LVIEW_DRAG_SELECTION == _eDragMode)
        {
           (VOID)SetCapture();
        }
        //TestTrackingRngs(pt); /* check if the mouse still in current hovering item */
        //SetFocus(m_hWnd);
        return 0;
    }

    LRESULT OnLButtonDown(DWORD dwFlags, LPoint& pt)
    {
        _nMouseStatus |= LVIEW_LBUTTONDOWN;
        return OnButtonDown(dwFlags, pt);
    }
    LRESULT OnRButtonDown(DWORD dwFlags, LPoint& pt)
    {
        _nMouseStatus |= LVIEW_RBUTTONDOWN;
        return OnButtonDown(dwFlags, pt);
    }

    LRESULT OnButtonUp(DWORD dwFlags, LPoint& pt)
    {
        // if (LVIEW_DRAGDROP_ENABLE & _uDragFlags)
        if (LVIEW_DRAG_DROP == _eDragMode || LVIEW_DRAG_SELECTION == _eDragMode)
        {
            if (LVIEW_DRAG_DROP == _eDragMode)
            {
                // UpdateCanvasOffset();
                //_curLayer->offset -= _ptDragDlt;
                for (int i = 0; i < _layers.Count(); i++) {
                    Layer *layer = _layers[i];
                    layer->offset -= _ptDragDlt;
                }
                SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGEND, _ptDragDlt);
                // SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGGING, pt);
                _ptDragDlt.x = 0;
                _ptDragDlt.y = 0;
            }
            else if (LVIEW_DRAG_SELECTION == _eDragMode)
            {
                //T* pThis = static_cast<T *>(this); /* for 'overiding' */
                LRect rc(_ptMouseDown.x, _ptMouseDown.y, pt.x, pt.y);
                _curLayer->ClientToLayer(rc);
                _pThis->OnMouseDragSelectionDone(dwFlags, rc);
            }
           (VOID)ReleaseCapture();
            InvalidateRect(NULL, FALSE);
            // InvalidateOnDragDrop(_ptDragDlt);
            TestTrackingRngs(pt);
        }
        return 0;
    }

    LRESULT OnLButtonUp(DWORD dwFlags, LPoint& pt)
    {
        _nMouseStatus &= ~LVIEW_LBUTTONDOWN;
        return OnButtonUp(MK_LBUTTON, pt);
    }

    LRESULT OnRButtonUp(DWORD dwFlags, LPoint& pt)
    {
        _nMouseStatus &= ~LVIEW_RBUTTONDOWN;
        return OnButtonUp(MK_RBUTTON, pt);
    }

    LRESULT OnMouseMove(DWORD dwFlags, LPoint& pt)
    {
        _ptMouseMove = pt;
        if (LVIEW_MOUSEDOWN & _nMouseStatus)
        {   /* mouse dragging */
            //if (NULL != _sbHover && _sbHover->_rcThumb.PtInRect(pt))
            // if (NULL != _sbHover && (_sbHover->_flags || _sbHover->_rcThumb.PtInRect(pt)))
            if (_curLayer->sbHover && _curLayer->sbHover->_flags)
            {   /* dragging scroll bar */
                //_sbHover->_flags = 1;
                if (ScrollBar::VERTICAL & _curLayer->sbHover->_dir)
                    _ptDragDlt.y = _curLayer->sbHover->Scale(_ptMouseDown.y - pt.y);//(int)((float)(_ptMouseDown.y - pt.y) * _sbHover->_scale);
                else _ptDragDlt.x = _curLayer->sbHover->Scale(_ptMouseDown.x - pt.x); //(int)((float)(_ptMouseDown.x - pt.x) * _sbHover->_scale);

                // (VOID)_pThis->OnMouseDragDrop(0/*_nMouseStatus*/, pt, _ptDragDlt);
                //_ptLastDrag = pt;
                SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGGING, _ptDragDlt);
                // InvalidateOnDragDrop(_ptDragDlt);
                InvalidateRect(NULL, FALSE);
                //InvalidateRect(NULL, TRUE);
                return 0;
            }

            /* dragging the layer */
            if (LVIEW_DRAG_DROP == _eDragMode/* && (LVIEW_DRAGDROP_ENABLE & _uDragFlags)*/)
            {
                //T* pThis = static_cast<T *>(this); /* for 'overiding' */
                //_ptDragDlt.x = LVIEW_DRAGDROP_HORZ & _uDragFlags ? pt.x - _ptMouseDown.x : 0;
                //_ptDragDlt.y = LVIEW_DRAGDROP_VERT & _uDragFlags ? pt.y - _ptMouseDown.y : 0;
                _ptDragDlt.x = pt.x - _ptMouseDown.x;
                _ptDragDlt.y = pt.y - _ptMouseDown.y;
                // InvalidateOnDragDrop(_ptDragDlt);
               // (VOID)_pThis->OnMouseDragDrop(0/*_nMouseStatus*/, pt, _ptDragDlt);
                SyncMouseDragDrop(LVIEW_MOUSESYNC_DRAGGING, _ptDragDlt);
                // InvalidateOnDragDrop(_ptDragDlt);
                InvalidateRect(NULL, FALSE);
            }
            else if (LVIEW_DRAG_SELECTION == _eDragMode)
            { /* do nothing? */ }

            if (LVIEW_DRAG_SELECTION == _eDragMode || LVIEW_DRAG_DROP == _eDragMode) {
                InvalidateRect(NULL, FALSE);
                // InvalidateOnDragDrop(_ptDragDlt);
                //LPoint ptNewOff = _curLayer->offset - _ptDragDlt;
                //if (ptNewOff != _curLayer->offset)
                //    InvalidateRect(NULL, FALSE); /* FALSE: prevent screen from flashing */
            }
        }
        else
        {   /* checking if the mouse is in one of tracking regions */
            TestTrackingRngs(pt);
        }
        //InvalidateRect(NULL, FALSE);
        return 0;
    }

    VOID AddTrackingRng(Layer *layer, LVIEW_TRACKING_RNG& tr)
    {
        layer->_rngTrackings.Add(tr);
    }
    VOID AddTrackingRng(Layer *layer, LRect& rc, INT i)
    {
        LVIEW_TRACKING_RNG tr;
        tr.rc = rc;
        tr.i = i;
        layer->_rngTrackings.Add(tr);
    }
    VOID AddTrackingRng(Layer *layer, LRect& rc, VOID* pv)
    {
        LVIEW_TRACKING_RNG tr;
        tr.rc = rc;
        tr.pv = pv;
        layer->_rngTrackings.Add(tr);
    }
    template <typename T>
    VOID AddTrackingRng(Layer *layer, LArray<T> &rs)
    {
        for (int i = 0; i < rs.Count(); i++)
        {
            AddTrackingRng(layer, rs[i], &rs[i]);
        }
    }
    //VOID ResetTrackingRngs()
    //{
    //    _curLayer->_rngTrackings.Reset();
    //}

    /* add scroll-bar tracking region */
    VOID UpdateTrackingRngs(Layer *layer)
    {
        //LVIEW_TRACKING_RNG tr;

        //tr.wnd = this;
        ////tr.flags = TRACKING_RNG_CLIENT;
        //tr.pfnOnMouseHoverIn = (LVIEW_MOUSETRACKING_HOVER_IN)&LWndView<T>::OnMouseHoverScrollbar;
        //tr.pfnOnMouseHoverOut = (LVIEW_MOUSETRACKING_HOVER_OUT)&LWndView<T>::OnMouseLeaveScrollbar;
        //if (LVIEW_DRAGDROP_VERT & _uDragFlags) {
        //    tr.rc = _sbVert._rc;
        //    tr.pv = &_sbVert;
        //    AddTrackingRng(tr);
        //}
        //if (LVIEW_DRAGDROP_HORZ & _uDragFlags) {
        //    tr.rc = _sbHorz._rc;
        //    tr.pv = &_sbHorz;
        //    AddTrackingRng(tr);
        //}
    }

    LRESULT OnKillFocus(HWND hWndGetFocus)
    {
        _nMouseStatus &= ~LVIEW_MOUSEDOWN; /* reset mouse click status */
        TestTrackingRngs(LPoint(-1, -1)); /* cancel mouse move tracking */
        return __super::OnKillFocus(hWndGetFocus);
    }

protected:
    VOID _OnMouseHoverRect(Layer *layer, LVIEW_TRACKING_RNG* trIn, LVIEW_TRACKING_RNG* trOut, LPoint& pt) 
    {
        if (layer->IsTrackingScrollbar(trIn, TRUE)) {
            InvalidateRect(NULL, FALSE);
            return;
        }
        _pThis->OnMouseHoverRect(layer, trIn, trOut, pt);
    }
    VOID _OnMouseLeaveRect(Layer *layer, LVIEW_TRACKING_RNG* trOut)
    {
        if (layer->IsTrackingScrollbar(trOut, FALSE)) {
            InvalidateRect(NULL, FALSE);
            return;
        }
        _pThis->OnMouseLeaveRect(layer, trOut);
    }
    VOID TestTrackingRngs(LPoint& ptClient) // TODO: move to Layer
    {
        _curLayer->TestTrackingRngs(ptClient, this,
            (LVIEW_MOUSETRACKING_HOVER_IN)&LWndView<T>::_OnMouseHoverRect, 
            (LVIEW_MOUSETRACKING_HOVER_OUT)&LWndView<T>::_OnMouseLeaveRect);
#if 0
        LPoint ptCanvas = ptClient;

        if (_rcClient.PtInRect(ptClient))
        {
            _curLayer->ClientToLayer(ptCanvas);
            if (NULL != _curLayer->_rngTracking && _curLayer->_rngTracking->rc.PtInRect(
                   (TRACKING_RNG_CLIENT & _curLayer->_rngTracking->flags) ? ptClient : ptCanvas))
            {   /* mouse still in current tracking region */
                SetCapture(); /* keep capturing */
                return;
            }
            for (int i = 0; i < _curLayer->_rngTrackings.Count(); i++)
            {
                LVIEW_TRACKING_RNG& tr = _curLayer->_rngTrackings[i];
                if (tr.rc.PtInRect((TRACKING_RNG_CLIENT & tr.flags) ? ptClient : ptCanvas))
                {
                    SetCapture();
                    if (tr.wnd && tr.pfnOnMouseHoverIn)
                       (tr.wnd->*tr.pfnOnMouseHoverIn)(&tr, _curLayer->_rngTracking, ptClient);
                    else _pThis->OnMouseHoverRect(&tr, _curLayer->_rngTracking, ptClient);
                    _curLayer->_rngTracking = &tr;
                    return;
                }
            }
        }

        if (NULL != _curLayer->_rngTracking)
        {   /* now mouse is not in any regions */
            LVIEW_TRACKING_RNG& tr = *_curLayer->_rngTracking;
            if (tr.wnd && tr.pfnOnMouseHoverOut)
               (tr.wnd->*tr.pfnOnMouseHoverOut)(_curLayer->_rngTracking);
            else _pThis->OnMouseLeaveRect(_curLayer->_rngTracking);
            ReleaseCapture();
            _curLayer->_rngTracking = NULL;
        }
#endif
    }

//protected:
#if 0
    VOID AdjustVisibleCanvas()
    {
#if 0
        _rcVisibleCanvas.left = 0;
        _rcVisibleCanvas.top = 0;
        _rcVisibleCanvas.right = min(_rcClient.Width(), _curLayer->cx);
        _rcVisibleCanvas.bottom = min(_rcClient.Height(), _curLayer->cy);
        _rcVisibleCanvas.InflateRect(-30, 0);
        //LRect rc(0, 0, min(_rcClient.Width(), _curLayer->cx), min(_rcClient.Height(), _curLayer->cy));
        INT x = 0, y = 0, xDlt, yDlt;
        xDlt = _rcClient.Width() - _curLayer->cx;
        yDlt = _rcClient.Height() - _curLayer->cy;
        if (xDlt > 0)
        {
            if (LVS_HALIGN_RIGHT & _uCtrlStyle)
                x = xDlt;
            else if (!(LVS_HALIGN_LEFT & _uCtrlStyle))
                x = xDlt / 2; /* center */
            _rcVisibleCanvas.Offset(x, 0);
            _curLayer->offset.x = _rcVisibleCanvas.left;
        }
        if (yDlt > 0)
        {
            if (LVS_VALIGN_BOTTOM & _uCtrlStyle)
                y = yDlt;
            else if (!(LVS_VALIGN_TOP & _uCtrlStyle))
                y = yDlt / 2; /* center */
            _rcVisibleCanvas.Offset(0, y);
            _curLayer->offset.y = _rcVisibleCanvas.top;
        }
#endif
        _uDragFlags &= ~(LVIEW_DRAGDROP_HORZ | LVIEW_DRAGDROP_VERT);
        _offsetX = _offsetY = 0;
        if (_curLayer->cx > _prcDraw->Width())
            _uDragFlags |= LVIEW_DRAGDROP_HORZ;
        else
            _offsetX = (_prcDraw->Width() - _curLayer->cx) / 2;
        if (_curLayer->cy > _prcDraw->Height())
            _uDragFlags |= LVIEW_DRAGDROP_VERT;
        else
            _offsetY = (_prcDraw->Height() - _curLayer->cy) / 2;
    }
#endif
    virtual void SetActiveLayer(Layer *layer)
    {
        _curLayer = layer;
        InvalidateRect(NULL, FALSE);
    }
    //VOID SetActiveLayer(Layer *layer)
    //{
    //    //INT sbx, sby;

    //    //_uDragFlags &= ~(LVIEW_DRAGDROP_HORZ | LVIEW_DRAGDROP_VERT);

    //    //if ((sbx = layer->cx - layer->viewPort.Width()) > 0)
    //    //    _uDragFlags |= LVIEW_DRAGDROP_HORZ;

    //    //if ((sby = layer->cy - layer->viewPort.Height()) > 0)
    //    //    _uDragFlags |= LVIEW_DRAGDROP_VERT;

    //    //if (sbx > 0) {
    //    //    LRect rc = layer->viewPort; // _rcClient;
    //    //    rc.right -= (sby > 0 ? _sbVert._size : 0);
    //    //    _sbHorz.SetScrollInfo(rc, 0, sbx);
    //    //}

    //    //if (sby > 0) {
    //    //    LRect rc = layer->viewPort;// _rcClient;
    //    //    rc.bottom -= (sbx > 0 ? _sbHorz._size : 0);
    //    //    _sbVert.SetScrollInfo(rc, 0, sby);
    //    //}
    //    _curLayer = layer;
    //}
    //BOOL SetLayerSize(Layer *layer, INT cx, INT cy/*, UINT uRedrawFlags = REDRAW_CALCULATE*/)
    //{
    //    INT sbx, sby;
    //    //LRect rc;
    //    // GetClientRect(_rcClient);
    //    // if (bestfit?)
    //    if (!(LVS_NO_STRETCH_CANVAS & _uCtrlStyle))
    //    {   /* stretch to client */
    //        cx = max(cx, _rcView.Width());
    //        cy = max(cy, _rcView.Height());
    //    }

    //    if (_curLayer->cx != cx || _curLayer->cy != cy)
    //    {
    //        if (FALSE == _curLayer->Create(m_hWnd, cx, cy))
    //            return FALSE;
    //    }

    //    _uDragFlags &= ~(LVIEW_DRAGDROP_HORZ | LVIEW_DRAGDROP_VERT);

    //    if ((sbx = _curLayer->cx - _rcView.Width()) > 0)
    //        _uDragFlags |= LVIEW_DRAGDROP_HORZ;

    //    if ((sby = _curLayer->cy - _rcView.Height()) > 0)
    //        _uDragFlags |= LVIEW_DRAGDROP_VERT;

    //    if (sbx > 0) {
    //        LRect rc = _rcView; // _rcClient;
    //        rc.right -= (sby > 0 ? _sbVert._size : 0);
    //        _sbHorz.SetScrollInfo(rc, 0, sbx);
    //    }
    //    if (sby > 0) {
    //        LRect rc = _rcView;// _rcClient;
    //        rc.bottom -= (sbx > 0 ? _sbHorz._size : 0);
    //        _sbVert.SetScrollInfo(rc, 0, sby);
    //    }

    //    _trackingLayer->Create(m_hWnd, _rcClient.Width(), _rcClient.Height());

    //    return TRUE;
    //}

//private:
    VOID SyncMouseDragDrop(DWORD dwFlags, /*LPoint& pt,*/ LPoint& ptDragDlt)
    {
        LVIEW_DRAGDROP_INFO msi;
        //int i;
        //LWnd* wnd;

        //if (NULL == _wndDragSync) {
        //    return;
        //}
        //msi.ptMouse = pt;
        msi.flags = dwFlags;
        msi.ptDragDlt = ptDragDlt;
        // SyncMessage(WM_LVIEW_MOUSESYNC, dwFlags,(LPARAM)&msi);
        SyncMessage(MOUSESYNC_MSG_LVIEW_DRAGDROP, &msi);
        //for (i = 0; i < _wndDragSync->Count(); i++)
        //{
        //    wnd = (*_wndDragSync)[i];
        //    if (this != wnd) {
        //       (VOID)::SendMessage(*wnd, WM_LVIEW_MOUSESYNC, dwFlags,(LPARAM)&msi);
        //    }
        //}
    }


};

class LWndDrawView : public LWndView<LWndDrawView>
{
    //LPoint __pt;
    int _rcId;
    LArray<LRect> _rcTracks;
public:
    // VOID OnPaintx(LDC& dc)
    LWndDrawView()
    {
        _rcId = -1;
    }
    VOID CalcLayerSize(Layer *layer, UINT drawFlags, int& cx, int& cy)
    {
        cx = 1500;
        cy = 1500;
    }
    //VOID CalcViewRegion(Layer *layer, LRect& rc)
    //{
    //    // rc = _rcClient;
    //    GetClientRect(rc);
    //    rc.InflateRect(-20, -20);
    //    rc.left += 20;
    //    rc.Center(layer->cx, layer->cy);
    //    //rc.Offset(20, 0);
    //}
    VOID UpdateTrackingRngs(Layer *layer)
    {
        //__super::UpdateTrackingRngs();
        // AddTrackingRng<LRect>(_rcTracks);
        for (int i = 0; i < _rcTracks.Count(); i++)
        {
            AddTrackingRng(layer, _rcTracks[i], i);
        }
    }

    BOOL DrawLayer(Layer *layer, UINT& uDrawFlags)
    {
        LDC& dc = layer->dc;
        //rc.right = 10;
        //rc.bottom = 1000;
        //
        //SetCanvasSize(10, 1000);

        // GetLayerRect(rc);
        if (REDRAW_CALCULATE & uDrawFlags)
        {
            _rcTracks.Reset();
            LRect rc, rc0;
            GetLayerRect(rc);
            rc0 = rc;
            rc.right = rc.left + 100;
            rc.bottom = rc.top + 100;
            rc.right = min(rc.right, rc0.right);
            rc.bottom = min(rc.bottom, rc0.bottom);
            _rcTracks.Add(rc);

            GetLayerRect(rc);
            rc.left = rc.right - 100;
            rc.top = rc.bottom - 100;
            rc.left = max(rc.left, rc0.left);
            rc.top = max(rc.top, rc0.top);
            _rcTracks.Add(rc);
        }
        for (int i = 0; i < _rcTracks.Count(); i++)
        {
            LPen pen0(dc);
            pen0.CreatePen(_rcId == i ? PS_SOLID : PS_DOT, 1, RGB(255, 0, 0));
            dc.Rectangle(_rcTracks[i]);
        }

        {
            //LRect rc = _rcClient;
            //rc.InflateRect(-20, -20);
            //dc.Rectangle(rc);
            //int x = rc.left + rc.Width() / 2;
            //int y = rc.top + rc.Height() / 2;
            //int angle[] = {0, 900, 1800, 2700};
            //for (int i = 0; i < sizeof(angle)/sizeof(angle[0]); i++) {
            //    LFont ft(dc);
            //    ft.CreateFont(_T("Tahoma"), 20, 
            //        FW_NORMAL, LFont::CLR_NOT_SPECIFIED,
            //        angle[i]);
            //    rc.left = x;
            //    rc.top = y;
            //    rc.right = x + 200;
            //    rc.bottom = y + 200;
            //    //dc.DrawText(_T("::Hello world!"), rc, DT_CENTER/* | DT_VCENTER*/ | DT_SINGLELINE);
            //    dc.TextOut(x, y, _T("Hello world!"));
            //}

            //LFont ft(dc);
            //ft.CreateFont(_T("Tahoma"), 20, 
            //    FW_NORMAL, LFont::CLR_NOT_SPECIFIED,
            //    900);
            //rc.left = y;
            //rc.right = y - 100;
            //rc.right = max(rc.right, 0);
            //rc.top = x;
            //rc.bottom = x + 30;

            //rc.left = x;
            //rc.right = x + 100;
            //rc.bottom = y;
            //rc.top = 0;

            //dc.Rectangle(rc);
            ////LRect rc2;
            ////dc.CalcTextRect(_T("::Hello world!"), rc2);
            ////dc.Rectangle(rc2);
            //dc.DrawText(_T("::Hello world!"), rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            ////dc.TextOut(x, y, _T("Hello world!"));

            //dc.MoveTo(x, 0);
            //dc.LineTo(x, _rcClient.bottom);
            //dc.MoveTo(0, y);
            //dc.LineTo(_rcClient.right, y);

        }
#if 0
        rc.top = rc.bottom - 100;
        rc.left = rc.right - 100;
        //ResetTrackingRngs();

        LPen pen0(dc);
        pen0.CreatePen(_rcId == 0x8888 ? PS_SOLID : PS_DOT, 1, RGB(255, 0, 0));
        dc.Rectangle(rc);
        AddTrackingRng(rc, 0x8888);

        GetLayerRect(rc);
        rc.bottom = rc.top + 50;
        rc.right = rc.left + 50;

        LPen pen1(dc);
        pen0.CreatePen(_rcId == 0x9999 ? PS_SOLID : PS_DOT, 1, RGB(255, 0, 0));
        dc.Rectangle(rc);
        AddTrackingRng(rc, 0x9999);

        //GetClientRect(rc);
        //rc.InflateRect(-100, -200);
        //LPen pen1(dc);
        //pen1.CreatePen(_rcId == 0x9999 ? PS_SOLID : PS_DOT, 1, RGB(255, 0, 0));
        //dc.Rectangle(rc);
        //AddTrackingRng(rc, 0x9999);

        LSTR str;
        TCHAR info[256];
        GetWindowText(str);
        dc.SetBkMode(TRANSPARENT);
        _stprintf(info, _T("LWndDrawView(%p:%s)::OnPaint %d"), this,(LPCTSTR)str, GetDlgCtrlID(m_hWnd));
        dc.TextOut(0, 0, info);
#endif
        //uDrawFlags = 0;
        return TRUE;
    }

    //LRESULT OnMouseDragDrop(DWORD dwFlags, LPoint& pt, LPoint& ptDragDlt)
    //{
    //    __pt = pt;
    //    //SetCanvasOffset(ptDlt);
    //    // InvalidateRect(NULL, FALSE); /* FALSE: prevent flashing */
    //    __super::OnMouseDragDrop(dwFlags, pt, ptDragDlt);
    //    return 0;
    //}
    LRESULT OnCreate(LPCREATESTRUCT lpcs) 
    {
        __super::OnCreate(lpcs);


        // _cachedLayer->clrBkgrnd = RGB(255, 255, 255);
        _layers[0]->clrBkgrnd = RGB(255, 255, 255);
        _layers[0]->transparency = 0;
        return 0;
    }

    /* rc in layer coordination */
    VOID OnMouseDragSelectionDone(DWORD dwFlags, LRect& rc)
    {
        LSTR str;
        if (rc.Width() > 0 && rc.Height() > 0)
        {
            str.Format(_T("dwFlags:%x, %d,%d,%d,%d"), dwFlags, rc.left, rc.top, rc.right, rc.bottom);
            MessageBox(str);
        }
    }

    VOID OnMouseHoverRect(Layer *layer, LVIEW_TRACKING_RNG* trIn, LVIEW_TRACKING_RNG* trOut, LPoint& pt) 
    {
        _rcId = trIn->i;
        SetRedrawFlags(REDRAW_PAINT, TRUE);
    }
    VOID OnMouseLeaveRect(Layer *layer, LVIEW_TRACKING_RNG* trOut) 
    {
        _rcId = -1;
        SetRedrawFlags(REDRAW_PAINT, TRUE);
    }

    //VOID DrawDragTracking(LDC& dc)
    //{
    //    //LSTR str;
    //    //LPoint pt = __pt;
    //    //dc.MoveTo(__pt.x, __pt.y - 40);
    //    //dc.LineTo(__pt.x, __pt.y + 40);
    //    //dc.MoveTo(__pt.x - 40, __pt.y);
    //    //dc.LineTo(__pt.x + 40, __pt.y);
    //    //ClientToLayer(pt);
    //    //str.Format(_T("[%d,%d]->[%d,%d] this is a dragging text!"), pt.x, pt.y, __pt.x, __pt.y);
    //    //dc.TextOut(__pt.x + 5, __pt.y + 5, str);
    //    //if (LVIEW_DRAG_DROP == GetDragMode())
    //    //    dc.TextOut(__pt.x, __pt.y, _T("this is a draging text"));
    //    __super::DrawDragTracking(dc);
    //}

    LRESULT OnSetFocus(HWND hWndLostFocus)
    {
        LSTR str;
        str.Format(_T("%p get focus, %p lost"), m_hWnd, hWndLostFocus);
        MessageBox(str);
        return __super::OnSetFocus(hWndLostFocus);
    }
};

//template <class T>
//LRESULT LWndView<T>::SyncMouseDragDrop(DWORD dwFlags, LPoint& pt, LPoint& ptDragDlt)
//{
//    if (NULL != __wndDragSync)
//    {
//        LVIEW_MOUSESYNC_INFO msi;
//        msi.ptMouse = pt;
//        msi.ptDragDlt = ptDragDlt;
//        ::SendMessage(*__wndDragSync, WM_LVIEW_MOUSESYNC, dwFlags,(LPARAM)&msi);
//        // __wndDragSync->OnMouseDragDrop(dwFlags, pt, ptDragDlt);
//    }
//    return 0;
//}

//class LDlgView : public LWndView<LDlgView>
//{
//public:
//    //enum {IDD = IDD_DIALOG1};
//};