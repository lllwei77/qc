#ifndef H_QC_MSGCHAIN
#define H_QC_MSGCHAIN

#include "qc_prelude.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_pool.h"
#include "qc_thread.h"
#include "qc_msg_bucket.h"



typedef struct{
    int count_limit;    //limit of message count
	int msg_count;      //current message count

    int bucket_count;
    QcMsgBucket **msgBuckets;
    int cursor_bucketsn;

	QcList *tempList;  //for blocking getter
}QcMsgChain;



#ifdef __cplusplus
extern "C" {
#endif


QcMsgChain* qc_msgchain_create(int count_limit, int priority_max);

int qc_msgchain_destroy(QcMsgChain *msgChain);

int qc_msgchain_msgcount(QcMsgChain *msgChain);

int qc_msgchain_pushmsg(QcMsgChain *msgChain, QcMessage *message);

QcMessage* qc_msgchain_popmsg(QcMsgChain *msgChain);

int qc_msgchain_puttemp(QcMsgChain *msgChain, QcMessage *message);  //for getter

QcMessage* qc_msgchain_gettemp(QcMsgChain *msgChain);  //for getter


#ifdef __cplusplus
}
#endif


#endif /*H_QC_MSGCHAIN*/
