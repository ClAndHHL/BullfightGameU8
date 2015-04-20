#pragma once
#include "cocos2d.h"
#include "MainScene.h"
//#include "GameLogic.h"
#include "CMD_LogonServer.h"
USING_NS_CC;
class DataModel:public CCObject{
public:
	static bool isSound;
	static bool isMusic;
	bool isSit;
	CCSize deviceSize;

	CC_SYNTHESIZE(MainScene *, mainScene, MainScene);

	CCArray *m_aTagGameKind;

	std::vector <tagGameServer *> tagGameServerList;
	//�û���Ϣ����
	std::map<long ,tagUserInfo>mTagUserInfo;
	//std::list<QueueData *> queueData;
	CMD_MB_LogonSuccess *logonSuccessUserInfo;
	long long m_lTurnMaxScore;//����ע��
	BYTE card[2][5];
	//tagGameServer *tagGameServerList[];
	//��������
	tagUserInfo *userInfo;
public:
	DataModel();
	~DataModel();
	static DataModel* sharedDataModel();
private:
	void initDataModel();
	//bool less_second(const tagGameServer & m1, const tagGameServer & m2);
public:
	//����vector
	void sortVector();
	void removeTagGameServerList();
};
