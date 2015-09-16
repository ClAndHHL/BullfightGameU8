/*
 * PopDialogBoxTipInfo.h
 *
 *  Created on: 2015年5月30日
 *      Author: xw007
 */

#pragma once
#include "PopDialogBox.h"
struct IPopAssistTipInfo//提示语回调
{
	virtual ~IPopAssistTipInfo(){}
	//关闭回调
	virtual void onCloseTipInfo(CCLayer *pTipInfo) = 0;
};
class PopDialogBoxTipInfo :public PopDialogBox{
public:
	CC_SYNTHESIZE(IPopAssistTipInfo*, pIPopAssistTipInfo, IPopAssistTipInfo);

	//关闭
	Button *pBClose;
	enum TipButtonType
	{
		TIP_BUTTON_SURE=0,							//确定
		TIP_BUTTON_QUICK_BINDING_PHONE,				//绑定手机	
	};
private:
	Label *pLInfo;
public:
	PopDialogBoxTipInfo();
	virtual ~PopDialogBoxTipInfo();
	CREATE_FUNC(PopDialogBoxTipInfo);
	//设置提示内容
	void setTipInfoContent(const char *content);
	//设置按键内容
	void setTipButtonContent(TipButtonType eTipButtonType);
private:
	virtual void onEnter();
	virtual void onExit();
	//关闭////////////////////////////////////////////////////////////////////////
	void onMenuCloseTipInfo(CCObject *object, TouchEventType type);
};
