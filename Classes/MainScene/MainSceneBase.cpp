//
//  MainSceneBase.cpp
//  BullfightGame
//
//  Created by on 15/3/16.
//
//
#include "MainSceneBase.h"
MainSceneBase::MainSceneBase()
:gameState(STATE_OBSERVER)
{
}
MainSceneBase::~MainSceneBase(){
}
void MainSceneBase::onEnter(){
	CCLayer::onEnter();
	//addBg();
	//initCardLayer();
	initPlayerLayer();
	//initGameControl();
}
void MainSceneBase::onExit(){
	CCLayer::onExit();
}
void MainSceneBase::initPlayerLayer(){
	playerLayer = PlayerLayer::create();
	this->addChild(playerLayer);
}
//�յ�׼����ɻص�
void MainSceneBase::onEventReadyFnish(){
	CCLog("׼����ɵȴ��������Ӧ.");
	setServerStateWithUpdate(STATE_WAIT);
}
