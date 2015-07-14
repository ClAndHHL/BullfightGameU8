/*
 * PopDialogBoxKnapsack.h
 * 我的背包
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#pragma once

#include "PopDialogBox.h"
//#include "../Network/TCPSocket/TCPSocketControl.h"
#include "../Network/CMD_Server/CMD_LogonServer.h"

struct IPopAssistKnapsack//弹框背包辅助接口
{
	virtual ~IPopAssistKnapsack(){}
	//关闭背包
	virtual void onCloseKnapsack() = 0;
};

class PopDialogBoxKnapsack: public PopDialogBox  {
public:
	//背包接口
	CC_SYNTHESIZE(IPopAssistKnapsack*, pIPopAssistKnapsack, IPopAssistKnapsack);
private:
    
	enum KnapsackItem
	{
		KNAPSACK_LIST=0,				//背包列表
		KNAPSACK_EXCHANGE,				//兑换
	};
	CC_SYNTHESIZE(KnapsackItem, knapsackItem, KnapsackItem);
	
	std::vector<CMD_GP_Knapsack> vecGoods;

	//物品列表
	UIListView *pListViewGoods;
	//信息背景
	ImageView *pIInfoBg;
	//空包背景
	ImageView *pINothing;
	//当前选择物品索引
	int iCurSelectIndex;
	//物品名称
	Label *pLGoodsName;
	//物品图片
	ImageView *pIVGoods;
public:
	PopDialogBoxKnapsack();
	~PopDialogBoxKnapsack();
	CREATE_FUNC(PopDialogBoxKnapsack);
private:
	virtual void onEnter();
	virtual void onExit();
	//更新网络消息
	void update(float delta);
	//初始化物品列表
	void initListGoods();
	//更新物品列表
	void updateListGoods();
	//初始化物品信息
	void initGoodsInfo();
	//更新物品信息
	void updateGoodInfo(int index);
	//复选框回调
	void onCheckBoxSelectedStateEvent(CCObject *pSender, CheckBoxEventType type);
	//兑换按键
	void onMenuExchange(CCObject *object, TouchEventType type);
	//////////////////////////////////////////////////////////////////////////
	//连接成功
	void connectSuccess();
	//网络消息
	virtual void onEventReadMessage(WORD wMainCmdID, WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize);
	//用户服务
	void onEventUserService(WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize);
	//背包列表
	void onSubGoodsList(void * pDataBuffer, unsigned short wDataSize);
	//使用物品
	void onSubUseGoods(void * pDataBuffer, unsigned short wDataSize);
};
