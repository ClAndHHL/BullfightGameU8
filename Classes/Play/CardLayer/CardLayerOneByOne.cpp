

#include "CardLayerOneByOne.h"
#include "../../Tools/GameConfig.h"
#include "Card.h"
#include "../../Tools/DataModel.h"
#include "../../Tools/BaseAttributes.h"
#define SELF_SEAT									3						//�Լ���λ��
CardLayerOneByOne::CardLayerOneByOne()
{
}
CardLayerOneByOne::~CardLayerOneByOne() {
	CCLog("~ <<%s>>",__FUNCTION__);
	this->removeAllChildrenWithCleanup(true);
}
void CardLayerOneByOne::onEnter(){
	CardLayerBase::onEnter();
}
void CardLayerOneByOne::onExit(){
	CardLayerBase::onExit();
}



//��ʾţţ
bool CardLayerOneByOne::promptOx(int oxIndex){
	BYTE tempCard[5];
	memcpy(tempCard, DataModel::sharedDataModel()->card[oxIndex], sizeof(tempCard));
	//bool isOxCard = GetOxCard(tempCard, 5);
	BYTE bCardValue = GetCardType(tempCard, MAX_COUNT, tempCard);
	if (bCardValue>0)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				if (tempCard[i]==DataModel::sharedDataModel()->card[oxIndex][j])
				{
					float originY=pCard[3*MAX_COUNT+j]->m_cpArmatureCard->getPositionY();
					pCard[3*MAX_COUNT+j]->m_cpArmatureCard->setPositionY(originY+30);
					break;
				}
			}
		}
		CCLog("niu:%d",GetCardType(tempCard,5));
	}
	return bCardValue>0;
}
//
//************************************
// ������ţţ��
// Parameter: int chairIDʵ������λ��
// Parameter: int showChairiD��ʾ����λ��
//************************************
void CardLayerOneByOne::sortingOx(int chairID,int showChairiD){
	//��ʾ����
	bool bOxSound = false;
	//�˿�����
	BYTE bCardData[MAX_COUNT];
	memcpy(bCardData, DataModel::sharedDataModel()->card[chairID], sizeof(bCardData));
	BYTE bCardValue = GetCardType(bCardData, MAX_COUNT, bCardData);
	CCLog("====:%d<<%s>>",bCardValue,__FUNCTION__);
	//assert(bCardValue > 0);
	float orgCradY = 2000;
	//����ţţ��˳��
	for (int i = 0; i < MAX_COUNT; i++)
	{
		int cardColor = GetCardColor(bCardData[i]);
		int cardValue = GetCardValue(bCardData[i]);
		pCard[showChairiD*MAX_COUNT + i]->changeCard(false, cardColor, cardValue, i, getCardScale(showChairiD));
		
		orgCradY = MIN(pCard[showChairiD*MAX_COUNT + i]->m_cpArmatureCard->getPositionY(), orgCradY);
	}
	//��ʾ���͵���
	showOxType(showChairiD, bCardValue);
	//������ʾ����Լ�����
	if (showChairiD==3)
	{
		CCPoint cardPos = CCPointZero;
		for (int i = 0; i < MAX_COUNT; i++)
		{
			CCArmature *pArmature = pCard[showChairiD*MAX_COUNT + i]->m_cpArmatureCard;
			if (i == 0)
			{
				cardPos = pArmature->getPosition();
				cardPos.y = orgCradY;
			}
			if (bCardValue == 0)
			{
				pArmature->setColor(ccc3(100, 100, 100));
			}
			else{
				if (i < 3)
				{
					pArmature->setColor(ccc3(100, 100, 100));
				}
			}
			int offsetSpace = 25;
			if (showChairiD == 3)
			{
				offsetSpace = 100;
			}
			CCPoint movePos = ccp(i*offsetSpace, 0);
			//movePos = designResolutionToFrame(movePos);

			pArmature->setPosition(cardPos);
			pArmature->runAction(CCMoveTo::create(0.01 + i*0.02, ccpAdd(cardPos, movePos)));
		}
	}
	

	 
	/*bool bOxSound = false;
	for (WORD i = 0; i<GAME_PLAYER; i++)
	{
		WORD wViewChairID = m_wViewChairID[i];
		if (i == GetMeChairID() && !IsLookonMode())continue;
		m_GameClientView.m_CardControl[wViewChairID].SetDisplayItem(true);

		//ţţ����
		if (m_cbHandCardData[i][0]>0)
		{
			if (m_bUserOxCard[i] == TRUE)
			{
				//�˿�����
				BYTE bCardData[MAX_COUNT];
				CopyMemory(bCardData, m_cbHandCardData[i], sizeof(bCardData));

				//��ȡţţ����
				//m_GameLogic.GetOxCard(bCardData,MAX_COUNT);

				BYTE bCardValue = m_GameLogic.GetCardType(bCardData, MAX_COUNT, bCardData);
				ASSERT(bCardValue > 0);

				//��������
				m_GameClientView.m_CardControl[wViewChairID].SetCardData(bCardData, 3);
				m_GameClientView.m_CardControlOx[wViewChairID].SetCardData(&bCardData[3], 2);

				//��ʾ����
				if (bCardValue >= 10)bOxSound = true;
				m_GameClientView.SetUserOxValue(wViewChairID, bCardValue);
			}
			else
			{
				//��ţ
				m_GameClientView.SetUserOxValue(wViewChairID, 0);
			}
		}
	}*/

	/*
	CCPoint cardPos = getMainScene()->posChair[showChairiD];

	BYTE tempCard[5];
	memcpy(tempCard, DataModel::sharedDataModel()->card[chairID], sizeof(tempCard));
	bool isOxCard = GetOxCard(tempCard, 5);
	if (isOxCard)
	{
		//����ţţ��˳��
		for (int i = 0; i < MAX_COUNT; i++)
		{
			int cardColor = GetCardColor(tempCard[i]);
			int cardValue = GetCardValue(tempCard[i]);
			pCard[showChairiD*MAX_COUNT+i]->changeCard(false,cardColor,cardValue,i,getCardScale(showChairiD));
		}
		
	}
	//��ʾ����
	int iValue=GetCardType(tempCard,5);
	if (iValue>10)
	{
		iValue=10;
	}
	showOxType(showChairiD,iValue);
	{
		int jumpSpace=0;
		for (int i = 0; i < MAX_COUNT; i++)
		{
			int offsetX=BaseAttributes::sharedAttributes()->iCardOffsetX[showChairiD]+120;
			int offsetY=BaseAttributes::sharedAttributes()->iCardOffsetY[showChairiD];
			int offsetSpace=BaseAttributes::sharedAttributes()->iCardOffsetSpace[showChairiD]-40;
		
			CCArmature *pArmature=pCard[showChairiD*MAX_COUNT+i]->m_cpArmatureCard;
			if (i>2&&isOxCard)
			{
				jumpSpace=0;
			}else 
			{
				pArmature->setColor(ccc3(100,100,100));
			}
			CCPoint offPos = ccp(offsetX,offsetY);
			offPos=designResolutionToFrame(offPos);

			CCPoint movePos = ccp(offsetX+i*offsetSpace+jumpSpace,offsetY);
			movePos=designResolutionToFrame(movePos);

			//pArmature->setScale(pArmature->getScale()-0.2);
			pArmature->setPosition(ccpAdd(cardPos,offPos));
			pArmature->runAction(CCMoveTo::create(0.01+i*0.02,ccpAdd(cardPos,movePos)));
		}
	}*/
}
void CardLayerOneByOne::showOxType(int chairiD,int oxType){
	float orgCradY=2000;
	for (int i = 0; i < MAX_COUNT; i++)
	{
		orgCradY=MIN(pCard[chairiD*MAX_COUNT+i]->m_cpArmatureCard->getPositionY(),orgCradY);
	}
	CCPoint cardPos=ccp(pCard[chairiD*MAX_COUNT+2]->m_cpArmatureCard->getPositionX(),orgCradY-20);

	pAOxType[chairiD]->setTag(oxType);
	pAOxType[chairiD]->setPosition(cardPos);
	pAOxType[chairiD]->setScale(getCardScale(chairiD));
	CCSequence *seq=CCSequence::create(CCDelayTime::create(0.01+5*0.02),
	CCCallFuncN::create(this,SEL_CallFuncN(&CardLayerOneByOne::onPlayOxAnimation)),
	NULL);
	pAOxType[chairiD]->runAction(seq);
}
void CardLayerOneByOne::onPlayOxAnimation(CCNode *obj){
	CCArmature *oxAnimation=(CCArmature*)obj;
	oxAnimation->setVisible(true);
	oxAnimation->getAnimation()->play(CCString::createWithFormat("Ox%d",oxAnimation->getTag())->getCString());
}
//������
void CardLayerOneByOne::sendCardIng(){
	sSendCardCount = 0;
	//ƫ������
	int offsetIndex = 0;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
//		int bankerUser=DataModel::sharedDataModel()->getMainSceneOxTwo()->getGameControlOxTwo()->getBankViewID();
		int bankerUser = 0;
		int index=(bankerUser+i)%MAX_PLAYER;
		if (canSendCard[index])
		{
			offsetIndex++;
			sendFiveCard(index, offsetIndex);
		}
		else{
			
		}
	}

	
}

