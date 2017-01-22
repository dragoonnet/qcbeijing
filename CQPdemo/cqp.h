/*
* CoolQ SDK for VC++ 
* Api Version 9.6
* Written by Coxxs & Thanks for the help of orzFly
*/
#pragma once

#define CQAPIVER 9
#define CQAPIVERTEXT "9"

#ifndef CQAPI
#define CQAPI(ReturnType) extern "C" __declspec(dllimport) ReturnType __stdcall
#endif

#define CQEVENT(ReturnType, Name, Size) __pragma(comment(linker, "/EXPORT:" #Name "=_" #Name "@" #Size))\
 extern "C" __declspec(dllexport) ReturnType __stdcall Name

typedef int32_t CQBOOL;

#define EVENT_IGNORE 0        //�¼�_����
#define EVENT_BLOCK 1         //�¼�_����

#define REQUEST_ALLOW 1       //����_ͨ��
#define REQUEST_DENY 2        //����_�ܾ�

#define REQUEST_GROUPADD 1    //����_Ⱥ���
#define REQUEST_GROUPINVITE 2 //����_Ⱥ����

#define CQLOG_DEBUG 0           //���� ��ɫ
#define CQLOG_INFO 10           //��Ϣ ��ɫ
#define CQLOG_INFOSUCCESS 11    //��Ϣ(�ɹ�) ��ɫ
#define CQLOG_INFORECV 12       //��Ϣ(����) ��ɫ
#define CQLOG_INFOSEND 13       //��Ϣ(����) ��ɫ
#define CQLOG_WARNING 20        //���� ��ɫ
#define CQLOG_ERROR 30          //���� ��ɫ
#define CQLOG_FATAL 40          //�������� ���

//1.��������Ϣ
CQAPI(int32_t) CQ_sendPrivateMsg(int32_t AuthCode, int64_t QQID, const char *msg);
//2.��Ⱥ��Ϣ
CQAPI(int32_t) CQ_sendGroupMsg(int32_t AuthCode, int64_t groupid, const char *msg);
//3.����������Ϣ
CQAPI(int32_t) CQ_sendDiscussMsg(int32_t AuthCode, int64_t discussid, const char *msg);
//4.����
CQAPI(int32_t) CQ_sendLike(int32_t AuthCode, int64_t QQID);
//5.Ⱥ����
CQAPI(int32_t) CQ_setGroupKick(int32_t AuthCode, int64_t groupid, int64_t QQID, CQBOOL rejectaddrequest);
//6.����Ⱥ��Ա����
CQAPI(int32_t) CQ_setGroupBan(int32_t AuthCode, int64_t groupid, int64_t QQID, int64_t duration);
//7.����Ⱥ����Ա
CQAPI(int32_t) CQ_setGroupAdmin(int32_t AuthCode, int64_t groupid, int64_t QQID, CQBOOL setadmin);
//8.ȫȺ����/���
CQAPI(int32_t) CQ_setGroupWholeBan(int32_t AuthCode, int64_t groupid, CQBOOL enableban);
//9.��������
CQAPI(int32_t) CQ_setGroupAnonymousBan(int32_t AuthCode, int64_t groupid, const char *anomymous, int64_t duration);
//10.�����Ƿ���������
CQAPI(int32_t) CQ_setGroupAnonymous(int32_t AuthCode, int64_t groupid, CQBOOL enableanomymous);
//11.����Ⱥ��Ƭ
CQAPI(int32_t) CQ_setGroupCard(int32_t AuthCode, int64_t groupid, int64_t QQID, const char *newcard);
//12.�˳�Ⱥ�¼�
CQAPI(int32_t) CQ_setGroupLeave(int32_t AuthCode, int64_t groupid, CQBOOL isdismiss);
//13.���ó�ԱȺͷ��
CQAPI(int32_t) CQ_setGroupSpecialTitle(int32_t AuthCode, int64_t groupid, int64_t QQID, const char *newspecialtitle, int64_t duration);
//14.�˳��������¼�
CQAPI(int32_t) CQ_setDiscussLeave(int32_t AuthCode, int64_t discussid);
//15.���������¼�
CQAPI(int32_t) CQ_setFriendAddRequest(int32_t AuthCode, const char *responseflag, int32_t responseoperation, const char *remark);
//16.��Ⱥ�����¼�
CQAPI(int32_t) CQ_setGroupAddRequestV2(int32_t AuthCode, const char *responseflag, int32_t requesttype, int32_t responseoperation, const char *reason);
//17.��ȡȺ��Ա��Ϣ
CQAPI(const char *) CQ_getGroupMemberInfoV2(int32_t AuthCode, int64_t groupid, int64_t QQID, CQBOOL nocache);
//18.��ȡİ������Ϣ
CQAPI(const char *) CQ_getStrangerInfo(int32_t AuthCode, int64_t QQID, CQBOOL nocache);
//19.���������־
CQAPI(int32_t) CQ_addLog(int32_t AuthCode, int32_t priority, const char *category, const char *content);
//20.��ȡcookies��¼��Ϣ
CQAPI(const char *) CQ_getCookies(int32_t AuthCode);
//21.��ȡ֤������
CQAPI(int32_t) CQ_getCsrfToken(int32_t AuthCode);
//22.��ȡ��¼��qq����
CQAPI(int64_t) CQ_getLoginQQ(int32_t AuthCode);
//23.��ȡ��¼�ߵ��ǳ�
CQAPI(const char *) CQ_getLoginNick(int32_t AuthCode);
//24.��ȡӦ�ó���Ŀ¼
CQAPI(const char *) CQ_getAppDirectory(int32_t AuthCode);
//25.�׳�һ�����󣨾���Ч����û�ù�)
CQAPI(int32_t) CQ_setFatal(int32_t AuthCode, const char *errorinfo);
