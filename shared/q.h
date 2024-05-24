/*
 * Example usage:
 *
 * #define Q_IMPL
 * #include "q.h"
 * 
 * typedef struct { int x; int y; } Point;
 * 
 * int main() {
 *     Q q;
 *     q_init(&q);
 *     Point a = { .x = 111, .y = 222 };
 *     Point b = { .x = 333, .y = 444 };
 *     q_enqueue(&q, &a);
 *     q_enqueue(&q, &b);
 * 
 *     while (!q_empty(&q)) {
 *         Point *point = q_dequeue(&q);
 *         printf("(%d, %d)\n", point->x, point->y);
 *     }
 * }
 */

#ifndef Q_H
#define Q_H

#include <stdint.h>
#include <stddef.h>

typedef struct QNode {
    void *val;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *head;
    QNode *tail;
} Q;

void q_init(Q *q);
void q_enqueue(Q *q, void *val);
void *q_dequeue(Q *q);
#define q_empty(q_ptr) !(q_ptr)->head

#endif // end header

#ifdef Q_IMPL

#include <malloc.h>
#include <memory.h>

void q_init(Q *q) {
    q->head = NULL;
    q->tail = NULL;
}

void q_enqueue(Q *q, void *val) {
    QNode *new_node = malloc(sizeof(*new_node));
    new_node->val = malloc(sizeof(void *));
    memcpy(&new_node->val, &val, sizeof(void *));
    new_node->next = NULL;
    if (!q->head) {
        q->head = new_node;
        q->tail = q->head;
        return;
    }
    q->tail->next = new_node;
    q->tail = q->tail->next;
}

void *q_dequeue(Q *q) {
    if (!q->head)
        return NULL;
    QNode *tmp = q->head;
    void *res = tmp->val;
    q->head = q->head->next;
    free(tmp);
    if (!q->head)
        q->tail = NULL;
    return res;
}

#endif // end implementation
