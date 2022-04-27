#ifndef __MACRO_H__
#define __MACRO_H__

#include <string.h>
#include "log.h"
#include "util.h"

#define ZCS_ASSERT(x) \
    if(!(x)) { \
        ZCS_ERROR(ZCS_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << zcs::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define ZCS_ASSERT2(x, w) \
    if(!(x)) { \
        ZCS_ERROR(ZCS_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << zcs::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#endif //__MACRO_H__