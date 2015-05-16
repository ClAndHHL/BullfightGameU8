#pragma once
#include "cocos2d.h"
#include "../MainScene/MainSceneOxTwo.h"
#include "../MainScene/MainSceneOxHundred.h"
#include "../MainScene/MainSceneOxOneByOne.h"
#include "../Network/CMD_Server/CMD_LogonServer.h"
#include "../Play/OxHundred/JettonNode.h"
USING_NS_CC;
class DataModel:public CCObject{
public:
	//��Ч����
	static bool isSound;
	//���ֿ���
	static bool isMusic;
	//�Ƿ�����
	bool isSit;
	CCSize deviceSize;
	//��¼�ʺ�
	std::string sLogonAccount;
	//��¼����
	std::string sLogonPassword;
	//����������
	CC_SYNTHESIZE(MainSceneOxTwo *, mainSceneOxTwo, MainSceneOxTwo);
	CC_SYNTHESIZE(MainSceneOxHundred *, mainSceneOxHundred, MainSceneOxHundred);
	CC_SYNTHESIZE(MainSceneOxOneByOne *, mainSceneOxOneByOne, MainSceneOxOneByOne);

	CCArray *m_aTagGameKind;

	std::vector <tagGameServer *> tagGameServerListOxTwo;
	std::vector <tagGameServer *> tagGameServerListOxHundred;
	std::vector <tagGameServer *> tagGameServerListOxOneByOne;
	//�û���Ϣ����
	std::map<long ,tagUserInfo>mTagUserInfo;
	//��¼�ɹ���Ϣ
	//CMD_MB_LogonSuccess *logonSuccessUserInfo;
	//����ע��
	long long m_lTurnMaxScore;
	//������
	BYTE card[2][5];
	//��������
	tagUserInfo *userInfo;
	//��Ϣ����
	std::queue<ReadData>readDataQueue;
	//����
	std::vector<JettonNode *>vecJettonNode;
public:
	DataModel();
	~DataModel();
	static DataModel* sharedDataModel();
private:
	void initDataModel();
public:
	//����vector
	void sortVector(std::vector <tagGameServer *> vTagGameServer);
	//�Ƴ������б�
	void removeTagGameServerList(std::vector <tagGameServer *> vTagGameServer);
};
