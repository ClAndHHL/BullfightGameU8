//
//  GameLobbyScene.cpp
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//
#include "GameLobbyScene.h"
#include "../Tools/Tools.h"
#include "../Tools/GameConfig.h"
#include "../Tools/BaseAttributes.h"
#include "../Tools/RotateMenu.h"
#include "../PopDialogBox/PopDialogBoxUserInfo.h"
#include "../PopDialogBox/PopDialogBoxLoading.h"
#include "../PopDialogBox/PopDialogBoxTipInfo.h"
#include "../PopDialogBox/PopDialogBoxSign.h"
#include "ClassicLobbyScene.h"
#include "LobbyHornLayer.h"
#include "../Play/GameControl/GameControlOxHundred.h"
#include "../Tools/DataModel.h"
#include "../PopDialogBox/PopDialogBoxSetUp.h"
#include "../MainScene/MainSceneOxHundred.h"
//#include "../Network/ListernerThread/LogonGameListerner.h"
#include "../Network/MD5/MD5.h"
#include "../Network/CMD_Server/PacketAide.h"
//#include "../Network/ListernerThread/LobbyGameListerner.h"
//#include "../Network/ListernerThread/GameIngListerner.h"
#include "../Network/CMD_Server/Packet.h"
#include "../Network/SEvent.h"
bool GameLobbyScene::isShowUpTip = false;

