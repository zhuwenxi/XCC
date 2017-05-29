#ifndef __UTIL_H__
#define __UTIL_H__

#define TYPE_CAST(data, type) ((type)(data))

#define GET_DATA(node, type) (*((type *)(node->data)))

#endif
