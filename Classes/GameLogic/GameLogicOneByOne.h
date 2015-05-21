/************************************************************************/
/* ͨ��ţţ logic                                                                     */
/************************************************************************/
#pragma once
#include "cocos2d.h"
//�����androidƽ̨��Ҫ�����
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID||CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define CopyMemory RtlCopyMemory
typedef unsigned char       BYTE;

#endif
//////////////////////////////////////////////////////////////////////////
//�궨��

#define MAX_COUNT					5									//�����Ŀ

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿�����
#define OX_VALUE0					0									//�������
#define OX_THREE_SAME				102									//��������
#define OX_FOUR_SAME				103									//��������
#define OX_FOURKING					104									//��������
#define OX_FIVEKING					105									//��������

//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//������Ŀ
	BYTE 							cbThreeCount;						//������Ŀ
	BYTE 							cbDoubleCount;						//������Ŀ
	BYTE							cbSignedCount;						//������Ŀ
	BYTE 							cbFourLogicVolue[1];				//�����б�
	BYTE 							cbThreeLogicVolue[1];				//�����б�
	BYTE 							cbDoubleLogicVolue[2];				//�����б�
	BYTE 							cbSignedLogicVolue[5];				//�����б�
	BYTE							cbFourCardData[MAX_COUNT];			//�����б�
	BYTE							cbThreeCardData[MAX_COUNT];			//�����б�
	BYTE							cbDoubleCardData[MAX_COUNT];		//�����б�
	BYTE							cbSignedCardData[MAX_COUNT];		//������Ŀ
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class GameLogicOneByOne
{
	//��������
private:
	static BYTE						m_cbCardListData[54];				//�˿˶���

	//��������
public:
	//���캯��
	GameLogicOneByOne();
	//��������
	virtual ~GameLogicOneByOne();

	//���ͺ���
public:
	//��ȡ����
	//BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount,BYTE *bcOutCadData = NULL);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return (cbCardData&LOGIC_MASK_COLOR)>>4; }
	//��ȡ����
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡţţ
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ����
	bool IsIntValue(BYTE cbCardData[], BYTE cbCardCount);

	//���ƺ���
public:
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//���ܺ���
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	BYTE GetCardNewValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount,bool FirstOX,bool NextOX);
};

//////////////////////////////////////////////////////////////////////////
