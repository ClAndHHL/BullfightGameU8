//
//  GameHUD.h
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//

#ifndef __BullfightGame__GameHUD__
#define __BullfightGame__GameHUD__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "GameLogic.h"
#include "TCPSocket.h"
#include "GameEndLayer.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;
#define MAX_TIMER		10		//计时器最大值
class BaseGameControl:public CCLayer,GameLogic
{
private:
	//操作者提示动画
	CCArmature *pArmatureActionPrompt;
	//准备容器
	UIPanel *pPanelReady;
	//抢庄容器
	UIPanel *pFightForBanker;
	//配置牛牛容器
	UIPanel *pOptOx;
	//投注容器
	UIPanel *pBetting;
	//加注按键
	UIButton *pbBetting[4];
	//计时器
	UIImageView *pITimer;
	//计时器数字
	UILabelAtlas *pLTimerNum;
	//计时变量
	int iTimerCount;
	//庄家用户
	BYTE wBankerUser;
	//游戏结算层
	GameEndLayer *pEndLayer;
public:
	BaseGameControl();
	~BaseGameControl();
	virtual void onEnter();
	virtual void onExit();
	//更新消息
	void update(float delta);
	//更新状态
	void updateState();
public:
	//显示指定索引提示动画
	void showActionPrompt(int promptIndex);
	//隐藏提示动画
	void hideActionPrompt();
	//获取庄家视图位置
	int getBankViewID();
private:
	//初始化操作者提示动画
	void initActionPrompt();
	//初始化计时器
	void initTimer(UILayer *pWidget);
	//设置计时器
	void resetTimer();
	//隐藏计时器
	void hideTimer();
	//更新计时器
	void updateTimer(float dt);
	//延时操作
	void delayedAction();
private:
	//菜单////////////////////////////////////////////////////////////////////////
	void menuPause(CCObject* pSender, TouchEventType type);
	//开牌
	void menuOpenCard(CCObject* pSender, TouchEventType type);
	//提示按键
	void menuPrompt(CCObject* pSender, TouchEventType type);
	//更换桌子
	void menuChangeChair(CCObject* pSender, TouchEventType type);
	//准备按键
	void menuReady(CCObject* pSender, TouchEventType type);
	//不抢按键
	void menuNotFight(CCObject* pSender, TouchEventType type);
	//抢庄按键
	void menuFight(CCObject* pSender, TouchEventType type);
	//投注
	void menuBetting(CCObject* pSender, TouchEventType type);
	//获取我的椅子位置
	int getMeChairID();
	//是不是观察者
	bool IsLookonMode();
	//获得椅子索引
	virtual int getChairIndex(int meChairID,int chairID)=0;
//////////////////////////////////////////////////////////////////////////
	//网络消息
	void OnEventGameMessage(CCObject *pObj);
	//用户叫庄
	bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户下注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//开牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//用户强退
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	
	//用户站立
	void OnUserFree(CCObject *obj);
	//用户进入
	void OnUserEnter(CCObject *obj);
	/*void onAddScore(CCObject *obj);
	void onSendCard(CCObject *obj);*/
};

#endif /* defined(__BullfightGame__GameHUD__) */
