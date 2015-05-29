/************************************************************************/
/* 通比牛牛 logic                                                                     */
/************************************************************************/
#pragma once
#include "cocos2d.h"
//如果是android平台需要定义宏
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID||CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define CopyMemory RtlCopyMemory
typedef unsigned char       BYTE;

#endif
//////////////////////////////////////////////////////////////////////////
//宏定义

#define MAX_COUNT					5									//最大数目
//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define OX_VALUE0					0									//混合牌型
#define OX_THREE_SAME				102									//三条牌型
#define OX_FOUR_SAME				103									//四条牌型
#define OX_FOURKING					104									//天王牌型
#define OX_FIVEKING					105									//天王牌型

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbDoubleCount;						//两张数目
	BYTE							cbSignedCount;						//单张数目
	BYTE 							cbFourLogicVolue[1];				//四张列表
	BYTE 							cbThreeLogicVolue[1];				//三张列表
	BYTE 							cbDoubleLogicVolue[2];				//两张列表
	BYTE 							cbSignedLogicVolue[5];				//单张列表
	BYTE							cbFourCardData[MAX_COUNT];			//四张列表
	BYTE							cbThreeCardData[MAX_COUNT];			//三张列表
	BYTE							cbDoubleCardData[MAX_COUNT];		//两张列表
	BYTE							cbSignedCardData[MAX_COUNT];		//单张数目
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class GameLogicOneByOne
{
	//变量定义
private:
	static BYTE						m_cbCardListData[54];				//扑克定义

	//函数定义
public:
	//构造函数
	GameLogicOneByOne();
	//析构函数
	virtual ~GameLogicOneByOne();

	//类型函数
public:
	//获取类型
	//BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取牌型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount,BYTE *bcOutCadData = NULL);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return (cbCardData&LOGIC_MASK_COLOR)>>4; }
	//获取倍数
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount);
	//获取牛牛
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);
	//获取整数
	bool IsIntValue(BYTE cbCardData[], BYTE cbCardCount);

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//功能函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	BYTE GetCardNewValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount,bool FirstOX,bool NextOX);
};

//////////////////////////////////////////////////////////////////////////
