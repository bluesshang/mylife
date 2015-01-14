
#include "stdafx.h"
#include "LWndFrame.h"

LWndCell::~LWndCell()
{
    if (cells) delete cells;
}

BOOL LWndCell::Split (int rows, int cols, float *widths/* = NULL*/, float *heights/* = NULL*/)
{
    type = CELLS;
    cells = new LWndCells();
    cells->SetCellsInfo (rows, cols, this, widths, heights);
    return TRUE;
}

template<class T>
void LWndFrame<T>::LayoutCells (LWndCell& cell, LRect& rc)
{
    // cell.rc = rc;
    if (LWndCell::WNDS == cell.type)
    {
    }
    else
    {
    }
    
}

template<class T>
void LWndFrame<T>::DrawFrame (LWndCell& cell, LDC& dc)
{
}
