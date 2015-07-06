/*
 * PopDialogBoxUserInfo.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#include "PopDialogBoxUserInfo.h"
#include "../Tools/DataModel.h"
#include "../Tools/GameConfig.h"
#include "../Tools/BaseAttributes.h"
#include "../Tools/Tools.h"

#include "../Network/MD5/MD5.h"
//////////////////////////////////////////////////////////////////////////
PopDialogBoxUserInfo::PopDialogBoxUserInfo()
:isShowChange(false)
, eUserInfoType(USER_GET_MONEY)
{
	scheduleUpdate();
}
PopDialogBoxUserInfo::~PopDialogBoxUserInfo() {
	CCLog("~ <<%s>>",__FUNCTION__);
	unscheduleUpdate();
	TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(SOCKET_USER_INFO);
}
void PopDialogBoxUserInfo::onEnter(){
	CCLayer::onEnter();
	Layout* layoutPauseUI = static_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIPopDialogBoxUserInfo.ExportJson)));
	pUILayer->addWidget(layoutPauseUI);
	
    
	pWidgetBg = static_cast<UIImageView*>(pUILayer->getWidgetByName("bg"));
	pWidgetBg->setScale(0.8);

	UIButton *backButton = static_cast<UIButton*>(pUILayer->getWidgetByName("buttonClose"));
	backButton->addTouchEventListener(this, toucheventselector(PopDialogBox::onMenuBackWithReadMsg));
	//绑定手机
	pBBindingPhone = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonBindPhone"));
	pBBindingPhone->addTouchEventListener(this, toucheventselector(PopDialogBoxUserInfo::onMenuBindingPhone));
	resetBindingButton();
	
	//设置游戏ID
	UILabel *labelUserID=static_cast<UILabel*>(pUILayer->getWidgetByName("LabelUserID"));
	labelUserID->setText(CCString::createWithFormat("ID:%ld",DataModel::sharedDataModel()->userInfo->dwGameID)->getCString());
	//昵称输入框
	pLabelNickName=static_cast<UITextField*>(pUILayer->getWidgetByName("TextFieldNickName"));
	pLabelNickName->setText(Tools::GBKToUTF8(DataModel::sharedDataModel()->userInfo->szNickName));
	pLabelNickName->setTouchEnabled(false);
	//性别选择
	pcbSexGirl=static_cast<UICheckBox*>(pUILayer->getWidgetByName("CheckBoxSexGirl"));
	pcbSexGirl->addEventListenerCheckBox(this,SEL_SelectedStateEvent(&PopDialogBoxUserInfo::onCheckBoxSelectedStateEvent));
	plSexBoyInfo=static_cast<UILabel*>(pUILayer->getWidgetByName("LabelSexBoyInfo"));

	pcbSexBoy=static_cast<UICheckBox*>(pUILayer->getWidgetByName("CheckBoxSexBoy"));
	pcbSexBoy->addEventListenerCheckBox(this,SEL_SelectedStateEvent(&PopDialogBoxUserInfo::onCheckBoxSelectedStateEvent));
	//金币数
	pLGoldCount= static_cast<UILabel*>(pUILayer->getWidgetByName("LabelGoldCount"));
	//元宝数
	pLBigGoldCount = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelBigGoldCount"));
	//奖券数
	pLVoucherCount = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelVoucherCount"));
	//修改按键
	UIButton *bChange=static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonChange"));
	bChange->addTouchEventListener(this, SEL_TouchEvent(&PopDialogBoxUserInfo::menuChange));
	//修改昵称时背景
	piNickNameBg=static_cast<UIImageView*>(pUILayer->getWidgetByName("ImageChangeBg"));

	piSexIcon=static_cast<UIImageView*>(pUILayer->getWidgetByName("ImageSexIcon"));

	ppSexInfo=static_cast<UIPanel*>(pUILayer->getWidgetByName("PanelSexInfo"));
	ppSexSelect=static_cast<UIPanel*>(pUILayer->getWidgetByName("PanelSexSelect"));
	updateSex();
	setShowChangeView();
	//设置大厅不读取数据
	setLobbyReadMessage(false);
	//连接服务
	connectServer(SOCKET_USER_INFO);

	playAnimation();
}
void PopDialogBoxUserInfo::onExit(){
	CCLayer::onExit();
}
//重设绑定按键
void PopDialogBoxUserInfo::resetBindingButton(){
	UIImageView *pIVBindingFinish = static_cast<UIImageView*>(pUILayer->getWidgetByName("ImageBinding"));
	if (DataModel::sharedDataModel()->sPhone.length() > 0)
	{
		pBBindingPhone->setEnabled(false);
		pIVBindingFinish->setVisible(true);
	}
	else
	{
		pIVBindingFinish->setVisible(false);
	}
}
//关闭绑定手机回调
void PopDialogBoxUserInfo::onCloseBindingPhone(){
	isReadMessage = true;
	resetBindingButton();
}
void PopDialogBoxUserInfo::menuChange(CCObject *object, TouchEventType type){
	if (type==TOUCH_EVENT_ENDED)
	{
		isShowChange=!isShowChange;
		if (!isShowChange)
		{
			DataModel::sharedDataModel()->userInfo->cbGender=pcbSexBoy->getSelectedState()?1:2;
		}
		setShowChangeView();
	}
}
//绑定手机按键
void PopDialogBoxUserInfo::onMenuBindingPhone(CCObject *object, TouchEventType type){
	if (type == TOUCH_EVENT_ENDED)
	{
		isReadMessage = false;
		PopDialogBoxBindingPhone *pPopBoxBinding = PopDialogBoxBindingPhone::create();
		pPopBoxBinding->setIPopAssistBindingPhone(this);
		this->addChild(pPopBoxBinding);
	}
}
//设置显示修改窗口
void PopDialogBoxUserInfo::setShowChangeView(){
	
	if (isShowChange)
	{
		pLabelNickName->setTouchEnabled(true);
		pLabelNickName->setColor(ccc3(118,65,20));
		piNickNameBg->setVisible(true);


		ppSexInfo->setEnabled(false);
		ppSexSelect->setEnabled(true);
		ppSexSelect->setVisible(true);
	}else
	{
		pLabelNickName->setTouchEnabled(false);
		pLabelNickName->setColor(ccc3(255,255,255));
		piNickNameBg->setVisible(false);

		ppSexInfo->setEnabled(true);
		ppSexSelect->setEnabled(false);

		
	}
	updateSex();
}
void PopDialogBoxUserInfo::updateSex(){
	if (DataModel::sharedDataModel()->userInfo->cbGender==1)
	{
		piSexIcon->loadTexture("u_info_icon_boy.png",UI_TEX_TYPE_PLIST);
		plSexBoyInfo->setText(BaseAttributes::sharedAttributes()->sSexBoyName);
		if (isShowChange)
		{
			pcbSexBoy->setSelectedState(true);
			pcbSexGirl->setSelectedState(false);
			pcbSexBoy->setTouchEnabled(false);
		}
	}else
	{
		piSexIcon->loadTexture("u_info_icon_girl.png",UI_TEX_TYPE_PLIST);
		plSexBoyInfo->setText(BaseAttributes::sharedAttributes()->sSexGirlName);
		if (isShowChange)
		{
			pcbSexBoy->setSelectedState(false);
			pcbSexGirl->setSelectedState(true);
			pcbSexGirl->setTouchEnabled(false);
		}
		
	}
}
void PopDialogBoxUserInfo::onCheckBoxSelectedStateEvent(CCObject *pSender, CheckBoxEventType type){
	switch (type)
	{
	case CHECKBOX_STATE_EVENT_SELECTED:
		{
			UICheckBox *box=(UICheckBox*)pSender;
			box->setTouchEnabled(false);
			if (box->getTag()==1)
			{
				pcbSexGirl->setTouchEnabled(true);
				pcbSexGirl->setSelectedState(false);
			}else{
				pcbSexBoy->setTouchEnabled(true);
				pcbSexBoy->setSelectedState(false);
			}
		}
		break;
	case CHECKBOX_STATE_EVENT_UNSELECTED:
		{

		}
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void PopDialogBoxUserInfo::update(float delta){
	if (isReadMessage)
	{
		MessageQueue::update(delta);
	}
}
//读取网络消息回调
void PopDialogBoxUserInfo::onEventReadMessage(WORD wMainCmdID, WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_MB_SOCKET://socket连接成功
	{
		connectSuccess();
	}
	break;
	case MDM_GP_USER_SERVICE://用户服务
	{
		onEventUserService(wSubCmdID, pDataBuffer, wDataSize);
		//移除loading
		this->getChildByTag(TAG_LOADING)->removeFromParentAndCleanup(true);
		//关闭网络
		TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_USER_INFO);
	}
	break;
	default:
		CCLog("other:%d   %d<<%s>>", wMainCmdID, wSubCmdID, __FUNCTION__);
		break;
	}
}
//连接成功
void PopDialogBoxUserInfo::connectSuccess(){
	switch (eUserInfoType)
	{
	case PopDialogBoxUserInfo::USER_GET_MONEY:	//获取财富
	{
		CMD_GP_UserID userID;
		userID.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

		MD5 m;
		m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
		std::string md5PassWord = m.GetMd5();
		strcpy(userID.szPassword, md5PassWord.c_str());

		getSocket()->SendData(MDM_GP_USER_SERVICE, SUB_GP_TREASURE, &userID, sizeof(CMD_GP_UserID));
	}
		break;
	default:
		break;
	}
}
//用户服务
void PopDialogBoxUserInfo::onEventUserService(WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GP_TREASURE://财富详细 
	{
		onSubTreasure(pDataBuffer, wDataSize);
	}
	break;
	default:
		CCLog("   %d<<%s>>", wSubCmdID, __FUNCTION__);
		break;
	}
}
//财富
void PopDialogBoxUserInfo::onSubTreasure(void * pDataBuffer, unsigned short wDataSize){
	if (wDataSize != sizeof(CMD_GP_UserTreasure)) return;
	CMD_GP_UserTreasure * pUserTreasure = (CMD_GP_UserTreasure *)pDataBuffer;
	pLGoldCount->setText(CCString::createWithFormat("%lld",DataModel::sharedDataModel()->userInfo->lScore)->getCString());
	pLBigGoldCount->setText(CCString::createWithFormat("%lld", pUserTreasure->lIngotScore)->getCString());
	pLVoucherCount->setText(CCString::createWithFormat("%lld",pUserTreasure->lLottery)->getCString());
}