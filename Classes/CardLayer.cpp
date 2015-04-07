

#include "CardLayer.h"
#include "GameConfig.h"
#include "Card.h"
#include "DataModel.h"
#define MAX_CARD_COUNT								5						//�������
#define SEND_CARD_DELAY_TIME						0.1						//������ʱʱ�� 
CardLayer::CardLayer()
:sendCardState(SEND_STATE_WAIT)
, sSendCardCount(0)
{
}
CardLayer::~CardLayer() {
}
bool CardLayer::init(){
	if (!CCLayer::init())
	{
		return false;
	}
	//��ʼ���˿˼�
	batchCard = CCBatchNode::create();
	this->addChild(batchCard, 1);
	//�����˿˶����ļ�
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(CCS_PATH_SCENE(AnimationCard.ExportJson));

	setCanSendCard();
	return true;
}
void CardLayer::onEnter(){
	CCLayer::onEnter();
}
void CardLayer::onExit(){

}
//����
void CardLayer::sendCard(){
	switch (getSendCardState())
	{
	case SEND_STATE_WAIT:
	{
		setSendCardState(SEND_STATE_ING);
		sendCard();
	}
		break;
	case SEND_STATE_ING:
	{
		sendCardIng();
	}
		break;
	default:
		break;
	}
	/*BYTE mCrad[5] = {0x02,0x11,0x22,8,6};

	BYTE cardColor;
	BYTE cardValue;
	for (int i = 0; i < 5;i++)
	{
		cardColor = DataModel::sharedDataModel()->gameLogic->GetCardColor(mCrad[i]);
		cardValue = DataModel::sharedDataModel()->gameLogic->GetCardValue(mCrad[i]);

		Card *card = Card::create();
		card->createCardArmature(batchCard,cardColor/16,cardValue);
		this->addChild(card);
		card->setTag(i);
		card->m_cpArmatureCard->setPosition(ccp(100+i*140, 120));
	}
	
	BYTE tempCard[5];
	CopyMemory(tempCard, mCrad, sizeof(tempCard));
	bool isOxCard = DataModel::sharedDataModel()->gameLogic->GetOxCard(tempCard, 5);
	if (isOxCard)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				if (tempCard[i]==mCrad[j])
				{
					CCLog("--%d", j);
					Card *card = (Card*)this->getChildByTag(j);
					float y = card->m_cpArmatureCard->getPositionY() + 30;
					card->m_cpArmatureCard->setPositionY(y);
					break;
				}
			}
		}
	}*/
}
void CardLayer::sendCardIng(){
	Card *card = Card::create();
	card->createCardArmature(batchCard, 5, 0,0);
	this->addChild(card);
	card->m_cpArmatureCard->setScale(0.3);
	card->m_cpArmatureCard->setPosition(ccp(DataModel::sharedDataModel()->deviceSize.width/2, DataModel::sharedDataModel()->deviceSize.height/2));
	//ƫ������
	int offsetIndex = 0;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (canSendCard[i])
		{
			sendFiveCard(i, offsetIndex);
		}
		else{
			offsetIndex++;
		}
	}
	
}
//��5����
void CardLayer::sendFiveCard(int index,int offsetIndex){
	UIPanel *playerPanel = DataModel::sharedDataModel()->getMainScene()->playerLayer->playerPanel[index];
	UIImageView *iPlayerIcon = (UIImageView*)playerPanel->getChildByName("headPortrait");
	CCPoint playerPos = playerPanel->getPosition();
	CCPoint cardPos = ccpAdd(playerPos, iPlayerIcon->getPosition());
	for (int i = 0; i < MAX_CARD_COUNT; i++)
	{
		Card *cardMove = Card::create();
		if (index==2)
		{
			//BYTE mCrad[5] =DataModel::sharedDataModel()->card[1];
			int cardColor = DataModel::sharedDataModel()->gameLogic->GetCardColor(DataModel::sharedDataModel()->card[1][i])/16;
			int cardValue = DataModel::sharedDataModel()->gameLogic->GetCardValue(DataModel::sharedDataModel()->card[1][i]);
			CCLog("Color:%d   values:%d",cardColor,cardValue);
			cardMove->createCardArmature(batchCard, cardColor, cardValue, 1);
		}else
		{
			cardMove->createCardArmature(batchCard, 5, 0, 1);
		}
		
		this->addChild(cardMove);
		cardMove->m_cpArmatureCard->setScale(0.3);
		int offx = rand() % 3;
		int offy = rand() % 3;
		cardMove->m_cpArmatureCard->setPosition(ccp(DataModel::sharedDataModel()->deviceSize.width / 2 + offx, DataModel::sharedDataModel()->deviceSize.height / 2 + offy));
		CCPoint offPos = ccp(60+i*50,0);
		moveCardAction(cardMove->m_cpArmatureCard, (index-offsetIndex)*SEND_CARD_DELAY_TIME*MAX_CARD_COUNT + i*SEND_CARD_DELAY_TIME, ccpAdd(cardPos, offPos),index);
	}

}
//�ƶ�������
void CardLayer::moveCardAction(CCArmature *armature, float fTime, CCPoint targetPos,int index){
	float moveSpeed=0.2;
	CCDelayTime *delayTime = CCDelayTime::create(fTime);
	CCMoveTo *moveTo = CCMoveTo::create(moveSpeed, targetPos);
	CCScaleTo *scaleTo = CCScaleTo::create(moveSpeed, getCardScale(index));
	CCSpawn *spawn = CCSpawn::create(moveTo,scaleTo, NULL);
	CCCallFunc *callbackFunc = CCCallFunc::create(this,SEL_CallFunc(&CardLayer::onSendCardFinish));
	CCSequence *seq = CCSequence::create(delayTime,spawn,callbackFunc,NULL);
	armature->runAction(seq);

}
void CardLayer::onSendCardFinish(){
	sSendCardCount++;
	if (sSendCardCount==getCurAllCardCount()*MAX_CARD_COUNT)
	{
		DataModel::sharedDataModel()->getMainScene()->setGameStateWithUpdate(MainScene::STATE_OPT_OX);
		//DataModel::sharedDataModel()->getMainScene()->setServerStateWithUpdate(MainScene::STATE_FIGHT_BANKER);
	}
}
short CardLayer::getCurAllCardCount(){
	int count = 0;
	for (int i = 0; i < MAX_CARD_COUNT; i++)
	{
		if (canSendCard[i])
		{
			count++;
		}
	}
	return count;
}
void CardLayer::updateState(){
	switch (DataModel::sharedDataModel()->getMainScene()->getServerState())
	{
	case MainScene::STATE_SEND_CARD:
	{
		sendCard();
	}
		break;
	default:
		break;
	}
}
void CardLayer::setCanSendCard(){
	canSendCard[0] = false;
	canSendCard[1] = true;
	canSendCard[2] = true;
	canSendCard[3] = true;
	canSendCard[4] = true;

}
float CardLayer::getCardScale(int index){
	if (index==2)
	{
		return 0.5;
	}
	return 0.3;
}