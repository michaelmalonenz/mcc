#ifndef LIBERAL_H_
#define LIBERAL_H_

#include <assert.h>

#include "fileBuffer.h"
#include "list.h"
#include "stack.h"
#include "stringBuffer.h"
#include "hash_table.h"

#define NULL_DATA ((uintptr_t) 0)

#if MCC_DEBUG
# define MCC_ASSERT(x) assert(x)
#else
# define MCC_ASSERT(x)
#endif

#ifndef UNUSED
#define UNUSED(x) x __attribute__ ((unused))
#endif

#endif
