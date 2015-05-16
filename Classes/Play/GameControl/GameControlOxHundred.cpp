//
//
//
//

#include "GameControlOxHundred.h"
#include "Tools/GameConfig.h"
#include "Tools/Tools.h"
#include "GameLobby/GameLobbyScene.h"
#include "Tools/DataModel.h"
#include "Network/CMD_Server/cmd_game.h"
#include "Network/CMD_Server/PacketAide.h"
#include "PopDialogBox/PopDialogBoxUpBank.h"
#include "PopDialogBox/PopDialogBoxOnLine.h"
#include "PopDialogBox/PopDialogBoxTrend.h"
#include "Network/SEvent.h"
#include "MTNotificationQueue/MTNotificationQueue.h"
using namespace std;
GameControlOxHundred::GameControlOxHundred()
:iCurSelectJettonIndex(0)
,m_lMeMaxScore(0)
,m_bMeApplyBanker(false)
,m_wBankerUser(-100)
{
	nJetton[0]=1000;
	nJetton[1]=5000;
	nJetton[2]=10000;
	nJetton[3]=100000;
	nJetton[4]=500000;
	//ׯ����Ϣ
	m_wBankerUser=INVALID_CHAIR;	

	resetData();
	DataModel::sharedDataModel()->userInfo->wChairID=-10;
}
GameControlOxHundred::~GameControlOxHundred(){
	TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(SOCKET_LOGON_ROOM);
	DataModel::sharedDataModel()->mTagUserInfo.clear();
	DataModel::sharedDataModel()->	vecJettonNode.clear();
}
void GameControlOxHundred::onEnter(){
	CCLayer::onEnter();
	UILayer *pWidget = UILayer::create();
	this->addChild(pWidget);

	UILayout *pLayout = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIGameIngHundred.ExportJson)));
	pWidget->addWidget(pLayout);

	UIButton* button = static_cast<UIButton*>(pWidget->getWidgetByName("ButtonBack"));
	button->addTouchEventListener(this, SEL_TouchEvent(&GameControlOxHundred::onMenuBack));
	//����ͼ����
	button = static_cast<UIButton*>(pWidget->getWidgetByName("ButtonTrend"));
	button->addTouchEventListener(this, SEL_TouchEvent(&GameControlOxHundred::onMenuTrend));
	//�����û�����
	pBOnline= static_cast<UIButton*>(pWidget->getWidgetByName("ButtonOnline"));
	pBOnline->addTouchEventListener(this, SEL_TouchEvent(&GameControlOxHundred::onMenuOnLine));
	//���밴ť
	for (int i = 0; i < MAX_JETTON_BUTTON_COUNT; i++)
	{
		pIJettonButton[i]=static_cast<UIImageView*>(pWidget->getWidgetByName(CCString::createWithFormat("ImageJetton%d",i)->getCString()));
		pIJettonButton[i]->addTouchEventListener(this, SEL_TouchEvent(&GameControlOxHundred::onMenuSelectJetton));
		//����
		pIJettonButton[i]->setColor(ccc3(100,100,100));
		pIJettonButton[i]->setTouchEnabled(false);
	}
	//����ѡ����
	pIJettonSelect=static_cast<UIImageView*>(pWidget->getWidgetByName("ImageSelectJetton"));
	pIJettonSelect->getChildByName("ImageSelectJettonBg")->runAction(CCRepeatForever::create(CCRotateBy::create(0.8,360)));
	pIJettonSelect->setVisible(false);

	//ׯ���Ʊ���
	UIImageView *pIBankCardBg=static_cast<UIImageView*>(pWidget->getWidgetByName("ImageBankCardBg"));
	getMainScene()->posChair[0]=ccpAdd(pIBankCardBg->getPosition(),ccp(0,-pIBankCardBg->getContentSize().height/2));
	//�û�����
	for (int i = 0; i < MAX_PLAYER_HUNDRED_COUNT; i++)
	{
		pPlayerData[i]=PlayerDataHundred::create();
		this->addChild(pPlayerData[i]);
		pPlayerData[i]->pIPlayerBg=static_cast<UIImageView*>(pWidget->getWidgetByName(CCString::createWithFormat("ImageIcon%d",i)->getCString()));
		pPlayerData[i]->pIPlayerBg->setVisible(false);
		//��������
		pPlayerData[i]->pLResult=static_cast<UILabelAtlas*>(pPlayerData[i]->pIPlayerBg->getChildByName("AtlasLabelResult"));
		pPlayerData[i]->pLResult->setVisible(false);
		//�������
		pPlayerData[i]->pLUserName=static_cast<UILabel*>(pPlayerData[i]->pIPlayerBg->getChildByName("LabelName"));
		//��ҽ��
		pPlayerData[i]->pLGoldCount=static_cast<UILabel*>(pPlayerData[i]->pIPlayerBg->getChildByName("LabelGold"));
	}
	pPlayerData[0]->setUserInfo(*DataModel::sharedDataModel()->userInfo);
	//��ׯ����
	pIUpBank=static_cast<UIImageView*>(pWidget->getWidgetByName("ImageUpBank"));
	pIUpBank->addTouchEventListener(this, SEL_TouchEvent(&GameControlOxHundred::onMenuUpBank));
	CCSprite *pSUpBank=CCSprite::create();
	CCArray *animFrames=CCArray::create();

	CCSpriteFrame *pSFrame=CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("u_gih_add_score.png");
	animFrames->addObject(pSFrame);
	CCSpriteFrame *pSFrame1=CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("u_gih_add_score_pre.png");
	animFrames->addObject(pSFrame1);

	CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames,0.1f); 
	animation->setLoops(-1); 
	CCAnimate *action=CCAnimate::create(animation); 
	pSUpBank->runAction(action);

	pIUpBank->addNode(pSUpBank);

	initTimer(pWidget);
	initSeatData(pWidget);
}
void GameControlOxHundred::onExit(){
	CCLayer::onExit();
}
void GameControlOxHundred::resetData(){
	//������ע
	memset(m_lUserJettonScore,0,sizeof(m_lUserJettonScore));
	//ȫ����ע
	memset(m_lAllJettonScore,0,sizeof(m_lAllJettonScore));
	
	//m_wBankerTime=0;
	m_lBankerScore=0L;	
	//m_lBankerWinScore=0L;
	//m_lTmpBankerWinScore=0;
	//m_blCanStore=false;
	/*
	//���ֳɼ�
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//״̬��Ϣ
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinTianMen=false;
	m_bWinHuangMen=false;
	m_bWinXuanMen=false;
	m_bFlashResult=false;
	m_blMoveFinish = false;
	m_blAutoOpenCard = true;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		
	*/
	//m_lAreaLimitScore=0L;	
	/*
	//λ����Ϣ
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//��ʷ�ɼ�
	m_lMeStatisticScore=0;
	*/
}

