//
//
//
//

#include "GameControlOxHundred.h"
#include "GameConfig.h"
#include "Tools.h"
#include "GameLobbyScene.h"
#include "DataModel.h"
#include "cmd_game.h"
#include "MainSceneBase.h"
#include "PacketAide.h"
GameControlOxHundred::GameControlOxHundred()
:iCurSelectJettonIndex(0)
,m_lMeMaxScore(0)
{
	nJetton[0]=1000;
	nJetton[1]=5000;
	nJetton[2]=10000;
	nJetton[3]=100000;
	nJetton[4]=500000;
	resetData();
}
GameControlOxHundred::~GameControlOxHundred(){
	TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(SOCKET_LOGON_ROOM);
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

	pBOnline= static_cast<UIButton*>(pWidget->getWidgetByName("ButtonOnline"));
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
	//ׯ����Ϣ
	m_wBankerUser=INVALID_CHAIR;		
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
MainSceneBase*GameControlOxHundred::getMainScene(){
	return (MainSceneBase*)this->getParent();
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

		pSeatData[i]->resetData();
	}
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
			TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_LOGON_ROOM);
			Tools::setTransitionAnimation(0, 0, GameLobbyScene::scene());
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
void GameControlOxHundred::update(float delta){
	MessageQueue::update(delta);
}
void GameControlOxHundred::delayedAction(){
	switch (DataModel::sharedDataModel()->getMainSceneOxHundred()->getGameState())
	{
	case MainSceneOxHundred::STATE_GAME_FREE:
		{
			hideTimer();
			
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
	CCLog("=lBankerScore:%lld    %lld<<%s>>",lBankerScore,m_lBankerScore,__FUNCTION__);
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
	{
		lBankerScore-=m_lAllJettonScore[nAreaIndex]*iTimer;
		CCLog("%d ---------%lld          <<%s>>",nAreaIndex,m_lAllJettonScore[nAreaIndex],__FUNCTION__);
	}
	CCLog("===lBankerScore:%lld<<%s>>",lBankerScore,__FUNCTION__);
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
	lMeMaxScore=min(lMeMaxScore,lBankerScore);

	lMeMaxScore /= iTimer; 

	//��������
	//ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);
	
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
	//////////////////////////////////////////////////////////////////////////
	if (bEnablePlaceJetton)
	{
		for (int i = 0; i < MAX_JETTON_BUTTON_COUNT; i++)
		{
			pIJettonButton[i]->setColor(ccc3(255,255,255));
			pIJettonButton[i]->setTouchEnabled(true);
			pIJettonButton[0]->setVisible(true);
		}
		pIJettonSelect->setVisible(true);
		//////////////////////////////////////////////////////////////////////////
		//�������
		long long lCurrentJetton=nJetton[iCurSelectJettonIndex];//��ǰ����
		long long lLeaveScore=m_lMeMaxScore;//�ҵĽ��
		for (int nAreaIndex=1; nAreaIndex<=MAX_AREA_COUNT; ++nAreaIndex) lLeaveScore -= m_lUserJettonScore[nAreaIndex];
		
		//�����ע
		long long lUserMaxJetton=getUserMaxJetton();
		CCLog("---�����ע:%lld<<%s>>",lUserMaxJetton,__FUNCTION__);
		//���ù��
		lLeaveScore = min((lLeaveScore/10),lUserMaxJetton); //�û����·� �����ֱȽ� �������屶 
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
		pIJettonButton[0]->setVisible((lLeaveScore>=1000*iTimer && lUserMaxJetton>=1000*iTimer)?true:false);
		pIJettonButton[1]->setVisible((lLeaveScore>=5000*iTimer && lUserMaxJetton>=5000*iTimer)?true:false);
		pIJettonButton[2]->setVisible((lLeaveScore>=10000*iTimer && lUserMaxJetton>=10000*iTimer)?true:false);
		pIJettonButton[3]->setVisible((lLeaveScore>=100000*iTimer && lUserMaxJetton>=100000*iTimer)?true:false);
		pIJettonButton[4]->setVisible((lLeaveScore>=500000*iTimer && lUserMaxJetton>=500000*iTimer)?true:false);
		CCLog("%lld    %lld<<%s>>",lLeaveScore,lUserMaxJetton,__FUNCTION__);
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
//����ׯ��
void GameControlOxHundred::setBankerInfo(unsigned short  wBanker,long long lScore){
	//ׯ�����Ӻ�
	WORD wBankerUser=INVALID_CHAIR;
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
void GameControlOxHundred::onEventReadMessage(WORD wMainCmdID,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_GR_USER://�û���Ϣ
		onSubUserState(wSubCmdID,pDataBuffer,wDataSize);
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
		break;
	case SUB_S_CHANGE_BANKER://�л�ׯ��
		break;
	case SUB_S_CHANGE_USER_SCORE://���»���
		break;
	case SUB_S_SEND_RECORD://��Ϸ��¼
		break;
	case SUB_S_PLACE_JETTON_FAIL://��עʧ��
		onSubPlaceJettonFail(pDataBuffer,wDataSize);
		break;
	case SUB_S_CANCEL_BANKER://ȡ������
		break;
	case SUB_S_SEND_ACCOUNT://�����˺�
		break;
	case SUB_S_ADMIN_CHEAK://��ѯ�˺�
		break;
	case SUB_S_QIANG_ZHUAN://��ׯ
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
	m_lAllJettonScore[pPlaceJetton->cbJettonArea] += pPlaceJetton->lJettonScore;
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
	pJetton->setJettonTypeWithMove(pPlaceJetton->lJettonScore,pBOnline->getPosition(),pos);
	CCLog("chair:%d jettonScore: %lld<<%s>>",pPlaceJetton->wChairID,pPlaceJetton->lJettonScore,__FUNCTION__);
	updateButtonContron();
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
	
	//����������
	for (int i = 0; i < sizeof(pGameEnd->cbTableCardArray)/sizeof(pGameEnd->cbTableCardArray[0]); i++)
	{
		for (int j = 0; j < sizeof(pGameEnd->cbTableCardArray[0]); j++)
		{
			getMainScene()->cardLayer->card[i][j]=pGameEnd->cbTableCardArray[i][j];
		}
	} 
	//���÷���״̬
	DataModel::sharedDataModel()->getMainSceneOxHundred()->setGameStateWithUpdate(MainSceneOxHundred::STATE_GAME_SEND_CARD);
	//�ٿظ���
	updateButtonContron();
}
//////////////////////////////////////////////////////////////////////////
//�û�״̬
void GameControlOxHundred::onSubUserState(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_USER_STATUS://�û�״̬
		{
			CMD_GR_UserStatus *info= (CMD_GR_UserStatus*)pDataBuffer;
			switch (info->UserStatus.cbUserStatus)
			{
			case US_SIT://����
				{
					if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID){
						DataModel::sharedDataModel()->userInfo->wTableID=info->UserStatus.wTableID;
						DataModel::sharedDataModel()->userInfo->wChairID=info->UserStatus.wChairID;
					}
				}
				break;
			case US_FREE://վ��
				{
					if (info->dwUserID==DataModel::sharedDataModel()->userInfo->dwUserID)
					{
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
		break;
	default:
		break;
	}
	
}
