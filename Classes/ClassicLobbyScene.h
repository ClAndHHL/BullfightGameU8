//
//  ClassicLobbyScene.h
//	经典厅
//
#pragma once
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;
class ClassicLobbyScene:public CCLayer
{
private:
	enum GameLevel
	{
		LEVEL_0=1,//初级
	};
public:
    ClassicLobbyScene();
    ~ClassicLobbyScene();
	virtual void onEnter();
	virtual void onExit();

    static CCScene* scene();
    CREATE_FUNC(ClassicLobbyScene);
private:
	void initTCPLogon();
	//弹出框
	void popDialogBoxUserInfo();
	//菜单设置用户信息
	void menuResetUser(CCObject* pSender, TouchEventType type);
	void menuStar(CCObject* pSender, TouchEventType type);

	void enterMainSceneByMode(int mode);
};