#include <zlib.h>

voidpf bolt_zcalloc(voidpf opaque, unsigned items, unsigned size);
void bolt_zcfree(voidpf opaque, voidpf ptr);
