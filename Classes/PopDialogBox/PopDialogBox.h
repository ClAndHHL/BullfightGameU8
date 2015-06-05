/*
 * PopDialogBox.h
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "../Tools/GameConfig.h"
#include "../Tools/CStringAide.h"
#include "../Network/TCPSocket/TCPSocketControl.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

struct IPopAssist//弹框辅助接口
{
	virtual ~IPopAssist(){}
	//购买数量回调
	virtual void onBuyNum(long lNum) = 0;
};

class PopDialogBox: public CCLayer,public CStringAide {
public:
	CC_SYNTHESIZE(IPopAssist*, pIPopAssist, IPopAssist);
public:
	enum BuyType
	{
		BUY_AUCTION=0,					//拍卖购买
	};
	CC_SYNTHESIZE(BuyType, buyType, BuyType);
	UILayer * pUILayer;
	UIImageView *pWidgetBg;
	static std::string sSocketName;
public:
	PopDialogBox();
	~PopDialogBox();
	//菜单////////////////////////////////////////////////////////////////////////
	void menuBack(CCObject *object, TouchEventType type);

	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){return true; }
	//播放放大动画
	void playAnimation();

	TCPSocket *getSocket(){return TCPSocketControl::sharedTCPSocketControl()->getTCPSocket(sSocketName); }
	//连接服务器
	void connectServer(std::string socketName);
	void setSocketName(std::string sName);
	//显示提示语
	void showTipInfo(const char* sInfo);
	//设置大厅是否读取网络消息
	void setLobbyReadMessage(bool isRead);
	//显示数量输入框
	void showInputNumBox(BuyType eBuyType, const char* cPropName, const char* cPropImagePuth, long lMaxNum, long long lPice,IPopAssist* pIPop);
};
