
#pragma once

#include "utils\larray.h"

class LDB
{
public:
    virtual DVS* Fetch(TCHAR *sql) = 0;
};
