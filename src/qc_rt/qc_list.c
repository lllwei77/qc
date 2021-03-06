/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "qc_prelude.h"
#include "qc_thread.h"
#include "qc_list.h"


/*////////////////////////////////////////////////////////////////////////////////
//                               Static List
////////////////////////////////////////////////////////////////////////////////*/



#define QC_STLIST_CELL_SIZE sizeof(_StaticCell)


void *get_staticlist_cell(QcStaticList *staticList, int cell)
{
    char *buff = (char*)staticList + staticList->buff_offset;
    return (void *)((char *)buff + QC_STLIST_CELL_SIZE*cell);
}


int qc_staticlist_init(QcStaticList *staticList, int limit, void* buff_ptr)
{
    int i;
    void *_buff_ptr;

    _StaticCell *curCell = NULL;

    if(limit <= 0)
    {
        return -1;
    }

	memset(staticList, 0, sizeof(QcStaticList));

    if(NULL == buff_ptr)
    {
        if(NULL == (_buff_ptr = malloc(limit*QC_STLIST_CELL_SIZE)))
            return -1;
    }
    else
    {
        _buff_ptr = buff_ptr;
    }

    staticList->buff_offset = (off_t)((char*)_buff_ptr - (char*)staticList);

    memset(_buff_ptr, 0, limit*QC_STLIST_CELL_SIZE);

    staticList->head = QC_INVALID_INT;
    staticList->tail = QC_INVALID_INT;
    staticList->limit = limit;
    staticList->num  = 0;

    for (i=0; i<limit; i++)
    {
        curCell = get_staticlist_cell(staticList, i);
        curCell->previous  = i==0?QC_INVALID_INT:i-1;
        curCell->next = i + 1;
    }
    curCell->next = QC_INVALID_INT;

    staticList->head = 0;
    staticList->tail = limit-1;
    staticList->num  = limit;

    return 0;
}


void qc_staticlist_clear(QcStaticList *staticList)
{
    int i, num;

    num = staticList->num;

    for(i=0; i<num; i++)
    {
        qc_staticlist_get_head(staticList);
    }
}


void qc_staticlist_release(QcStaticList *staticList)
{
    char *buff;
    buff = (char*)staticList + staticList->buff_offset;
    free(buff);
    //free(staticList);  no!
}


int qc_staticlist_count(QcStaticList *staticList)
{
    if(NULL == staticList)
        return -1;

    return staticList->num;
}


int qc_staticlist_get_head(QcStaticList *staticList)
{
    int head;
    _StaticCell *headCell, *secondCell;

    if(NULL == staticList)
        return -1;

    head = staticList->head;
    if(head != QC_INVALID_INT)
    {
        headCell = get_staticlist_cell(staticList, staticList->head);
        staticList->head = headCell->next;
        if(staticList->head == QC_INVALID_INT)
        {
            staticList->tail = QC_INVALID_INT;
        }
        else
        {
            secondCell = get_staticlist_cell(staticList, staticList->head);
            secondCell->previous = QC_INVALID_INT;
        }
        staticList->num --;

        headCell->previous  = QC_INVALID_INT;
        headCell->next = QC_INVALID_INT;
    }

    return head;
}


int qc_staticlist_get_tail(QcStaticList *staticList)
{
    int tail;
    _StaticCell *tailCell, *secondCell;

    if(NULL == staticList)
        return -1;

    tail = staticList->tail;
    if(tail != QC_INVALID_INT)
    {
        tailCell = get_staticlist_cell(staticList, staticList->tail);
        staticList->tail = tailCell->previous;
        if(staticList->tail == QC_INVALID_INT)
        {
            staticList->head = QC_INVALID_INT;
        }
        else
        {
            secondCell = get_staticlist_cell(staticList, staticList->tail);
            secondCell->next = QC_INVALID_INT;
        }
        staticList->num --;

        tailCell->previous  = QC_INVALID_INT;
        tailCell->next = QC_INVALID_INT;
    }

    return tail;
}


