#include "queue.h"



void init_queue(Queue* queue, int capacity) {
    queue->data = (void**)malloc(capacity * sizeof(void*));
    queue->front = queue->rear = queue->size = 0;
    queue->capacity = capacity;
}

void enqueue(Queue* queue, void* data) {
    if (queue->size >= queue->capacity) {
        printf("Queue is full!\n");
        return;
    }
    queue->data[queue->rear] = data;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
}

void dequeue(Queue* queue) {
    if (queue->size == 0) {
        printf("Queue is empty!\n");
        return;
    }
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
}

void* front(Queue* queue) {
    if (queue->size == 0) {
        printf("Queue is empty!\n");
        return NULL;
    }
    return queue->data[queue->front];
}

bool is_empty(Queue* queue) {
    return queue->size == 0;
}

bool is_full(Queue* queue) {
    return queue->size == queue->capacity;
}

void free_queue(Queue* queue) {
    free(queue->data);
}
