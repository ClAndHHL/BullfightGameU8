/*
 * PopDialogBoxMsg.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */

#include "PopDialogBoxMsg.h"
#include "../Tools/DataModel.h"
#include "../Tools/GameConfig.h"
//////////////////////////////////////////////////////////////////////////
PopDialogBoxMsg::PopDialogBoxMsg()
{
	
    
}
PopDialogBoxMsg::~PopDialogBoxMsg() {
	CCLog("~ <<%s>>",__FUNCTION__);
}
void PopDialogBoxMsg::onEnter(){
	CCLayer::onEnter();
	Layout* layoutPauseUI = static_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIPopDialogBoxMsg.ExportJson)));
	pUILayer->addWidget(layoutPauseUI);
	
	pWidgetBg = static_cast<UIImageView*>(pUILayer->getWidgetByName("bg"));
	pWidgetBg->setScale(0.8);
	//关闭
	UIButton *backButton = static_cast<UIButton*>(pUILayer->getWidgetByName("buttonClose"));
	backButton->addTouchEventListener(this, toucheventselector(PopDialogBox::menuBack));
	//系统消息按键
	UIButton *pBSystemMsg = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonSystem"));
	pBSystemMsg->addTouchEventListener(this, toucheventselector(PopDialogBoxMsg::onMenuMsg));
	pBSystemMsg->setVisible(false);
	//我的消息
	UIButton *pBSystemMy = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonMy"));
	pBSystemMy->addTouchEventListener(this, toucheventselector(PopDialogBoxMsg::onMenuMsg));
	pBSystemMy->setVisible(false);
	//游标
	pIVCursor = static_cast<UIImageView*>(pUILayer->getWidgetByName("ImageCursor"));
	//消息名称
	pLMsgName = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelMsgName"));
	
	//消息列表
	pLVMsgList = static_cast<UIListView*>(pUILayer->getWidgetByName("ListViewMsg"));
	//设置cell模式
	pLVMsgList->setItemModel(pLVMsgList->getItem(0));
	pLVMsgList->removeAllItems();

	//没有消息
	pIVNothing = static_cast<UIImageView*>(pUILayer->getWidgetByName("ImageNothing"));
	pIVNothing->setVisible(false);

	//DataModel::sharedDataModel()->vecSystemMsg.push_back("1233");
	//更新列表
	updateListMsg(DataModel::sharedDataModel()->vecSystemMsg);

	playAnimation();
}
void PopDialogBoxMsg::onExit(){
	CCLayer::onExit();
}
//消息按键回调
void PopDialogBoxMsg::onMenuMsg(CCObject *object, TouchEventType type){
	if (type==TOUCH_EVENT_ENDED)
	{
		UIButton *pBTemp = (UIButton *)object;
		switch (pBTemp->getTag())
		{
		case PopDialogBoxMsg::MSG_SYSTEM:
		{
			pIVCursor->stopAllActions();
			pIVCursor->runAction(CCMoveTo::create(0.1,ccp(-88,0)));
			pLMsgName->setText("系统消息");
			//更新列表
			updateListMsg(DataModel::sharedDataModel()->vecSystemMsg);
		}
			break;
		case PopDialogBoxMsg::MSG_MY:
		{
			pIVCursor->stopAllActions();
			pIVCursor->runAction(CCMoveTo::create(0.1, ccp(88, 0)));
			pLMsgName->setText("我的消息");
			//更新列表
			updateListMsg(DataModel::sharedDataModel()->vecMyMsg);
		}
			break;
		default:
			break;
		}
	}
}

//更新消息列表
void PopDialogBoxMsg::updateListMsg(std::vector<std::string> qMsg){
	pLVMsgList->removeAllItems();
	
	int tempSize = qMsg.size();
	if (tempSize == 0)
	{
		pIVNothing->setVisible(true);
		return;
	}
	pIVNothing->setVisible(false);

	for (int i = 0; i < tempSize; i++)
	{
		pLVMsgList->insertDefaultItem(pLVMsgList->getItems()->count());
		
		//加载商品图片
		UIImageView *pIVItem = static_cast<UIImageView*>(pLVMsgList->getItem(pLVMsgList->getItems()->count() - 1)->getChildByName("ImageBg"));
		
		UIButton *pButton = static_cast<UIButton*>(pIVItem->getChildByName("ButtonAction"));
		pButton->setEnabled(false);
				
		UILabel *pLContent = static_cast<UILabel*>(pIVItem->getChildByName("LabelContent"));
		CCLog("-:%s <<%s>>", GBKToUTF8(qMsg[i].c_str()), __FUNCTION__);
		pLContent->setText(GBKToUTF8(qMsg[i].c_str()));
		
		
	}
	/*
	for (int j = 0; j < ROW_COUNT; j++)
	{
		if (j >(tempSize - 1) % (ROW_COUNT)){
			UIImageView *pItem = static_cast<UIImageView*>(pListViewCommodity->getItem(pListViewCommodity->getItems()->count() - 1)->getChildByName(CCString::createWithFormat("ImageCommodity%d", j)->getCString()));
			pItem->setEnabled(false);
		}

	}*/
}