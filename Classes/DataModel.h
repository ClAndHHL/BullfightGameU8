#pragma once
#include "cocos2d.h"
#include "MainScene.h"
#include "CMD_LogonServer.h"
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
	CC_SYNTHESIZE(MainScene *, mainScene, MainScene);

	CCArray *m_aTagGameKind;

	std::vector <tagGameServer *> tagGameServerList;
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
public:
	DataModel();
	~DataModel();
	static DataModel* sharedDataModel();
private:
	void initDataModel();
public:
	//����vector
	void sortVector();
	//�Ƴ������б�
	void removeTagGameServerList();
};