//��ʼ����ʱ��
void GameControlOxHundred::initTimer(UILayer *pWidget){
	pITimer = static_cast<UIImageView*>(pWidget->getWidgetByName("ImageTimer"));
	pITimer->setVisible(false);

	pLTimerNum=static_cast<UILabelAtlas*>(pWidget->getWidgetByName("AtlasLabelTimer"));
	iTimerCount=-1;

	pLTimerPromptContent=static_cast<UILabel*>(pWidget->getWidgetByName("LabelPromptContent"));
}
//��ʼ����λ
void GameControlOxHundred::initSeatData(UILayer *pWidget){
	for (int i = 0; i < MAX_SEAT_COUNT; i++)
	{
		pSeatData[i]=SeatData::create();
		this->addChild(pSeatData[i]);
		UIImageView *bg=static_cast<UIImageView*>(pWidget->getWidgetByName(CCString::createWithFormat("ImageSeatBg%d",i)->getCString()));
		bg->addTouchEventListener(this, SEL_TouchEvent(&GameControlOxHundred::onMenuPlaceJetton));
		//�������ĵ�
		pSeatData[i]->posCenter=bg->getPosition();
		getMainScene()->posChair[i+1]=ccpAdd(bg->getPosition(),ccp(0,-bg->getContentSize().height/2-50));
		//������λ��С
		pSeatData[i]->seatSize=bg->getContentSize();
		//���г���
		pSeatData[i]->pLAllJetton=static_cast<UILabelAtlas*>(bg->getChildByName("AtlasLabelAllScore"));
		pSeatData[i]->pIFontWan=static_cast<UIImageView*>(bg->getChildByName("AtlasLabelAllScore")->getChildByName("ImageFontWan"));
		//�û�����
		pSeatData[i]->pLUserJetton=static_cast<UILabelAtlas*>(bg->getChildByName("ImageB")->getChildByName("AtlasLabelAddScore"));
		pSeatData[i]->pIUserFontWan=static_cast<UIImageView*>(bg->getChildByName("ImageB")->getChildByName("AtlasLabelAddScore")->getChildByName("ImageFontWan1"));
		//δ��ע
		pSeatData[i]->pINotAddJetton=static_cast<UIImageView*>(bg->getChildByName("ImageNotAdd"));
		//��������
		pSeatData[i]->pLResult=static_cast<UILabelAtlas*>(bg->getChildByName("AtlasLabelResultNum"));

		pSeatData[i]->resetData();
	}
}
//������λ����
void GameControlOxHundred::setSeatResult(int iSeatIndex,int iOXType){
	int oxType=iOXType;
	if (oxType==0)
	{
		oxType==1;
	}
	long long lMeJetton=m_lUserJettonScore[iSeatIndex]*oxType;
	pSeatData[iSeatIndex-1]->setResult(lMeJetton);
}
//��ʾ�ܽ��
void GameControlOxHundred::showAllResult(){
	for (int i = 0; i < MAX_PLAYER_HUNDRED_COUNT; i++)
	{
		pPlayerData[i]->showResultAnimation();
	}
	//��������ͼ
	MTNotificationQueue::sharedNotificationQueue()->postNotification(UPDATE_LIST,NULL);
}
//��ȡ�û���Ϣͨ�����Ӻ�
tagUserInfo* GameControlOxHundred::getUserInfo(int iChair){
	map<long,tagUserInfo>::iterator it;
	for(it=DataModel::sharedDataModel()->mTagUserInfo.begin();it!=DataModel::sharedDataModel()->mTagUserInfo.end();it++)
	{
		if (it->second.wChairID==iChair)
		{
			return &it->second;
		}
	}
	return NULL;
}

