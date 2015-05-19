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
#include "../PopDialogBox/PopDialogBoxUserInfo.h"
#include "../PopDialogBox/PopDialogBoxLoading.h"
#include "ClassicLobbyScene.h"
#include "../Play/GameControl/GameControlOxHundred.h"
#include "../Tools/DataModel.h"
#include "../PopDialogBox/PopDialogBoxSetUp.h"
#include "../MainScene/MainSceneOxHundred.h"
#include "../Network/ListernerThread/LogonGameListerner.h"
#include "../Network/MD5/MD5.h"
#include "../Network/CMD_Server/PacketAide.h"
GameLobbyScene::GameLobbyScene()
{
	scheduleUpdate();
}
GameLobbyScene::~GameLobbyScene(){
	CCLog("~ <<%s>>", __FUNCTION__);
	//unscheduleUpdate();
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
CCScene* GameLobbyScene::scene()
{
    CCScene *scene = CCScene::create();
    GameLobbyScene *layer = GameLobbyScene::create();
    scene->addChild(layer);
    return scene;
}
void GameLobbyScene::onEnter(){
	BaseLobbyScene::onEnter();
	/*//添加背景
	CCSize deviceSize=DataModel::sharedDataModel()->deviceSize;
	CCSprite *spriteBg=CCSprite::create("res/main_bg.jpg");
	this->addChild(spriteBg);
	spriteBg->setPosition(ccp(deviceSize.width/2,deviceSize.height/2));
	float scale=deviceSize.height/spriteBg->getContentSize().height;
	spriteBg->setScale(scale);*/
	UILayer *m_pWidget = UILayer::create();
	this->addChild(m_pWidget);


	UILayout *pWidget = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIGameLobby.ExportJson)));
	m_pWidget->addWidget(pWidget);

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


	//Tools::iconv_convert(&DataModel::sharedDataModel()->userInfo->szNickName, sizeof(DataModel::sharedDataModel()->userInfo->szNickName), "GBK", &DataModel::sharedDataModel()->userInfo->szNickName, sizeof(DataModel::sharedDataModel()->userInfo->szNickName), "UTF-8");
	std::string nickName = Tools::GBKToUTF8(DataModel::sharedDataModel()->userInfo->szNickName);
	userName->setText(Tools::subUTF8(nickName, 0, 4));
	//userName->setText(Tools::GBKToUTF8(DataModel::sharedDataModel()->userInfo->szNickName));
	/*//用户名
	userName=static_cast<UILabel*>(m_pWidget->getWidgetByName("labelUserName"));
	//金币
	pLabelGoldCount=static_cast<UILabel*>(m_pWidget->getWidgetByName("LabelGoldCount"));*/
}

void GameLobbyScene::onExit(){
	BaseLobbyScene::onExit();
}
void GameLobbyScene::menuResetUser(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		popDialogBox();
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
		enterLobbyByMode(button->getTag());
	}
		break;
	default:
		break;
	}
}

