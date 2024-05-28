#ifndef __LIST_H__
# define __LIST_H__

# include <stddef.h>
# include <stdbool.h>
# include <stdlib.h>

#ifndef BUILTIN_EXPECT_AVAILABLE
#define likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect(!!(x), 0))
#else
#define likely(x) x
#define unlikely(x) x
#endif

typedef struct list_node_s{
  struct {
    void  *ptr;
    size_t size;
  } _data;
	struct list_node_s *_next;
} list_node_t;

typedef struct {
	void  (*_free)(void *);
  struct {
    size_t          nmemb;
    list_node_t *head;
  } _list;
} list_t;

list_t       *list_new (void (*_free)(void *));
void         list_kill (list_t *l);
list_node_t  *list_new_node (void *data, size_t size);
void         list_kill_node (list_t *list, list_node_t * node);
list_node_t  *list_access (const list_t *l, size_t pos);
void         list_push(list_t *l, list_node_t *node);
void         list_pushf(list_t *l, list_node_t *node);
void         list_pop(list_t *l);
void         list_popf(list_t *l);
	
# endif /* __LIST_H__ */
