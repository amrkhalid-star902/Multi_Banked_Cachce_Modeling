/*
 * queue.h
 *
 *  Created on: Jul 13, 2023
 *      Author: AKhal
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Define the node structure for the linked list
typedef struct {
    void** data;
    int front, rear, size, capacity;
} Queue;




void init_queue(Queue* queue, int capacity);
void enqueue(Queue* queue, void* data);
void dequeue(Queue* queue);   //give the front item after removing it from the queue
void* front(Queue* queue);     //give the front item without removing it from the queue
bool is_empty(Queue* queue);
bool is_full(Queue* queue);
void free_queue(Queue* queue);




#endif /* QUEUE_H_ */
