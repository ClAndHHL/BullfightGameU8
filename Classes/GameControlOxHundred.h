//
//
//
#pragma once
#include "GameControlBase.h"
#include "SeatData.h"
#include "JettonNode.h"
//#include "cmd_game.h"
#include "PlayerDataHundred.h"
class MainSceneBase;
#define MAX_SEAT_COUNT 4
#define MAX_JETTON_BUTTON_COUNT			5			//���밴������
#define MAX_AREA_COUNT								4			//������Ŀ
#define MAX_PLAYER_HUNDRED_COUNT			2			//����ţţ�û���
class GameControlOxHundred:public GameControlBase
{
	//������Ϣ
protected:
	//LONGLONG						m_lMeMaxScore;						//�����ע
	long long						m_lAreaLimitScore;					//��������
	//��ע��Ϣ
protected:
	long long						m_lUserJettonScore[MAX_AREA_COUNT+1];	//������ע
	long long						m_lAllJettonScore[MAX_AREA_COUNT+1];		//ȫ����ע
	//ׯ����Ϣ
protected:	
	WORD							m_wBankerUser;						//��ǰׯ��
	long long						m_lBankerScore;						//ׯ�һ���

private:
	long long						m_lMeMaxScore;						//�����ע
	int nJetton[MAX_JETTON_BUTTON_COUNT];
	SeatData *pSeatData[MAX_SEAT_COUNT];
	UIButton *pBOnline ;
	//���밴ť
	UIImageView *pIJettonButton[MAX_JETTON_BUTTON_COUNT];
	//����ѡ����
	UIImageView *pIJettonSelect;
	//��ǰ��ѡ��������
	int iCurSelectJettonIndex;
	//�û���Ϣ
	PlayerDataHundred *pPlayerData[MAX_PLAYER_HUNDRED_COUNT];
	//��ׯ����
	UIImageView *pIUpBank;
	//��Ϸ״̬
protected:
	bool								m_bAllowLookon;						//�����Թ�
	BYTE							m_cbGameStatus;						//��Ϸ״̬
public:
	GameControlOxHundred();
	~GameControlOxHundred();
	virtual void onEnter();
	virtual void onExit();
    CREATE_FUNC(GameControlOxHundred);
	//������λ����
	void setSeatResult(int iSeatIndex,int iOXType);
	//��ʾ�ܽ��
	void showAllResult();
	//����״̬
	virtual void updateState();
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
	//�������
	void onSubGameFrame(WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize);
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
	//��עʧ��
	void onSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	void onSubGameEnd(const void * pBuffer, WORD wDataSize);
private:
	//��ע����
	void onMenuPlaceJetton(CCObject* pSender, TouchEventType type);
	//ѡ�����
	void onMenuSelectJetton(CCObject* pSender, TouchEventType type);
	//��ׯ
	void onMenuUpBank(CCObject* pSender, TouchEventType type);
	//�����ע
	long long getUserMaxJetton();
	//���¿���
	void updateButtonContron();
	//����ׯ��
	void setBankerInfo(unsigned short  wBanker,long long lScore);
	//��������
	void resetData();
	
};
