#pragma once

#include "..\stdafx.h"
#include "..\utils\larray.h"
#include "..\utils\lstr.h"

#define VERTEX_W 9

class Line : public GV
{
// protected:
public:
    _DV* _dv;
    COLORREF _color;
    LSTR _name;

public:
    Line() {}
    Line(_DV *data, COLORREF color = RGB(0, 0, 0), LPCTSTR name = _T("[No Name]"))
        :_dv(data), _color(color), _name(name)
    {
    }
    // int 
    virtual INT GetMaxInt(BOOL all = FALSE) { return _dv->GetMaxInt(all); }
    virtual INT GetMinInt(BOOL all = FALSE) { return _dv->GetMinInt(all); }
    virtual INT Size(BOOL all = FALSE) { return _dv->Size(all); }
    virtual _DV* GetDV(LPCTSTR dvName) 
    {
        if (dvName == NULL)
            return _dv; /* return self */
        return &(*_dv->dvs)[dvName]; 
    }
    virtual VOID Draw(LDC& dc, AxisInfo& ai)
    {
        _DV& dv = *_dv;
        LPen pen(dc);
        pen.CreatePen(PS_SOLID, 1, _color);
        // dc.MoveTo(0, dv[0]);
        // dc.MoveTo(ai.xwidth / 2, cy - (int)((double)(dv[0] - mi) * scale));
        dc.MoveTo(ai.d2x(_dv->dvs->x0), ai.d2y(dv[0]));
        for (int i = _dv->dvs->x0 + 1; i < _dv->dvs->xn; i++)
        {
            //int x = i * (ai.xwidth + 1) + ai.xwidth / 2;
            //int y = cy - (int)((double)(dv[i] - mi) * scale);
            //int x = ai.d2x(i);
            //int y = ai.d2y(dv[i]);
            dc.LineTo(ai.d2x(i), ai.d2y(dv[i]));
        }
    }
};

class FilledLine : public Line
{
public:
    COLORREF colorFill;
    int extraBottom;
    //int x1, x2;
public:
    // FilledLine
    FilledLine(_DV *data, COLORREF color = RGB(0, 0, 0), COLORREF colorFill = RGB(200, 200, 0), int extraBottom = 30) 
        : Line(data, color), colorFill(colorFill), extraBottom(extraBottom)
    {
        //x1 = 0;
        //x2 = data->Size() - 1;
    }

    //void SetFilledRange(int x1, int x2)
    //{
    //    this->x1 = x1;
    //    this->x2 = x2;
    //}
    virtual VOID Draw(LDC& dc, AxisInfo& ai)
    {
        //SIZE wndExt, viewExt;
        //dc.GetWindowExtEx(&wndExt);
        //dc.GetViewportExtEx(&viewExt);
        //int width = 1; // (int)((float)viewExt.cx / (float)wndExt.cx);

        /* set 1:1 scale on x-axis */
        //dc.SetWindowExt(1, wndExt.cy);
        //dc.SetViewportExt(1, viewExt.cy);

        //LSTR str;
        //str.Format(_T("Line::Draw, total %d points, min %d, max %d"), _dv->Size(), _dv->GetMinInt(), _dv->GetMaxInt());
        //dc.TextOut(0, _dv->GetMinInt() * 2, str);

        _DV& dv = *_dv;
        //int mx = dv.GetMaxInt();
        //int mi = dv.GetMinInt();
        //int cy = ai.rc.bottom - extraBottom;
        //int cx = ai.rc.right;
        //double scale = (double)cy / (double)(mx - mi);
        //int yOff = extraBottom;
        //int yMax = dv.GetMaxInt(TRUE);
        //int yMaxPos = ai.d2y(yMax);
        //if (yMaxPos - yOff < ai.rc.top)
        //    yOff -= (ai.rc.top - yMaxPos);
        {
            //int x1 = rand() % dv.Size(), x2 = x1 + rand() % 800;
            //x2 -= 1;
            //extern LRect _rcLastZoomed;
            //x1 = ai.x2d(_rcLastZoomed.left);
            //x2 = ai.x2d(_rcLastZoomed.right);
            //x1 = max(0, x1);
            //x2 = min(x2, dv.Size() - 1);
            //if (x2 == -1)
            //    x2 = dv.Size() - 1;
            int x1 = dv.dvs->x0;
            int x2 = dv.dvs->xn;
            int numPts = x2 - x1/* + 1*/;
            POINT *pts = new POINT[numPts + 2];
            int j;

            // pts[0].x = x1 * width;
            // pts[0].x = (int)((double)x1 * (double)cx / (double)_dv->Size());
            pts[0].x = ai.d2x(x1);
            //pts[0].y = cy + extraBottom;
            pts[0].y = ai.rc.bottom + extraBottom;
            j = 1;
            for (int i = x1; i < x2; i++)
            {
                // pts[j].x = i * width;
                // pts[j].x = (int)((double)i * (double)cx / (double)_dv->Size());;
                pts[j].x = ai.d2x(i);
                // pts[j].y = dv[i];
                // pts[j].y = cy - (int)((double)(dv[i] - mi) * scale);
                pts[j].y = ai.d2y(dv[i]);
                j++;
            }
            // pts[j].x = x2 * width;
            pts[j].x = ai.d2x(x2 - 1); // (int)((double)x2 * (double)cx / (double)_dv->Size());;
            pts[j].y = ai.rc.bottom + extraBottom;

            HRGN rgn = CreatePolygonRgn(pts, numPts + 2, WINDING);
            LBrush br(dc);
            br.CreateSolidBrush(colorFill);
            FillRgn(dc, rgn, br);
            delete[] pts;
            DeleteObject(rgn);
        }

        LPen pen(dc);
        pen.CreatePen(PS_SOLID, 1, _color);
        // dc.MoveTo(0, dv[0]);
        // dc.MoveTo(ai.xwidth / 2, cy - (int)((double)(dv[0] - mi) * scale));
        dc.MoveTo(ai.d2x(0), ai.d2y(dv[0])/* - yOff*/);
        for (int i = 1; i < _dv->Size(TRUE); i++) {
            //int x = i * (ai.xwidth + 1) + ai.xwidth / 2;
            //if (ai.xwidth < 0)
            //    x = (int)((double)i * (double)cx / (double)_dv->Size());
            //int y = cy - (int)((double)(dv[i] - mi) * scale);
            int x = ai.d2x(i);
            int y = ai.d2y(dv[i])/* - yOff*/;
            dc.LineTo(x, y);
        }
        /* restore the original scale */
        //dc.SetWindowExt(wndExt.cx, wndExt.cy);
        //dc.SetViewportExt(viewExt.cx, viewExt.cy); bluesssssssss
        // dc.FrameRect(ai.rc, )
    }
};

