#include "qc_prelude.h"
#include "qc_protocol.h"



void qc_prtcl_head_hton(QcPrtclHead *head)
{
	head->protocol = htons(head->protocol);
	head->version = htons(head->version);
	head->type = htons(head->type);
	head->packsn = htonl(head->packsn);
	head->body_len = htonl(head->body_len);
}


void qc_prtcl_head_ntoh(QcPrtclHead *head)
{
	head->protocol = ntohs(head->protocol);
	head->version = ntohs(head->version);
	head->type = ntohs(head->type);
	head->packsn = ntohl(head->packsn);
	head->body_len = ntohl(head->body_len);
}


void qc_prtcl_msgput_hton(QcPrtclMsgPut *msgput)
{
	msgput->msg_prioriy = htons(msgput->msg_prioriy);
	msgput->wait_msec = htonl(msgput->wait_msec);
	msgput->msg_len = htonl(msgput->msg_len);
}


void qc_prtcl_msgput_ntoh(QcPrtclMsgPut *msgput)
{
	msgput->msg_prioriy = ntohs(msgput->msg_prioriy);
	msgput->wait_msec = ntohl(msgput->wait_msec);
	msgput->msg_len = ntohl(msgput->msg_len);
}


void qc_prtcl_msgget_hton(QcPrtclMsgGet *msgget)
{
	msgget->wait_msec = htonl(msgget->wait_msec);
}


void qc_prtcl_msgget_ntoh(QcPrtclMsgGet *msgget)
{
	msgget->wait_msec = ntohl(msgget->wait_msec);
}


void qc_prtcl_reply_hton(QcPrtclReply *reply)
{
	reply->result = htonl(reply->result);
	reply->msg_len = htonl(reply->msg_len);
}


void qc_prtcl_reply_ntoh(QcPrtclReply *reply)
{
	reply->result = ntohl(reply->result);
	reply->msg_len = ntohl(reply->msg_len);
}
