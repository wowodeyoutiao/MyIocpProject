/**************************************************************************************
@author: 陈昌
@content: DispatchGate的常量和变量的声明 
**************************************************************************************/
#ifndef __CC_DISPATCH_GATE_GLOBAL_H__
#define __CC_DISPATCH_GATE_GLOBAL_H__

#include "stdafx.h"

using namespace std;

enum TIpType
{
	itUnKnow,
	itAllow,
	itDeny,
	itMaster
};

typedef struct _TIpRuleNode
{
	std::string sMatchIP;
	TIpType ipType;
}TIpRuleNode, *PIpRuleNode;

const int MAX_CENTER_SERVER_COUNT = 3;    //中央服务器最大数量
const int MAX_RESSERVER_COUNT = 40;       //资源服务器最大数量

//配置文件名---全局变量使用extern在.h文件声明，在.cpp文件定义
extern TServerConnectInfo G_ResServerInfos[];  //资源服务器的连接信息

//服务器全局对象


#endif //__CC_DISPATCH_GATE_GLOBAL_H__