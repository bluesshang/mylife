
#pragma once

#include "ldb.h"

#import "C:\\Windows\\winsxs\\x86_microsoft-windows-m..ents-mdac-ado15-dll_31bf3856ad364e35_6.1.7601.17857_none_0e101351cf067d1f\\msado15.dll" \
    no_namespace rename("EOF", "adoEOF")

class OraDB : public LDB
{
    _ConnectionPtr _conn;

public:
    OraDB(TCHAR *connStr)
    {
        try {
            _conn.CreateInstance(__uuidof(Connection));
            _conn->Open(connStr, "", "", adModeUnknown);
        } catch (_com_error e) {
            MessageBox(NULL, e.Description(), e.ErrorMessage(), 0);
        }
    }
    
    ~OraDB()
    {
        _conn->Close();
    }

    DVS* Fetch(TCHAR *sql)
    {
        // DVS *dvs = new  DVS()
        try {
            _RecordsetPtr rs;
            rs.CreateInstance(__uuidof(Recordset));
            //rs->Open(sql, _variant_t((IDispatch *)_conn, true), adOpenKeyset, adLockOptimistic, adCmdText);
            //VARIANT rows;
            //rows.vt = VT_INT;
            //rows.intVal = 1;
            //rs = _conn->Execute(sql, &rows, adOptionUnspecified); // can't use the rs->RecordCount
            rs->Open(sql, _variant_t((IDispatch *)_conn, true), adOpenKeyset, adLockOptimistic, adCmdText);

            long numRow = rs->RecordCount;
            DVS *dvs = new DVS(numRow);
            //rs->GetRows()
            int cols = rs->Fields->Count;
            for (int i = 0; i < cols; i++) {
                // dvs->Add(new DV<int>(0, rs->Fields[i]->Name));
                _variant_t varIdx;// = (long)i;
                varIdx.vt = VT_I4;
                varIdx.intVal = i;

                FieldPtr fld = rs->Fields->GetItem(varIdx);
                _variant_t val = fld->Value;

                switch (val.vt) {
                case VT_DATE:
                {
                    DV<int> *dv = new DV<int>(numRow, fld->Name);
                    dv->SetTextFormat(format_date);
                    dvs->Add(dv);
                    break;
                }
                case VT_BSTR:
                    dvs->Add(new DV<LSTR>(numRow, fld->Name));
                    break;
                default:
                case VT_I4:
                    //DV<int> &dv = *(DV<int>*)&(*dvs)[j];
                    //dv[i] = val.iVal;
                    dvs->Add(new DV<int>(numRow, fld->Name));
                    break;
                }

                //varIdx.iVal
                //dvs->Add(new DV<int>(numRow, rs->Fields->GetItem(varIdx)->Name));
            }

            for (int i = 0; i < numRow; i++) {
                for (int j = 0; j < cols; j++) {
                    FieldPtr fld = rs->Fields->GetItem((long)j);
                    _variant_t val = fld->Value;
                    switch (val.vt) {
                    case VT_BSTR: {
                        DV<LSTR> &dv = *(DV<LSTR>*)&(*dvs)[j];
                        dv[i] = val.bstrVal;
                        }
                        break;
                    case VT_DATE:
                    {
                        DV<int> &dv = *(DV<int>*)&(*dvs)[j];
                        SYSTEMTIME st;
                        VariantTimeToSystemTime(val.date, &st);
                        dv[i] = st.wYear << 16 | st.wMonth << 8 | st.wDay;
                    }
                        break;
                    case VT_DECIMAL: {
                        DV<int> &dv = *(DV<int>*)&(*dvs)[j];
                        dv[i] = val.decVal.Lo32;
                        break;
                    }
                    default:
                    case VT_I4: {
                        DV<int> &dv = *(DV<int>*)&(*dvs)[j];
                        dv[i] = val.iVal;
                        }
                        // TODO: dvs->set_int(i, j, val.iVal);
                        break;
                    }
                }
                rs->MoveNext();
            }
            rs->Close();
            return dvs;
        } catch (_com_error e) {
            MessageBox(NULL, e.Description(), e.ErrorMessage(), 0);
            return NULL;
        }
    }
};
