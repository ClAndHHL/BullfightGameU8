/*
 * PopDialogBoxAbout.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#include "PopDialogBoxAbout.h"
#include "DataModel.h"
#include "GameConfig.h"
//////////////////////////////////////////////////////////////////////////
PopDialogBoxAbout::PopDialogBoxAbout()
{
	
}
PopDialogBoxAbout::~PopDialogBoxAbout() {
	CCLog("~ <<%s>>",__FUNCTION__);
}
void PopDialogBoxAbout::onEnter(){
	CCLayer::onEnter();
	Layout* layoutPauseUI = static_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIPopDialogBoxAbout.ExportJson)));
	pUILayer->addWidget(layoutPauseUI);
	
	pWidgetBg = static_cast<UIImageView*>(pUILayer->getWidgetByName("bg"));
	pWidgetBg->setScale(0.8);
	//关闭
	UIButton *backButton = static_cast<UIButton*>(pUILayer->getWidgetByName("buttonClose"));
	backButton->addTouchEventListener(this, toucheventselector(PopDialogBox::menuBack));
	
	playAnimation();
}
void PopDialogBoxAbout::onExit(){
	CCLayer::onExit();
}