GameLobbyScene::GameLobbyScene()
{
	scheduleUpdate();

}
GameLobbyScene::~GameLobbyScene(){
	CCLOG("~ <<%s>>", __FUNCTION__);
	unscheduleUpdate();
	//
	//移除对象
	this->removeAllChildrenWithCleanup(true);
	//清理数据
	GUIReader::purge();
	CCArmatureDataManager::purge();
	//清理纹理
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}
CCScene* GameLobbyScene::scene(bool showUpTip, std::string strTipContent)
{
    CCScene *scene = CCScene::create();
    GameLobbyScene *layer = GameLobbyScene::create();
    scene->addChild(layer);
	isShowUpTip = showUpTip;
	layer->strUpTipContent = strTipContent;
	//this->strUpTipContent = strUpTipContent;
    return scene;
}
void GameLobbyScene::onEnter(){
	BaseLobbyScene::onEnter();
	lobbyScene = this;
	/*//添加背景
	CCSize deviceSize=DataModel::sharedDataModel()->deviceSize;
	CCSprite *spriteBg=CCSprite::create("res/main_bg.jpg");
	this->addChild(spriteBg);
	spriteBg->setPosition(ccp(deviceSize.width/2,deviceSize.height/2));
	float scale=deviceSize.height/spriteBg->getContentSize().height;
	spriteBg->setScale(scale);*/
	m_pWidget = UILayer::create();
	this->addChild(m_pWidget);


	UILayout *pWidget = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIGameLobby.ExportJson)));
	m_pWidget->addWidget(pWidget);


	//UIButton* pBRecharge = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonRecharge"));
	//pBRecharge->addTouchEventListener(this, SEL_TouchEvent(&BaseLobbyScene::onMenuFirstRecharge));
	/*UIButton* button = static_cast<UIButton*>(m_pWidget->getWidgetByName("buttonUser"));
	button->addTouchEventListener(this, SEL_TouchEvent(&GameLobbyScene::menuResetUser));

	button = static_cast<UIButton*>(m_pWidget->getWidgetByName("ButtonSetUp"));
	button->addTouchEventListener(this, SEL_TouchEvent(&GameLobbyScene::menuSetUp));*/
	UIButton* button=NULL;
	for (int i = 0; i < 3;i++)
	{
		button = static_cast<UIButton*>(m_pWidget->getWidgetByName(CCString::createWithFormat("buttonMode%d",i+1)->getCString()));
		button->addTouchEventListener(this, SEL_TouchEvent(&GameLobbyScene::menuSelectMode));
	}
	scroll=static_cast<UIScrollView*>(m_pWidget->getWidgetByName("ScrollView"));
	scroll->setInnerContainerSize(scroll->getContentSize());
	scroll->setEnabled(false);

	//Tools::iconv_convert(&DataModel::sharedDataModel()->userInfo->szNickName, sizeof(DataModel::sharedDataModel()->userInfo->szNickName), "GBK", &DataModel::sharedDataModel()->userInfo->szNickName, sizeof(DataModel::sharedDataModel()->userInfo->szNickName), "UTF-8");
	std::string nickName = Tools::GBKToUTF8(DataModel::sharedDataModel()->userInfo->szNickName);
	userName->setText(Tools::subUTF8(nickName, 0, 6));
	//userName->setText(Tools::GBKToUTF8(DataModel::sharedDataModel()->userInfo->szNickName));
	/*//用户名
	userName=static_cast<UILabel*>(m_pWidget->getWidgetByName("labelUserName"));
	//金币
	pLabelGoldCount=static_cast<UILabel*>(m_pWidget->getWidgetByName("LabelGoldCount"));*/
	if (isShowUpTip)
	{
		showUpTip();
	}
	//是否显示签到
	//isShowSign = true;
	if (isShowSign())
	{
		PopDialogBoxSign *pSign = PopDialogBoxSign::create();
		this->addChild(pSign,10);
	}


	/*TCPSocket *tcp = TCPSocketControl::sharedTCPSocketControl()->getTCPSocket(SOCKET_LOBBY);
	if (tcp&&tcp->eSocketState != TCPSocket::SOCKET_STATE_CONNECT_SUCCESS){
		tcp->createSocket(DataModel::sharedDataModel()->sLobbyIp.c_str(), DataModel::sharedDataModel()->lLobbyProt, new LobbyGameListerner());
		//tcp->createSocket("112.1.1.1", pLobbyIp->dwServerPort, new LobbyGameListerner());
	}*/
	
	/*if (CSocketControl::sharedSocketControl()->getTCPSocket(SOCKET_LOBBY)->getSocketState() != CGameSocket::SOCKET_STATE_CONNECT_SUCCESS)
	{
		CSocketControl::sharedSocketControl()->getTCPSocket(SOCKET_LOBBY)->Create(DataModel::sharedDataModel()->sLobbyIp.c_str(), DataModel::sharedDataModel()->lLobbyProt);
		

		CMD_GL_LogonAccounts LogonAccounts;
		strcpy(LogonAccounts.szAccounts, DataModel::sharedDataModel()->sLogonAccount.c_str());
		strcpy(LogonAccounts.szMachineID, "12");

		MD5 m;
		m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
		std::string md5PassWord = m.GetMd5();
		strcpy(LogonAccounts.szPassword, md5PassWord.c_str());
		CSocketControl::sharedSocketControl()->getTCPSocket(SOCKET_LOBBY)->SendData(MDM_GL_C_DATA, SUB_GL_MB_LOGON_ACCOUNTS, &LogonAccounts, sizeof(LogonAccounts));
		CSocketControl::sharedSocketControl()->startSendThread();
	}*/
	//旋转菜单////////////////////////////////////////////////////////////////////////
	RotateMenu *menu = RotateMenu::create();
	for (int i = 0; i < 4; i++)
	{
		menu->addMenuItem(createMenuItem(i));
	}
	menu->setPosition(DataModel::sharedDataModel()->deviceSize / 2);
	menu->setPositionY(menu->getPositionY() + 30);
	this->addChild(menu,-1);
	//喇叭按键
	pIVHorn = static_cast<UIImageView*>(m_pWidgetBase->getWidgetByName("ImageHornBg"));
	pIVHorn->addTouchEventListener(this, SEL_TouchEvent(&GameLobbyScene::onMenuShowHornMsg));
	//喇叭数量
	pLHornNum = static_cast<UILabel*>(m_pWidgetBase->getWidgetByName("LabelHornNum"));
	//重设喇叭数
	resetHornNum();
	showClipLabel();

	//添加监听事件
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GameLobbyScene::resetClipLabelData), UPDATE_HORN_MSG, NULL);
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GameLobbyScene::onUpdateHornCount), UPDATE_HORN_COUNT, NULL);

}
//重设喇叭数
void GameLobbyScene::resetHornNum(){
	pLHornNum->setText(CCString::createWithFormat("%d", DataModel::sharedDataModel()->userInfo->dwHornCount)->getCString());
}
CCMenuItemSprite *GameLobbyScene::createMenuItem(int index){
	//CCSprite * selectedSprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("mode%d.png", index)->getCString());
	//CCSprite * normalSprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("mode%d_press.png", index)->getCString());
	

	CCSprite * selectedSprite = CCSprite::createWithSpriteFrameName("mode.png");
	CCSprite * normalSprite = CCSprite::createWithSpriteFrameName("mode_press.png");



	CCMenuItemSprite *pItem = CCMenuItemSprite::create(normalSprite, selectedSprite, this, menu_selector(GameLobbyScene::onMenuStartGame));
	pItem->setTag(index + 1);
	//icon
	ccColor3B colorNormal = ccc3(200, 200, 200);
	ccColor3B colorSelect = ccc3(255, 255, 255);
	CCPoint	posIcon = ccp(normalSprite->getContentSize().width/2,normalSprite->getContentSize().height/2+26);
	if (index==2)
	{
		posIcon = ccpAdd(posIcon, ccp(-10,0));
	}
	CCSprite * pNModeIcon = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("u_mode_icon%d.png", index)->getCString());
	normalSprite->addChild(pNModeIcon);
	pNModeIcon->setPosition(posIcon);
	pNModeIcon->setColor(colorNormal);

	CCSprite * pSModeIcon = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("u_mode_icon%d.png", index)->getCString());
	selectedSprite->addChild(pSModeIcon);
	pSModeIcon->setPosition(posIcon);
	pSModeIcon->setColor(colorSelect);
	//名字
	CCPoint	posName = ccp(normalSprite->getContentSize().width / 2, normalSprite->getContentSize().height / 2 - 110);
	CCSprite * pNModeName = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("u_mode_name%d.png", index)->getCString());
	normalSprite->addChild(pNModeName);
	pNModeName->setPosition(posName);
	pNModeName->setColor(colorNormal);

	CCSprite * pSModeName = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("u_mode_name%d.png", index)->getCString());
	selectedSprite->addChild(pSModeName);
	pSModeName->setPosition(posName);
	pSModeName->setColor(colorSelect);
	return pItem;
}
//开始游戏
void GameLobbyScene::onMenuStartGame(cocos2d::CCObject* pSender){
	CCMenuItemSprite *button = (CCMenuItemSprite*)pSender;
	//if (button->getTag() < 3)
	{
		enterLobbyByMode(button->getTag());
	}
	/*else
	{
		PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
		this->addChild(tipInfo);
		tipInfo->setTipInfoContent(BaseAttributes::sharedAttributes()->sWaitCodeing.c_str());
	}*/
}
//显示签到
bool GameLobbyScene::isShowSign(){
	map<long, int >::iterator l_it;
	l_it = DataModel::sharedDataModel()->mapSignRecord.find(DataModel::sharedDataModel()->userInfo->dwUserID);
	if (l_it != DataModel::sharedDataModel()->mapSignRecord.end())
	{
		if (l_it->second == Tools::getCurDay())
		{
			return false;
		}
	}
	else
	{
		return true;
	}
	return true;
}
//显示站立提示
void GameLobbyScene::showUpTip(){
	PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
	this->addChild(tipInfo,2);
	if (strUpTipContent.length()==0)
	{
		strUpTipContent = "长时间不操作，自动退出！";
	}
	tipInfo->setTipInfoContent(strUpTipContent.c_str());
}
void GameLobbyScene::onExit(){
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, UPDATE_HORN_MSG);
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, UPDATE_HORN_COUNT);
	BaseLobbyScene::onExit();
}
void GameLobbyScene::menuResetUser(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		popDialogBoxUserInfo();
		//PopDialogBoxSign *pSign = PopDialogBoxSign::create();
		//this->addChild(pSign, 10);
	}
		break;
	default:
		break;
	}
}
void GameLobbyScene::menuSetUp(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			PopDialogBox *pdbSetUp = PopDialogBoxSetUp::create();
			this->addChild(pdbSetUp);
		}
		break;
	default:
		break;
	}
}
void GameLobbyScene::menuSelectMode(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		UIButton *button = (UIButton*)pSender;
		if (button->getTag()<3)
		{
			enterLobbyByMode(button->getTag());
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
//显示VIP
void GameLobbyScene::addVip(){
	popDialogBox(POP_VIP);
}
void GameLobbyScene::popDialogBoxUserInfo(){
	popDialogBox(POP_USER_INFO);
	//PopDialogBox *pdb = PopDialogBoxUserInfo::create();
	//this->addChild(pdb);
}
void GameLobbyScene::enterLobbyByMode(int mode){
	switch (mode)
	{
	/*case MODE_CLASSIC:
		{
			ClassicLobbyScene *pLayer = ClassicLobbyScene::create();
			this->addChild(pLayer, 100);
				//Tools::setTransitionAnimation(0, 0, ClassicLobbyScene::scene(false));
		}
		break;*/
	case MODE_TWO:
	{
		if (DataModel::sharedDataModel()->tagGameServerListOxTwo.size() <= 0)
		{
			PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
			this->addChild(tipInfo);
			tipInfo->setTipInfoContent(("获取房间列表失败!"));
		}
		else
		{
			ClassicLobbyScene *pLayer = ClassicLobbyScene::create();
			pLayer->setGameItem(ClassicLobbyScene::ITEM_0);
			this->addChild(pLayer, 100);
		}
	}
	break;
	case MODE_ONE_BY_ONE:
	{
		if (DataModel::sharedDataModel()->tagGameServerListOxOneByOne.size() <= 0)
		{
			PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
			this->addChild(tipInfo);
			tipInfo->setTipInfoContent(("获取房间列表失败!"));
		}
		else
		{
			ClassicLobbyScene *pLayer = ClassicLobbyScene::create();
			pLayer->setGameItem(ClassicLobbyScene::ITEM_1);
			this->addChild(pLayer, 100);
		}
	}
	break;
	case MODE_SIX_SWAP:
	{
		if (DataModel::sharedDataModel()->tagGameServerListSixSwap.size() <= 0)
		{
			PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
			this->addChild(tipInfo);
			tipInfo->setTipInfoContent(("获取房间列表失败!"));
		}
		else
		{
			ClassicLobbyScene *pLayer = ClassicLobbyScene::create();
			pLayer->setGameItem(ClassicLobbyScene::ITEM_2);
			this->addChild(pLayer, 100);
		}
	}
	break;
	case MODE_Hundred:
		{
			if (DataModel::sharedDataModel()->tagGameServerListOxHundred.size()<=0)
			{
				PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
				this->addChild(tipInfo);
				tipInfo->setTipInfoContent(("获取房间列表失败!"));
			}
			else
			{
				ClassicLobbyScene *pLayer = ClassicLobbyScene::create();
				pLayer->setGameItem(ClassicLobbyScene::ITEM_3);
				this->addChild(pLayer, 100);
				
				//Tools::setTransitionAnimation(0, 0, ClassicLobbyScene::scene(true));
				/*PopDialogBox *pLoading = PopDialogBoxLoading::create();
				this->addChild(pLoading, 10, TAG_LOADING);

				tagGameServer *tgs = DataModel::sharedDataModel()->tagGameServerListOxHundred[0];
				GameIngMsgHandler::sharedGameIngMsgHandler()->connectServer(tgs->szServerAddr,tgs->wServerPort);
				onEventConnect(1, NULL, 0);*/
			}
			
			//Tools::setTransitionAnimation(0, 0, MainSceneOxHundred::scene());
		}
		break;
	/*case MODE_ONE_BY_ONE:
		{
			PopDialogBox *pLoading = PopDialogBoxLoading::create();
			this->addChild(pLoading);
			tagGameServer *tgs = DataModel::sharedDataModel()->tagGameServerListOxOneByOne[0];
			getSocket()->createSocket(tgs->szServerAddr,tgs->wServerPort,new LogonGameListerner());
		}
		break;
	case MODE_SIX_SWAP:
	{
		PopDialogBox *pLoading = PopDialogBoxLoading::create();
		this->addChild(pLoading);
		tagGameServer *tgs = DataModel::sharedDataModel()->tagGameServerListSixSwap[0];
		getSocket()->createSocket(tgs->szServerAddr, tgs->wServerPort, new LogonGameListerner());
	}
		break;*/
	default:
		break;
	}
}
void GameLobbyScene::update(float delta){
	if (isReadMessage)
	{
		//MessageQueue::update(delta);
		MessageQueueGameIng::update(delta);
	}
}
void GameLobbyScene::onEventReadMessage(WORD wMainCmdID,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_MB_UNCONNECT:
	{
		PopDialogBoxLoading *pLoading = (PopDialogBoxLoading*)this->getChildByTag(TAG_LOADING);
		if (pLoading)
		{
			pLoading->removeFromParentAndCleanup(true);
		}
		GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.Destroy(false);
		PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
		this->addChild(tipInfo);
		tipInfo->setTipInfoContent("与服务器断开连接");
	}
	break;
	case MDM_MB_SOCKET:
		onEventConnect(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GR_LOGON:
		onSubLogon(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GR_CONFIG:
		onSubConfig(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GR_USER:
		onSubUserState(wSubCmdID,pDataBuffer,wDataSize);
		break;
	default:
		CCLOG("main:%d  sub:%d<<%s>>",wMainCmdID,wSubCmdID,__FUNCTION__);
		break;
	}
}

//连接成功
void GameLobbyScene::onEventConnect(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GP_SOCKET_OPEN:
		{
			/*CMD_GR_LogonMobile logonMobile;
			memset(&logonMobile, 0, sizeof(CMD_GR_LogonMobile));

			logonMobile.wGameID=30;
			logonMobile.dwProcessVersion=17039361;
			//设备类型
			logonMobile.cbDeviceType=2;
			logonMobile.wBehaviorFlags=BEHAVIOR_LOGON_IMMEDIATELY;
			logonMobile.wPageTableCount=10;

			logonMobile.dwUserID=DataModel::sharedDataModel()->userInfo->dwUserID;
			//logonMobile.dwUserID = 11;
			MD5 m;
			m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(),DataModel::sharedDataModel()->sLogonPassword.length());
			std::string md5PassWord = m.GetMd5();
			strcpy(logonMobile.szPassword,md5PassWord.c_str());
			//bool isSend = getSocket()->SendData(MDM_GR_LOGON, SUB_GR_LOGON_MOBILE, &logonMobile, sizeof(logonMobile));
			GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.SendData(MDM_GR_LOGON, SUB_GR_LOGON_MOBILE, &logonMobile, sizeof(logonMobile));
			*/

			CMD_GR_LogonMobile logonMobile;
			memset(&logonMobile, 0, sizeof(CMD_GR_LogonMobile));

			logonMobile.wGameID = 130;
			logonMobile.dwProcessVersion = 17235969;
			
			//设备类型
			logonMobile.cbDeviceType = DEVICE_TYPE_ANDROID;
			logonMobile.wBehaviorFlags = BEHAVIOR_LOGON_IMMEDIATELY;
			logonMobile.wPageTableCount = 10;

			logonMobile.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

			MD5 m;
			//MD5::char8 str[] = "z12345678";
			//m.ComputMd5(str,sizeof(str)-1);
			m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(), DataModel::sharedDataModel()->sLogonPassword.length());
			std::string md5PassWord = m.GetMd5();
			strcpy(logonMobile.szPassword, md5PassWord.c_str());

			strcpy(logonMobile.szMachineID, Tools::getMachineID().c_str());

			bool isSend = GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.SendData(MDM_GR_LOGON, SUB_GR_LOGON_MOBILE, &logonMobile, sizeof(logonMobile));

		}
		break;
	default:
		break;
	}
}
//登录
void GameLobbyScene::onSubLogon(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_UPDATE_NOTIFY:
		{
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_UpdateNotify));
			if (wDataSize < sizeof(CMD_GR_UpdateNotify)) return;

			CMD_GR_UpdateNotify *lf = (CMD_GR_UpdateNotify*)pDataBuffer;
			CCLOG("升级提示");
		}
		break;
	case SUB_GR_LOGON_SUCCESS:
		{
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_LogonSuccess));
			if (wDataSize < sizeof(CMD_GR_LogonSuccess)) return;

			CMD_GR_LogonSuccess *lf = (CMD_GR_LogonSuccess*)pDataBuffer;
			CCLOG("登录成功");
		}
		break;
	case SUB_GR_LOGON_FINISH:
		{
			CCLOG("登录完成");
		
		}
		break;
	case SUB_GR_LOGON_FAILURE:
		{
			CMD_GR_LogonFailure *lf = (CMD_GR_LogonFailure*)pDataBuffer;
			CCLOG("登录失败:%s", Tools::GBKToUTF8(lf->szDescribeString).c_str());
			if (this->getChildByTag(TAG_LOADING))
			{
				this->getChildByTag(TAG_LOADING)->removeFromParentAndCleanup(true);
			}
			
			//TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_LOGON_GAME);
			PopDialogBoxTipInfo *tipInfo = PopDialogBoxTipInfo::create();
			this->addChild(tipInfo);
			tipInfo->setTipInfoContent(GBKToUTF8(lf->szDescribeString).c_str());

			GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.Destroy(true);
			//TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_LOGON_ROOM);
		}
		break;
	default:
		break;
	}
}
void GameLobbyScene::onSubConfig(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_CONFIG_COLUMN://列表配置
		{
			//CMD_GR_ConfigColumn *pConfigColumn = (CMD_GR_ConfigColumn*)pDataBuffer;
			CCLOG("列表配置<<%s>>",__FUNCTION__);
		}
		break;			
	case SUB_GR_CONFIG_SERVER://房间配置
		{
			CCLOG("房间配置<<%s>>",__FUNCTION__);
		}
		break;		
	case SUB_GR_CONFIG_PROPERTY://道具配置
		{
			CCLOG("道具配置<<%s>>",__FUNCTION__);
		}
		break;
	case SUB_GR_CONFIG_FINISH://配置完成
		{
			CCLOG("配置完成<<%s>>",__FUNCTION__);
			unscheduleUpdate();
			/*//构造数据
			CMD_GF_GameOption GameOption;
			GameOption.dwFrameVersion=VERSION_FRAME;
			GameOption.cbAllowLookon=0;
			GameOption.dwClientVersion=VERSION_CLIENT;
			//发送
			//bool isSend = getSocket()->SendData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, &GameOption, sizeof(GameOption));
			bool isSend = GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.SendData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, &GameOption, sizeof(GameOption));
			if (isSend)
			{
				
				Tools::setTransitionAnimation(0,0,MainSceneOxHundred::scene());
				//enterMainSceneByMode(1);
			}*/
			Tools::setTransitionAnimation(0, 0, MainSceneOxOneByOne::scene());
		}
		break;
	case SUB_GR_CONFIG_USER_RIGHT://玩家权限
		{
			CCLOG("玩家权限<<%s>>",__FUNCTION__);
		}
		break;							
	default:
		CCLOG("sub:%d<<%s>>",wSubCmdID,__FUNCTION__);
		break;
	}
}
void GameLobbyScene::onSubUserState(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER://用户进入
		{
			CCLOG("用户进入<<%s>>",__FUNCTION__);
			onSubUserEnter(pDataBuffer,wDataSize);
		}
		break;
	case SUB_GR_USER_SCORE://用户分数
		{
			CCLOG("用户分数<<%s>>",__FUNCTION__);
		}
		break;
	case SUB_GR_USER_STATUS://用户状态
		{
			 CCLOG("用户状态<<%s>>",__FUNCTION__);
			onSubUserState(pDataBuffer,wDataSize);
		}
		break;
	default:
		CCLOG("sub:%d<<%s>>",wSubCmdID,__FUNCTION__);
		break;
	}
}
//用户进入
void GameLobbyScene::onSubUserEnter(void * pDataBuffer, unsigned short wDataSize){
	//效验参数
	assert(wDataSize>=sizeof(tagMobileUserInfoHead));
	if (wDataSize<sizeof(tagMobileUserInfoHead)) return ;
	int size=sizeof(tagMobileUserInfoHead);
	//消息处理
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)pDataBuffer;
	int wPacketSize=0;
	//变量定义
	tagUserInfo UserInfo;
	UserInfo.dwUserID=pUserInfoHead->dwUserID;
	UserInfo.lScore=pUserInfoHead->lScore;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET + sizeof(TCP_Head)];
	CopyMemory(cbDataBuffer, pDataBuffer, wDataSize);
	//CCLOG("-------------------------%d",wDataSize);
	//CCLOG("userID:%ld , state:%d <<%s>>",pUserInfoHead->dwUserID,pUserInfoHead->cbUserStatus,__FUNCTION__);
	wPacketSize+=sizeof(tagMobileUserInfoHead);
	while (true)
	{
		void * pDataBuffer1 = cbDataBuffer + wPacketSize;
		tagDataDescribe *DataDescribe = (tagDataDescribe*)pDataBuffer1;
		wPacketSize+=sizeof(tagDataDescribe);
		switch (DataDescribe->wDataDescribe)
		{
		case DTP_GR_NICK_NAME:		//用户昵称
			{
				CopyMemory(&UserInfo.szNickName, cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szNickName[CountArray(UserInfo.szNickName)-1]=0;
				CCLOG("nick:%s  %s", UserInfo.szNickName, Tools::GBKToUTF8(UserInfo.szNickName).c_str());
				//if (strcmp(Tools::GBKToUTF8(UserInfo.szNickName),"(null)")==0)
				{
					//CCLOG("null"); 
				}
			}
			break;
		case DTP_GR_GROUP_NAME:
			{
				CCLOG("社团");
			}
			break;
		case DTP_GR_UNDER_WRITE:
			{
				CopyMemory(UserInfo.szUnderWrite,cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite)-1]=0;
				CCLOG("签名:%s", Tools::GBKToUTF8(UserInfo.szUnderWrite).c_str());
			}
			break;
		}
		wPacketSize+=DataDescribe->wDataSize;
		if (wPacketSize>=wDataSize)
		{
			break;
		}
	}
	map<long ,tagUserInfo >::iterator l_it;
	l_it=DataModel::sharedDataModel()->mTagUserInfo.find(pUserInfoHead->dwUserID);
	if (l_it!=DataModel::sharedDataModel()->mTagUserInfo.end())
	{
		l_it->second=UserInfo;
	}else
	{
		DataModel::sharedDataModel()->mTagUserInfo.insert(map<long,tagUserInfo>::value_type(pUserInfoHead->dwUserID,UserInfo));
	}
	
	/*//效验参数
	assert(wDataSize>=sizeof(tagUserInfoHead));
	if (wDataSize<sizeof(tagUserInfoHead)) return ;
	
	//变量定义
	tagUserInfo UserInfo;
	tagCustomFaceInfo CustomFaceInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&CustomFaceInfo,sizeof(CustomFaceInfo));

	//消息处理
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pData;

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//变量定义
	bool bHideUserInfo=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
	bool bMySelfUserItem=pGlobalUserData->dwUserID==pUserInfoHead->dwUserID;
	bool bMasterUserOrder=((pUserInfoHead->cbMasterOrder>0)||((m_pIMySelfUserItem!=NULL)&&(m_pIMySelfUserItem->GetMasterOrder()>0)));

	//读取信息
	if ((bHideUserInfo==false)||(bMySelfUserItem==true)||(bMasterUserOrder==true))
	{
		//用户属性
		UserInfo.wFaceID=pUserInfoHead->wFaceID;
		UserInfo.dwGameID=pUserInfoHead->dwGameID;
		UserInfo.dwUserID=pUserInfoHead->dwUserID;
		UserInfo.dwGroupID=pUserInfoHead->dwGroupID;
		UserInfo.dwCustomID=pUserInfoHead->dwCustomID;

		//用户状态
		UserInfo.wTableID=pUserInfoHead->wTableID;
		UserInfo.wChairID=pUserInfoHead->wChairID;
		UserInfo.cbUserStatus=pUserInfoHead->cbUserStatus;

		//用户属性
		UserInfo.cbGender=pUserInfoHead->cbGender;
		UserInfo.cbMemberOrder=pUserInfoHead->cbMemberOrder;
		UserInfo.cbMasterOrder=pUserInfoHead->cbMasterOrder;

		//用户积分
		UserInfo.lScore=pUserInfoHead->lScore;
		UserInfo.lGrade=pUserInfoHead->lGrade;
		UserInfo.lInsure=pUserInfoHead->lInsure;
		UserInfo.dwWinCount=pUserInfoHead->dwWinCount;
		UserInfo.dwLostCount=pUserInfoHead->dwLostCount;
		UserInfo.dwDrawCount=pUserInfoHead->dwDrawCount;
		UserInfo.dwFleeCount=pUserInfoHead->dwFleeCount;
		UserInfo.dwUserMedal=pUserInfoHead->dwUserMedal;
		UserInfo.dwExperience=pUserInfoHead->dwExperience;
		UserInfo.lLoveLiness=pUserInfoHead->lLoveLiness;

		//变量定义
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pUserInfoHead+1,wDataSize-sizeof(tagUserInfoHead));

		//扩展信息
		while (true)
		{
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_NICK_NAME:		//用户昵称
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(UserInfo.szNickName));
					if (DataDescribe.wDataSize<=sizeof(UserInfo.szNickName))
					{
						CopyMemory(&UserInfo.szNickName,pDataBuffer,DataDescribe.wDataSize);
						UserInfo.szNickName[CountArray(UserInfo.szNickName)-1]=0;
					}
					break;
				}
			case DTP_GR_GROUP_NAME:		//用户社团
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(UserInfo.szGroupName));
					if (DataDescribe.wDataSize<=sizeof(UserInfo.szGroupName))
					{
						CopyMemory(&UserInfo.szGroupName,pDataBuffer,DataDescribe.wDataSize);
						UserInfo.szGroupName[CountArray(UserInfo.szGroupName)-1]=0;
					}
					break;
				}
			case DTP_GR_UNDER_WRITE:	//个性签名
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(UserInfo.szUnderWrite));
					if (DataDescribe.wDataSize<=sizeof(UserInfo.szUnderWrite))
					{
						CopyMemory(UserInfo.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
						UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite)-1]=0;
					}
					break;
				}
			}
		}

		//自定头像
		if (pUserInfoHead->dwCustomID!=0L)
		{
			//加载头像
			CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
			bool bSuccess=pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID,pUserInfoHead->dwCustomID,CustomFaceInfo);

			//下载头像
			if (bSuccess==false)
			{
				pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID,pUserInfoHead->dwCustomID);
			}
		}
	}
	else
	{
		//用户信息
		UserInfo.dwUserID=pUserInfoHead->dwUserID;
		lstrcpyn(UserInfo.szNickName,TEXT("游戏玩家"),CountArray(UserInfo.szNickName));

		//用户状态
		UserInfo.wTableID=pUserInfoHead->wTableID;
		UserInfo.wChairID=pUserInfoHead->wChairID;
		UserInfo.cbUserStatus=pUserInfoHead->cbUserStatus;

		//用户属性
		UserInfo.cbGender=pUserInfoHead->cbGender;
		UserInfo.cbMemberOrder=pUserInfoHead->cbMemberOrder;
		UserInfo.cbMasterOrder=pUserInfoHead->cbMasterOrder;
	}
	//if (pUserInfoHead->wTableID !=INVALID_TABLE)
	//{
	//	CString sUserInfo;
	//	sUserInfo.Format("%s | %d |  %d",UserInfo.szNickName,pUserInfoHead->wTableID,pUserInfoHead->wChairID);
	//	m_ChatMessage.InsertSystemString(sUserInfo);
	//}

	//激活用户
	IClientUserItem * pIClientUserItem=m_PlazaUserManagerModule->SearchUserByUserID(UserInfo.dwUserID);
	if (pIClientUserItem==NULL) 
	{
		pIClientUserItem=m_PlazaUserManagerModule->ActiveUserItem(UserInfo,CustomFaceInfo);
		//m_ChatMessage.InsertSystemString(UserInfo.szNickName);
	}
	else
	{		//删除用户

		m_ChatMessage.InsertSystemString(TEXT("ss1111111111111"));
		m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);
		pIClientUserItem=m_PlazaUserManagerModule->ActiveUserItem(UserInfo,CustomFaceInfo);
	}

	//获取对象
	CServerListData * pServerListData=CServerListData::GetInstance();

	//人数更新
	pServerListData->SetServerOnLineCount(m_GameServer.dwServerID,m_PlazaUserManagerModule->GetActiveUserCount());

	//变量定义
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//好友提示
	if (((bHideUserInfo==false)&&(bMySelfUserItem==false))||(bMasterUserOrder==true))
	{
		if(pParameterGlobal->m_bNotifyFriendCome && pIClientUserItem->GetUserCompanion()==CP_FRIEND)
		{
			//提示消息
			CString strDescribe;
			strDescribe.Format(TEXT("您的好友 [%s] 进来了！"),pIClientUserItem->GetNickName());
			m_ChatMessage.InsertSystemString(strDescribe);
			//m_ChatMessage.InsertUserEnter(pIClientUserItem->GetNickName());//InsertNormalString(strDescribe);	
		}
	}*/
}
//用户状态
void GameLobbyScene::onSubUserState(void * pDataBuffer, unsigned short wDataSize){
	CMD_GR_UserStatus *info= (CMD_GR_UserStatus*)pDataBuffer;
	switch (info->UserStatus.cbUserStatus)
	{
	case US_SIT://坐下
		{
			CCLOG("state==sit-----------%ld<<%s>>",info->dwUserID,__FUNCTION__);
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID){
			
				//DataModel::sharedDataModel()->isSit=true;
				CCLOG("坐下:table: %d desk:%d",info->UserStatus.wTableID,info->UserStatus.wChairID);
				DataModel::sharedDataModel()->userInfo->wTableID=info->UserStatus.wTableID;
				DataModel::sharedDataModel()->userInfo->wChairID=info->UserStatus.wChairID;
				
				
			}
		}
		break;
	case US_FREE://站立
		{
			CCLOG("state==free-----------%ld",info->dwUserID);
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID)
			{
				//MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_US_FREE,NULL);
			}else
			{

			}
		}
		break;
	case US_READY://同意
		{
			CCLOG("state==ready-----------%ld<<%s>>",info->dwUserID,__FUNCTION__);
		}
		break;
	case US_PLAYING:
		{
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID)
			{
				//MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_CONFIG_FINISH,NULL);
			}
			CCLOG("state==playing-----------%ld",info->dwUserID);
		}
		break;
	default:
		CCLOG("state==Other userID:%ld 状态：%d",info->dwUserID,info->UserStatus.cbUserStatus);
		break;
	}
}

