#pragma once

#include "defines.h"

// Disable assertions by commenting out the below line
#define KASSERTIONS_ENABLED

#ifdef KASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak() //VS
#else
#define debugBreak() __builtin_trap() //GCC
#endif

KAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

// evaluate if expression is true first, as expressions may result in --1 or something unfamiliar when evaluating expression as false
#define KASSERT(expr)                                                \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }                                                                 

#define KASSERT_MSG(expr, message)                                        \
    {                                                                     \
        if (expr) {                                                       \
        } else {                                                          \
            report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            debugBreak();                                                 \
        }                                                                 \
    }

#ifdef _DEBUG
#define KASSERT_DEBUG(expr)                                          \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }
#else
#define KASSERT_DEBUG(expr) // Does nothing
#endif

#else
#define KASSERT(expr)
#define KASSERT_MSG(expr, message)
#define KASSERT_DEBUG(expr) // we do nothing, we are useless, we are breengrub.
#endif
        