/*
 * Card.h
 *
 *  Created on: 2015年3月17日
 *      Author: zhang
 */
#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

class Card:public CCNode {
public:
public:
	Card();
	~Card();
	CREATE_FUNC(Card);
};
