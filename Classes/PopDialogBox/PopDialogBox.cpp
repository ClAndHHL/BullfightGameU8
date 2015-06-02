/*
 * PopDialogBox.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#include "PopDialogBox.h"
#include "../Network/ListernerThread/LogonGameListerner.h"
#include "PopDialogBoxLoading.h"
#include "PopDialogBoxTipInfo.h"
std::string PopDialogBox::sSocketName = "";
PopDialogBox::PopDialogBox()
:pUILayer(NULL)
,pWidgetBg(NULL){
	pUILayer = UILayer::create();
	addChild(pUILayer);
	
	this->setTouchEnabled(true);
	this->setTouchPriority(0);
	this->setTouchMode(kCCTouchesOneByOne);
}


PopDialogBox::~PopDialogBox() {
	CCLog("~ <<%s>>",__FUNCTION__);
	//GUIReader::purge();
	//this->removeFromParentAndCleanup(true);
	//CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	//this->removeAllChildrenWithCleanup(true);
	//SceneReader::sharedSceneReader()->purge();
	//ActionManager::shareManager()->purge();
	//CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("ccs/UI/UIAboutHelp/UIAboutHelp0.plist");
	//CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void PopDialogBox::menuBack(CCObject *object, TouchEventType type){
	if (type == TOUCH_EVENT_ENDED)
	{
		/*Tools::playSound(kSoundBack);
		GameHUD *gameHUD = DataModel::sharedDataModel()->getMainSceneManage()->getGameHUD();
		gameHUD->changePauseState(false);
		//this->getParent()->removeChild(this, true);
		*/
		this->removeFromParentAndCleanup(true);
	}
}
void PopDialogBox::playAnimation(){
	if (pWidgetBg)
	{
		pWidgetBg->runAction(CCEaseBackOut::create(CCScaleTo::create(0.2, 1)));
	}
}
//连接服务器
void PopDialogBox::connectServer(std::string socketName){
	PopDialogBox *box = PopDialogBoxLoading::create();
	this->addChild(box, 10, TAG_LOADING);
	box->setSocketName(socketName);

	//setSocketName(socketName);

	TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(socketName);
	TCPSocket *tcp = getSocket();
	if (tcp)
	{
		tcp->createSocket(GAME_IP, PORT_LOGON, new LogonGameListerner());
	}
}
void PopDialogBox::setSocketName(std::string sName){
	sSocketName = sName;
}
void PopDialogBox::showTipInfo(const char* sInfo){
	PopDialogBoxTipInfo *pTipInfo = PopDialogBoxTipInfo::create();
	this->addChild(pTipInfo, 10);
	pTipInfo->setTipInfoContent(sInfo);
}