//��5����
void CardLayerOneByOne::sendFiveCard(int index,int offsetIndex){
	CCPoint cardPos = getMainScene()->posChair[index];
	int jg = 25;
	if (index == 3)
	{
		jg = 100;
	}
	for (int i = 0; i < MAX_CARD_COUNT; i++)
	{
		
		pCard[i + index*MAX_COUNT]->m_cpArmatureCard->setScale(0.42);
		int offx = rand() % 3;
		int offy = rand() % 3;
		pCard[i + index*MAX_COUNT]->m_cpArmatureCard->setPosition(ccp(DataModel::sharedDataModel()->deviceSize.width / 2 + offx, SCENE_SIZE.height/2 + offy));
		float offsetX = i * jg - (4 * jg / 2);
		CCPoint offPos = ccp(offsetX, 0);
		moveCardAction(pCard[i + index*MAX_COUNT]->m_cpArmatureCard,
			//(index-offsetIndex)*SEND_CARD_DELAY_TIME*MAX_CARD_COUNT + i*SEND_CARD_DELAY_TIME, 
			offsetIndex*SEND_CARD_DELAY_TIME*MAX_CARD_COUNT + i*SEND_CARD_DELAY_TIME,
			ccpAdd(cardPos, offPos), index);
	}
}
//�ƶ�������
void CardLayerOneByOne::moveCardAction(CCArmature *armature, float fTime, CCPoint targetPos,int index){
	float moveSpeed = 0.35;
	CCDelayTime *delayTime = CCDelayTime::create(fTime);
	//CCMoveTo *moveTo = CCMoveTo::create(moveSpeed, targetPos);
	CCJumpTo *moveTo = CCJumpTo::create(moveSpeed, targetPos, 100, 1);
	CCScaleTo *scaleTo = CCScaleTo::create(moveSpeed, getCardScale(index));
	CCSpawn *spawn = CCSpawn::create(moveTo, scaleTo, NULL);
	CCCallFunc *callbackFunc = CCCallFunc::create(this, SEL_CallFunc(&CardLayerOneByOne::onSendCardFinish));
	CCSequence *seq = CCSequence::create(delayTime, spawn, callbackFunc, NULL);
	armature->runAction(seq);
}
//�����Ʒ���ص�
void CardLayerOneByOne::onSendCardFinish(){
	setSendCardState(SEND_STATE_WAIT);
	sSendCardCount++;
	if (sSendCardCount==getCurAllCardCount()*MAX_CARD_COUNT)
	{
		CCLog("CardLayer::onSendCardFinish-->sendFinish");
		getMainScene()->setGameStateWithUpdate(MainSceneBase::STATE_OPT_OX);
		//DataModel::sharedDataModel()->getm()->setGameStateWithUpdate(MainSceneOxTwo::STATE_OPT_OX);
		//DataModel::sharedDataModel()->getMainSceneOxTwo()->setServerStateWithUpdate(MainScene::STATE_FIGHT_BANKER);
		showCard(SELF_SEAT,DataModel::sharedDataModel()->userInfo->wChairID);
		sSendCardCount=0;
	}
}

