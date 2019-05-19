#include "qc_message.h"



struct __QcMessage{
    char *buff;
    int  bufflen;
    int  priority;
};



QcMessage* qc_message_create(const char *buff, int len, int flag_dupbuff){
    QcMessage *message;
    qc_malloc(message, sizeof(QcMessage));
    if(NULL == message)
        return NULL;
    
    if(NULL == (char *)buff) len = 0;

    if(flag_dupbuff){
        qc_malloc(message->buff, len);
        if(NULL == message->buff){
            qc_free(message);
            return NULL;
        }
        memcpy(message->buff, buff, len);
    }
    else{
        message->buff = (char *)buff;        
    }

    message->bufflen = len;
    message->priority = 1;

    return message;
}


void qc_message_release(QcMessage *message, int flag_freebuff){
    if(NULL == message)
        return;

    if(flag_freebuff && message->buff) qc_free(message->buff);
    qc_free(message);
    return;
}


const char* qc_message_buff(QcMessage *message){
    qc_assert(message);
    return message->buff;
}


int qc_message_bufflen(QcMessage *message){
    qc_assert(message);    
    return message->bufflen;
}


void qc_message_setpriority(QcMessage *message, int priority){
    qc_assert(message);
    qc_assert(priority>=0)

    message->priority = priority;
}


int qc_message_priority(QcMessage *message){
    qc_assert(message);
    return message->priority;
}

