#pragma once
#include <assert.h>

/* Custom assert macro for error handling. */
#define CUSTOM_ASSERT(condition, message)  assert((condition) && (message))

/* Always false assert for critical errors. */
#define CUSTOM_ASSERT_FALSE(message)       assert(false && (message))
