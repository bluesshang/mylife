
#pragma once

#define LWIN_ASSERT(expr) if (!(expr)) { __asm {int 3} }
