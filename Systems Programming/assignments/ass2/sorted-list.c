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
    new->iterators = 0;
    new-> deleted = 0;

    // empty list
    if(!list->list)
    {
        list->list = new;

        return 1;
    }

    Node *ptr = list->list;
    Node *prev = NULL;
    while((ptr) && list->compare(newObj, ptr->data) == -1)
    {
        prev = ptr;
        ptr = ptr->next;
    }

    if(!ptr) // insert at end of list
    {
        prev->next = new;
    }
    else if(!prev) // insert at front of list
    {
        new->ptr;
        list->list = new;
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

int SLRemove(SortedListPtr list, void *newObj)
{
    Node* prev;
    Node* ptr;
    while((ptr) && (list->compare(ptr->data, newObj) != 0))
    {
        prev = ptr;
        ptr = ptr->next;
    }

    if(!ptr) // not found
    {
        return 0;
    }
    else if(!prev) // target is at front of list
    {
        list->list = ptr->next;

        if(ptr->iterators == 0)
        {
            list->destroy(ptr->data);
            free(ptr);
        }
        else
        {
            ptr->deleted = 1;
        }
    }
    else if(!ptr->next) // target is at end of list
    {
        prev->next = NULL;

        if(ptr->iterators == 0)
        {
            list->destroy(ptr->data);
            free(ptr);
        }
        else
        {
            ptr->deleted = 1;
        }
    }
    else
    {
        prev->next = ptr->next;

        if(ptr->iterators == 0)
        {
            list->destroy(ptr->data);
            free(ptr);
        }
        else
        {
            ptr->deleted = 1;
        }
    }

    return 1;
}


/************************* SortedListIterator ***********************/
SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
    SortedListIteratorPtr slip = (SortedListIteratorPtr*) malloc(sizeof(SortedListIterator));
    slip->currentItem = NULL;
    slip->finished = 0;
    slip->slp = list;

    if(!list) // sorted list is empty so we cant make an iterator
    {
        free(slip);

        return NULL:
    }

    return slip;
}

void SLDestroyIterator(SortedListIteratorPtr iter)
{
    free(iter);

    return;
}

void* SLNextItem(SortedListIteratorPtr iter)
{
    if(iter->finished) // traversed through entire list
    {
        return NULL;
    }

    if(!iter->currentItem) // first item to be retrieved
    {
            if(!iter->slp->list) //empty list
            {
                iter->finished = 1;
                return NULL;
            }
            else
            {
                iter->currentItem = iter->slp->list;
                iter->currentItem->iterators++;
                return iter->currentItem;
            }
    }

    // find next item to be returned
    Node *temp = iter->currentItem;
    while((iter->currentItem) && (iter->currentItem->deleted)) // also skip over nodes which should be deleted
    {
        iter->currentItem = iter->currentItem->next;
    }

    if(--(temp->iterators) == 0 && (temp->deleted)) // this node should be deleted and all iterators have left it
    {
        iter->slp->destroy(temp->data);
        free(temp);
    }

    if(!iter->currentItem) // empty list or reached end of list
    {
        iter->finished = 1;
        return NULL;
    }

    (iter->currentItem->iterators)++;

    return iter->currentItem;
}

void* SLGetItem(SortedListIteratorPtr iter)
{
    return iter->currentItem;
}