#include "sorted-list.h"

/******************** Sorted Lists Methods ***********************/
SortedListPtr SLCreate(CompareFuncT compare, DestructFuncT destroy)
{
    SortedListPtr slp = (SortedListPtr) malloc(sizeof(SortedList));
    slp->compare = compare;
    slp->destroy = destroy;
    slp->list = NULL;

    return slp;
}

void SLDestroy(SortedListPtr list)
{
    // free list
    Node *ptr = list->list;
    while(ptr)
    {
        Node *tmp = ptr;
        ptr = ptr->next;
        list->destroy(tmp->data);
        free(tmp);
    }

    // free rest of SortedList struct
    free(list);

    return;
}

int SLInsert(SortedListPtr list, void *newObj)
{
    // Create Node with new data object
    Node *new = malloc(sizeof(Node));
    new->data = newObj;
    new->next = NULL;

    // empty list
    if(!list->list)
    {
        list->list = new;

        return 1;
    }

    Node *ptr = list->list;
    Node *prev = NULL;
    while((ptr) && list->compare(newObj, ptr->data) >= 0)
    {
        prev = ptr;
        ptr = ptr->next;
    }

    if(!ptr) // insert at end of list
    {
        prev->next = new;
    }
    else if(list->compare(newObj, ptr->data) == 0) // found duplicate
    {
        list->destroy(new->data);
        free(new);

        return 0;
    }
    else
    {
        new->next = ptr;
        prev->next = new;
    }

    return 1;
}