class KLine : public Line
{
    _DV *_open, *_close, *_high, *_low;
public:
    KLine(_DV *open, _DV *close, _DV *high, _DV *low)
        : _open(open), _close(close), _high(high), _low(low)
    {
        _dv = _close; // XXX
        //_dv = new DV<float>(882);
        //DV<float> &l0 = *(DV<float>*)_dv;
        ////fmin = 400000; fmax = 0;
        //{
        //    FILE *f = fopen("d:\\stock.txt", "r");
        //    int price;
        //    int i = 0;
        //    while(!feof(f))
        //    {
        //        price = 0.0;
        //        fscanf(f, "%d\r\n", &price);
        //        //price /= 15;
        //        l0[i] = price / 1;
        //        //fmin = min(fmin, l0[i]);
        //        //fmax = max(fmax, l0[i]);
        //        i += 1;
        //    }
        //    fclose(f);
        //}
    }

    INT GetMaxInt(BOOL all = FALSE) { return _high->GetMaxInt(all); }
    INT GetMinInt(BOOL all = FALSE) { return _low->GetMinInt(all); }
    INT Size(BOOL all = FALSE) { return _open->Size(all); }

    VOID Draw(LDC& dc, AxisInfo& ai)
    {
        //LSTR str;
        //str.Format(_T("KLine::Draw, total %d points, min %d, max %d"), _dv->Size(), _dv->GetMinInt(), _dv->GetMaxInt());
        //dc.TextOut(0, _dv->GetMinInt() * 2, str);
        //SIZE wndExt, viewExt;
        //dc.GetWindowExtEx(&wndExt);
        //dc.GetViewportExtEx(&viewExt);
        //int width = (int)((float)viewExt.cx / (float)wndExt.cx);

        _DV& dv = *_dv;

                ////if (i == 200) {
                //    int x = 200 * VERTEX_W, y = dv[200];
                //    LRect rc = {x, y, x, y};
                //    rc.InflateRect(VERTEX_W / 2, 200);
                //    ////dc.Ellipse(rc);
                //    dc.Rectangle(rc);
                ////}
        /* set 1:1 scale on x-axis */
        //dc.SetWindowExt(1, wndExt.cy);
        //dc.SetViewportExt(1, viewExt.cy);
        
        //dc.MoveTo(0, dv[0]);
        //int mx = dv.GetMaxInt();
        //int mi = dv.GetMinInt();
        //int cy = ai.rc.bottom;
        //double scale = (double)cy / (double)(mx - mi);

        // for (int i = 0; i < _dv->Size(); i++)
        for (int i = _dv->dvs->x0; i < _dv->dvs->xn; i++) {
            //int x = i * (ai.xwidth + 1);
            //int x = ai.d2x(i) - ai.xwidth / 2;
            int x = ai.d2x(i);// - ai.xwidth / 2;
            //{
            //    str.Format(_T("%d"), dv[i]);
            //    //if (i % 50 == 0) {
            //    if (dv[i] == 270) {
            //        dc.MoveTo(x - 50, dv[i]);
            //        dc.LineTo(x, dv[i]);
            //        dc.TextOut(x, dv[i], str);
            //        dc.MoveTo(x, dv[i - 1]);
            //    }

            //}
            //int y = dv[i];
            LRect rc;// = {x, 0, x + ai.xwidth, 0};
            // rc.InflateRect(width / 2, 0);
            // rc.top = (*_open)[i];
            //rc.top = cy - (int)((double)((*_open)[i] - mi) * scale);
            //rc.bottom = cy - (int)((double)((*_close)[i] - mi) * scale);
            rc.left = x - ai.xwidth / 2;
            rc.right = x + ai.xwidth / 2 + (ai.xwidth & 1); /* xwidth is odd, should extend one more pixel */
            rc.top = ai.d2y((*_open)[i]); // cy - (int)((double)((*_open)[i] - mi) * scale);
            rc.bottom = ai.d2y((*_close)[i]); // cy - (int)((double)((*_close)[i] - mi) * scale);
            //if (rc.top == rc.bottom)
            //    rc.bottom += 1;
            // rc.left += 1;
            //dc.Ellipse(rc);
            //dc.MoveTo(x + ai.xwidth / 2, cy - (int)((double)((*_high)[i] - mi) * scale));
            //dc.LineTo(x + ai.xwidth / 2, cy - (int)((double)((*_low)[i] - mi) * scale));
            dc.MoveTo(x, ai.d2y((*_high)[i]));
            dc.LineTo(x, ai.d2y((*_low)[i]));

            LBrush br(dc);
            COLORREF clr = rc.bottom < rc.top ? RGB(0, 0, 0) : RGB(255, 255, 255);
            //if (i == 34)
            //    clr = RGB(255, 0, 0);
            br.CreateSolidBrush(clr);
            if (rc.top == rc.bottom) {
                dc.MoveTo(rc.left, rc.top);
                dc.LineTo(rc.right, rc.top);
            } else
                dc.Rectangle(rc);
        }

        //dc.MoveTo(0, 1760);
        //dc.LineTo(_dv->Size() * ai.xwidth, 1760);
        ///* restore the original scale */
        //dc.SetWindowExt(wndExt.cx, wndExt.cy);
        //dc.SetViewportExt(viewExt.cx, viewExt.cy);
    }
};

