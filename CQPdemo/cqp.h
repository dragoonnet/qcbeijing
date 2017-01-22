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

#define EVENT_IGNORE 0        //事件_忽略
#define EVENT_BLOCK 1         //事件_拦截

#define REQUEST_ALLOW 1       //请求_通过
#define REQUEST_DENY 2        //请求_拒绝

#define REQUEST_GROUPADD 1    //请求_群添加
#define REQUEST_GROUPINVITE 2 //请求_群邀请

#define CQLOG_DEBUG 0           //调试 灰色
#define CQLOG_INFO 10           //信息 黑色
#define CQLOG_INFOSUCCESS 11    //信息(成功) 紫色
#define CQLOG_INFORECV 12       //信息(接收) 蓝色
#define CQLOG_INFOSEND 13       //信息(发送) 绿色
#define CQLOG_WARNING 20        //警告 橙色
#define CQLOG_ERROR 30          //错误 红色
#define CQLOG_FATAL 40          //致命错误 深红

//1.发个人消息
CQAPI(int32_t) CQ_sendPrivateMsg(int32_t AuthCode, int64_t QQID, const char *msg);
//2.发群消息
CQAPI(int32_t) CQ_sendGroupMsg(int32_t AuthCode, int64_t groupid, const char *msg);
//3.发讨论组消息
CQAPI(int32_t) CQ_sendDiscussMsg(int32_t AuthCode, int64_t discussid, const char *msg);
//4.点赞
CQAPI(int32_t) CQ_sendLike(int32_t AuthCode, int64_t QQID);
//5.群踢人
CQAPI(int32_t) CQ_setGroupKick(int32_t AuthCode, int64_t groupid, int64_t QQID, CQBOOL rejectaddrequest);
//6.设置群成员禁言
CQAPI(int32_t) CQ_setGroupBan(int32_t AuthCode, int64_t groupid, int64_t QQID, int64_t duration);
//7.设置群管理员
CQAPI(int32_t) CQ_setGroupAdmin(int32_t AuthCode, int64_t groupid, int64_t QQID, CQBOOL setadmin);
//8.全群禁言/解禁
CQAPI(int32_t) CQ_setGroupWholeBan(int32_t AuthCode, int64_t groupid, CQBOOL enableban);
//9.匿名禁言
CQAPI(int32_t) CQ_setGroupAnonymousBan(int32_t AuthCode, int64_t groupid, const char *anomymous, int64_t duration);
//10.设置是否允许匿名
CQAPI(int32_t) CQ_setGroupAnonymous(int32_t AuthCode, int64_t groupid, CQBOOL enableanomymous);
//11.设置群名片
CQAPI(int32_t) CQ_setGroupCard(int32_t AuthCode, int64_t groupid, int64_t QQID, const char *newcard);
//12.退出群事件
CQAPI(int32_t) CQ_setGroupLeave(int32_t AuthCode, int64_t groupid, CQBOOL isdismiss);
//13.设置成员群头衔
CQAPI(int32_t) CQ_setGroupSpecialTitle(int32_t AuthCode, int64_t groupid, int64_t QQID, const char *newspecialtitle, int64_t duration);
//14.退出讨论组事件
CQAPI(int32_t) CQ_setDiscussLeave(int32_t AuthCode, int64_t discussid);
//15.好友申请事件
CQAPI(int32_t) CQ_setFriendAddRequest(int32_t AuthCode, const char *responseflag, int32_t responseoperation, const char *remark);
//16.入群申请事件
CQAPI(int32_t) CQ_setGroupAddRequestV2(int32_t AuthCode, const char *responseflag, int32_t requesttype, int32_t responseoperation, const char *reason);
//17.获取群成员信息
CQAPI(const char *) CQ_getGroupMemberInfoV2(int32_t AuthCode, int64_t groupid, int64_t QQID, CQBOOL nocache);
//18.获取陌生人信息
CQAPI(const char *) CQ_getStrangerInfo(int32_t AuthCode, int64_t QQID, CQBOOL nocache);
//19.输出调试日志
CQAPI(int32_t) CQ_addLog(int32_t AuthCode, int32_t priority, const char *category, const char *content);
//20.获取cookies登录信息
CQAPI(const char *) CQ_getCookies(int32_t AuthCode);
//21.获取证书令牌
CQAPI(int32_t) CQ_getCsrfToken(int32_t AuthCode);
//22.获取登录者qq号码
CQAPI(int64_t) CQ_getLoginQQ(int32_t AuthCode);
//23.获取登录者的昵称
CQAPI(const char *) CQ_getLoginNick(int32_t AuthCode);
//24.获取应用程序目录
CQAPI(const char *) CQ_getAppDirectory(int32_t AuthCode);
//25.抛出一个错误（具体效果我没用过)
CQAPI(int32_t) CQ_setFatal(int32_t AuthCode, const char *errorinfo);
