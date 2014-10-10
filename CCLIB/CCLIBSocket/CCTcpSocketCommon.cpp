/**************************************************************************************
@author: 陈昌
@content: 
**************************************************************************************/

#include "CCTcpSocketCommon.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


//函数的实现必须定义到cpp文件中，不然多个文件引用后，编译时声称的obj对象里面都会有这个函数标识符，链接时候会导致重复
bool DoInitialWinSocket()
{
	WSADATA wsa;
	return (WSAStartup(0x2020, &wsa) == NO_ERROR);
}

void DoFinalizeWinSocket()
{
	WSACleanup();
}

void SendDebugString(const std::string& sInfo)
{
	std::cout << sInfo.c_str() << std::endl;
}