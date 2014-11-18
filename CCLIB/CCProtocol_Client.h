/**************************************************************************************
@author: 陈昌
@content: 游戏客户端与服务器间通信的常量和结构定义
**************************************************************************************/

#ifndef __CC_PROTOCOL_CLIENT_H__
#define __CC_PROTOCOL_CLIENT_H__

// Client与服务器间的通讯头
typedef struct _TClientSocketHead
{
	unsigned long ulSign;				// 分隔符 CS_SEGMENTATION_SIGN
	unsigned short usPackageLen;		// 封包总长度
	unsigned short usIdent;				// Ident or $8000 = 压缩   [Ident>=$8000 数据头后2字节表示压缩前数据长度，后续数据为压缩数据区。Ident<$8000 数据头后为数据区]
	unsigned long ulIdx;			    // 包序号,后续数据可能会被压缩
}TClientSocketHead, *PClientSocketHead;

//客户端的窗口类型
enum TClientWindowType{
	cwInValid = 0,                                          // 无效窗口
	cwAuthenWindow = 1,                                     // 登录认证窗口
	cwWarRule,                                              // 同意对战协议窗口
	cwQueue,                                                // 排队窗口
	cwCreateRole,                                           // 创建角色窗口
	cwMessageBox,                                           // 弹出窗口
	cwDialog,
	cwNotice,                                               // 文明公约
	cwDealRequest,                                          // 交易请求提示框
	cwDealDialog,                                           // 交易框
	cwVerifyCode,                                           // 验证码窗口
	cwReLive,                                               // 复活窗口
	cwTransDialog,                                          // 事务窗口，这类型的窗口确认返回时必须将TransID原样返回
	/*
		cwTransDialog类型窗口具体说明：
		SCM_OPEN_WINDOW：Param 0:确认框 1：输入框 2 : 2个输入框
						 TransID：事务ID，TClientWindowRec结构后为提示信息
		CM_CLOSE_WINDOW：Param 0 : 取消   1：确认 TransID：事务ID，若是输入框TClientWindowRec结构后为输入内容
	*/

	cwScriptDialog,                                         // 脚本对话框
	cwNpcItemOper,                                          // Npc对单独物品的操作窗口
	/*
		cwNpcItemOper类型窗口具体说明：
		Param : 1 物品修理窗口
	*/

	cwNpcShop,                                              // Npc商店窗口
	cwIMTransDialog,                                        // IMServer弹出对话框
	cwStorage,                                              // 仓库窗口
	cwMonCry,                                               // 怪物说话
	/*
		cwMonCry类型窗口具体说明：
		Param: 头像索引
		TransID : 音效索引
		buf : 说话内容
	*/

	cwCopyHint,                                             // 副本提示
	/*
		cwCopyHint类型窗口具体说明：
		Param: 副本ID
	    TransID : 0 : 开启 1 : 通过 2：失败
	    buf : 提示信息
	*/

	cwColdTime,                                             // 倒计时
	/*
		cwColdTime类型窗口具体说明：
		Param:倒计时时长
		TransID : 倒计时类型ID
		buf : 倒计时提示
	*/

	cwWeaponUpgrade,                                        // 武器升级
	cwGuildSet,                                             // 行会设置界面
	cwYBDeal,                                               // 元宝交易框
	cwGridData,                                             // 网格数据窗口
	/*
		cwGridData类型窗口具体说明：
		Param:TGridType
		buf : 窗口定义数据
	*/

	cwSafeCard,                                             //密保卡认证
	/*
	    cwSafeCard类型窗口具体说明：
		SCM_OPEN_WINDOW
		param : 为3个矩阵值
		CM_CLOSE_WINDOW : 
		BUF : 为3个矩阵对应输入值字符串
	*/

	cwPayPwd,                                               // 支付密码
	cwWeaponMelting,                                        // 武器熔炼
	cwRoleReName,                                           // 角色重命名
	cwHopePool,                                             // 许愿池
	cwEmail,                                                // 邮件框
	cwPlayerShop,                                           // 玩家商铺
	cwBag,                                                  // 背包
	cwYearCeremony                                          // 年度盛典
};

//CM_CLOSE_WINDOW、SCM_OPEN_WINDOW、SCM_CLOSE_WINDOW
typedef struct _TClientWindowRec
{
	TClientWindowType WinType;
	int Param;
	unsigned long TransID;          // 事务ID
	//buf:提示信息字符串
}TClientWindowRec, *PClientWindowRec;

const int CS_SEGMENTATION_CLIENTSIGN = 0XAABBCCDD;          // 游戏客户端和服务器间通讯协议起始标志

//游戏客户端发送给服务器的消息

const int CM_CLOSE_WINDOW = 14;                             // 关闭窗体

//服务器端发给游戏客户端的消息

const int SCM_OPEN_WINDOW = 14;                             // 开启窗口
const int SCM_CLOSE_WINDOW = 15;                            // 服务端请求关闭窗口



#endif //__CC_PROTOCOL_CLIENT_H__