//
//  MainSceneOxHundred.h
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//

#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainSceneBase.h"
#include "../Play/GameControl/GameControlOxHundred.h"
#include "../Network/TCPSocket/TCPSocket.h"
#include "../Play/PlayerLayer/PlayerLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;
class MainSceneOxHundred:public TCPSocket,public MainSceneBase
{
public:
	//游戏状态
	enum GameState
	{
		STATE_GAME_FREE=0,							//空闲状态
		STATE_GAME_PLACE_JETTON,				//下注
		STATE_GAME_SEND_CARD,					//发牌
		STATE_GAME_SHOW_CARE_FINISH,		//显示牌完成
		STATE_GAME_END,								//结算
	};
	CC_SYNTHESIZE(GameState,gameState,GameState);
	CC_SYNTHESIZE(GameState,serverState,ServerState);

	CC_SYNTHESIZE(GameControlOxHundred *, gameControl, GameControlOxHundred);
	//玩家信息层
	PlayerLayer *playerLayer;
private:
public:
    MainSceneOxHundred();
    ~MainSceneOxHundred();
	virtual void onEnter();
	virtual void onExit();
    static CCScene* scene();
   // virtual bool init();
    CREATE_FUNC(MainSceneOxHundred);
	

	//设置状态并更新
	void setGameStateWithUpdate(GameState gameState);
	void setServerStateWithUpdate(GameState serverState);
private:
	//初始化
	void initGameControl();
	//初始化扑克层
	void initCardLayer();
	//初始化玩家信息层
	void initPlayerLayer();
	//添加背景
	void addBg();
	//更新状态
	void updateGameState();
	void updateServerState();
};
