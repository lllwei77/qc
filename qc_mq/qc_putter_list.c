#include "qc_log.h"
#include "qc_putter_list.h"



QcPutter* qc_putter_create(){
    QcPutter *putter;

    qc_malloc(putter, sizeof(QcPutter));
    if(NULL == putter){
        qc_error("malloc QcPutter faild.");
        return NULL;
    }

    putter->cond = qc_thread_cond_create();
    if(NULL == putter->cond){
        qc_error("create putter->cond failed.");
        qc_free(putter);
        return NULL;
    }

    putter->condlock = qc_thread_condlock_create();
    if(NULL == putter->condlock){
        qc_error("create putter->condlock failed.");
        qc_free(putter);
        return NULL;
    }

    putter->message = NULL;
    putter->is_timedout = 0;

    return putter;
}


int qc_putter_destroy(QcPutter *putter){
    if(NULL == putter){
        qc_error("invalid params.");
        return -1;
    }

    qc_thread_cond_destroy(putter->cond);
    qc_thread_condlock_destroy(putter->condlock);
    
    qc_free(putter);

    return 0;
}

//--------------------------------------------------------------------------------

QcPutterList* qc_putterlist_create(){
	QcPutterList *putterList;
	qc_malloc(putterList, sizeof(QcPutterList));
    if(NULL == putterList){
        return NULL;
    }

    QcList *list = qc_list_create(1);
    if(NULL == list){
        qc_free(list);
        return NULL;
    }

    putterList->_putterList = list;
    return putterList;
}


int qc_putterlist_destroy(QcPutterList *putterList){
    if(NULL == putterList){
        return -1;
    }

    if(putterList->_putterList){
        qc_list_destroy(putterList->_putterList);
    }

    qc_free(putterList);

    return 0;
}


int qc_putterlist_push(QcPutterList *putterList, QcPutter *putter){

    qc_assert(NULL!=putterList && NULL!=putter);
    
    QcListEntry *listEntry;

    qc_list_w_lock(putterList->_putterList);
    int ret = qc_list_inserttail2(putterList->_putterList, putter, &listEntry);
    qc_assert(ret == 0);

    putter->_entry = listEntry;
    qc_list_w_unlock(putterList->_putterList);
    return 0;
}


QcPutter* qc_putterlist_pop(QcPutterList *putterList){
    QcPutter *putter;
    qc_list_w_lock(putterList->_putterList);
    putter = qc_list_pophead(putterList->_putterList);
    qc_list_w_unlock(putterList->_putterList);

    return putter;
}


int qc_putterlist_remove(QcPutterList *putterList, QcPutter *putter){
    qc_assert(NULL!=putterList && NULL!=putter);

    QcListEntry *listEntry = putter->_entry;
    qc_list_w_lock(putterList->_putterList);
    int ret = qc_list_removeentry(putterList->_putterList, listEntry);
    qc_list_w_unlock(putterList->_putterList);

    qc_assert(ret == 0);
    return 0;
}

