#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_queue = malloc(sizeof(struct list_head));

    /* check malloc */
    if (!new_queue) {
        return NULL;
    }

    INIT_LIST_HEAD(new_queue);
    return new_queue;
}


/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    struct list_head *node, *safe;

    list_for_each_safe (node, safe, head) {
        free(list_entry(node, element_t, list)->value);
        free(list_entry(node, element_t, list));
    }
    free(head);
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *new_content = malloc(sizeof(element_t));

    if (!new_content) {
        return false;
    }

    INIT_LIST_HEAD(&new_content->list);
    new_content->value = strdup(s);

    if (!new_content->value) {
        free(new_content);
        return false;
    }

    list_add(&new_content->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *new_content = malloc(sizeof(element_t));

    if (!new_content) {
        return false;
    }

    INIT_LIST_HEAD(&new_content->list);
    new_content->value = strdup(s);

    if (!new_content->value) {
        free(new_content);
        return false;
    }

    list_add_tail(&new_content->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *first = list_first_entry(head, element_t, list);
    list_del(&first->list);

    if (!sp) {
        return first;
    }

    size_t len = strlen(first->value);
    size_t copy_size = (len < bufsize - 1) ? len : (bufsize - 1);

    strncpy(sp, first->value, copy_size);
    sp[copy_size] = '\0';

    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *tail = list_last_entry(head, element_t, list);
    list_del(&tail->list);

    if (!sp) {
        return tail;
    }

    size_t len = strlen(tail->value);
    size_t copy_size = (len < bufsize - 1) ? len : (bufsize - 1);

    strncpy(sp, tail->value, copy_size);
    sp[copy_size] = '\0';

    return tail;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Shuffle the node in the queue*/
void q_shuffle(struct list_head *head)
{
    int len = q_size(head);

    if (len <= 1) {
        return;
    }

    for (int remain = len; remain > 0; remain--) {
        int rand_num = rand() % remain;

        struct list_head *node;

        list_for_each (node, head) {
            if (rand_num == 0) {
                break;
            }
            rand_num--;
        }

        list_del(node);
        list_add_tail(node, head);
    }
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next;
    struct list_head *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    element_t *mid = list_entry(slow, element_t, list);
    list_del(slow);
    free(mid->value);
    free(mid);
    return true;
}


/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *node, *safe;
    bool dup = false;

    list_for_each_safe (node, safe, head) {
        element_t *first = list_entry(node, element_t, list);
        const element_t *second = list_entry(safe, element_t, list);
        if (safe != head && !strcmp(first->value, second->value)) {
            list_del(node);
            free(first->value);
            free(first);
            dup = true;
        } else if (dup) {
            element_t *tmp = list_entry(node, element_t, list);
            list_del(node);
            free(tmp->value);
            free(tmp);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *first, *second;
    list_for_each_safe (first, second, head) {
        if (second == head) {
            break;
        }
        first->next = second->next;
        second->next->prev = first;

        second->prev = first->prev;
        first->prev->next = second;

        second->next = first;
        first->prev = second;

        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    node->next = node->prev;
    node->prev = safe;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head) {
        return;
    }

    int split_time = q_size(head) / k;
    struct list_head *tail;

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    for (int i = 0; i < split_time; i++) {
        int j = 0;
        list_for_each (tail, head) {
            if (j >= k) {
                break;
            }
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &new_head);
    }
    list_splice_init(&new_head, head);
}

/* Start of sort */
typedef int (*list_cmp_func_t)(const struct list_head *,
                               const struct list_head *);

static int cmp(const struct list_head *a, const struct list_head *b)
{
    const element_t *element_a = list_entry(a, element_t, list);
    const element_t *element_b = list_entry(b, element_t, list);
    return strcmp(element_a->value, element_b->value);
}

typedef int (*compare_func_t)(struct list_head *, struct list_head *);

static struct list_head *merge(list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    unsigned char count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        if (__glibc_unlikely(!++count))
            cmp(b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void list_sort(struct list_head *head, list_cmp_func_t cmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (__glibc_likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(cmp, b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(cmp, pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(cmp, head, pending, list);
}

void q_sort(struct list_head *head, bool descend)
{
    list_sort(head, cmp);

    if (descend) {
        q_reverse(head);
    }
}

/* End of sort */


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    const char *min = list_entry(head->prev, element_t, list)->value;

    struct list_head *current, *safe;
    for (current = (head)->prev, safe = current->prev; current != head;
         current = safe, safe = current->prev) {
        element_t *current_entry = list_entry(current, element_t, list);
        if (strcmp(current_entry->value, min) > 0) {
            list_del(current);
            free(current_entry->value);
            free(current_entry);
        } else if (strcmp(current_entry->value, min) < 0) {
            min = current_entry->value;
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere
 * to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    const char *max = list_entry(head->prev, element_t, list)->value;

    struct list_head *current, *safe;
    for (current = (head)->prev, safe = current->prev; current != head;
         current = safe, safe = current->prev) {
        element_t *current_entry = list_entry(current, element_t, list);
        if (strcmp(current_entry->value, max) < 0) {
            list_del(current);
            free(current_entry->value);
            free(current_entry);
        } else if (strcmp(current_entry->value, max) > 0) {
            max = current_entry->value;
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */

int merge_queue(struct list_head *first, struct list_head *second)
{
    if (!first || !second) {
        return 0;
    }

    LIST_HEAD(tmp);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *element_1 = list_first_entry(first, element_t, list);
        element_t *element_2 = list_first_entry(second, element_t, list);
        element_t *element_min = strcmp(element_1->value, element_2->value) < 0
                                     ? element_1
                                     : element_2;
        list_move_tail(&element_min->list, &tmp);
    }
    list_splice_tail_init(first, &tmp);
    list_splice_tail_init(second, &tmp);
    list_splice(&tmp, first);
    return q_size(first);
}

int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    } else if (list_is_singular(head)) {
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    }

    int size = q_size(head);
    int count = (size % 2) ? size / 2 + 1 : size / 2;
    int queue_size = 0;

    for (int i = 0; i < count; i++) {
        queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second =
            list_entry(first->chain.next, queue_contex_t, chain);

        while (!list_empty(first->q) && !list_empty(second->q)) {
            queue_size = merge_queue(first->q, second->q);
            list_move_tail(&second->chain, head);
            first = list_entry(first->chain.next, queue_contex_t, chain);
            second = list_entry(first->chain.next, queue_contex_t, chain);
        }
    }
    if (descend) {
        q_reverse(head);
    }
    return queue_size;
}