void CardLayerOneByOne::updateServerState(){
	switch (getMainScene()->getServerState())
	{
	case MainSceneOxTwo::STATE_SEND_CARD:
	{
		sendCard();
	}
		break;
	default:
		break;
	}
}
void CardLayerOneByOne::updateGameState(){
	switch (getMainScene()->getGameState())
	{
	case MainSceneOxTwo::STATE_READY:
		{
			resetCard();
		}
		break;
	default:
		break;
	}
}
void CardLayerOneByOne::setCanSendCard(){
	canSendCard[0] = false;
	canSendCard[1] = false;
	canSendCard[2] = false;
	canSendCard[3] = false;
	canSendCard[4] = false;
	canSendCard[5] = false;
}
float CardLayerOneByOne::getCardScale(int index){
	if (index==SELF_SEAT)
	{
		return 0.8-(1-DataModel::sharedDataModel()->deviceSize.height/SCENE_SIZE.height);
	}
	return 0.5-(1-DataModel::sharedDataModel()->deviceSize.height/SCENE_SIZE.height);
}
//��ʾ��
void CardLayerOneByOne::showCard(int index,int dataIndex){
	int beginCardIndex=index*MAX_COUNT;
	for (int i = 0; i < MAX_COUNT; i++)
	{
		int cardColor = GetCardColor(DataModel::sharedDataModel()->card[dataIndex][i]);
		int cardValue = GetCardValue(DataModel::sharedDataModel()->card[dataIndex][i]);
		pCard[beginCardIndex + i]->changeCard(true, cardColor, cardValue, beginCardIndex + i, getCardScale(index));
	}
}


