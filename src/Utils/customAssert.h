#pragma once
#include <assert.h>

/* Custom assert macro for error handling. */
#define CUSTOM_ASSERT(condition, message)  assert((condition) && (message))

/* Always false assert and crash for critical errors in all build modes. */
#define ASSERT_FATAL_ERROR(message)          \
    do                                       \
    {                                        \
        assert(false && (message));          \
        __builtin_trap();                    \
    } while (0)
