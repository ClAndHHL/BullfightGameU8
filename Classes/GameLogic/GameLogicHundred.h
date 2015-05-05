/************************************************************************/
/* 百人牛牛 logic                                                                     */
/************************************************************************/
#pragma once
#include "cocos2d.h"
//#include "Stdafx.h"
//如果是android平台需要定义宏
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID||CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define CopyMemory RtlCopyMemory
typedef unsigned char       BYTE;

#endif
//////////////////////////////////////////////////////////////////////////
//牌型

enum emCardType
{
	CT_ERROR			=		0,								//错误类型
	CT_POINT			=		1,								//点数类型
	CT_SPECIAL_NIU1		=		3,								//特殊类型
	CT_SPECIAL_NIU2		=		4,								//特殊类型
	CT_SPECIAL_NIU3		=		5,								//特殊类型
	CT_SPECIAL_NIU4		=		6,								//特殊类型
	CT_SPECIAL_NIU5		=		7,								//特殊类型
	CT_SPECIAL_NIU6		=		8,								//特殊类型
	CT_SPECIAL_NIU7		=		9,								//特殊类型
	CT_SPECIAL_NIU8		=		10,								//特殊类型
	CT_SPECIAL_NIU9		=	    11,								//特殊类型
	CT_SPECIAL_NIUNIU	=		12,								//特殊类型
	CT_SPECIAL_NIUNIUXW	=		13,								//特殊类型
	CT_SPECIAL_NIUNIUDW	=		14,								//特殊类型
	CT_SPECIAL_NIUYING	=		15,								//特殊类型
	CT_SPECIAL_NIUKING	=	    16,								//特殊类型
	CT_SPECIAL_BOMEBOME	=		17								//特殊类型

};

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//排序类型
#define	ST_VALUE					1									//数值排序
#define	ST_NEW					    3									//数值排序
#define	ST_LOGIC					2									//逻辑排序

//扑克数目
#define CARD_COUNT					54									//扑克数目
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////	

//////////////////////////////////////////////////////////////////////////

//游戏逻辑
class GameLogicHundred
{
	//变量定义
public:
	static const BYTE				m_cbCardListData[CARD_COUNT];		//扑克定义

	//函数定义
public:
	//构造函数
	GameLogicHundred();
	//析构函数
	virtual ~GameLogicHundred();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) 
	{ 
		return cbCardData&LOGIC_MASK_VALUE; 
	}
	//获取花色
	BYTE GetCardColor(BYTE cbCardData)
	{
		return (cbCardData&LOGIC_MASK_COLOR)>>4;
	}

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//混乱扑克
	void RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);

	int RetType(int itype);

	//逻辑函数
public:
	//获取牌点
	BYTE GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);
	//获取牌型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount,BYTE *bcOutCadData = NULL);
	//大小比较
	int CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount,BYTE &Multiple);
	//逻辑大小
	BYTE GetCardLogicValue(BYTE cbCardData);

	BYTE GetCardNewValue(BYTE cbCardData);
};
//////////////////////////////////////////////////////////////////////////