//快速游戏
void GameLobbyScene::quickGame(){
	PopDialogBox *pLoading = PopDialogBoxLoading::create();
	this->addChild(pLoading, 10, TAG_LOADING);

	tagGameServer *tgs = DataModel::sharedDataModel()->tagGameServerListOxOneByOne[0];
	GameIngMsgHandler::sharedGameIngMsgHandler()->connectServer(tgs->szServerAddr, tgs->wServerPort);
	onEventConnect(1, NULL, 0);
}

//显示喇叭消息
void GameLobbyScene::onMenuShowHornMsg(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		LobbyHornLayer *pLayer = LobbyHornLayer::create();
		pLayer->setPosition(CCPointZero);
		this->addChild(pLayer, 100);

		//pIVHorn->runAction(CCMoveTo::create(0.2,ccp(pIVHorn->getPositionX(),-pIVHorn->getContentSize().height)));

		CCEaseExponentialIn  *out = CCEaseExponentialIn::create(CCMoveTo::create(0.2, ccp(pIVHorn->getPositionX(), -pIVHorn->getSize().height)));
		//CCCallFunc *call = CCCallFunc::create(this, SEL_CallFunc(&LobbyHornLayer::removSelf));
		CCSequence *seq = CCSequence::create(out, /*call,*/ NULL);
		pIVHorn->runAction(seq);
		CCClippingNode* clip = (CCClippingNode*)this->getChildByTag(TAG_CLIP_LABEL);
		if (clip)
		{
			CCEaseExponentialIn  *outClip = CCEaseExponentialIn::create(CCMoveTo::create(0.2, ccp(clip->getPositionX(), -pIVHorn->getSize().height-fontHeight)));
			CCSequence *seqClip = CCSequence::create(outClip, /*call,*/ NULL);
			clip->runAction(seqClip);
		}
	}
	break;
	default:
		break;
	}
}
//显示喇叭框
void GameLobbyScene::showHorn(){
	CCEaseExponentialOut  *out = CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(pIVHorn->getPositionX(), DataModel::sharedDataModel()->deviceSize.height*0.07)));
	CCSequence *seq = CCSequence::create(out, NULL);
	pIVHorn->runAction(seq);
	
	CCClippingNode* clip = (CCClippingNode*)this->getChildByTag(TAG_CLIP_LABEL);
	if (clip)
	{
		CCEaseExponentialOut  *outClip = CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(clip->getPositionX(), DataModel::sharedDataModel()->deviceSize.height*0.07-fontHeight)));
		CCSequence *seqClip = CCSequence::create(outClip, NULL);
		clip->runAction(seqClip);
	}
}
void GameLobbyScene::showClipLabel(){
	UILabel *txt = UILabel::create();
	txt->setFontSize(30);
	txt->setColor(ccc3(255,255,255));     //裁剪内容
	txt->setText(" ");
	CCClippingNode* clip = CCClippingNode::create();
	//  以下模型是drawnode遮罩
	int width = pIVHorn->getSize().width-80;
	int height = pIVHorn->getSize().height;
	CCDrawNode* front=CCDrawNode::create();
	ccColor4F yellow = {1, 1, 0, 1};
	CCPoint rect[4] = { ccp(0, 0), ccp(width, 0), ccp(width, height), ccp(0, height) };
	front->drawPolygon(rect, 4, yellow, 0, yellow); //绘制四边形
	front->setPosition(ccp(0, 0));
	clip->setStencil(front); //一定要有，设置裁剪模板

	txt->setAnchorPoint(CCPointZero);
	clip->addChild(txt,0,999);

	clip->setInverted(false);    //设置裁剪区域可见还是非裁剪区域可见  这里为裁剪区域可见
	clip->setAlphaThreshold(0);
	fontHeight =  txt->getSize().height / 2;
	clip->setPosition(pIVHorn->getPositionX() - pIVHorn->getSize().width / 2 + 40, pIVHorn->getPositionY()-fontHeight);
	this->addChild(clip, 0, TAG_CLIP_LABEL);
}
//设置跑马灯内容
void GameLobbyScene::resetClipLabelData(CCObject *obj){
	CCClippingNode* clip = (CCClippingNode*)this->getChildByTag(TAG_CLIP_LABEL);
	if (clip)
	{
		UILabel *txt =(UILabel*) clip->getChildByTag(999);
		if (txt)
		{
			if (DataModel::sharedDataModel()->listHornMsg.size() > 0)
			{
				//std::list <std::string> ::iterator iter = DataModel::sharedDataModel()->listHornMsg.rbegin();
				txt->setText(DataModel::sharedDataModel()->listHornMsg.rbegin()->c_str());
			}
			else
			{
				txt->setText("");
			}
			txt->stopAllActions();
			txt->setPositionX(clip->getContentSize().width);
			float speed = 70.0f;
			CCMoveTo* to2 = CCMoveTo::create(txt->getContentSize().width / speed, ccp(-txt->getContentSize().width-10, 0));
			CCCallFunc *call = CCCallFunc::create(this, SEL_CallFunc(&GameLobbyScene::onMoveFinsh));
			txt->runAction(CCSequence::create(CCDelayTime::create(3), to2, CCDelayTime::create(3), call, NULL));

		}
	}
}
//移动完成
void GameLobbyScene::onMoveFinsh(){
	resetClipLabelData(NULL);
}
//设置喇叭数
void GameLobbyScene::onUpdateHornCount(CCObject *obj){
	resetHornNum();
}