#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

struct node
{
  void *data;          //Pointer to current data field
  struct node *next;
  struct node *prev;   
};

struct queue {
  int length;
  struct node *front;
  struct node *back;
};

/*
 * queue_create - Allocate an empty queue
 *
 * Create a new object of type 'struct queue' and return its address.
 *
 * Return: Pointer to new empty queue. NULL in case of failure when allocating
 * the new queue.
 */
queue_t queue_create(void)
{
    queue_t Queue = malloc(sizeof(struct queue));
    if (Queue != NULL) 
    {
        Queue->length = 0;
        Queue->front = NULL;
        Queue->back = NULL;
    }
    return Queue;
}


/*
 * queue_destroy - Deallocate a queue
 * @queue: Queue to deallocate
 *
 * Deallocate the memory associated to the queue object pointed by @queue.
 *
 * Return: -1 if @queue is NULL of if @queue is not empty. 0 if @queue was
 * successfully destroyed.
 */
int queue_destroy(queue_t queue)
{
    if (queue == NULL)
    {
        return -1;
    }

    struct node* current_node = queue->front;
    while (current_node != NULL)
    {
        struct node* node_to_delete = current_node;
        current_node = current_node->next;
        free(node_to_delete);
    }

    free(queue);
    return 0;
}


/*
 * queue_enqueue - Enqueue data item
 * @queue: Queue in which to enqueue item
 * @data: Address of data item to enqueue
 *
 * Enqueue the address contained in @data in the queue @queue.
 *
 * Return: -1 if @queue or @data are NULL, or in case of memory allocation error
 * when enqueing. 0 if @data was successfully enqueued in @queue.
 */
int queue_enqueue(queue_t queue, void *data)
{
    if ((queue == NULL) || (data == NULL))
    {
        return -1;
    }

    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == 0)
    {
        return -1;
    }

    new_node->data = data;
    queue->length++;

    // Empty queue condition
    if (queue->front == NULL)
    {
        queue->front = new_node;
        queue->back = new_node;

        new_node->prev = NULL;
        new_node->next = NULL;
        return 0;
    }

    // Non-empty queue condition
    new_node->prev = queue->back;
    queue->back->next = new_node;
    queue->back = new_node;
    new_node->next = NULL;
    return 0;
}


/*
 * queue_dequeue - Dequeue data item
 * @queue: Queue in which to dequeue item
 * @data: Address of data pointer where item is received
 *
 * Remove the oldest item of queue @queue and assign this item (the value of a
 * pointer) to @data.
 *
 * Return: -1 if @queue or @data are NULL, or if the queue is empty. 0 if @data
 * was set with the oldest item available in @queue.
 */
int queue_dequeue(queue_t queue, void **data)
{
    if ((queue == NULL) || (data == NULL)) 
    {
        return -1;
    }
   
    struct node *current_front = queue->front;
    if (current_front == NULL)
    {
        return -1;
    }

    *data = current_front->data;

    struct node *new_front = current_front->next;
    queue->front = new_front;
    if (new_front != NULL)
    {
        new_front->prev = NULL;
    }

    free(current_front);
    queue->length--;

    return 0;
}


/*
 * queue_delete - Delete data item
 * @queue: Queue in which to delete item
 * @data: Data to delete
 *
 * Find in queue @queue, the first (ie oldest) item equal to @data and delete
 * this item.
 *
 * Return: -1 if @queue or @data are NULL, of if @data was not found in the
 * queue. 0 if @data was found and deleted from @queue.
 */
int queue_delete(queue_t queue, void *data)
{
    if ((queue == NULL) || (data == NULL)) 
    {
        return -1;
    }

    struct node* current_node = queue->front;
    while (current_node != NULL)
    {
        if (current_node->data == data)
        {
            struct node* prev_node = current_node->prev;
            struct node* next_node = current_node->next;

            if ((prev_node == NULL) && (next_node == NULL))
            {
                queue->front = queue->back = NULL;
                queue->length = 0;
                free(current_node); 
                return 0;
            }

            if ((prev_node == NULL) && (next_node != NULL))
            {
                queue->front = next_node;
                queue->front->prev = NULL;
                queue->length--;
                free(current_node); 
                return 0;
            }

            if ((prev_node != NULL) && (next_node == NULL))
            {
                queue->back = prev_node;
                queue->back->next = NULL;
                queue->length--;
                free(current_node); 
                return 0;
            }

            prev_node->next = next_node;
            next_node->prev = prev_node;
            free(current_node);
            queue->length--;
            return 0;
        }
        current_node = current_node->next;
    }
    return -1;
}


int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    if ((queue == NULL) || (func == NULL)) 
    {
        return -1;
    }
 
    // int func_return_value = 0;
    struct node* current_node = queue->front;
    while (current_node != NULL)
    {
        int func_return_value = func(current_node->data, arg);
        if ((func_return_value == 1) && (data != NULL))
        {
            *data = current_node->data;
            return 0;
        }
        current_node = current_node->next;
    }
    return 0;
}


/*
 * queue_length - Queue length
 * @queue: Queue to get the length of
 *
 * Return the length of queue @queue.
 *
 * Return: -1 if @queue is NULL. Length of @queue otherwise.
 */
int queue_length(queue_t queue)
{
    if (queue == NULL) {
       return -1;
    } 

    return queue->length;
}


