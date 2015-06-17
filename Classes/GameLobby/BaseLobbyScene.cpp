//
//  BaseLobbyScene.cpp
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//

#include "BaseLobbyScene.h"
#include "../Tools/Tools.h"
#include "../Tools/BaseAttributes.h"
#include "../PopDialogBox/PopDialogBoxUserInfo.h"
#include "../PopDialogBox/PopDialogBoxSetUp.h"
#include "../PopDialogBox/PopDialogBoxMore.h"
#include "../PopDialogBox/PopDialogBoxActivity.h"
#include "../PopDialogBox/PopDialogBoxTask.h"
#include "../PopDialogBox/PopDialogBoxBank.h"
#include "../PopDialogBox/PopDialogBoxShop.h"
#include "../PopDialogBox/PopDialogBoxAuction.h"
#include "../PopDialogBox/PopDialogBoxRanking.h"
#include "../PopDialogBox/PopDialogBoxTipInfo.h"
#include "../PopDialogBox/PopDialogBoxMsg.h"
#include "../PopDialogBox/PopDialogBoxVip.h"

#include "../Tools/DataModel.h"
BaseLobbyScene::BaseLobbyScene()
	:isReadMessage(true){
	
}

BaseLobbyScene::~BaseLobbyScene(){
	CCLog("~ <<%s>>", __FUNCTION__);
}
//进入场景
void BaseLobbyScene::onEnter(){
	CCLayer::onEnter();
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("res/cloud.plist");
	//添加背景
	CCSize deviceSize=DataModel::sharedDataModel()->deviceSize;
	CCSprite *spriteBg=CCSprite::create("res/main_bg.jpg");
	this->addChild(spriteBg,-1);
	spriteBg->setPosition(ccp(deviceSize.width/2,deviceSize.height/2));
	float scale=deviceSize.height/spriteBg->getContentSize().height;
	spriteBg->setScale(scale);
	//////////////////////////////////////////////////////////////////////////
	//随机生成云
	for (int i = 0; i < 5; i++)
	{
		createCloudRandom(spriteBg);
	}
	//////////////////////////////////////////////////////////////////////////
	//粒子
	CCParticleSystemQuad *emitter1 = CCParticleSystemQuad::create("particle/sakura.plist");
	emitter1->setPosition(ccp(deviceSize.width+20, deviceSize.height+20));    // 设置发射粒子的位置
	//emitter1->setAutoRemoveOnFinish(true);                          // 完成后制动移除
	//emitter1->setDuration(10);                                      // 设置粒子系统的持续时间秒
	spriteBg->addChild(emitter1, 1);
	//////////////////////////////////////////////////////////////////////////
	//门
	CCSprite *pDoor = CCSprite::create("res/main_door.png");
	spriteBg->addChild(pDoor, 2);
	pDoor->setAnchorPoint(ccp(1,1));
	pDoor->setPosition(ccp(SCENE_SIZE.width, SCENE_SIZE.height));

	CCSprite *pDoor1 = CCSprite::create("res/main_door.png");
	spriteBg->addChild(pDoor1, 2);
	pDoor1->setAnchorPoint(ccp(1, 1));
	pDoor1->setPosition(ccp(0, SCENE_SIZE.height));
	pDoor1->setScaleX(-1);
	//创建UI层
	UILayer *m_pWidget = UILayer::create();
	this->addChild(m_pWidget);
	//加载UI层
	UILayout *pWidget = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIBaseLobby.ExportJson)));
	m_pWidget->addWidget(pWidget);
	//绑定设置按键
	UIButton* button  = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonSetUp"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定用户信息按键
	pBUserInfo= static_cast<UIButton*>(m_pWidget->getWidgetByName("buttonUser"));
	pBUserInfo->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::menuResetUser));
	//绑定更多按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonMore"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定活动按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonActivity"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定任务按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonTask"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定银行按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonBank"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定商店按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonShop"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定拍卖按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonAuction"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定排行按键
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonRanking"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定消息
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonMsg"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定背包
	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonKnapsack"));
	button->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuCallback));
	//绑定VIP
	UIImageView *pIVip = static_cast<UIImageView*>(m_pWidget->getWidgetByName("ImageVip"));
	pIVip->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuVip));
	
	//用户名
	userName=static_cast<UILabel*>(m_pWidget->getWidgetByName("labelUserName"));
	//金币
	pLabelGoldCount=static_cast<UILabel*>(m_pWidget->getWidgetByName("LabelGoldCount"));
	pLabelGoldCount->setText(CCString::createWithFormat("%lld",DataModel::sharedDataModel()->userInfo->lScore)->getCString());
}
//退出场景
void BaseLobbyScene::onExit(){
	CCLayer::onExit();
}
//弹出框
void BaseLobbyScene::popDialogBox(PopType type){
	PopDialogBox *pdb = NULL;
	switch (type)
	{
	case BaseLobbyScene::POP_USER_INFO:
		pdb=PopDialogBoxUserInfo::create();
		break;
	case BaseLobbyScene::POP_SETUP:
		pdb=PopDialogBoxSetUp::create();
		break;
	case BaseLobbyScene::POP_MORE:
		pdb=PopDialogBoxMore::create();
		break;
	case BaseLobbyScene::POP_ACTIVITY:
		pdb=PopDialogBoxActivity::create();
		break;
	case BaseLobbyScene::POP_TASK:
		pdb=PopDialogBoxTask::create();
		break;
	case BaseLobbyScene::POP_BANK:
		pdb=PopDialogBoxBank::create();
		break;
	case BaseLobbyScene::POP_SHOP:
		pdb = PopDialogBoxShop::create();
		break;
	case BaseLobbyScene::POP_AUCTION:
		pdb=PopDialogBoxAuction::create();
		break;
	case BaseLobbyScene::POP_RANKING:
		pdb=PopDialogBoxRanking::create();
		break;
	case BaseLobbyScene::POP_MSG:
		pdb = PopDialogBoxMsg::create();
		break;
	case BaseLobbyScene::POP_VIP:
	{
		pdb = PopDialogBoxVip::create();
		((PopDialogBoxVip *)pdb)->setIPopAssistVip(this);
	}
		
		break;
	case BaseLobbyScene::POP_KNAPSACK:
	{
		isReadMessage = false;
		pdb = PopDialogBoxKnapsack::create();
		((PopDialogBoxKnapsack *)pdb)->setIPopAssistKnapsack(this);
	}
		break;
	default:
		break;
	}
	this->addChild(pdb);
}
//菜单回调
void BaseLobbyScene::onMenuCallback(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			UIButton *button=(UIButton*)pSender;
			if(strcmp(button->getName(),"ButtonSetUp")==0)
			{
				popDialogBox(POP_SETUP);
			}
			else if(strcmp(button->getName(),"ButtonMore")==0)
			{
				popDialogBox(POP_MORE);
			}
			else if(strcmp(button->getName(),"ButtonActivity")==0)
			{
				popDialogBox(POP_ACTIVITY);
			}
			else if(strcmp(button->getName(),"ButtonTask")==0)
			{
				popDialogBox(POP_TASK);
			}
			else if(strcmp(button->getName(),"ButtonTask")==0)
			{
				popDialogBox(POP_TASK);
			}
			else if(strcmp(button->getName(),"ButtonBank")==0)
			{
				popDialogBox(POP_BANK);
			}
			else if(strcmp(button->getName(),"ButtonShop")==0)
			{
				popDialogBox(POP_SHOP);
			}
			else if(strcmp(button->getName(),"ButtonAuction")==0)
			{
				popDialogBox(POP_AUCTION);
			}
			else if(strcmp(button->getName(),"ButtonRanking")==0)
			{
				popDialogBox(POP_RANKING);
			}
			else if (strcmp(button->getName(), "ButtonMsg") == 0)
			{
				popDialogBox(POP_MSG);
			}
			else if (strcmp(button->getName(), "ButtonKnapsack") == 0)
			{
				popDialogBox(POP_KNAPSACK);
			}
			else
			{
				PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
				this->addChild(tipInfo);
				tipInfo->setTipInfoContent(BaseAttributes::sharedAttributes()->sWaitCodeing.c_str());
			}
		}
		break;
	default:
		break;
	}
}
//用户信息回调
void BaseLobbyScene::menuResetUser(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		popDialogBox(POP_USER_INFO);
		break;
	default:
		break;
	}
}
//VIP
void BaseLobbyScene::onMenuVip(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		popDialogBox(POP_VIP);
		break;
	default:
		break;
	}
}
//关闭背包回调
void BaseLobbyScene::onCloseKnapsack(){
	isReadMessage = true;
}
//关闭VIP回调
void BaseLobbyScene::onCloseVipToShop(){
	popDialogBox(POP_SHOP);
}
//随机生成云
void BaseLobbyScene::createCloudRandom(CCSprite *pBg){
	int indexCloud = abs(rand() % 2);
	CCSprite *pCloud = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("cloud_%d.png",indexCloud)->getCString());
	pBg->addChild(pCloud);

	int x = abs(rand() % (int)DataModel::sharedDataModel()->deviceSize.width);
	int y = DataModel::sharedDataModel()->deviceSize.height-80 - (abs(rand() % 80));
	pCloud->setPosition(ccp(x,y));

	int speed = abs(rand() % 12) + 6;
	int time = pCloud->getPositionX() / speed;
	pCloud->runAction(CCSequence::create(
		CCMoveTo::create(time,ccp(-pCloud->getContentSize().width/2,pCloud->getPositionY())),
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLobbyScene::onMoveFinsh)),
		NULL));
}
//
void BaseLobbyScene::onMoveFinsh(CCNode *node){
	node->stopAllActions();

	int indexCloud = abs(rand() % 2);
	CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("cloud_%d.png", indexCloud)->getCString());
	((CCSprite*)node)->setDisplayFrame(frame);

	node->setPositionX(node->getContentSize().width/2+DataModel::sharedDataModel()->deviceSize.width);
	node->setPositionY(DataModel::sharedDataModel()->deviceSize.height - 80 - (abs(rand() % 80)));

	int speed = abs(rand() % 12) + 6;
	int time = node->getPositionX() / speed;
	node->runAction(CCSequence::create(
		CCMoveTo::create(time, ccp(-node->getContentSize().width / 2, node->getPositionY())),
		CCCallFuncN::create(this, SEL_CallFuncN(&BaseLobbyScene::onMoveFinsh)),
		NULL));
}