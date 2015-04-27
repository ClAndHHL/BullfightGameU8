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
#include "GameControlOxHundred.h"
#include "TCPSocket.h"
#include "GameLogic.h"
#include "CardLayer.h"
#include "PlayerLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;
class MainSceneOxHundred:public CCLayer,public TCPSocket,public GameLogic
{
public:
	//游戏状态
	enum GameState
	{
		STATE_OBSERVER=0,			//旁观状态
		STATE_READY,				//准备状态
		STATE_WAIT,					//等待服务端响应
		STATE_SEND_CARD,			//发牌状态
		STATE_CALL_BANKER,			//抢庄状态
		STATE_BETTING,				//投注
		STATE_OPT_OX,				//选牛
		STATE_GAME_END,				//结算
	};
	CC_SYNTHESIZE(GameState,gameState,GameState);
	CC_SYNTHESIZE(GameState,serverState,ServerState);

	//CC_SYNTHESIZE(GameControlOxTwo *, gameControl, GameControlOxTwo);
	//扑克层
	CardLayer *cardLayer;
	//玩家信息层
	PlayerLayer *playerLayer;
private:
public:
    MainSceneOxHundred();
    ~MainSceneOxHundred();
	virtual void onEnter();
	virtual void onExit();
    static CCScene* scene();
    
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
