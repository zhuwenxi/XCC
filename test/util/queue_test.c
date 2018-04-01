#include "queue_test.h"

bool queue_enqueue_dequeue_test()
{
	queue_type *queue = queue_create();

	int data_list[] = {1, 3, 5, 7, 9};

	enqueue(queue, &data_list[0]);
	enqueue(queue, &data_list[1]);
	enqueue(queue, &data_list[2]);
	enqueue(queue, &data_list[3]);
	enqueue(queue, &data_list[4]);

	dequeue(queue);
	dequeue(queue);
	dequeue(queue);
	//dequeue(queue);
	//dequeue(queue);

	//LOG(TRUE, "queue: %s", get_linked_list_debug_str(queue, int_to_str, NULL));
	
	queue_destroy(queue, NULL);
	return TRUE;
}