void GameLobbyScene::popDialogBox(){
	PopDialogBox *pdb = PopDialogBoxUserInfo::create();
	this->addChild(pdb);
}
void GameLobbyScene::enterLobbyByMode(int mode){
	switch (mode)
	{
	case MODE_CLASSIC:
		{
				Tools::setTransitionAnimation(0, 0, ClassicLobbyScene::scene());
		}
		break;
	case MODE_Hundred:
		{
			PopDialogBox *pLoading = PopDialogBoxLoading::create();
			this->addChild(pLoading);
			tagGameServer *tgs=DataModel::sharedDataModel()->tagGameServerListOxHundred[0];
			getSocket()->createSocket(tgs->szServerAddr,tgs->wServerPort,new LogonGameListerner());
			//Tools::setTransitionAnimation(0, 0, MainSceneOxHundred::scene());
		}
		break;
	case MODE_ONE_BY_ONE:
		{
			PopDialogBox *pLoading = PopDialogBoxLoading::create();
			this->addChild(pLoading);
			tagGameServer *tgs=DataModel::sharedDataModel()->tagGameServerListOxOneByOne[0];
			getSocket()->createSocket(tgs->szServerAddr,tgs->wServerPort,new LogonGameListerner());
		}
		break;
	default:
		break;
	}
}
void GameLobbyScene::update(float delta){
	MessageQueue::update(delta);
}
void GameLobbyScene::onEventReadMessage(WORD wMainCmdID,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
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
		CCLog("main:%d  sub:%d<<%s>>",wMainCmdID,wSubCmdID,__FUNCTION__);
		break;
	}
}
//连接成功
void GameLobbyScene::onEventConnect(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GP_SOCKET_OPEN:
		{
			CMD_GR_LogonMobile logonMobile;
			memset(&logonMobile, 0, sizeof(CMD_GR_LogonMobile));

			logonMobile.wGameID=30;
			logonMobile.dwProcessVersion=17039361;
			//设备类型
			logonMobile.cbDeviceType=2;
			logonMobile.wBehaviorFlags=BEHAVIOR_LOGON_IMMEDIATELY;
			logonMobile.wPageTableCount=10;

			logonMobile.dwUserID=DataModel::sharedDataModel()->userInfo->dwUserID;

			MD5 m;
			m.ComputMd5(DataModel::sharedDataModel()->sLogonPassword.c_str(),DataModel::sharedDataModel()->sLogonPassword.length());
			std::string md5PassWord = m.GetMd5();
			strcpy(logonMobile.szPassword,md5PassWord.c_str());
			bool isSend = getSocket()->SendData(MDM_GR_LOGON, SUB_GR_LOGON_MOBILE, &logonMobile, sizeof(logonMobile));
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
			CCLog("升级提示");
		}
		break;
	case SUB_GR_LOGON_SUCCESS:
		{
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_LogonSuccess));
			if (wDataSize < sizeof(CMD_GR_LogonSuccess)) return;

			CMD_GR_LogonSuccess *lf = (CMD_GR_LogonSuccess*)pDataBuffer;
			CCLog("登录成功");
		}
		break;
	case SUB_GR_LOGON_FINISH:
		{
			CCLog("登录完成");
		
		}
		break;
	case SUB_GR_LOGON_FAILURE:
		{
			CMD_GR_LogonFailure *lf = (CMD_GR_LogonFailure*)pDataBuffer;
			CCLog("登录失败:%s",Tools::GBKToUTF8(lf->szDescribeString));
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
			CCLog("列表配置<<%s>>",__FUNCTION__);
		}
		break;			
	case SUB_GR_CONFIG_SERVER://房间配置
		{
			CCLog("房间配置<<%s>>",__FUNCTION__);
		}
		break;		
	case SUB_GR_CONFIG_PROPERTY://道具配置
		{
			CCLog("道具配置<<%s>>",__FUNCTION__);
		}
		break;
	case SUB_GR_CONFIG_FINISH://配置完成
		{
			CCLog("配置完成<<%s>>",__FUNCTION__);
			unscheduleUpdate();
			//构造数据
			CMD_GF_GameOption GameOption;
			GameOption.dwFrameVersion=VERSION_FRAME;
			GameOption.cbAllowLookon=0;
			GameOption.dwClientVersion=VERSION_CLIENT;
			//发送
			bool isSend = getSocket()->SendData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, &GameOption, sizeof(GameOption));
			if (isSend)
			{
				
				Tools::setTransitionAnimation(0,0,MainSceneOxHundred::scene());
				//enterMainSceneByMode(1);
			}
		}
		break;
	case SUB_GR_CONFIG_USER_RIGHT://玩家权限
		{
			CCLog("玩家权限<<%s>>",__FUNCTION__);
		}
		break;							
	default:
		CCLog("sub:%d<<%s>>",wSubCmdID,__FUNCTION__);
		break;
	}
}
void GameLobbyScene::onSubUserState(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER://用户进入
		{
			CCLog("用户进入<<%s>>",__FUNCTION__);
			onSubUserEnter(pDataBuffer,wDataSize);
		}
		break;
	case SUB_GR_USER_SCORE://用户分数
		{
			CCLog("用户分数<<%s>>",__FUNCTION__);
		}
		break;
	case SUB_GR_USER_STATUS://用户状态
		{
			 CCLog("用户状态<<%s>>",__FUNCTION__);
			onSubUserState(pDataBuffer,wDataSize);
		}
		break;
	default:
		CCLog("sub:%d<<%s>>",wSubCmdID,__FUNCTION__);
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
	//CCLog("-------------------------%d",wDataSize);
	//CCLog("userID:%ld , state:%d <<%s>>",pUserInfoHead->dwUserID,pUserInfoHead->cbUserStatus,__FUNCTION__);
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
				CCLog("nick:%s  %s",UserInfo.szNickName,Tools::GBKToUTF8(UserInfo.szNickName));
				//if (strcmp(Tools::GBKToUTF8(UserInfo.szNickName),"(null)")==0)
				{
					//CCLog("null"); 
				}
			}
			break;
		case DTP_GR_GROUP_NAME:
			{
				CCLog("社团");
			}
			break;
		case DTP_GR_UNDER_WRITE:
			{
				CopyMemory(UserInfo.szUnderWrite,cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite)-1]=0;
				CCLog("签名:%s",Tools::GBKToUTF8(UserInfo.szUnderWrite));
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
			CCLog("state==sit-----------%ld<<%s>>",info->dwUserID,__FUNCTION__);
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID){
			
				//DataModel::sharedDataModel()->isSit=true;
				CCLog("坐下:table: %d desk:%d",info->UserStatus.wTableID,info->UserStatus.wChairID);
				DataModel::sharedDataModel()->userInfo->wTableID=info->UserStatus.wTableID;
				DataModel::sharedDataModel()->userInfo->wChairID=info->UserStatus.wChairID;
				
				
			}
		}
		break;
	case US_FREE://站立
		{
			CCLog("state==free-----------%ld",info->dwUserID);
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
			CCLog("state==ready-----------%ld<<%s>>",info->dwUserID,__FUNCTION__);
		}
		break;
	case US_PLAYING:
		{
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID)
			{
				//MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_CONFIG_FINISH,NULL);
			}
			CCLog("state==playing-----------%ld",info->dwUserID);
		}
		break;
	default:
		CCLog("state==Other userID:%ld 状态：%d",info->dwUserID,info->UserStatus.cbUserStatus);
		break;
	}
}
