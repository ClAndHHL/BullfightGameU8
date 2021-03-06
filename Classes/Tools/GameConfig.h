#pragma once


#define DEBUG_TEST 2 //0调试模式、1测试模式、2发布模式
#define GAME_VERSION "Version1.2.1"//游戏版本号

#define VERSION_FRAME				16777217
#define VERSION_CLIENT				17170433
#define VERSION_PLAZA 				17235969			//广场版本号

#define LISTENER_NAME "listener_bullfight"//监听通知名称

#define SCENE_SIZE CCSize(1136,640)			//设计尺寸
//#define CCS_PATH_SCENE(name) CCString::createWithFormat("publish/%s",(name))->getCString()
#define CCS_PATH_SCENE(name) "publish/"#name
#define CCS_PATH_ANIMATION(name) "publish/animation/"#name
#define IMAGE_URL(name)  CCString::createWithFormat("http://121.41.116.223:8090/image/PropIcon/%s.png",(name))->getCString()
#define IMAGE_NAME(name)  CCString::createWithFormat("%s.png",(name))->getCString()
//"http ://125.88.145.41:8090/image/PropIcon/"#name".png"


#define MAX_PLAYER							6						 //最大玩家数

//存档////////////////////////////////////////////////////////////////////////
#define RMS_IS_MUSIC "isMusic"
#define RMS_IS_SOUND "isSound"
#define RMS_LOGON_TYPE						"logonType"					//登录模式
#define RMS_LOGON_ACCOUNT					"logonAccount"				//登录帐号
#define RMS_LOGON_PASSWORD					"logonPassword"				//登录密码
#define RMS_SIGN_RECORD						"signRecord"				//签到记录
#define RMS_MORE_ACCOUNT					"moreAccount"				//更多帐号
#define RMS_NEW_MSG_TIP						"newMsgTip"					//新消息提示
#define RMS_FIRST_LOGON						"firstLogon"				//是否第一次登录
#define RMS_LINE_INDEX						"lineIndex"					//登录线路索引

//层级////////////////////////////////////////////////////////////////////////
#define K_Z_ORDER_POP				12			//弹出层
#define K_Z_ORDER_CARD				11			//牌
#define K_Z_ORDER_HUD				10			//UI层

#define K_Z_GI_PLAYER				2			//玩家信息层
#define K_Z_GI_CORD					1			//扑克
#define K_Z_GI_TITLE				0			//标题


//标识////////////////////////////////////////////////////////////////////////
#define TAG_LOADING								1000							//加载中
#define TAG_POP_DIALOG_BOX						1001							//弹出框
#define TAG_INPUT_BOX							1002							//输入框
#define TAG_INPUT_EDIT_BOX						1003							//输入框
#define TAG_UP_BANK								1004							//百人上庄
#define TAG_ONLINE								1005							//百人在线玩家
#define TAG_CLIP_LABEL							1006							//裁剪文本






