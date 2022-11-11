// circular_queue.h
#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

void insertFirst(struct node **processQueue, struct pcb *newPcb);

struct pcb* deleteFirst(struct node *processQueue);

int isEmpty(struct node *processQueue);

struct pcb* deleteByNode(struct node *processQueue, struct node *deleteNode);

struct node* createCircularQueue();

void printProcessQueue(struct node *processQueue);


#endif // circular_queue.h