class VolLine : public Line
{
public:
    VolLine(_DV *dv) : Line(dv) {}
    VOID Draw(LDC& dc, AxisInfo& ai)
    {
        //SIZE wndExt, viewExt;
        //dc.GetWindowExtEx(&wndExt);
        //dc.GetViewportExtEx(&viewExt);
        //int width = (int)((float)viewExt.cx / (float)wndExt.cx);

        ///* set 1:1 scale on x-axis */
        //dc.SetWindowExt(1, wndExt.cy);
        //dc.SetViewportExt(1, viewExt.cy);

        //int mx = _dv->GetMaxInt();
        //int mi = _dv->GetMinInt();
        //int cy = ai.rc.bottom;
        //double scale = (double)cy / (double)(mx - mi);

        // for (int i = 0; i < _dv->Size(); i++)
        for (int i = _dv->dvs->x0; i <= _dv->dvs->xn; i++) {
            // int x = i * (ai.xwidth + 1);
            int x = ai.d2x(i);
            //{
            //    str.Format(_T("%d"), dv[i]);
            //    //if (i % 50 == 0) {
            //    if (dv[i] == 270) {
            //        dc.MoveTo(x - 50, dv[i]);
            //        dc.LineTo(x, dv[i]);
            //        dc.TextOut(x, dv[i], str);
            //        dc.MoveTo(x, dv[i - 1]);
            //    }

            //}
            //int y = dv[i];
            LRect rc;// = {x, 0, x + ai.xwidth, 0};
            //rc.InflateRect(width / 2, 0);
            // rc.top = (*_dv)[i];
            rc.left = x - ai.xwidth / 2;
            rc.right = x + ai.xwidth / 2 + (ai.xwidth & 1);
            rc.top = ai.d2y((*_dv)[i]);// cy - (int)((double)((*_dv)[i] - mi) * scale);
            rc.bottom = ai.rc.bottom;
            if (rc.right == rc.left)
                rc.right += 1;
            if (rc.bottom == rc.top)
                rc.top -= 1;
            //rc.left += 1;
            //dc.Ellipse(rc);
            //dc.MoveTo(x, (*_high)[i]);
            //dc.LineTo(x, (*_low)[i]);
            LBrush br(dc);
            // br.CreateSolidBrush(rc.bottom > rc.top ? RGB(0, 0, 0) : RGB(255, 255, 255));
            br.CreateSolidBrush(RGB(0, 0, 0));
            dc.Rectangle(rc);
        }

        /* restore the original scale */
        //dc.SetWindowExt(wndExt.cx, wndExt.cy);
        //dc.SetViewportExt(viewExt.cx, viewExt.cy);
    }
};
