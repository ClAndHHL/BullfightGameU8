
#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "GameConfig.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;
class CardLayer:public CCLayer {
private:
	//扑克集
	CCBatchNode *batchCard;
	//发牌状态
	enum SendCardState
	{
		SEND_STATE_WAIT=0,			//等待发牌
		SEND_STATE_ING,				//发牌中
		SEND_STATE_END,				//发牌结束
	};
	CC_SYNTHESIZE(SendCardState, sendCardState, SendCardState);
	//能否发牌
	bool canSendCard[MAX_PLAYER];
	//发牌计数器
	short sSendCardCount;
public:
	CardLayer();
	~CardLayer();
	CREATE_FUNC(CardLayer);
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void setCanSendCard();
public:
	//更新状态
	void updateState();
private:
	//发牌
	void sendCard();
	//发牌中
	void sendCardIng();
	//发5张牌
	void sendFiveCard(int index, int offsetIndex);
	//移动扑克
	void moveCardAction(CCArmature *armature, float fTime, CCPoint targetPos,int index);
	//获取牌放大倍数
	float getCardScale(int index);
	//单张牌发完回调
	void onSendCardFinish();
	//获取当前局总牌数
	short getCurAllCardCount();
};