int qc_staticlist_get_at(QcStaticList *staticList, int cell)
{
    int pre, next;
    _StaticCell *curCell, *preCell, *nextCell;

    if(NULL == staticList || cell<0 || cell>=staticList->limit)
        return -1;

    if(staticList->head == QC_INVALID_INT)
        return QC_INVALID_INT;

    curCell = get_staticlist_cell(staticList, cell);

    pre  = curCell->previous;
    next = curCell->next;
    
    if(pre != QC_INVALID_INT)
    {
        preCell = get_staticlist_cell(staticList, pre);
        preCell->next = next;
    }
    else
    {
        staticList->head = next;
    }

    if(next != QC_INVALID_INT)
    {
        nextCell = get_staticlist_cell(staticList, next);
        nextCell->previous = pre;
    }
    else
    {
        staticList->tail = pre;
    }

    staticList->num --;

    curCell->previous  = QC_INVALID_INT;
    curCell->next = QC_INVALID_INT;

    return cell;
}


int qc_staticlist_add_head(QcStaticList *staticList, int cell)
{
    _StaticCell *curCell, *headCell;

    if(NULL == staticList || cell<0 || cell>=staticList->limit)
        return -1;

    curCell = get_staticlist_cell(staticList, cell);
    if(staticList->head == QC_INVALID_INT)
    {
        curCell->previous  = QC_INVALID_INT;
        curCell->next = QC_INVALID_INT;
        staticList->head = cell;
        staticList->tail = cell;
        staticList->num = 1;
    }
    else
    {
        headCell = get_staticlist_cell(staticList, staticList->head);
        
        curCell->previous  = QC_INVALID_INT;
        curCell->next = staticList->head;
        
        headCell->previous = cell;
        staticList->head = cell;
        staticList->num ++;
    }

    return cell;
}


int qc_staticlist_add_tail(QcStaticList *staticList, int cell)
{
    _StaticCell *curCell, *tailCell;

    if(NULL == staticList || cell<0 || cell>=staticList->limit)
        return -1;

    curCell = get_staticlist_cell(staticList, cell);
    if (staticList->head == QC_INVALID_INT)
    {
        curCell->previous  = QC_INVALID_INT;
        curCell->next = QC_INVALID_INT;
        staticList->head = cell;
        staticList->tail = cell;
        staticList->num = 1;
    }
    else
    {
        tailCell = get_staticlist_cell(staticList, staticList->tail);

        curCell->previous  = staticList->tail;
        curCell->next = QC_INVALID_INT;

        tailCell->next = cell;
        staticList->tail = cell;
        staticList->num ++;
    }

    return cell;
}


int qc_staticlist_add_at(QcStaticList *staticList, int previous_cell, int cell)
{
    int next;
    _StaticCell *curCell, *preCell, *nextCell;

    if(NULL == staticList)
        return -1;

    if(previous_cell<0 || previous_cell>=staticList->limit)
        return -1;

    if(cell<0 || cell>=staticList->limit)
        return -1;

    if(staticList->head == QC_INVALID_INT)
        return QC_INVALID_INT;

    curCell = get_staticlist_cell(staticList, cell);
    preCell=get_staticlist_cell(staticList, previous_cell);
    next=preCell->next;

    if(next!= QC_INVALID_INT)
    {
       nextCell=get_staticlist_cell(staticList, next);
       preCell->next=cell;
       nextCell->previous=cell;
       curCell->previous=previous_cell;
       curCell->next=next;
    }
    else
    {
       preCell->next=cell;
       curCell->previous=previous_cell;
       curCell->next=QC_INVALID_INT;
       staticList->tail=cell;
    }
    staticList->num ++;

    return cell;
}


int qc_staticlist_peek_head(QcStaticList *staticList)
{
    if(staticList == NULL)
        return -1;
    
    return staticList->head;
}


int qc_staticlist_peek_tail(QcStaticList *staticList)
{
    if(staticList == NULL)
        return -1;

    return staticList->tail;
}


int qc_staticlist_peek_precell(QcStaticList *staticList, int cell)
{
    _StaticCell *curCell;

    if(NULL == staticList || cell<0 || cell>=staticList->limit)
        return -1;

    curCell = get_staticlist_cell(staticList, cell);

    return curCell->previous;
}


