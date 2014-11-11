/**************************************************************************************
@author: 陈昌
@content: 服务器间通信的常量和结构定义
**************************************************************************************/

#ifndef __CC_PROTOCOL_SERVER_H__
#define __CC_PROTOCOL_SERVER_H__

using namespace std;

// 默认的侦听端口
extern const int DEFAULT_AuthenServer_PORT;                         // AuthenServer <- DB
extern const int DEFAULT_DispatchGate_DB_PORT;                      // DispatchGate <- DB
extern const int DEFAULT_DispatchGate_CLIENT_PORT;                  // DispatchGate <- CLIENT
extern const int DEFAULT_DBServer_GS_PORT;                          // DBServer     <- GS
extern const int DEFAULT_DBServer_GG_PORT;                          // DBServer     <- GG
extern const int DEFAULT_GameServer_PORT;                           // GameServer   <- GG
extern const int DEFAULT_GameGate_PORT;                             // GameGate     <- CLIENT
extern const int DEFAULT_IMServer_GS_PORT;                          // IMServer  <- GS
extern const int DEFAULT_IMServer_GG_PORT;                          // IMServer <- GG
extern const int DEFAULT_RESCENTER_PORT;                            // ResourceServer <- Client
extern const int DEFAULT_RESCENTERCENTER_CR_PORT;                   // ResourceCenter <- ResourceServer
extern const int DEFAULT_RESCENTERCENTER_CD_PORT;                   // ResourceCenter <- DispatchGate
extern const int DEFAULT_WEBINTERFACE_PORT;                         // WEB <- WebClient
extern const int DEFAULT_WEBSERVER_PORT;                            // WEBInterface <- AuthenServer
extern const int DEFAULT_WEBQUERYSRV_PORT;                          // WEBQuery <- WebClient
extern const int DEFAULT_ACTLOG_SERVER_PORT;                        // GS <- ACTLog
extern const int DEFAULT_PIG_SERVER_PORT;                           // DispatchGate <- PigServer
extern const int DEFAULT_CONTROL_SERVER_PORT;                       // EventLog <- ControlClient


#endif //__CC_PROTOCOL_SERVER_H__