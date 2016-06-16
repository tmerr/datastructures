#ifndef IHASHTABLE_H

#include "stdint.h"

#define PREFIX i
#define KTYPE int32_t
#define VTYPE int32_t
#include "hashtable.h"
#undef PREFIX
#undef KTYPE
#undef VTYPE

#endif
