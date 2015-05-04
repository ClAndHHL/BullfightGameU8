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
GameControlOxHundred::GameControlOxHundred()
{

}
GameControlOxHundred::~GameControlOxHundred(){
	
	TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(SOCKET_LOGON_ROOM);
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
	initTimer(pWidget);
	initSeatData(pWidget);
}
void GameControlOxHundred::onExit(){
	CCLayer::onExit();
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
		//�������ĵ�
		pSeatData[i]->posCenter=bg->getPosition();
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
void GameControlOxHundred::onEventReadMessage(WORD wMainCmdID,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_GR_USER://�û���Ϣ
		//onSubUserState(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GF_GAME://��Ϸ����
		onEventGameIng(wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GF_FRAME://�������
		break;
	default:
		CCLog("main:%d sub:%d<<%s>>",wMainCmdID,wSubCmdID,__FUNCTION__);
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
		CCLog("sub:%d<<%s>>",wSubCmdID,__FUNCTION__);
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
	for (int i = 0; i < MAX_SEAT_COUNT; i++)
	{
		pSeatData[i]->resetData();
	}
}
//��Ϸ��ʼ
void GameControlOxHundred::onSubGameStart(const void * pBuffer, WORD wDataSize){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	CCLog("gameStart=time--:%d<<%s>>",pGameStart->cbTimeLeave,__FUNCTION__);
	//����ʱ��
	resetTimer(pGameStart->cbTimeLeave,Tools::GBKToUTF8("����ע"));
}
//�û���ע
void GameControlOxHundred::onSubPlaceJetton(const void * pBuffer, WORD wDataSize,bool bGameMes){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return ;

	//��Ϣ����
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	pSeatData[pPlaceJetton->cbJettonArea-1]->setAllJettonByAdd(pPlaceJetton->lJettonScore);
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
}
//��Ϸ����
void GameControlOxHundred::onSubGameEnd(const void * pBuffer, WORD wDataSize){
	//Ч������
	assert(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return;
	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	CCLog("end:%lld<<%s>>",pGameEnd->lBankerScore,__FUNCTION__);
	//����ʱ��
	resetTimer(pGameEnd->cbTimeLeave,Tools::GBKToUTF8("��Ϣһ��..."));
	//�������г���
	for (int i = 0; i < DataModel::sharedDataModel()->vecJettonNode.size(); i++)
	{
		DataModel::sharedDataModel()->vecJettonNode[i]->hideJetton();
	}
	/*for (int i = 0; i < 5; i++)
	{
		CCLog("------------------------------<<%s>>",__FUNCTION__);
		for (int j = 0; j < 5; j++)
		{
			BYTE valueOx=pGameEnd->cbTableCardArray[i][j]&LOGIC_MASK_VALUE;
			
			CCLog("%d<<%s>>",valueOx,__FUNCTION__);
		}
		CCLog("-----------------------<<%s>>",__FUNCTION__);
	}*/
	DataModel::sharedDataModel()->getMainSceneOxHundred()->setGameStateWithUpdate(MainSceneOxHundred::STATE_SEND_CARD);
}
//////////////////////////////////////////////////////////////////////////
//�û�״̬
void GameControlOxHundred::onSubUserState(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){

}