#include "stdint.h"

#define PREFIX i
#define KTYPE int32_t
#define VTYPE int32_t
#include "hashtable.c"
#undef PREFIX
#undef KTYPE
#undef VTYPE
