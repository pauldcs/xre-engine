#include "list.h"
#include "xre_compiler.h"
#include <stdbool.h>
#include <string.h>

bool list_init(list_t **self)
{
	*self = malloc(sizeof(list_t));
	if (unlikely(!*self))
		return (false);

	(void)memset(*self, 0, sizeof(list_t));
	return (true);
}

list_t *list_new(void (*_free)(void *))
{
	list_t *list = NULL;

	if (unlikely(!list_init(&list)))
		return (NULL);

	list->_free = _free;
	return (list);
}

list_node_t *list_new_node(void *data, size_t size)
{
	__return_val_if_fail__(data, NULL);

	list_node_t *n = malloc(sizeof(list_node_t));
	if (unlikely(!n))
		return (NULL);

	(void)memset(n, 0, sizeof(list_node_t));
	n->_data.ptr  = data;
	n->_data.size = size;

	return (n);
}

void list_kill_node(list_t *self, list_node_t *node)
{
	__return_if_fail__(self);
	__return_if_fail__(node);

	if (self->_free) {
		self->_free(node->_data.ptr);
		node->_data.ptr = NULL;
	}

	(void)memset(node, 0xff, sizeof(list_node_t));

	free(node);
	node = NULL;
}

void list_kill(list_t *self)
{
	__return_if_fail__(self);

	if (unlikely(!self->_list.nmemb))
		return;

	list_node_t *temp1 = self->_list.head;
	list_node_t *temp2 = NULL;
	size_t	     size  = self->_list.nmemb;

	while (size--) {
		temp2 = temp1;
		temp1 = temp1->_next;
		list_kill_node(self, temp2);
	}
	free(self);
}

list_node_t *list_access(const list_t *self, size_t pos)
{
	__return_val_if_fail__(self, NULL);
	__return_val_if_fail__(pos < self->_list.nmemb, NULL);

	list_node_t *temp = self->_list.head;

	while (pos--)
		temp = temp->_next;

	return (temp);
}

void list_push(list_t *self, list_node_t *node)
{
	__return_if_fail__(self);
	__return_if_fail__(node);

	list_node_t *temp = self->_list.head;

	if (likely(temp)) {
		while (temp->_next)
			temp = temp->_next;
		temp->_next = node;

	} else {
		self->_list.head = node;
	}

	++(self->_list.nmemb);
}

void list_pushf(list_t *self, list_node_t *node)
{
	__return_if_fail__(self);
	__return_if_fail__(node);

	list_node_t *temp = self->_list.head;
	if (likely(temp))
		node->_next = temp;

	self->_list.head = node;
	++(self->_list.nmemb);
}

void list_pop(list_t *self)
{
	__return_if_fail__(self);
	__return_if_fail__(self->_list.nmemb > 0);

	list_node_t *temp = self->_list.head;
	list_node_t *ptr  = NULL;

	while (temp->_next) {
		ptr  = temp;
		temp = temp->_next;
	}

	list_kill_node(self, temp);
	if (ptr) {
		ptr->_next = NULL;
	} else {
		self->_list.head = NULL;
	}

	--(self->_list.nmemb);
}

void list_popf(list_t *self)
{
	__return_if_fail__(self);
	__return_if_fail__(self->_list.nmemb > 0);

	list_node_t *temp = self->_list.head;

	if (self->_list.head->_next)
		self->_list.head = self->_list.head->_next;

	list_kill_node(self, temp);
	--(self->_list.nmemb);
}
