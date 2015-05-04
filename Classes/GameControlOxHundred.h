//
//
//
#pragma once
#include "GameControlBase.h"
#include "SeatData.h"
#include "JettonNode.h"
class MainSceneBase;
#define MAX_SEAT_COUNT 4
class GameControlOxHundred:public GameControlBase
{
private:
	SeatData *pSeatData[MAX_SEAT_COUNT];
	UIButton *pBOnline ;
public:
	GameControlOxHundred();
	~GameControlOxHundred();
	virtual void onEnter();
	virtual void onExit();
    CREATE_FUNC(GameControlOxHundred);
private:
	MainSceneBase*getMainScene();
	//��ʼ����ʱ��
	void initTimer(UILayer *pWidget);
	//��ʼ����λ
	void initSeatData(UILayer *pWidget);
	//��ȡ�������
	JettonNode *getJettonNode();
private:
	 int getChairIndex(int meChairID,int chairID);
	//�˵�////////////////////////////////////////////////////////////////////////
	void onMenuBack(CCObject* pSender, TouchEventType type);

	virtual void update(float delta);
	//���¼�ʱ��
	virtual void delayedAction();
	//��ȡ������Ϣ�ص��¼�
	virtual void onEventReadMessage(WORD wMainCmdID,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize);
	//��Ϸ��
	void onEventGameIng(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize);
	//�û�״̬
	virtual void onSubUserState(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize);
	//////////////////////////////////////////////////////////////////////////
	//��Ϸ����
	void onSubGameFree(const void * pBuffer, WORD wDataSize);
	//��ʼ��Ϸ
	void onSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û���ע
	void onSubPlaceJetton(const void * pBuffer, WORD wDataSize,bool bGameMes);
	//��Ϸ����
	void onSubGameEnd(const void * pBuffer, WORD wDataSize);
};
