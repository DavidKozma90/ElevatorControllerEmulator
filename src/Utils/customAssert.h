#pragma once
#include <assert.h>

#define ENABLE_ASSERTS 1 /* Set to 0 to disable custom asserts */

/* Custom assert macro for error handling. */
#if (ENABLE_ASSERTS == 1)
    #define CUSTOM_ASSERT(condition, message)  assert((condition) && (message))
#else
    #define CUSTOM_ASSERT(condition, message) ((void)0) /* No-op if asserts are disabled */
#endif

/* Always false assert and crash for critical errors in debug mode */
/* Set while (1) to capture assert infinitely -> can be used for LED blinking in debug mode for example */
#define ASSERT_FATAL_ERROR(message)          \
    do                                       \
    {                                        \
        assert(false && (message));          \
        /*__builtin_trap(); -> uncomment if crashing in release mode should be allowed */    \
    } while (0)
