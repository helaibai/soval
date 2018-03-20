#ifndef RPMFUNCTIONCOMPAT
#define RPMFUNCTIONCOMPAT

#include <rpm/rpmtypes.h>

int headerGetEntry(Header header, int_32 tag_id, int_32 *type, char *pointer, int_32 data_size);

#endif