//��ó������
JettonNode *GameControlOxHundred::getJettonNode(){
	for (int i = 0; i < DataModel::sharedDataModel()->vecJettonNode.size(); i++)
	{
		JettonNode *tempJetton=DataModel::sharedDataModel()->vecJettonNode[i];
		if (tempJetton->isReuse)
		{
			tempJetton->resetData();
			return tempJetton;
		}
	}
	JettonNode *pJetton=JettonNode::create();
	this->addChild(pJetton,1000);
	DataModel::sharedDataModel()->vecJettonNode.push_back(pJetton);
	return pJetton;
}
int GameControlOxHundred::getChairIndex(int meChairID,int chairID){
	if (meChairID==0)
	{
		if (chairID==0)
		{
			return 3;
		}
	}else
	{
		if (chairID==1)
		{
			return 3;
		}
	}
	return 0;
}
//�˵��ص������أ�
void GameControlOxHundred::onMenuBack(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			standUpWithExit();
		}
		break;
	default:
		break;
	}
}
//��ע����
void GameControlOxHundred::onMenuPlaceJetton(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			if (iCurSelectJettonIndex==-1||
				DataModel::sharedDataModel()->getMainSceneOxHundred()->getGameState()!=MainSceneOxHundred::STATE_GAME_PLACE_JETTON)
			{
				CCLog("nonononon<<%s>>",__FUNCTION__);
				return;
			}
			UIImageView *pIButton=(UIImageView*)pSender;
			//��������
			CMD_C_PlaceJetton PlaceJetton;
			memset(&PlaceJetton,0,sizeof(PlaceJetton));
			//�������
			PlaceJetton.cbJettonArea=pIButton->getTag();
			PlaceJetton.lJettonScore=nJetton[iCurSelectJettonIndex];
			//������ע
			m_lUserJettonScore[PlaceJetton.cbJettonArea]+=PlaceJetton.lJettonScore;
			m_lAllJettonScore[PlaceJetton.cbJettonArea] += PlaceJetton.lJettonScore;

			pSeatData[PlaceJetton.cbJettonArea-1]->setUserJetton(m_lUserJettonScore[PlaceJetton.cbJettonArea]);


			//������Ϣ
			TCPSocketControl::sharedTCPSocketControl()->getTCPSocket(SOCKET_LOGON_ROOM)->SendData(MDM_GF_GAME,SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
					
		}
		break;
	default:
		break;
	}
}
//ѡ�����
void GameControlOxHundred::onMenuSelectJetton(CCObject* pSender, TouchEventType type){
		switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			UIImageView *pIButton=(UIImageView*)pSender;
			pIJettonSelect->setPosition(pIButton->getPosition());
			iCurSelectJettonIndex=pIButton->getTag()-1;
		}
		break;
	default:
		break;
	}
}
//�����û�
void GameControlOxHundred::onMenuOnLine(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			PopDialogBoxOnLine *pDBUpBank=PopDialogBoxOnLine::create();
			getParent()->addChild(pDBUpBank,K_Z_ORDER_POP);
		}
		break;
	default:
		break;
	}
}
//����ͼ
void GameControlOxHundred::onMenuTrend(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			PopDialogBoxTrend *pDBUpBank=PopDialogBoxTrend::create();
			getParent()->addChild(pDBUpBank,K_Z_ORDER_POP);
		}
		break;
	default:
		break;
	}
}
//�û���ׯ
void GameControlOxHundred::onMenuUpBank(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		{
			PopDialogBoxUpBank *pDBUpBank=PopDialogBoxUpBank::create();
			getParent()->addChild(pDBUpBank,K_Z_ORDER_POP);
		}
		break;
	default:
		break;
	}
}
void GameControlOxHundred::update(float delta){
	MessageQueue::update(delta);
}
void GameControlOxHundred::delayedAction(){
	switch (DataModel::sharedDataModel()->getMainSceneOxHundred()->getGameState())
	{
	case MainSceneOxHundred::STATE_GAME_FREE:
		{
			hideTimer(true);
			
		}
		break;
	default:
		break;
	}
}
//�����ע
long long GameControlOxHundred::getUserMaxJetton()
{
	int iTimer = 10;
	//����ע��
	long long lNowJetton = 0;
	assert(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)lNowJetton += m_lUserJettonScore[nAreaIndex]*iTimer;

	//ׯ�ҽ��
	long long lBankerScore=2147483647;
	//if (m_wBankerUser!=INVALID_CHAIR) 
	lBankerScore=m_lBankerScore;
	//CCLog("=lBankerScore:%lld    %lld<<%s>>",lBankerScore,m_lBankerScore,__FUNCTION__);
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
	{
		lBankerScore-=m_lAllJettonScore[nAreaIndex]*iTimer;
		//CCLog("%d ---------%lld          <<%s>>",nAreaIndex,m_lAllJettonScore[nAreaIndex],__FUNCTION__);
	}
	//CCLog("===lBankerScore:%lld<<%s>>",lBankerScore,__FUNCTION__);
	//��������
	long long lMeMaxScore=0;
	if((m_lMeMaxScore-lNowJetton)/iTimer>m_lAreaLimitScore)
	{
		lMeMaxScore= m_lAreaLimitScore*iTimer;

	}else
	{
		lMeMaxScore = m_lMeMaxScore-lNowJetton;
		lMeMaxScore = lMeMaxScore;
	}

	//ׯ������
	lMeMaxScore=MIN(lMeMaxScore,lBankerScore);

	lMeMaxScore /= iTimer; 

	//��������
	//ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = MAX(lMeMaxScore, 0);
	
	return lMeMaxScore;
}
//���¿���
void GameControlOxHundred::updateButtonContron(){
	bool bEnablePlaceJetton=true; 
	switch (DataModel::sharedDataModel()->getMainSceneOxHundred()->getGameState())
	{
	case MainSceneOxHundred::STATE_GAME_FREE:
		{
				bEnablePlaceJetton=false;
		}
		break;
	case MainSceneOxHundred::STATE_GAME_PLACE_JETTON:
		break;
	case MainSceneOxHundred::STATE_GAME_SEND_CARD:
		{
			bEnablePlaceJetton=false;
		}
		break;
	default:
		break;
	}
	if (m_bMeApplyBanker)
	{
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if (bEnablePlaceJetton)
	{
		for (int i = 0; i < MAX_JETTON_BUTTON_COUNT; i++)
		{
			pIJettonButton[i]->setColor(ccc3(255,255,255));
			pIJettonButton[i]->setTouchEnabled(true);
			pIJettonButton[0]->setVisible(true);
		}
		if (iCurSelectJettonIndex<0)
		{
			iCurSelectJettonIndex=0;
		}
		pIJettonSelect->setVisible(true);
		//////////////////////////////////////////////////////////////////////////
		//�������
		long long lCurrentJetton=nJetton[iCurSelectJettonIndex];//��ǰ����
		long long lLeaveScore=m_lMeMaxScore;//�ҵĽ��
		for (int nAreaIndex=1; nAreaIndex<=MAX_AREA_COUNT; ++nAreaIndex) lLeaveScore -= m_lUserJettonScore[nAreaIndex];
		
		//�����ע
		long long lUserMaxJetton=getUserMaxJetton();
		//CCLog("---�����ע:%lld<<%s>>",lUserMaxJetton,__FUNCTION__);
		//���ù��
		lLeaveScore = MIN((lLeaveScore/10),lUserMaxJetton); //�û����·� �����ֱȽ� �������屶 
		//CCLog("---lLeaveScore:%lld<<%s>>",lLeaveScore,__FUNCTION__);
		if (lCurrentJetton>lLeaveScore)
		{
			/*if (lLeaveScore>=5000000L) m_GameClientView.SetCurrentJetton(5000000L);
			else if (lLeaveScore>=1000000L) m_GameClientView.SetCurrentJetton(1000000L);
			else if (lLeaveScore>=500000L) m_GameClientView.SetCurrentJetton(500000L);
			else if (lLeaveScore>=100000L) m_GameClientView.SetCurrentJetton(100000L);
			else if (lLeaveScore>=50000L) m_GameClientView.SetCurrentJetton(50000L);
			else if (lLeaveScore>=10000L) m_GameClientView.SetCurrentJetton(10000L);
			else if (lLeaveScore>=1000L) m_GameClientView.SetCurrentJetton(1000L);
			else if (lLeaveScore>=100L) m_GameClientView.SetCurrentJetton(100L);
			else m_GameClientView.SetCurrentJetton(0L);*/

		}
		
		
		//���ư�ť
		int iTimer = 1;
		for (int i = 0; i < MAX_JETTON_BUTTON_COUNT; i++)
		{
			if (lLeaveScore<nJetton[i]*iTimer || lUserMaxJetton<nJetton[i]*iTimer)
			{
				pIJettonButton[i]->setColor(ccc3(100,100,100));
				pIJettonButton[i]->setTouchEnabled(false);
				if (iCurSelectJettonIndex==i)
				{
					iCurSelectJettonIndex--;
					if (iCurSelectJettonIndex>=0)
					{
						pIJettonSelect->setPosition(pIJettonButton[iCurSelectJettonIndex]->getPosition());
					}else
					{
						pIJettonSelect->setVisible(false);
					}
				}
			}
		}
		/**/
		/*pIJettonButton[0]->setVisible((lLeaveScore>=100*iTimer && lUserMaxJetton>=100*iTimer)?true:false);
		pIJettonButton[1]->setVisible((lLeaveScore>=5000*iTimer && lUserMaxJetton>=5000*iTimer)?true:false);
		pIJettonButton[2]->setVisible((lLeaveScore>=10000*iTimer && lUserMaxJetton>=10000*iTimer)?true:false);
		pIJettonButton[3]->setVisible((lLeaveScore>=100000*iTimer && lUserMaxJetton>=100000*iTimer)?true:false);
		pIJettonButton[4]->setVisible((lLeaveScore>=500000*iTimer && lUserMaxJetton>=500000*iTimer)?true:false);
		CCLog("%lld    %lld<<%s>>",lLeaveScore,lUserMaxJetton,__FUNCTION__);*/
		/*m_GameClientView.m_btJetton100.EnableWindow((lLeaveScore>=100*iTimer && lUserMaxJetton>=100*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton1000.EnableWindow((lLeaveScore>=1000*iTimer && lUserMaxJetton>=1000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow((lLeaveScore>=10000*iTimer && lUserMaxJetton>=10000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton50000.EnableWindow((lLeaveScore>=50000*iTimer && lUserMaxJetton>=50000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton100000.EnableWindow((lLeaveScore>=100000*iTimer && lUserMaxJetton>=100000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow((lLeaveScore>=500000*iTimer && lUserMaxJetton>=500000*iTimer)?TRUE:FALSE);		
		m_GameClientView.m_btJetton1000000.EnableWindow((lLeaveScore>=1000000*iTimer && lUserMaxJetton>=1000000*iTimer)?TRUE:FALSE);		
		m_GameClientView.m_btJetton5000000.EnableWindow((lLeaveScore>=5000000*iTimer && lUserMaxJetton>=5000000*iTimer)?TRUE:FALSE);
		*/


	}else
	{
		for (int i = 0; i < MAX_JETTON_BUTTON_COUNT; i++)
		{
			pIJettonButton[i]->setColor(ccc3(100,100,100));
			pIJettonButton[i]->setTouchEnabled(false);
		}
		pIJettonSelect->setVisible(false);
	}
}
//����״̬
void GameControlOxHundred::updateState(){
	switch (DataModel::sharedDataModel()->getMainSceneOxHundred()->getGameState())
	{
			case MainSceneOxHundred::STATE_GAME_SHOW_CARE_FINISH:
			{
				showAllResult();
			}
			break;
			case MainSceneOxHundred::STATE_GAME_END:
				{
					resetData();
					for (int i = 0; i < MAX_SEAT_COUNT; i++)
					{
						pSeatData[i]->resetData();
					}
					//�������г���
					for (int i = 0; i < DataModel::sharedDataModel()->vecJettonNode.size(); i++)
					{
						DataModel::sharedDataModel()->vecJettonNode[i]->hideJetton();
					}
					//������
					getMainScene()->cardLayer->resetCard();
					showTimer();

				}
				break;
	default:
		break;
	}
}
//����ׯ��
void GameControlOxHundred::setBankerInfo(unsigned short  wBanker,long long lScore){
	//ׯ�����Ӻ�
	WORD wBankerUser=INVALID_CHAIR;

	//�������Ӻ�
	if (0!=wBanker)
	{
		map<long,tagUserInfo>::iterator it;
		for(it=DataModel::sharedDataModel()->mTagUserInfo.begin();it!=DataModel::sharedDataModel()->mTagUserInfo.end();it++)
		{
			if (it->second.wChairID==wBanker)
			{
					wBankerUser=wBanker;
			}
		}
		/*for (WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
		{
			IClientUserItem *pUserItem = GetClientUserItem(wChairID);
			if(pUserItem == NULL) continue;
			tagUserInfo  *pUserData=pUserItem->GetUserInfo();
			if (NULL!=pUserData && dwBankerUserID==pUserData->dwUserID)
			{
				wBankerUser=wChairID;
				break;
			}
		}*/
	}

	//�л��ж�
	if (m_wBankerUser!=wBankerUser)
	{
		m_wBankerUser=wBankerUser;
		//m_wBankerTime=0L;
		//m_lBankerWinScore=0L;	
		//m_lTmpBankerWinScore=0L;
	}
	m_lBankerScore=lScore;
}
//վ�����˳�
void GameControlOxHundred::standUpWithExit(){

	//tagUserInfo userInfo=DataModel::sharedDataModel()->mTagUserInfo.find(DataModel::sharedDataModel()->userInfo.dwUserID);
	CMD_GR_UserStandUp  userStandUp;
	userStandUp.wTableID=DataModel::sharedDataModel()->userInfo->wTableID;
	userStandUp.wChairID=DataModel::sharedDataModel()->userInfo->wChairID;
	userStandUp.cbForceLeave=true;
	if (userStandUp.wChairID==-10||userStandUp.wChairID>MAX_CHAIR)
	{
		TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_LOGON_ROOM);
		Tools::setTransitionAnimation(0, 0, GameLobbyScene::scene());
	}else
	{
		CCLog("-------userStandUp.wChairID :%d<<%s>>",userStandUp.wChairID,__FUNCTION__);
		//������Ϣ
		TCPSocketControl::sharedTCPSocketControl()->getTCPSocket(SOCKET_LOGON_ROOM)->SendData(MDM_GR_USER,SUB_GR_USER_STANDUP,&userStandUp,sizeof(userStandUp));
	}
}
//����ׯ��
void GameControlOxHundred::onApplyBanker(bool bApplyBanker){
	//��ǰ�ж�
	if (m_wBankerUser == DataModel::sharedDataModel()->userInfo->wChairID && bApplyBanker){
		return ;
	}

	if (bApplyBanker)
	{
		//������Ϣ
		TCPSocketControl::sharedTCPSocketControl()->getTCPSocket(SOCKET_LOGON_ROOM)->SendData(MDM_GF_GAME,SUB_C_APPLY_BANKER);
		//m_bMeApplyBanker=true;
	}else
	{
		//������Ϣ
		TCPSocketControl::sharedTCPSocketControl()->getTCPSocket(SOCKET_LOGON_ROOM)->SendData(MDM_GF_GAME,SUB_C_CANCEL_BANKER, NULL, 0);
		m_bMeApplyBanker=false;
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameControlOxHundred::onEventReadMessage(WORD wMainCmdID,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_GR_USER://�û���Ϣ
		onSubUserInfo(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GF_GAME://��Ϸ����
		onEventGameIng(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GF_FRAME://�������
		onSubGameFrame(wSubCmdID,pDataBuffer,wDataSize);
		break;
	default:
		CCLog("----------------main:%d sub:%d<<%s>>",wMainCmdID,wSubCmdID,__FUNCTION__);
		break;
	}
}
//�������
void GameControlOxHundred::onSubGameFrame(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_STATUS://��Ϸ״̬
		{
			//Ч�����
			assert(wDataSize==sizeof(CMD_GF_GameStatus));
			if (wDataSize!=sizeof(CMD_GF_GameStatus)) return ;
			//��Ϣ����
			CMD_GF_GameStatus * pGameStatus=(CMD_GF_GameStatus *)pDataBuffer;
			//���ñ���
			m_cbGameStatus=pGameStatus->cbGameStatus;
			m_bAllowLookon=pGameStatus->cbAllowLookon?true:false;
		}
		break;
	case SUB_GF_GAME_SCENE://��Ϸ����
		{
			switch (m_cbGameStatus)
			{
			case GAME_SCENE_FREE:
				{
					int ss=sizeof(CMD_S_StatusFree);
					//Ч������
					assert(wDataSize==sizeof(CMD_S_StatusFree));
					if (wDataSize!=sizeof(CMD_S_StatusFree)) return ;

					//��Ϣ����
					CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pDataBuffer;
					CCLog("GAME_SCENE_FREE<<%s>>",__FUNCTION__);
					m_lAreaLimitScore=pStatusFree->lAreaLimitScore;
				}
				break;
			case GAME_SCENE_PLACE_JETTON:		//��Ϸ״̬
			case GAME_SCENE_GAME_END:		//����״̬
				{
					//Ч������
					assert(wDataSize==sizeof(CMD_S_StatusPlay));
					if (wDataSize!=sizeof(CMD_S_StatusPlay)) return ;

					//��Ϣ����
					CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pDataBuffer;
					m_lAreaLimitScore=pStatusPlay->lAreaLimitScore;
					//��ע��Ϣ
					for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
					{
						m_lAllJettonScore[nAreaIndex]+=pStatusPlay->lAllJettonScore[nAreaIndex];
						pSeatData[nAreaIndex-1]->setAllJettonByAdd(pStatusPlay->lAllJettonScore[nAreaIndex]);
					}
					if (pStatusPlay->cbGameStatus==GAME_SCENE_GAME_END)
					{

					}
					CCLog("GAME_SCENE_PLACE_JETTON|GAME_SCENE_GAME_END<<%s>>",__FUNCTION__);
				}
				break;
			default:
				break;
			}
			
		}
		break;
	case SUB_GF_SYSTEM_MESSAGE://ϵͳ��Ϣ
		CCLog("SUB_GF_SYSTEM_MESSAGE<<%s>>",__FUNCTION__);
		break;
	default:
		CCLog("-:%d<<%s>>",wSubCmdID,__FUNCTION__);
		break;
	}
}
void GameControlOxHundred::onEventGameIng(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE://��Ϸ����
		onSubGameFree(pDataBuffer,wDataSize);
		break;
	case SUB_S_GAME_START://��Ϸ��ʼ
		onSubGameStart(pDataBuffer,wDataSize);
		break;
	case SUB_S_PLACE_JETTON://�û���ע
		onSubPlaceJetton(pDataBuffer,wDataSize,true);
		break;
	case SUB_S_GAME_END://��Ϸ����
		onSubGameEnd(pDataBuffer,wDataSize);
		break;
	case SUB_S_APPLY_BANKER://����ׯ��
		onSubUserApplyBanker(pDataBuffer,wDataSize);
		break;
	case SUB_S_CHANGE_BANKER://�л�ׯ��
		onSubChangeBanker(pDataBuffer,wDataSize);
		break;
	case SUB_S_CHANGE_USER_SCORE://���»���
		break;
	case SUB_S_SEND_RECORD://��Ϸ��¼
		onSubGameRecord(pDataBuffer,wDataSize);
		break;
	case SUB_S_PLACE_JETTON_FAIL://��עʧ��
		onSubPlaceJettonFail(pDataBuffer,wDataSize);
		break;
	case SUB_S_CANCEL_BANKER://ȡ������
		onSubUserCancelBanker(pDataBuffer,wDataSize);
		break;
	case SUB_S_SEND_ACCOUNT://�����˺�
		break;
	case SUB_S_ADMIN_CHEAK://��ѯ�˺�
		break;
	case SUB_S_QIANG_ZHUAN://��ׯ
		onQiangZhuanRet(pDataBuffer,wDataSize);
		break;
	case SUB_S_AMDIN_COMMAND://����Ա����
		break;
	default:
		CCLog("===========sub:%d<<%s>>",wSubCmdID,__FUNCTION__);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//��Ϸ����
void GameControlOxHundred::onSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	assert(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return ;
	//��Ϣ����
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;
	//����ʱ��
	resetTimer(pGameFree->cbTimeLeave,Tools::GBKToUTF8("���ּ�����ʼ"));
	CCLog("time:%d   count:%lld<<%s>>",pGameFree->cbTimeLeave,pGameFree->nListUserCount,__FUNCTION__);
	resetData();
	for (int i = 0; i < MAX_SEAT_COUNT; i++)
	{
		pSeatData[i]->resetData();
	}
	//�������г���
	for (int i = 0; i < DataModel::sharedDataModel()->vecJettonNode.size(); i++)
	{
		DataModel::sharedDataModel()->vecJettonNode[i]->hideJetton();
	}
	//������
	getMainScene()->cardLayer->resetCard();
	//���ÿ���״̬
	DataModel::sharedDataModel()->getMainSceneOxHundred()->setGameStateWithUpdate(MainSceneOxHundred::STATE_GAME_FREE);
	//�ٿظ���
	updateButtonContron();
}
//��Ϸ��ʼ
void GameControlOxHundred::onSubGameStart(const void * pBuffer, WORD wDataSize){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	m_lMeMaxScore=pGameStart->lUserMaxScore;
	//�����û���Ϣ
	tagUserInfo *uInfo=getUserInfo(pGameStart->wBankerUser); 
	if (uInfo)
	{
		pPlayerData[1]->setUserInfo(*uInfo);
	}
	
	CCLog("gameStart=time--:%d<<%s>>",pGameStart->cbTimeLeave,__FUNCTION__);
	//����ʱ��
	resetTimer(pGameStart->cbTimeLeave,Tools::GBKToUTF8("����ע"));
	//������ע״̬
	DataModel::sharedDataModel()->getMainSceneOxHundred()->setGameStateWithUpdate(MainSceneOxHundred::STATE_GAME_PLACE_JETTON);
	setBankerInfo(pGameStart->wBankerUser,pGameStart->lBankerScore);
	//�ٿظ���
	updateButtonContron();
}
//�û���ע
void GameControlOxHundred::onSubPlaceJetton(const void * pBuffer, WORD wDataSize,bool bGameMes){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return ;

	//��Ϣ����
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	pSeatData[pPlaceJetton->cbJettonArea-1]->setAllJettonByAdd(pPlaceJetton->lJettonScore);
	
	CCPoint posBegin=pBOnline->getPosition();
	if (DataModel::sharedDataModel()->userInfo->wChairID!=pPlaceJetton->wChairID)
	{
		m_lAllJettonScore[pPlaceJetton->cbJettonArea] += pPlaceJetton->lJettonScore;
	}else
	{
		posBegin=pPlayerData[0]->pIPlayerBg->getPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	

	//int fWidth=pSeatData[pPlaceJetton->cbJettonArea-1]->seatSize.width-jetton->getContentSize().width/2;
	//int fHeight=pSeatData[pPlaceJetton->cbJettonArea-1]->seatSize.height-jetton->getContentSize().height/2-70;
	int fWidth=pSeatData[pPlaceJetton->cbJettonArea-1]->seatSize.width-31;
	int fHeight=pSeatData[pPlaceJetton->cbJettonArea-1]->seatSize.height-70-31;

	int offsetX=rand()%fWidth;
	int offsetY=rand()%fHeight;
	CCPoint randPos=ccp(offsetX,offsetY);
	CCPoint pos=pSeatData[pPlaceJetton->cbJettonArea-1]->posCenter;
	pos=ccpAdd(pos,randPos);
	pos=ccpSub(pos,ccp(fWidth/2,fHeight/2));

	JettonNode *pJetton=getJettonNode();
	
	pJetton->setJettonTypeWithMove(pPlaceJetton->lJettonScore,posBegin,pos);
	//CCLog("chair:%d jettonScore: %lld<<%s>>",pPlaceJetton->wChairID,pPlaceJetton->lJettonScore,__FUNCTION__);
	updateButtonContron();
}
//����ׯ��
void GameControlOxHundred::onSubUserApplyBanker(const void * pBuffer, WORD wDataSize){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return ;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;
	//����ׯ���б�
	if (m_wBankerUser!=pApplyBanker->wApplyUser)
	{
		map<long,tagUserInfo>::iterator it;
		bool isFind=false;
		for(it=DataModel::sharedDataModel()->mTagUserInfo.begin();it!=DataModel::sharedDataModel()->mTagUserInfo.end();it++)
		{
			if (it->second.wChairID==pApplyBanker->wApplyUser)
			{
				tagApplyUser ApplyUser;
				ApplyUser.strUserName=it->second.szNickName;
				ApplyUser.lUserScore=it->second.lScore;
				listApplyUser.push_back(ApplyUser);
				isFind=true;
			}
		
		}
		if (!isFind)
		{
			CCLog("=no %d   %d-----------------------------<<%s>>",pApplyBanker->wApplyUser,DataModel::sharedDataModel()->userInfo->wChairID,__FUNCTION__);
			CCLog("<<%s>>",__FUNCTION__);
		}

		MTNotificationQueue::sharedNotificationQueue()->postNotification(UPDATE_BANK_LIST,NULL);
	}
	

	//CCLog("11============================  %d<<%s>>",pApplyBanker->wApplyUser,__FUNCTION__);
	/*
	IClientUserItem *pUserItem = GetTableUserItem(pApplyBanker->wApplyUser);
	tagUserInfo		*pUserData = NULL;
	if(pUserItem != NULL)
	pUserData = pUserItem->GetUserInfo();

	//�������
	if (m_wCurrentBanker!=pApplyBanker->wApplyUser && pUserData!=NULL)
	{
	tagApplyUser ApplyUser;
	ApplyUser.strUserName=pUserData->szNickName;
	ApplyUser.lUserScore=pUserData->lScore;
	m_GameClientView.m_ApplyUser.InserUser(ApplyUser);
	m_GameClientView.m_ApplyUser.m_AppyUserList.Invalidate(TRUE);

	if(m_GameClientView.m_ApplyUser.GetItemCount()>MAX_APPLY_DISPLAY) 
	{
	m_GameClientView.m_btUp.EnableWindow(true);
	m_GameClientView.m_btDown.EnableWindow(true);  

	}else
	{
	m_GameClientView.m_btUp.EnableWindow(false);
	m_GameClientView.m_btDown.EnableWindow(false); 
	}
	}
	*/
	//�Լ��ж�
	if ( DataModel::sharedDataModel()->userInfo->wChairID==pApplyBanker->wApplyUser) {
		m_bMeApplyBanker=true;
	}
	/*
	//���¿ؼ�
	UpdateButtonContron();
	m_GameClientView.m_btCancelBanker.EnableWindow(TRUE);
	*/
}
//ȡ����ׯ
void GameControlOxHundred::onSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	assert(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return ;

	//��Ϣ����
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;
	tagApplyUser ApplyUser;
	ApplyUser.strUserName=pCancelBanker->szCancelUser;

	//remove_if(listApplyUser.begin(),listApplyUser.end(),(std::string i){return strcmp(i.c_str(),ApplyUser.strUserName.c_str());});
	CCLog("%d<<%s>>",listApplyUser.size(),__FUNCTION__);
	std::list <tagApplyUser> ::iterator iter;
	for (iter = listApplyUser.begin(); iter != listApplyUser.end();)
	{
		if (strcmp(iter->strUserName.c_str(),ApplyUser.strUserName.c_str())==0)
		{
			//std::list<tagApplyUser>::iterator iter_e=iter;

			//listApplyUser.remove(ApplyUser);
			//CCLog("%s<<%s>>",iter->strUserName.c_str(),__FUNCTION__);
			listApplyUser.erase(iter++);
		}else
		{
			 iter++;
		}
	}
	//���͸����б�֪ͨ
	MTNotificationQueue::sharedNotificationQueue()->postNotification(UPDATE_BANK_LIST,NULL);

	/*tagApplyUser ApplyUser;
	ApplyUser.strUserName=pCancelBanker->szCancelUser;


	ApplyUser.lUserScore=0;


	m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);
	m_GameClientView.m_ApplyUser.m_AppyUserList.Invalidate(TRUE);

	if(m_GameClientView.m_ApplyUser.GetItemCount()>MAX_APPLY_DISPLAY)
	{
		m_GameClientView.m_btUp.EnableWindow(true);
		m_GameClientView.m_btDown.EnableWindow(true);  

	}else
	{
		m_GameClientView.m_btUp.EnableWindow(false);
		m_GameClientView.m_btDown.EnableWindow(false); 
	}*/

	//�Լ��ж�
	const tagUserInfo *pMeUserData=DataModel::sharedDataModel()->userInfo;
	if ( strcmp(pMeUserData->szNickName,pCancelBanker->szCancelUser)==0) 
		m_bMeApplyBanker=false;



	//���¿ؼ�
	updateButtonContron();
	//m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);
}
//�л�ׯ��
void GameControlOxHundred::onSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	assert(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return ;

	//��Ϣ����
	CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

	//��ʾͼƬ
	//m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

	//�Լ��ж�
	if (m_wBankerUser==DataModel::sharedDataModel()->userInfo->wChairID&& pChangeBanker->wBankerUser!=DataModel::sharedDataModel()->userInfo->wChairID) 
	{
		m_bMeApplyBanker=false;
	}
	else if ( pChangeBanker->wBankerUser==DataModel::sharedDataModel()->userInfo->wChairID)
	{
		m_bMeApplyBanker=true;
	}

	//ׯ����Ϣ
	setBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->lBankerScore);
	//m_GameClientView.SetBankerScore(0,0);
	//�����û���Ϣ
	tagUserInfo *uInfo=getUserInfo(pChangeBanker->wBankerUser); 
	if (uInfo)
	{
		pPlayerData[1]->setUserInfo(*uInfo);
	}
	//ɾ�����
	if (m_wBankerUser!=INVALID_CHAIR)
	{
		if (listApplyUser.size()>0)
		{
			listApplyUser.pop_front();
			MTNotificationQueue::sharedNotificationQueue()->postNotification(UPDATE_BANK_LIST,NULL);
		}
		/*IClientUserItem *pUserItem = GetTableUserItem(m_wCurrentBanker);
		tagUserInfo  *pBankerUserData = NULL;
		if(pUserItem!=NULL)
			pBankerUserData = pUserItem->GetUserInfo();

		if (pBankerUserData != NULL)
		{
			tagApplyUser ApplyUser;
			ApplyUser.strUserName = pBankerUserData->szNickName;
			m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);
			m_GameClientView.m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
		}*/
	}

	//���½���
	updateButtonContron();
	//m_GameClientView.InvalidGameView(0,0,0,0);

}
//��ׯ��Ϣ 
void GameControlOxHundred::onQiangZhuanRet( const void *pBuffer,WORD wDataSize )
{
	assert(wDataSize==sizeof(CMD_S_QiangZhuan));
	if(wDataSize!=sizeof(CMD_S_QiangZhuan)) return ;

	//��Ϣ����
	CMD_S_QiangZhuan * pCmd=(CMD_S_QiangZhuan *)pBuffer;
	CCLog("---<<%s>>",__FUNCTION__);
	for(int nIndex=pCmd->wSwap1;nIndex>pCmd->wSwap2;nIndex--)
	{
		list<tagApplyUser>::iterator iterSwap1=listApplyUser.begin();  advance( iterSwap1, nIndex );
		list<tagApplyUser>::iterator iterSwap2=listApplyUser.begin();  advance( iterSwap2, nIndex-1 );
		iter_swap( iterSwap1, iterSwap2 );
	}
	MTNotificationQueue::sharedNotificationQueue()->postNotification(UPDATE_BANK_LIST,NULL);
	//TCHAR szNameTemp[31],szScoreTemp[128]=TEXT("");
	/*tagApplyUser tagAppUser;
	for(int nIndex=pCmd->wSwap1;nIndex>pCmd->wSwap2;nIndex--)
	{
		//tagAppUser=listApplyUser.begin(1);
		//		crTemp	= m_AppyUserList.GetItemColor(nIndex-1);
		//m_AppyUserList.GetItemText(nIndex-1,0,szNameTemp,31);
		//m_AppyUserList.GetItemText(nIndex-1,1,szScoreTemp,128);

		//m_AppyUserList.SetItemText(nIndex,0,szNameTemp);
		//m_AppyUserList.SetItemText(nIndex,1,szScoreTemp);
		//m_AppyUserList.SetItemColor(nIndex,crTemp);
	}*/
	//_stprintf(szScorePer,_T("%I64d"),lMeMoney);
	//m_AppyUserList.SetItemText(wCurPos,0,szNamePer);
	//m_AppyUserList.SetItemText(wCurPos,1,szScorePer);

	//m_GameClientView.m_ApplyUser.ResetItemPos(pCmd->wSwap1,pCmd->wSwap2,pCmd->lMeMoney);
}
//��Ϸ��¼
void GameControlOxHundred::onSubGameRecord(const void * pBuffer, WORD wDataSize){
	//Ч�����
	assert(wDataSize%sizeof(tagServerGameRecord)==0);
	if (wDataSize%sizeof(tagServerGameRecord)!=0) return ;

	

	//���ü�¼
	WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
	for (WORD wIndex=0;wIndex<wRecordCount;wIndex++) 
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);
		tagGameRecord tRecord;
		memcpy(&tRecord,pServerGameRecord,sizeof(tagServerGameRecord));
		insertGameHistory(tRecord);
		//CCLog("==--::%d %d %d %d<<%s>>",pServerGameRecord->bWinShunMen, pServerGameRecord->bWinDuiMen, pServerGameRecord->bWinDaoMen,pServerGameRecord->bWinHuang,__FUNCTION__);
		//m_GameClientView.SetGameHistory(pServerGameRecord->bWinShunMen, pServerGameRecord->bWinDuiMen, pServerGameRecord->bWinDaoMen,pServerGameRecord->bWinHuang);
	}
	MTNotificationQueue::sharedNotificationQueue()->postNotification(UPDATE_LIST,NULL);
}
//������ʷ��¼
void GameControlOxHundred::insertGameHistory(tagGameRecord tagRecord){
	listGameRecord.push_back(tagRecord);
	if (listGameRecord.size()>MAX_SCORE_HISTORY)
	{
		listGameRecord.pop_front();
	}
}
//�ƶ�Ӯ��
void GameControlOxHundred::deduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan,bool &bWinHuan,BYTE &TianMultiple,BYTE &diMultiple,BYTE &TianXuanltiple,BYTE &HuangMultiple )
{
	//��С�Ƚ�
	bWinTian=CompareCard(getMainScene()->cardLayer->card[BANKER_INDEX],5,getMainScene()->cardLayer->card[SHUN_MEN_INDEX],5,TianMultiple)==1?true:false;
	bWinDi=CompareCard(getMainScene()->cardLayer->card[BANKER_INDEX],5,getMainScene()->cardLayer->card[DUI_MEN_INDEX],5,diMultiple)==1?true:false;
	bWinXuan=CompareCard(getMainScene()->cardLayer->card[BANKER_INDEX],5,getMainScene()->cardLayer->card[DAO_MEN_INDEX],5,TianXuanltiple)==1?true:false;
	bWinHuan=CompareCard(getMainScene()->cardLayer->card[BANKER_INDEX],5,getMainScene()->cardLayer->card[HUAN_MEN_INDEX],5,HuangMultiple)==1?true:false;
}
//��עʧ��
void GameControlOxHundred::onSubPlaceJettonFail(const void * pBuffer, WORD wDataSize){
	CCLog("======================jettonFail------------<<%s>>",__FUNCTION__);
	//Ч������
	assert(wDataSize==sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize!=sizeof(CMD_S_PlaceJettonFail)) return;

	//��Ϣ����
	CMD_S_PlaceJettonFail * pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;
	CCLog("%d<<%s>>",pPlaceJettonFail->wPlaceUser,__FUNCTION__);
	m_lAllJettonScore[pPlaceJettonFail->lJettonArea] -= pPlaceJettonFail->lPlaceScore;
	//�Լ��ж�
	if (DataModel::sharedDataModel()->userInfo->wChairID==pPlaceJettonFail->wPlaceUser 
		//&& false==IsLookonMode()
		)
	{
		//Ч�����
		BYTE cbViewIndex=pPlaceJettonFail->lJettonArea;
		long long lJettonCount=pPlaceJettonFail->lPlaceScore;
		//�Ϸ�У��
		assert(m_lUserJettonScore[cbViewIndex]>=lJettonCount);
		if (lJettonCount>m_lUserJettonScore[cbViewIndex]) return ;

		//������ע
		m_lUserJettonScore[cbViewIndex]-=lJettonCount;
		pSeatData[cbViewIndex-1]->setUserJetton(m_lUserJettonScore[cbViewIndex]);
	}
}
//��Ϸ����
void GameControlOxHundred::onSubGameEnd(const void * pBuffer, WORD wDataSize){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return;
	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	CCLog("end:%lld<<%s>>",pGameEnd->lUserScore,__FUNCTION__);
	//����ʱ��
	resetTimer(pGameEnd->cbTimeLeave,Tools::GBKToUTF8("��Ϣһ��..."));
	hideTimer(false);
	//�����ܽ���
	pPlayerData[0]->lGameScore=pGameEnd->lUserScore;
	DataModel::sharedDataModel()->userInfo->lScore+=pGameEnd->lUserScore;
	pPlayerData[0]->changePlayerGole(DataModel::sharedDataModel()->userInfo->lScore);

	
	pPlayerData[1]->lGameScore=pGameEnd->lBankerScore;
	tagUserInfo *uInfo =getUserInfo(m_wBankerUser);
	if (uInfo)
	{
		uInfo->lScore+=pGameEnd->lBankerScore;
		pPlayerData[1]->changePlayerGole(uInfo->lScore);
	}
	

	//����������
	for (int i = 0; i < sizeof(pGameEnd->cbTableCardArray)/sizeof(pGameEnd->cbTableCardArray[0]); i++)
	{
		for (int j = 0; j < sizeof(pGameEnd->cbTableCardArray[0]); j++)
		{
			getMainScene()->cardLayer->card[i][j]=pGameEnd->cbTableCardArray[i][j];
		}
	} 
	//�ƶ�Ӯ��
	bool bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang;
	BYTE TianMultiple,diMultiple,TianXuanltiple,HuangMultiple;
	deduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang,TianMultiple,diMultiple,TianXuanltiple,HuangMultiple);
	//m_GameClientView.SetGameHistory(pServerGameRecord->bWinShunMen, pServerGameRecord->bWinDuiMen, pServerGameRecord->bWinDaoMen,pServerGameRecord->bWinHuang);

	tagGameRecord tagRecord;
	tagRecord.bWinShunMen=bWinTianMen;
	tagRecord.bWinDuiMen=bWinDiMen;
	tagRecord.bWinDaoMen=bWinXuanMen;
	tagRecord.bWinHuang=bWinHuang;

	insertGameHistory(tagRecord);
	
	//���÷���״̬
	DataModel::sharedDataModel()->getMainSceneOxHundred()->setGameStateWithUpdate(MainSceneOxHundred::STATE_GAME_SEND_CARD);
	//�ٿظ���
	updateButtonContron();
}
//////////////////////////////////////////////////////////////////////////
//�û���Ϣ
void GameControlOxHundred::onSubUserInfo(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER://�û�����
		onSubUserEnter(pDataBuffer,wDataSize);
		break;
	case SUB_GR_USER_STATUS://�û�״̬
		onSubUserState(pDataBuffer,wDataSize);
		break;
	default:
		break;
	}
}
//�û�����
void GameControlOxHundred::onSubUserEnter(void * pDataBuffer, unsigned short wDataSize){
	//Ч�����
	assert(wDataSize>=sizeof(tagMobileUserInfoHead));
	if (wDataSize<sizeof(tagMobileUserInfoHead)) return ;
	//��Ϣ����
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)pDataBuffer;
	int wPacketSize=0;
	//��������
	tagUserInfo UserInfo;
	UserInfo.dwUserID=pUserInfoHead->dwUserID;
	UserInfo.dwGameID=pUserInfoHead->dwGameID;
	UserInfo.lScore=pUserInfoHead->lScore;
	UserInfo.wChairID=pUserInfoHead->wChairID;
	UserInfo.wTableID=pUserInfoHead->wTableID;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET + sizeof(TCP_Head)];
	CopyMemory(cbDataBuffer, pDataBuffer, wDataSize);

	wPacketSize+=sizeof(tagMobileUserInfoHead);
	while (true)
	{
		void * pDataBuffer1 = cbDataBuffer + wPacketSize;
		tagDataDescribe *DataDescribe = (tagDataDescribe*)pDataBuffer1;
		wPacketSize+=sizeof(tagDataDescribe);
		switch (DataDescribe->wDataDescribe)
		{
		case DTP_GR_NICK_NAME:		//�û��ǳ�
			{
				CopyMemory(&UserInfo.szNickName, cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szNickName[CountArray(UserInfo.szNickName)-1]=0;
				//CCLog("nick:%s  ch:%d<<%s>>",Tools::GBKToUTF8(UserInfo.szNickName),UserInfo.wChairID,__FUNCTION__);
			}
			break;
		case DTP_GR_GROUP_NAME:
			{
				CCLog("����");
			}
			break;
		case DTP_GR_UNDER_WRITE:
			{
				CopyMemory(UserInfo.szUnderWrite,cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite)-1]=0;
				CCLog("ǩ��:%s",Tools::GBKToUTF8(UserInfo.szUnderWrite));
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
	/*
	//Ч�����
	ASSERT(wDataSize>=sizeof(tagUserInfoHead));
	if (wDataSize<sizeof(tagUserInfoHead)) return false;

	//��������
	tagUserInfo UserInfo;
	tagCustomFaceInfo CustomFaceInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&CustomFaceInfo,sizeof(CustomFaceInfo));

	//��Ϣ����
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pData;

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//��������
	bool bHideUserInfo=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
	bool bMySelfUserItem=pGlobalUserData->dwUserID==pUserInfoHead->dwUserID;
	bool bMasterUserOrder=((pUserInfoHead->cbMasterOrder>0)||((m_pIMySelfUserItem!=NULL)&&(m_pIMySelfUserItem->GetMasterOrder()>0)));

	//��ȡ��Ϣ
	if ((bHideUserInfo==false)||(bMySelfUserItem==true)||(bMasterUserOrder==true))
	{
	//�û�����
	UserInfo.wFaceID=pUserInfoHead->wFaceID;
	UserInfo.dwGameID=pUserInfoHead->dwGameID;
	UserInfo.dwUserID=pUserInfoHead->dwUserID;
	UserInfo.dwGroupID=pUserInfoHead->dwGroupID;
	UserInfo.dwCustomID=pUserInfoHead->dwCustomID;

	//�û�״̬
	UserInfo.wTableID=pUserInfoHead->wTableID;
	UserInfo.wChairID=pUserInfoHead->wChairID;
	UserInfo.cbUserStatus=pUserInfoHead->cbUserStatus;

	//�û�����
	UserInfo.cbGender=pUserInfoHead->cbGender;
	UserInfo.cbMemberOrder=pUserInfoHead->cbMemberOrder;
	UserInfo.cbMasterOrder=pUserInfoHead->cbMasterOrder;

	//�û�����
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

	//��������
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pUserInfoHead+1,wDataSize-sizeof(tagUserInfoHead));

	//��չ��Ϣ
	while (true)
	{
	pDataBuffer=RecvPacket.GetData(DataDescribe);
	if (DataDescribe.wDataDescribe==DTP_NULL) break;
	switch (DataDescribe.wDataDescribe)
	{
	case DTP_GR_NICK_NAME:		//�û��ǳ�
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
	case DTP_GR_GROUP_NAME:		//�û�����
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
	case DTP_GR_UNDER_WRITE:	//����ǩ��
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

	//�Զ�ͷ��
	if (pUserInfoHead->dwCustomID!=0L)
	{
	//����ͷ��
	CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
	bool bSuccess=pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID,pUserInfoHead->dwCustomID,CustomFaceInfo);

	//����ͷ��
	if (bSuccess==false)
	{
	pCustomFaceManager->LoadUserCustomFace(pUserInfoHead->dwUserID,pUserInfoHead->dwCustomID);
	}
	}
	}
	else
	{
	//�û���Ϣ
	UserInfo.dwUserID=pUserInfoHead->dwUserID;
	lstrcpyn(UserInfo.szNickName,TEXT("��Ϸ���"),CountArray(UserInfo.szNickName));

	//�û�״̬
	UserInfo.wTableID=pUserInfoHead->wTableID;
	UserInfo.wChairID=pUserInfoHead->wChairID;
	UserInfo.cbUserStatus=pUserInfoHead->cbUserStatus;

	//�û�����
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

	//�����û�
	IClientUserItem * pIClientUserItem=m_PlazaUserManagerModule->SearchUserByUserID(UserInfo.dwUserID);
	if (pIClientUserItem==NULL) 
	{
	pIClientUserItem=m_PlazaUserManagerModule->ActiveUserItem(UserInfo,CustomFaceInfo);
	//m_ChatMessage.InsertSystemString(UserInfo.szNickName);
	}
	else
	{		//ɾ���û�

	m_ChatMessage.InsertSystemString(TEXT("ss1111111111111"));
	m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);
	pIClientUserItem=m_PlazaUserManagerModule->ActiveUserItem(UserInfo,CustomFaceInfo);
	}

	//��ȡ����
	CServerListData * pServerListData=CServerListData::GetInstance();

	//��������
	pServerListData->SetServerOnLineCount(m_GameServer.dwServerID,m_PlazaUserManagerModule->GetActiveUserCount());

	//��������
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//������ʾ
	if (((bHideUserInfo==false)&&(bMySelfUserItem==false))||(bMasterUserOrder==true))
	{
	if(pParameterGlobal->m_bNotifyFriendCome && pIClientUserItem->GetUserCompanion()==CP_FRIEND)
	{
	//��ʾ��Ϣ
	CString strDescribe;
	strDescribe.Format(TEXT("���ĺ��� [%s] �����ˣ�"),pIClientUserItem->GetNickName());
	m_ChatMessage.InsertSystemString(strDescribe);
	//m_ChatMessage.InsertUserEnter(pIClientUserItem->GetNickName());//InsertNormalString(strDescribe);	
	}
	}
	*/
}
//�û�״̬
void GameControlOxHundred::onSubUserState(void * pDataBuffer, unsigned short wDataSize){
	CMD_GR_UserStatus *info= (CMD_GR_UserStatus*)pDataBuffer;
	switch (info->UserStatus.cbUserStatus)
	{
	case US_SIT://����
		{
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID){
				DataModel::sharedDataModel()->userInfo->wTableID=info->UserStatus.wTableID;
				DataModel::sharedDataModel()->userInfo->wChairID=info->UserStatus.wChairID;
				CCLog("<<%s>>",__FUNCTION__);
			}
		}
		break;
	case US_FREE://վ��
		{
			map<long ,tagUserInfo >::iterator l_it;
			l_it=DataModel::sharedDataModel()->mTagUserInfo.find(info->dwUserID);
			if (l_it!=DataModel::sharedDataModel()->mTagUserInfo.end())
			{
				DataModel::sharedDataModel()->mTagUserInfo.erase(info->dwUserID);
			}
			/*std::vector<long> tVecRemove;
			std::map<long,tagUserInfo>::iterator iter;
			for (iter = DataModel::sharedDataModel()->mTagUserInfo.begin(); iter != DataModel::sharedDataModel()->mTagUserInfo.end(); iter++)
			{
				if (info->dwUserID==iter->second.dwUserID)
				{
					tVecRemove.push_back(info->dwUserID);
				}
			}
			for (int i = 0; i < tVecRemove.size(); i++)
			{
				DataModel::sharedDataModel()->mTagUserInfo.erase(tVecRemove[i]);
			}
			CCLog("tagSize:%d<<%s>>",DataModel::sharedDataModel()->mTagUserInfo.size(),__FUNCTION__);*/
			/*if (info->dwUserID==4947)
			{
				CCLog("-----------------------<<%s>>",__FUNCTION__);
			}*/
			if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID)
			{
				TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_LOGON_ROOM);
				Tools::setTransitionAnimation(0, 0, GameLobbyScene::scene());
				//MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_US_FREE,NULL);
			}else
			{

			}
		}
		break;
	case US_READY://ͬ��
		{
		}
		break;
	case US_PLAYING:
		{
		}
		break;
	default:
		CCLog("state==Other userID:%ld ״̬��%d<<%s>>",info->dwUserID,info->UserStatus.cbUserStatus,__FUNCTION__);
		break;
	}
}
//�û�״̬
void GameControlOxHundred::onSubUserState(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
}
