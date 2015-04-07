#pragma once
#include "cocos2d.h"
#include "MainScene.h"
#include "GameLogic.h"
#include "CMD_LogonServer.h"
USING_NS_CC;
class DataModel:public CCObject{
public:
	DataModel();
	~DataModel();
	static DataModel* sharedDataModel();
private:
	void initDataModel();
public:
	static bool isSound;
	static bool isMusic;
	bool isSit;
	CCSize deviceSize;

	CC_SYNTHESIZE(MainScene *, mainScene, MainScene);
	
	GameLogic *gameLogic;

	CCArray *m_aTagGameKind;

	std::vector <tagGameServer *> tagGameServerList;

	CMD_MB_LogonSuccess *logonSuccessUserInfo;
	int m_lTurnMaxScore;//����ע��
	BYTE card[2][5];
	//tagGameServer *tagGameServerList[];
};
