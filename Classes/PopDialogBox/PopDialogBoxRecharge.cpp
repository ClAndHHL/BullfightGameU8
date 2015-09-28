/*
 * PopDialogBoxRecharge.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#include "PopDialogBoxRecharge.h"
#include "../Tools/DataModel.h"
#include "../Tools/GameConfig.h"
#include "../Network/MD5/MD5.h"
#include "../Platform/coPlatform.h"
#include "../Network/SEvent.h"

#define MAX_GOLD_COUNT    6

std::string sGoldIconName[MAX_GOLD_COUNT] = { "u_recharge_gold0.png", "u_recharge_gold1.png", "u_recharge_gold2.png", "u_recharge_gold3.png", "u_recharge_gold4.png", "u_recharge_gold5.png" };
std::string sGoldExchangeNum[MAX_GOLD_COUNT] = { " 10万金币 ", " 30万金币 ", " 50万金币 ", " 100万金币 ", " 200万金币 ", " 500万金币 " };
std::string sGoldOtherNum[MAX_GOLD_COUNT] = { "", "", "", "", "", "" };

long lGoldPice[MAX_GOLD_COUNT] = { 10, 30, 50, 100, 200, 500 };

#define MAX_BIG_GOLD_COUNT    6
std::string sBigGoldIconName[MAX_BIG_GOLD_COUNT] = { "u_recharge_yb0.png", "u_recharge_yb1.png", "u_recharge_yb2.png", "u_recharge_yb3.png", "u_recharge_yb4.png", "u_recharge_yb5.png" };
std::string sBigGoldExchangeNum[MAX_BIG_GOLD_COUNT] = { " 10元宝 ", " 30元宝 ", " 50元宝 ", " 100元宝 ", " 200元宝 ", " 500元宝 " };
std::string sBigGoldOtherNum[MAX_BIG_GOLD_COUNT] = { " 加送1个红包碎片 ", " 加送5个红包碎片 ", " 加送2个红包 ", " 加送5个红包 ", " 加送12个红包 ", " 加送35个红包 " };

long lBigGoldPice[MAX_GOLD_COUNT] = { 10, 30, 50, 100, 200, 500 };

//////////////////////////////////////////////////////////////////////////
PopDialogBoxRecharge::PopDialogBoxRecharge()
	:eRechargeType(RECHARGE_GOLD)
	, eRechargeActionType(R_Action_GET_MONEY)
	, pIPopDialogBoxAssistCloseView(NULL)
	, isShowExchangeGold(true)
{
	for (int i = 0; i < MAX_GOLD_COUNT; i++)
	{
		RechargeData rData;
		rData.sImageIconName = sGoldIconName[i];
		rData.sExchangeNum = sGoldExchangeNum[i];
		rData.sOtherNum = sGoldOtherNum[i];
		rData.lExchangePice = lGoldPice[i];
		vecRechargeGold.push_back(rData);
	}
	for (int i = 0; i < MAX_BIG_GOLD_COUNT; i++)
	{
		RechargeData rData;
		rData.sImageIconName = sBigGoldIconName[i];
		rData.sExchangeNum = sBigGoldExchangeNum[i];
		rData.sOtherNum = sBigGoldOtherNum[i];
		rData.lExchangePice = lBigGoldPice[i];
		vecRechargeBigGold.push_back(rData);
	}
	

	
	scheduleUpdate();
}
PopDialogBoxRecharge::~PopDialogBoxRecharge() {
	CCLOG("~ <<%s>>",__FUNCTION__);
	unscheduleUpdate();
	//TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(SOCKET_RECHARGE);
	//gameSocket.Destroy(true);
}
void PopDialogBoxRecharge::onEnter(){
	CCLayer::onEnter();																					
	Layout* layoutPauseUI = static_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIPopDialogBoxRecharge.ExportJson)));
	pUILayer->addWidget(layoutPauseUI);
	
	pWidgetBg = static_cast<UIImageView*>(pUILayer->getWidgetByName("bg"));
	pWidgetBg->setScale(0.8);
	//关闭
	UIButton *backButton = static_cast<UIButton*>(pUILayer->getWidgetByName("buttonClose"));
	backButton->addTouchEventListener(this, toucheventselector(PopDialogBoxRecharge::onMenuCloseView));
	//兑换金币
	pBExchangeGold = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonGold"));
	pBExchangeGold->addTouchEventListener(this, toucheventselector(PopDialogBoxRecharge::onMenuChangeExchangeType));
	pBExchangeGold->setBright(false);
	pBExchangeGold->setTouchEnabled(false);
	//pBExchangeGold->setVisible(false);
	//兑换元宝
	pBExchangeBigGold = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonBigGold"));
	pBExchangeBigGold->addTouchEventListener(this, toucheventselector(PopDialogBoxRecharge::onMenuChangeExchangeType));
	//pBExchangeBigGold->setVisible(false);
	//游标
	//pIVCursor = static_cast<UIImageView*>(pUILayer->getWidgetByName("ImageCursor"));
	//类型名称
	//pLRechargeName = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelTypeName"));
	//我的当前金币
	pLCurGoldCount=static_cast<UILabel*>(pUILayer->getWidgetByName("LabelCurGoldCount"));
	//我的当前元宝
	pLCurBigGoldCount = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelCurBigGoldCount"));
	//消息列表
	pLVRechargeList = static_cast<UIListView*>(pUILayer->getWidgetByName("ListViewRecharge"));
	//设置cell模式
	pLVRechargeList->setItemModel(pLVRechargeList->getItem(0));
	pLVRechargeList->removeAllItems();

	if (isShowExchangeGold)
	{
		onMenuChangeExchangeType(pBExchangeGold, TOUCH_EVENT_ENDED);
	}
	else
	{
		onMenuChangeExchangeType(pBExchangeBigGold, TOUCH_EVENT_ENDED);
	}
	//DataModel::sharedDataModel()->vecSystemMsg.push_back("1233");
	//更新列表
	updateListRecharge(vecRechargeGold);
	//获取财富 
	connectServer();
	connectSuccess();

	playAnimation();

	//添加监听事件
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(PopDialogBoxRecharge::onUpdateDelta), UPDATE_DELTA, NULL);
}
void PopDialogBoxRecharge::onExit(){
	//移除监听事件 
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, UPDATE_DELTA);
	CCLayer::onExit();
}
//关闭窗口
void PopDialogBoxRecharge::onMenuCloseView(CCObject *object, TouchEventType type){
	if (type == TOUCH_EVENT_ENDED)
	{
		if (getIPopDialogBoxAssistCloseView())
		{
			getIPopDialogBoxAssistCloseView()->onCloseView();
		}
		this->removeFromParentAndCleanup(true);
	}
}
//更改兑换类型
void PopDialogBoxRecharge::onMenuChangeExchangeType(CCObject *object, TouchEventType type){
	if (type==TOUCH_EVENT_ENDED)
	{
		UIButton *pBTemp = (UIButton *)object;
		setRechargeType((RechargeType)pBTemp->getTag());
		switch (pBTemp->getTag())
		{
		case PopDialogBoxRecharge::RECHARGE_GOLD:
		{
			//pIVCursor->stopAllActions();
			//pIVCursor->runAction(CCMoveTo::create(0.1,ccp(-88,0)));
			//pLRechargeName->setText("金币");
			pBExchangeGold->setBright(false);
			pBExchangeGold->setTouchEnabled(false);

			pBExchangeBigGold->setBright(true);
			pBExchangeBigGold->setTouchEnabled(true);
			//更新列表
			updateListRecharge(vecRechargeGold);
		}
			break;
		case PopDialogBoxRecharge::RECHARGE_BIG_GOLD:
		{
			//pIVCursor->stopAllActions();
			//pIVCursor->runAction(CCMoveTo::create(0.1, ccp(88, 0)));
			//pLRechargeName->setText("元宝");
			pBExchangeGold->setBright(true);
			pBExchangeGold->setTouchEnabled(true);

			pBExchangeBigGold->setBright(false);
			pBExchangeBigGold->setTouchEnabled(false);
			//更新列表
			updateListRecharge(vecRechargeBigGold);
		}
			break;
		default:
			break;
		}
	}
}
//兑换按键
void PopDialogBoxRecharge::onMenuExchange(CCObject *object, TouchEventType type){
	if (type==TOUCH_EVENT_ENDED)
	{
		UIButton *pBTemp = (UIButton*)object;
		iCurSelectIndex = pBTemp->getTag();
		switch (eRechargeType)
		{
		case PopDialogBoxRecharge::RECHARGE_GOLD:
		{
			setRechargeActionType(R_Action_EXCHANGE);
			connectServer();
			connectSuccess();
		}
			break;
		case PopDialogBoxRecharge::RECHARGE_BIG_GOLD:
		{
			//CCString *sAction = CCString::createWithFormat("{\"act\":300 ,\"propName\":\"元宝\",\"propInfo\":\"%s\",\"propPice\":\"0.01\"}", sBigGoldExchangeNum[iCurSelectIndex].c_str());
			//platformAction(sAction->getCString());
			
			orderID = platformAction("{\"act\":800}");

			setRechargeActionType(R_Action_SEND_ORDER);
			connectServer();
			connectSuccess();
			CCLOG("orderID:%s <<%s>>",orderID.c_str(), __FUNCTION__);
			//CCLOG("orderID:%s <<%s>>",orderID, __FUNCTION__);
		}
			break;
		default:
			break;
		}
	}
}
//更新兑换列表
void PopDialogBoxRecharge::updateListRecharge(std::vector<RechargeData> qMsg){
	pLVRechargeList->removeAllItems();
	
	int tempSize = qMsg.size();
	if (tempSize == 0)
	{
		return;
	}

	for (int i = 0; i < tempSize; i++)
	{
		pLVRechargeList->insertDefaultItem(pLVRechargeList->getItems()->count());
		//背景框
		UIImageView *pIVItem = static_cast<UIImageView*>(pLVRechargeList->getItem(pLVRechargeList->getItems()->count() - 1)->getChildByName("ImageBg"));
		//兑换品图片
		UIImageView *pIVRechargeIcon = static_cast<UIImageView*>(pIVItem->getChildByName("ImageLight")->getChildByName("ImageRechargeIcon"));
		pIVRechargeIcon->loadTexture(qMsg[i].sImageIconName.c_str(), UI_TEX_TYPE_PLIST);
		
		//兑换按键
		UIButton *pButton = static_cast<UIButton*>(pIVItem->getChildByName("ButtonExchange"));
		pButton->addTouchEventListener(this, SEL_TouchEvent(&PopDialogBoxRecharge::onMenuExchange));
		pButton->setTag(i);
		
		//价格
		UILabel *pLPice = static_cast<UILabel*>(pButton->getChildByName("LabelExchangePice"));
		pLPice->setText(CCString::createWithFormat("%ld", qMsg[i].lExchangePice)->getCString());
		//兑换币种
		UIImageView *pPiceType = static_cast<UIImageView*>(pButton->getChildByName("ImageExchangeIcon"));
		if (eRechargeType == RECHARGE_GOLD)
		{
			pPiceType->loadTexture("u_a_icon_yb.png", UI_TEX_TYPE_PLIST);
		}
		else if (eRechargeType==RECHARGE_BIG_GOLD)
		{
			pPiceType->loadTexture("u_a_icon_rmb.png", UI_TEX_TYPE_PLIST);
		}
		
		//兑换数量		
		UILabel *pLContent0 = static_cast<UILabel*>(pIVItem->getChildByName("LabelGoldNum0"));
		pLContent0->setText(qMsg[i].sExchangeNum.c_str());
		UILabel *pLContent1 = static_cast<UILabel*>(pIVItem->getChildByName("LabelGoldNum1"));
		pLContent1->setText(qMsg[i].sExchangeNum.c_str());
		//加送数量
		UILabel *pLOtherNum = static_cast<UILabel*>(pIVItem->getChildByName("LabelOtherNum"));
		pLOtherNum->setText(qMsg[i].sOtherNum.c_str());
		
		
	}
}
//////////////////////////////////////////////////////////////////////////
void PopDialogBoxRecharge::update(float delta){
	if (isReadMessage)
	{
		//MessageQueue::update(delta);
	}
	gameSocket.updateSocketData(delta);
}
//读取网络消息回调
void PopDialogBoxRecharge::onEventReadMessage(WORD wMainCmdID, WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_MB_SOCKET://socket连接成功
	{
		//connectSuccess();
	}
	break;
	case MDM_GP_USER_SERVICE://用户服务
	{
		onEventUserService(wSubCmdID, pDataBuffer, wDataSize);
		//移除loading
		this->getChildByTag(TAG_LOADING)->removeFromParentAndCleanup(true);
		//关闭网络
		//TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_RECHARGE);
		gameSocket.Destroy(true);
	}
	break;
	default:
		CCLOG("other:%d   %d<<%s>>", wMainCmdID, wSubCmdID, __FUNCTION__);
		break;
	}
}
//连接成功
void PopDialogBoxRecharge::connectSuccess(){
	switch (eRechargeActionType)
	{
	case R_Action_GET_MONEY:	//获取财富
	{
		CMD_GP_UserID userID;
		userID.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

		MD5 m;
		m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
		std::string md5PassWord = m.GetMd5();
		strcpy(userID.szPassword, md5PassWord.c_str());

		gameSocket.SendData(MDM_GP_USER_SERVICE, SUB_GP_TREASURE, &userID, sizeof(CMD_GP_UserID));
	}
	break;
	case R_Action_EXCHANGE:
	{
		CMD_GP_UserExchangeIngot userExchange;
		userExchange.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;
		userExchange.dwIngot = lGoldPice[iCurSelectIndex];

		MD5 m;
		m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
		std::string md5PassWord = m.GetMd5();
		strcpy(userExchange.szPassword, md5PassWord.c_str());


		strcpy(userExchange.szMachineID, platformAction("{\"act\":100}").c_str());

		gameSocket.SendData(MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE_INGOT, &userExchange, sizeof(userExchange));
	}
		break;
	case R_Action_SEND_ORDER://发送订单号
	{
		CMD_GP_RechargeOrder rOrder;
		rOrder.dwFirst = 10;
		rOrder.dwOpTerminal = 2;
		rOrder.dwOrderAmount = lBigGoldPice[iCurSelectIndex];
		rOrder.dwShareID = 7;
		
		strcpy(rOrder.szAccounts, DataModel::sharedDataModel()->sLogonAccount.c_str());
		MD5 m;
		m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
		std::string md5PassWord = m.GetMd5();
		strcpy(rOrder.szLogonPass, md5PassWord.c_str());

		strcpy(rOrder.szRechargeOrder, orderID.c_str());
		
		CCLOG("orderID:%s <<%s>>", orderID.c_str(), __FUNCTION__);
		gameSocket.SendData(MDM_GP_USER_SERVICE, SUB_GP_RECHARGE_ORDER, &rOrder, sizeof(rOrder));
		/*CMD_GP_UserExchangeIngot userExchange;
		userExchange.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;
		userExchange.dwIngot = lGoldPice[iCurSelectIndex];

		MD5 m;
		m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
		std::string md5PassWord = m.GetMd5();
		strcpy(userExchange.szPassword, md5PassWord.c_str());


		strcpy(userExchange.szMachineID, platformAction("{\"act\":100}").c_str());

		gameSocket.SendData(MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE_INGOT, &userExchange, sizeof(userExchange));*/
	}
	break;
	default:
		break;
	}
}
//用户服务
void PopDialogBoxRecharge::onEventUserService(WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GP_TREASURE://财富详细 
	{
		onSubTreasure(pDataBuffer, wDataSize);
	}
	break;
	case SUB_GP_EXCHANGE_INGOT://兑换元宝
	{
		onSubExchangeBigGold(pDataBuffer, wDataSize);
	}
		break;
	case SUB_GP_OPERATE_FAILURE:
	{
		CMD_GP_OperateFailure *pFailure = (CMD_GP_OperateFailure*)pDataBuffer;
		//showTipInfo(GBKToUTF8(pFailure->szDescribeString));
		PopDialogBoxTipInfo *pTipInfo = PopDialogBoxTipInfo::create();
		this->addChild(pTipInfo, 10);
		pTipInfo->setTipInfoContent(GBKToUTF8(pFailure->szDescribeString).c_str());
		pTipInfo->setIPopAssistTipInfo(this);

		pTipInfo->pBClose->loadTextureNormal("QianWangChongZhi_Btn_Normal.png", UI_TEX_TYPE_PLIST);
		pTipInfo->pBClose->loadTexturePressed("QianWangChongZhi_Btn_Down.png", UI_TEX_TYPE_PLIST);
		//pTipInfo->pBClose->setTitleText(" 前往充值 ");
	}
	break;
	case SUB_GP_RECHARGE_ORDER:
	{
		CMD_GP_RechargeOrderLog *rOrderLog = (CMD_GP_RechargeOrderLog*)pDataBuffer;
		if (rOrderLog->dwRet==0)
		{
			CCString *sAction = CCString::createWithFormat("{\"act\":300 ,\"propName\":\"%s\",\"propInfo\":\"%s\",\"propPice\":\"%ld\"}", "元宝",sBigGoldExchangeNum[iCurSelectIndex].c_str(), lBigGoldPice[iCurSelectIndex]);
			platformAction(sAction->getCString());
		}
		else
		{
			showTipInfo(GBKToUTF8(rOrderLog->szDescribeString).c_str());
		}
	}
		break;
	default:
		CCLOG("   %d<<%s>>", wSubCmdID, __FUNCTION__);
		break;
	}
}
//财富
void PopDialogBoxRecharge::onSubTreasure(void * pDataBuffer, unsigned short wDataSize){
	if (wDataSize != sizeof(CMD_GP_UserTreasure)) return;
	CMD_GP_UserTreasure * pUserTreasure = (CMD_GP_UserTreasure *)pDataBuffer;
	pLCurGoldCount->setText(CCString::createWithFormat("%lld", pUserTreasure->lInsureScore)->getCString());
	pLCurBigGoldCount->setText(CCString::createWithFormat("%lld", pUserTreasure->lIngotScore)->getCString());
	//pLVoucherCount->setText(CCString::createWithFormat("%lld", pUserTreasure->lLottery)->getCString());
}
//兑换元宝
void PopDialogBoxRecharge::onSubExchangeBigGold(void * pDataBuffer, unsigned short wDataSize){
	if (wDataSize > sizeof(CMD_GP_ExchangeIngotSuccess)) return;
	CMD_GP_ExchangeIngotSuccess *pExchangeSuccess = (CMD_GP_ExchangeIngotSuccess*)pDataBuffer;

	pLCurGoldCount->setText(CCString::createWithFormat("%lld", pExchangeSuccess->lInsure)->getCString());
	pLCurBigGoldCount->setText(CCString::createWithFormat("%lld", pExchangeSuccess->lIngot)->getCString());

	showTipInfo(GBKToUTF8(pExchangeSuccess->szDescribeString).c_str());

}
//关闭回调
void PopDialogBoxRecharge::onCloseTipInfo(CCLayer *pTipInfo){
	pTipInfo->removeFromParentAndCleanup(true);

	UIButton *pBExchangeBigGold = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonBigGold"));
	onMenuChangeExchangeType(pBExchangeBigGold, TOUCH_EVENT_ENDED);
}
//付费成功回调
void PopDialogBoxRecharge::onUpdateDelta(CCObject *obj){
	pLCurGoldCount->setText(CCString::createWithFormat("%lld", DataModel::sharedDataModel()->userInfo->lScore)->getCString());
	pLCurBigGoldCount->setText(CCString::createWithFormat("%lld", DataModel::sharedDataModel()->userInfo->lIngot)->getCString());
}