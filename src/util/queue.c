#include "util/queue.h"

bool
enqueue(queue_type *queue, void *data)
{
	assert(queue);

	linked_list_insert_back(queue, data);
}

void *
dequeue(queue_type *queue)
{
	queue_node_type *tail_node = queue->tail;
	
	if (tail_node == NULL) return NULL;

	void *data = tail_node->data;

	linked_list_delete(queue, tail_node, NULL);
	return data;
}

bool
queue_empty(queue_type *queue)
{
	return queue->head != NULL;
}
