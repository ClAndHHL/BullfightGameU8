//
//  LobbyHornLayer.cpp
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//
#include "LobbyHornLayer.h"
#include "../Tools/DataModel.h"
#include "GameLobbyScene.h"
#include "../MTNotificationQueue/LobbyMsgHandler.h"
#include "../Network/SEvent.h"
LobbyHornLayer::LobbyHornLayer()
{
}
LobbyHornLayer::~LobbyHornLayer(){
	CCLOG("~ <<%s>>", __FUNCTION__);
}
void LobbyHornLayer::onEnter(){
	CCLayer::onEnter();
	UILayer *m_pWidget = UILayer::create();
	this->addChild(m_pWidget);


	UILayout *pWidget = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UILobbyHorn.ExportJson)));
	m_pWidget->addWidget(pWidget);

	pIVMsg  = static_cast<UIImageView*>(m_pWidget->getWidgetByName("ImageMsg"));
	//延时0.2秒弹出消息框
	CCCallFunc *call = CCCallFunc::create(this, SEL_CallFunc(&LobbyHornLayer::onShowMsg));
	CCSequence *seq = CCSequence::create(CCDelayTime::create(0.2), call, NULL);
	pIVMsg->runAction(seq);
	//输入框
	pTFHornMsgContent = static_cast<UITextField*>(m_pWidget->getWidgetByName("TextFieldInput"));
	addEditBox(pTFHornMsgContent, kEditBoxInputModeSingleLine);
	//发送消息按键
	UIImageView *pIVSendMsg = static_cast<UIImageView*>(m_pWidget->getWidgetByName("ImageSendHornMsg"));
	pIVSendMsg->addTouchEventListener(this, SEL_TouchEvent(&LobbyHornLayer::onMenuSendMsg));;
	//喇叭消息滚动容器
	pSVMsg = static_cast<UIScrollView*>(m_pWidget->getWidgetByName("ScrollViewMsg"));
	setScrollViewData();
	//重设置下，否则会被覆盖事件
	this->setTouchEnabled(true);
	this->setTouchPriority(0);
	this->setTouchMode(kCCTouchesOneByOne);

	//添加监听事件
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(LobbyHornLayer::onUpdateHornMsg),UPDATE_HORN_MSG, NULL);
}
void LobbyHornLayer::onExit(){
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, UPDATE_HORN_MSG);
	CCLayer::onExit();
}
//更新喇叭消息
void LobbyHornLayer::onUpdateHornMsg(CCObject *obj){
	setScrollViewData();
}
//菜单发送消息
void LobbyHornLayer::onMenuSendMsg(CCObject* pSender, TouchEventType type){
	if (type == TOUCH_EVENT_ENDED)
	{
		CCEditBox *pEBInputMsgContent = (CCEditBox*)pTFHornMsgContent->getNodeByTag(TAG_INPUT_EDIT_BOX);
		if (strcmp(pEBInputMsgContent->getText(), "") == 0)
		{
			showTipInfo("内容不能为空");
		}
		else
		{
			if (DataModel::sharedDataModel()->userInfo->dwHornCount<=0)
			{
				PopDialogBoxOptTipInfo *pUTipInfo = PopDialogBoxOptTipInfo::create();
				this->addChild(pUTipInfo, 100);
				pUTipInfo->setIHornMsgAssist(this);
				pUTipInfo->setTipInfoData("您可发送的喇叭数为0，继续发送将会从保险柜中扣除100万币，是否继续发送？");
			}
			else
			{
				sendHornMsg(UTF8ToGBK(pEBInputMsgContent->getText()));
			}
		}
	}
}
//发送确定回调
void LobbyHornLayer::onSendSure(){
	CCEditBox *pEBInputMsgContent = (CCEditBox*)pTFHornMsgContent->getNodeByTag(TAG_INPUT_EDIT_BOX);
	sendHornMsg(UTF8ToGBK(pEBInputMsgContent->getText()));
}
//发送消息
void LobbyHornLayer::sendHornMsg(std::string content){
	LobbyMsgHandler::sharedLobbyMsgHandler()->setIHornMsgAssist(this);
	CMD_GL_Laba HornMsg;
	HornMsg.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;
	strcpy(HornMsg.szNickName, DataModel::sharedDataModel()->userInfo->szNickName);
	HornMsg.dwKindID = 0;
	HornMsg.dwServerID = 0;
	HornMsg.dwPropNum = 0;
	strcpy(HornMsg.szLabaText,content.c_str());
	LobbyMsgHandler::sharedLobbyMsgHandler()->gameSocket.SendData(MDM_GL_C_DATA, SUB_GL_C_LABA, &HornMsg, sizeof(HornMsg));
}
bool LobbyHornLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
	CCPoint touchPoint = this->convertTouchToNodeSpace(pTouch);
	CCRect rect = CCRect(pIVMsg->getPositionX() - pIVMsg->getSize().width/2, pIVMsg->getPositionY(), pIVMsg->getSize().width, pIVMsg->getSize().height);
	if (!rect.containsPoint(touchPoint))
	{
		CCEaseExponentialIn  *out = CCEaseExponentialIn::create(CCMoveTo::create(0.2, ccp(pIVMsg->getPositionX(), -pIVMsg->getSize().height)));
		CCCallFunc *call = CCCallFunc::create(this, SEL_CallFunc(&LobbyHornLayer::removSelf));
		CCSequence *seq = CCSequence::create(out, call, NULL);
		pIVMsg->runAction(seq);
	}
	else
	{
		return false;
	}
	return true;
}
//移除自己
void LobbyHornLayer::removSelf(){
	((GameLobbyScene*)this->getParent())->showHorn();
	this->removeFromParentAndCleanup(true);
}
//显示消息框
void LobbyHornLayer::onShowMsg(){
	pIVMsg->stopAllActions();
	float moveY = DataModel::sharedDataModel()->deviceSize.height*0.02;
	CCEaseExponentialOut  *out = CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(pIVMsg->getPositionX(),moveY)));
	CCSequence *seq = CCSequence::create(out, NULL);
	pIVMsg->runAction(seq);
}
//隐藏消息框
void LobbyHornLayer::hideMsg(){
}
//设置滚动容器数据
void LobbyHornLayer::setScrollViewData(){
	pSVMsg->removeAllChildrenWithCleanup(true);
	int height = 0;
	std::list <std::string> ::iterator iter;
	for (iter = DataModel::sharedDataModel()->listHornMsg.begin(); iter != DataModel::sharedDataModel()->listHornMsg.end(); iter++)
	{
		UILabel *pLContent = createMsgContent(iter->c_str());
		height += pLContent->getSize().height;
		pLContent->setPosition(ccp(0, height));
		pSVMsg->addChild(pLContent);
		pSVMsg->setInnerContainerSize(CCSize(pLContent->getContentSize().width, height));
		
		UIImageView *pIVHornLine = UIImageView::create();
		pIVHornLine->loadTexture("u_line-between.png", UI_TEX_TYPE_PLIST);
		pIVHornLine->setScaleX(pSVMsg->getSize().width);
		height += pIVHornLine->getContentSize().height;
		pIVHornLine->setPosition(ccp(0,height));
		pSVMsg->addChild(pIVHornLine);
	} 
	pSVMsg->scrollToBottom(0.2, false);
}
//创建消息内容
UILabel *LobbyHornLayer::createMsgContent(std::string content){
	UILabel *pLMsgContent = UILabel::create();
	pLMsgContent->setFontSize(20);
	pLMsgContent->ignoreContentAdaptWithSize(true);
	pLMsgContent->setTextAreaSize(CCSize(pSVMsg->getSize().width, 0));
	pLMsgContent->setText(content.c_str());
	return pLMsgContent;
}
//添加EditBox
void LobbyHornLayer::addEditBox(UITextField *pTextField, EditBoxInputMode eInputMode){
	pTextField->setTouchEnabled(false);
	//CCEditBox* m_pEditName = CCEditBox::create(pTextField->getContentSize(), CCScale9Sprite::createWithSpriteFrameName("u_info_change_bg.png"));
	CCEditBox* pEditBox = CCEditBox::create(pTextField->getSize(), CCScale9Sprite::create("res/u_input_horn.png"));
	pEditBox->setPosition(CCPointZero);
	pEditBox->setFontSize(pTextField->getFontSize());
	pEditBox->setFontColor(pTextField->getColor());//设置文字颜色


	pEditBox->setPlaceHolder(pTextField->getPlaceHolder());//占位符
	pEditBox->setPlaceholderFontSize(pTextField->getFontSize());

	if (pTextField->getMaxLength() > 0) {
		pEditBox->setMaxLength(pTextField->getMaxLength());//最大长度
	}

	if (pTextField->isPasswordEnabled())
	{
		pEditBox->setInputFlag(kEditBoxInputFlagPassword);//设置为密码模式
	}
	pEditBox->setInputMode(eInputMode);//设置键盘模式
	pEditBox->setTouchPriority(0);
	pEditBox->setAnchorPoint(pTextField->getAnchorPoint());
	pEditBox->setTag(TAG_INPUT_EDIT_BOX);
	pEditBox->setText(pTextField->getStringValue());
	pEditBox->setReturnType(kKeyboardReturnTypeDefault);//键盘回车键名字
	pEditBox->setDelegate(this);
	//pEditBox->setColor(ccc3(243,218,185));
	//pEditBox->setOpacity(0);
	pTextField->addNode(pEditBox, 10);
}
void LobbyHornLayer::editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox)
{
	CCLOG("editBox %p DidBegin !", editBox);
}

void LobbyHornLayer::editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox)
{
	CCLOG("editBox %p DidEnd !", editBox);
}

void LobbyHornLayer::editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text)
{
	CCLOG("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void LobbyHornLayer::editBoxReturn(CCEditBox* editBox)
{
	CCLOG("editBox %p was returned !", editBox);
}
////显示提示语
void LobbyHornLayer::showTipInfo(const char* sInfo){
	PopDialogBoxTipInfo *pTipInfo = PopDialogBoxTipInfo::create();
	this->addChild(pTipInfo, 10);
	pTipInfo->setTipInfoContent(sInfo);
}
/*//显示提示语
void LobbyHornLayer::showTipInfo(const char* sInfo, IPopAssistTipInfo *pITipInfo, int eButtonType){
	PopDialogBoxTipInfo *pTipInfo = PopDialogBoxTipInfo::create();
	this->addChild(pTipInfo, 10);
	pTipInfo->setTipButtonContent((PopDialogBoxTipInfo::TipButtonType)eButtonType);
	pTipInfo->setIPopAssistTipInfo(pITipInfo);
	pTipInfo->setTipInfoContent(sInfo);
}*/
//发送失败
void LobbyHornLayer::onSendFail(std::string content){
	showTipInfo(content.c_str());
}