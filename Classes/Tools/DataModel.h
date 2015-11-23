#pragma once
#include "cocos2d.h"
#include "../MainScene/MainSceneOxTwo.h"
#include "../MainScene/MainSceneOxHundred.h"
#include "../MainScene/MainSceneOxOneByOne.h"
#include "../Network/CMD_Server/CMD_LogonServer.h"
#include "../Play/OxHundred/JettonNode.h"
USING_NS_CC;
//更多帐号
struct MoreAccount
{
	std::string userAccount;
	std::string userPwd;
};
//新消息ID
struct NewMsg
{
	std::string sMsgId;
};
class DataModel:public CCObject{
public:
	//登录url
	CCArray *urlLogon;
	//当前线路索引
	static int curLineIndex;
	//ip地址
	std::string ipaddr;
	//音效开关
	static bool isSound;
	//音乐开关
	static bool isMusic;
	//是否是游客
	static bool isQuickUser;
	//登录模式
	int logonType;
	//是否坐下
	bool isSit;
	//是否显示新消息提示
	bool isShowNewMsg;
	//是否显示新任务提示
	bool isShowNewTaskMsg;
	//是否显示新拍卖提示
	bool isShowNewAuctionMsg;

	CCSize deviceSize;
	//登录帐号
	std::string sLogonAccount;
	//登录密码
	std::string sLogonPassword;
	//更多帐号
	std::vector <MoreAccount> vecMoreAccount;
	//新消息提示
	std::map <long,NewMsg> mNewMsg;
	//长连接大厅地址
	std::string sLobbyIp;
	//手机号码
	std::string sPhone;
	//长连接大厅端口
	int lLobbyProt;
	//主场景对象
	CC_SYNTHESIZE(MainSceneOxTwo *, mainSceneOxTwo, MainSceneOxTwo);
	CC_SYNTHESIZE(MainSceneOxHundred *, mainSceneOxHundred, MainSceneOxHundred);
	CC_SYNTHESIZE(MainSceneOxOneByOne *, mainSceneOxOneByOne, MainSceneOxOneByOne);

    
	CCArray *m_aTagGameKind;
	CCArray *m_aMakeText;

	std::vector <tagGameServer *> tagGameServerListOxTwo;
	std::vector <tagGameServer *> tagGameServerListOxHundred;
	std::vector <tagGameServer *> tagGameServerListOxOneByOne;
	std::vector <tagGameServer *> tagGameServerListSixSwap;
	//用户信息数组
	std::map<long ,tagUserInfo>mTagUserInfo;
	//签到记录
	std::map < long, int  > mapSignRecord;
	//登录成功信息
	//CMD_MB_LogonSuccess *logonSuccessUserInfo;
	//最大加注数
	long long m_lTurnMaxScore;
	//牌数组
	BYTE card[MAX_PLAYER][5];
	//是否设置过银行密码
	bool cbInsurePwd;
	//变量定义
	tagUserInfo *userInfo;
	//消息队列游戏中
	std::queue<ReadData>readDataQueueGameIng;
	//消息队列
	//std::queue<ReadData>readDataQueue;
	//大厅消息队列
	std::queue<ReadData>readDataQueueLobby;
   
	//系统消息队列
	std::vector<std::string>vecSystemMsg;
	//我的消息队列
	std::vector<std::string>vecMyMsg;
	
	//筹码
	std::vector<JettonNode *>vecJettonNode;
public:
	DataModel();
	~DataModel();
	static DataModel* sharedDataModel();
private:
	void initDataModel();
	//初始化新消息提示
	void initNewMsgTip();
public:
	//排序vector
	void sortVector(std::vector <tagGameServer *> &vTagGameServer);
	//移除服务列表
	void removeTagGameServerList(std::vector <tagGameServer *> vTagGameServer);
	//获取ip地址
	std::string getLogonAddr();
	//重设连接
	void resetCon();
};
