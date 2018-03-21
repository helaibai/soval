#ifndef RPMFUNCTIONCOMPAT
#define RPMFUNCTIONCOMPAT

#include <rpm/rpmtypes.h>
#include <StdTypedefs.h>

int headerGetEntry(Header header, int_32 tag_id, int_32 *type, void *pointer, int_32 *data_size);

#endif