int qc_staticlist_peek_nextcell(QcStaticList *staticList, int cell)
{
    _StaticCell *curCell;

    if(NULL == staticList || cell<0 || cell>=staticList->limit)
        return -1;

    curCell = get_staticlist_cell(staticList, cell);

    return curCell->next;
}


/*////////////////////////////////////////////////////////////////////////////////
//                               List
////////////////////////////////////////////////////////////////////////////////*/


struct __QcListEntry{
    QcListEntry *next;
    QcListEntry *prev;
    void *data;
};


struct __QcList{
    QcListEntry *head;
    QcListEntry *tail;
    QcListEntry *cur;
    unsigned int count;
    QcRWLock *rwlock;
};


QcList *qc_list_create(int with_rwlock)
{       
    QcList *list;

    qc_malloc(list, sizeof(struct __QcList));
    if(NULL == list)
    {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->cur  = NULL;
    list->count = 0;
    list->rwlock = NULL;

    if(with_rwlock)
    {
        list->rwlock = qc_thread_rwlock_create();
        if(NULL == list->rwlock)
        {
            qc_free(list);
            return NULL;
        }
    }

    return list;
}


void qc_list_destroy(QcList *list)
{
    QcListEntry *entry;
    QcListEntry *del_entry;

    qc_assert(list);

    entry = list->head;
    while(NULL != entry)
    {
        del_entry = entry;
        entry = entry->next;

        qc_free(del_entry);
		del_entry = NULL;
    }

    if(list->rwlock)
        qc_thread_rwlock_destroy(list->rwlock);

    qc_free(list);
	list = NULL;
}


unsigned int qc_list_count(QcList *list)
{
    int count;

    qc_assert(list);

    count = list->count;

    return count;
}


int	qc_list_inserthead(QcList *list, void *data)
{
    QcListEntry  *entry;

    qc_assert(list);
    qc_assert(data);

    qc_malloc(entry, sizeof(struct __QcListEntry));
    if(NULL == entry)
    {
        return -1;
    }

    entry->data = data;
    entry->prev = NULL;
    entry->next = list->head;

    if(NULL != list->head)
    {
        list->head->prev = entry;
    }
    if(NULL == list->tail)
    {
        list->tail = entry;
    }

    list->head = entry;
    list->count++;

    return 0;
}


int	qc_list_inserttail(QcList *list, void *data)
{
    QcListEntry *entry;

    qc_assert(list);
    qc_assert(data);

    qc_malloc(entry, sizeof(struct __QcListEntry));
    if(!entry)
    {
        return -1;
    }

    entry->data = data;
    entry->next = NULL;

    entry->prev = list->tail;

    if(!list->tail)
    {
        list->head = entry;
    }
    else
    {
        list->tail->next = entry;
    }

    list->tail = entry;
    list->count++;
    
    return 0;
}


int qc_list_inserttail2(QcList *list, void *data, QcListEntry **pp_entry)
{
    QcListEntry *entry;

    qc_assert(list);
    qc_assert(data);

    qc_malloc(entry, sizeof(struct __QcListEntry));
    if(!entry)
    {
        *pp_entry = NULL;
        return -1;
    }

    entry->data = data;
    entry->next = NULL;

    entry->prev = list->tail;

    if(!list->tail)
    {
        list->head = entry;
    }
    else
    {
        list->tail->next = entry;
    }

    list->tail = entry;
    list->count++;
    *pp_entry  = entry;
    
    return 0;
}


void* qc_list_pophead(QcList *list)
{
	QcListEntry	*pop_entry;
	void *pop_data;

    qc_assert(list);

    if(NULL == list->head)
    {
		return NULL;
    }

    pop_entry = list->head;

    if(list->cur == list->head)
    {
        list->cur = list->head->next;
    }

    list->head = list->head->next;
    if(NULL != list->head)
    {
        list->head->prev = NULL;
    }
	
    list->count--;

    if(0 == list->count){
		list->tail = NULL;
	}
    
    pop_data = pop_entry->data;
    qc_free(pop_entry);

    return pop_data;
}


void* qc_list_poptail(QcList *list)
{
    QcListEntry	*pop_entry;
    void *pop_data;

    qc_assert(list);

    if(NULL == list->head)
    {
        return NULL;
	}

    pop_entry = list->tail;

    if(list->cur == list->tail)
    {
        list->cur = NULL;
    }
    if(list->tail == list->head)
    {
	    list->head = NULL;
    }
    else 
    {
        list->tail->prev->next = NULL;
    }
    list->tail = list->tail->prev;

    list->count--;

    pop_data = pop_entry->data;
	qc_free(pop_entry);

    return pop_data;
}


void qc_list_enumbegin(QcList *list)
{
    list->cur = list->head;
    return;
}


QcListEntry* qc_list_enumentry(QcList *list)
{
    QcListEntry *cur;
    
    cur = list->cur;
    if(cur)
    {
        list->cur = cur->next;
        return cur;
    }

    return NULL;
}


void* qc_list_data(QcListEntry *entry)
{
    return entry->data;
}


int qc_list_removeentry(QcList *list, QcListEntry *entry)
{
    qc_assert(list);
    qc_assert(entry);

    if(NULL != entry->prev)
    {
        entry->prev->next = entry->next;
    }
    else
    {
        if(NULL != entry->next)
        {
            entry->next->prev = NULL;
        }
        list->head = entry->next;
    }
    if(entry->next)
    {
        entry->next->prev = entry->prev;
    }
    else
    {
        if(NULL != entry->prev)
        {
            entry->prev->next = NULL;
        }
        list->tail = entry->prev;
    }
    if(list->cur == entry)
    {
        list->cur = entry->next;
    }

    list->count --;
    qc_free(entry);
    return 0;
}


int qc_list_removefirst(QcList *list, void *data){
    QcListEntry *entry = NULL;
    qc_assert(list);

    qc_list_enumbegin(list);

    entry = qc_list_enumentry(list);
    while(entry){
        if(entry->data == data){
            qc_list_removeentry(list, entry);
            return 0;
        }
        entry = qc_list_enumentry(list);
    }

    return -1;
}


int qc_list_insertsort(QcList *list, QC_LIST_COMPAREFUNC CompareFunc)
{
    QcListEntry *entry;
    QcListEntry *prev_entry;

    entry = list->head;
    prev_entry = NULL;

    if(NULL == entry)
    {
        return 0;
    }

    while(entry->next)
    {
        QcListEntry *temp_entry;

        temp_entry = list->head;
        prev_entry = NULL;
        while(temp_entry != entry->next)
        {
            if((*CompareFunc)(entry->next->data, temp_entry->data) < 0)
            {
                QcListEntry *cur_entry = entry->next;

                entry->next = entry->next->next;
                if(entry->next)
                {
                    entry->next->prev = entry;
                }

                if(prev_entry)
                {
                    prev_entry->next = cur_entry;
                    cur_entry->prev = prev_entry;

                    cur_entry->next = temp_entry;
                    temp_entry->prev = cur_entry;
                }
                else
                {
                    cur_entry->next = temp_entry;
                    cur_entry->prev = NULL;
                    list->head->prev = cur_entry;
                    list->head = cur_entry;
                }

                if(cur_entry == list->tail)
                {
                    list->tail = entry;
                }

                break;
            }

            prev_entry = temp_entry;
            temp_entry = temp_entry->next;
        } /* while ( temp_entry != entry->next ) */

        if(temp_entry == entry->next)
        {
            entry = entry->next;
        }
        else
        {
            /**/
        }
    } /* while ( entry->next != NULL ) */

    return 1;
}


void qc_list_r_lock(QcList *list)
{
    if(NULL != list->rwlock)
    {
        qc_thread_rdlock_lock(list->rwlock);
    }
}


void qc_list_r_unlock(QcList *list)
{
    if(NULL != list->rwlock)
    {
        qc_thread_rdlock_unlock(list->rwlock);
    }
}


void qc_list_w_lock(QcList *list)
{
    if(NULL != list->rwlock)
    {
        qc_thread_wrlock_lock(list->rwlock);
    }
}


void qc_list_w_unlock(QcList *list)
{
    if(NULL != list->rwlock)
    {
        qc_thread_wrlock_unlock(list->rwlock);
    }
}

