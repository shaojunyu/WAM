#ifndef QUEUE_H_
#define QUEUE_H_

/***
 *  通用循环链表队列。基本操作：初始化、删除、添加、判断是否为空
 *
 */

typedef struct Queue
{
    struct Queue *prev;
    struct Queue *next;
} Queue;

#define QUEUE_INIT(q) \
    (q)->next = (q); \
    (q)->prev = (q)

#define QUEUE_REMOVE(q) \
    (q)->prev->next = (q)->next; \
    (q)->next->prev = (q)->prev

#define QUEUE_INSERT_HEAD(h, q) \
    (h)->next->prev = (q); \
    (q)->prev       = (h); \
    (q)->next       = (h)->next; \
    (h)->next       = (q)

#define QUEUE_EMPTY(h) \
    (h)->next == (h)

#endif
