#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "util/linked_list.h"

typedef linked_list_type queue_type;
typedef linked_list_node_type queue_node_type;

#define queue_create linked_list_create
#define queue_destroy linked_list_destroy

bool enqueue(queue_type *queue, void *data);

void *dequeue(queue_type *queue);
#endif