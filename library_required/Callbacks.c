#include "Callbacks.h"

int CallbackChain_Invoke(CallbackChain* pcb)
{
	while (pcb) {
		int res = Callback_Invoke(&pcb->hook);
		if (res < 0) {
			return res;
		}
		pcb = pcb->pNext;
	}
	return 0;
}

void CallbackChain_Insert(CallbackChain* target, CallbackChain* item)
{
	CallbackChain* next = target->pNext;
	target->pNext = item;
	CallbackChain** current = &item->pNext;
	while (*current) {
		current = &(*current)->pNext;
	}
	*current = next;
}

// adds all
void CallbackChain_Append(CallbackChain** head, CallbackChain* item)
{
	int same_found = 0;
	while (*head) {
		if (*head == item) {
			same_found = 1;
			break;
		}
		head = &(*head)->pNext;
	}
	if (!same_found) {
		*head = item;
	}
}
// adds all
void CallbackChain_Prepend(CallbackChain** head, CallbackChain* item)
{
	CallbackChain** current = &item;
	int same_found = 0;
	while (*current) {
		if (*current == *head) {
			same_found = 1;
			break;
		}
		current = &(*current)->pNext;
	}
	if (!same_found) {
		*current = *head;
		*head = item;
	}
}

// removes one item
void CallbackChain_Remove(CallbackChain** head, CallbackChain* item)
{
	while (*head) {
		if (*head == item) {
			*head = item->pNext;
			item->pNext = 0;
			break;
		}
		head = &(*head)->pNext;
	}
}

// adds all
void DataChain_Append(DataHook_Duplicates** head, DataHook_Duplicates* item)
{
	int same_found = 0;
	while (*head) {
		if (*head == item) {
			same_found = 1;
			break;
		}
		head = &(*head)->pNext;
	}
	if (!same_found) {
		*head = item;
	}
}
// adds all
void DataChain_Prepend(DataHook_Duplicates** head, DataHook_Duplicates* item)
{
	DataHook_Duplicates** current = &item;
	int same_found = 0;
	while (*current) {
		if (*current == *head) {
			same_found = 1;
			break;
		}
		current = &(*current)->pNext;
	}
	if (!same_found) {
		*current = *head;
		*head = item;
	}
}

// removes one item
void DataChain_Remove(DataHook_Duplicates** head, DataHook_Duplicates* item)
{
	while (*head) {
		if (*head == item) {
			*head = item->pNext;
			item->pNext = 0;
			break;
		}
		head = &(*head)->pNext;
	}
}