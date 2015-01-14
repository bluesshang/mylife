
#pragma once

template<class T>
class LArray
{
    int count;
    int size;
    T *data;
public:
    ~LArray()
    {
        delete[] data;
    }
    LArray(int n = 256)
    {
        data = new T[n];
        size = n;
        count = 0;
    }

    //T& operator[](LPCTSTR key)
    //{
    //    for (int i = 0; i < count; i++) {
    //        if (data[i] == key)
    //            return data[i];
    //    }
    //    _ASSERT(0);
    //    return data[0];
    //}
    T& operator [](int pos)
    {
        _ASSERT(pos < size);
        return data[pos];
    }

    // int Size() {return size;}
    int Count() {return count;}
    T& Add(T t)
    {
        
        _ASSERT(count < size);
        T& _t = (*this)[count++] = t; /* do deep copy */
        return _t;
    }
    VOID Reset()
    {
        count = 0;
    }
};

class DVS;
class _DV
{
protected:
    LSTR _name, _dispname, _format;
    INT _size, _refs;
public:
    DVS *dvs;
    // virtual VOID GetText(INT pos, LSTR& str, LPCTSTR lpFmt = NULL) = 0;
    LPCTSTR GetDispName() { return _dispname; }
    LPCTSTR GetName() { return _name; }
    virtual INT GetText(INT pos, LSTR& str, LPCTSTR fmt = NULL) = 0;
    virtual INT GetInt(INT pos) = 0;
    virtual BOOL SetSize(INT size) = 0;
    virtual _DV* MA(int intval) = 0;
    INT operator[](INT pos) { return GetInt(pos); }
    // float operator[](INT pos) { return .0; }
    // void AddRef() { _refs += 1; }

    INT Size(BOOL all = FALSE);
    INT GetMaxInt(BOOL all = FALSE);
    INT GetMinInt(BOOL all = FALSE);
};

class VDATA32
{
    union
    {
        int i;
        long l;
        float f;
        void *ptr;
        LSTR *str;
    };
public:

};
template<class T>
class DV : public _DV
{
    T *_data;
    //VOID(*_format_text)(T*, LSTR&);
public:
    ~DV()
    {
        delete[] _data;
    }
    // DV(LPCTSTR name = NULL, LPCTSTR dispname = NULL, LPCTSTR format = NULL)
    DV(INT size, LPCTSTR name = NULL, LPCTSTR dispname = NULL, LPCTSTR format = NULL)
    {
        //_ASSERT(size > 0);
        _data = NULL;// new T[size];
        // _size = size;
        _name = name;
        _dispname = dispname ? dispname : name;
        _format = format ? format : _T("%d");
        //_format_text = NULL;
        //{
        //    for (int i = 0; i < size; i++)
        //    {
        //        _data[i] = rand();
        //    }
        //}
        SetSize(size);
    }
    BOOL SetSize(INT size)
    {
        if (NULL != _data && _size != size)
            delete[] _data;
        _data = new T[size];
        _size = size;
        //{
        //    for (int i = 0; i < size; i++)
        //    {
        //        _data[i] = (T)rand();
        //    }
        //}
        return TRUE;
    }
    T& operator [](int pos)
    {
        _ASSERT(pos < _size);
        return _data[pos];
    }
    INT GetText(INT pos, LSTR& str, LPCTSTR fmt = NULL)
    {
        //str = _data[pos];
        str.Format(fmt ? fmt : _format, _data[pos]);
        return str.Len();
    }
    INT GetInt(INT pos)
    {
        return (INT)_data[pos];
    }
    DV<T>* MA(int intval)
    {
        if (intval > _size)
            return NULL;

        DV<T> *dv = new DV<T>(_size);
        dv->dvs = dvs;

        T t = 0;
        for (int i = 0; i < intval - 1; i++) {
            (*dv)[i] = 0;
            t += _data[i];
        }

        //T *sum = new T[intval];
        for (int i = intval - 1; i < _size; i++) {
            t += _data[i];
            (*dv)[i] = t / (T)intval;
            t -= (*dv)[i - (intval - 1)];
        }
        return dv;
    }
};

class DVS
{
    LArray<_DV*> _dvs;
    INT _rows;
    INT *_rowids; /* for sorting */
public:
    int x0, xn; /* current focused range [x0, xn) on DV */
    DVS(INT rows) : _rows(rows)
    {
        _rowids = new INT[rows];
        for (int i = 0; i < rows; i++)
        {
            _rowids[i] = i;
        }

        x0 = 0;
        xn = rows;
    }
    ~DVS()
    {
        for (int i = 0; i < _dvs.Count(); i++)
            delete _dvs[i];
        delete[] _rowids;
    }
    //_DV* operator[](int row) { return _dvs[row]; }
    _DV& operator[](int col) { return *_dvs[col]; }
    _DV& operator[](LPCTSTR colName) 
    {
        for (int i = 0; i < _dvs.Count(); i++) {
            if (_tcscmp(_dvs[i]->GetName(), colName) == 0)
                return *_dvs[i];
        }
        _ASSERT(0);
        return *_dvs[0]; 
    }
    // DV<LSTR>& operator[](int row) { return *_dvs[row]; }
    INT Rows() { return _rows; }
    INT Cols() { return _dvs.Count(); }

    VOID Add(_DV* dv)
    {
        dv->dvs = this;
        _ASSERT(0 == dv->Size(TRUE) || dv->Size(TRUE) >= _rows);
        if (0 == dv->Size(TRUE))
            dv->SetSize(_rows);
        _dvs.Add(dv);
    }

    /* functions used for displaying */
    int GetText(int row, int col, LSTR& str, LPCTSTR fmt = NULL)
    {
        _ASSERT(NULL != _dvs[col]);
        _ASSERT(row < _rows);
        return _dvs[col]->GetText(_rowids[row], str, fmt);
    }
    int GetText(int cellid, LSTR& str, LPCTSTR fmt = NULL)
    {
        //_ASSERT(NULL != _dvs[col]);
        //_ASSERT(row < _rows);
        int row = cellid / Cols();
        int col = cellid % Cols();
        return _dvs[col]->GetText(_rowids[row], str, fmt);
    }
    int GetName(int col, LSTR& str)
    {
        _ASSERT(NULL != _dvs[col]);
        str = _dvs[col]->GetDispName();
        return str.Len();
    }
    UINT GenCellId(UINT row, UINT col)
    {
        return row * Cols() + col;
    }
};

INT _DV::Size(BOOL all/* = FALSE*/)
{ 
    if (all == FALSE)
        return dvs->xn - dvs->x0;
    return _size; 
}

INT _DV::GetMaxInt(BOOL all/* = FALSE*/)
{
    INT mx = 0, t;
    int x0 = all ? 0 : dvs->x0;
    int xn = all ? _size : dvs->xn;
    // for (int i = 0; i < _size; i++) {
    for (int i = x0; i < xn; i++) {
        t = GetInt(i);
        mx = max(mx, t);
    }
    return mx;
}
INT _DV::GetMinInt(BOOL all/* = FALSE*/)
{
    INT mi = 0x7fffffff, t;
    //for (int i = 0; i < _size; i++) {
    int x0 = all ? 0 : dvs->x0;
    int xn = all ? _size : dvs->xn;
    // for (int i = 0; i < _size; i++) {
    for (int i = x0; i < xn; i++) {
        t = GetInt(i);
        mi = min(mi, t);
    }
    return mi;
}
