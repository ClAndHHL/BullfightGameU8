/*
 * PopDialogBoxBank.cpp
 *
 *  Created on: 2015年3月17日
 *      Author: 恒
 */
//#include <stdlib.h>
#include "PopDialogBoxBank.h"
#include "../Tools/DataModel.h"
#include "../Tools/GameConfig.h"
#include "../Tools/BaseAttributes.h"
#include "../Network/MD5/MD5.h"
#include "../GameLobby/BaseLobbyScene.h"
//////////////////////////////////////////////////////////////////////////
PopDialogBoxBank::PopDialogBoxBank()
	:isGetBankInfo(false)
{
	scheduleUpdate();
	llQuickLimitNum[0] = 0;
	llQuickLimitNum[1] = 1000000;
	llQuickLimitNum[2] = 5000000;
	llQuickLimitNum[3] = 10000000;
	llQuickLimitNum[4] = 50000000;
	llQuickLimitNum[5] = 100000000;
}
PopDialogBoxBank::~PopDialogBoxBank() {
	CCLog("~ <<%s>>",__FUNCTION__);
	unscheduleUpdate();
	TCPSocketControl::sharedTCPSocketControl()->removeTCPSocket(SOCKET_BANK);
}
void PopDialogBoxBank::onEnter(){
	CCLayer::onEnter();
	Layout* layoutPauseUI = static_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIPopDialogBoxBank.ExportJson)));
	pUILayer->addWidget(layoutPauseUI);
	
	pWidgetBg = static_cast<UIImageView*>(pUILayer->getWidgetByName("bg"));
	pWidgetBg->setScale(0.8);
	//关闭
	UIButton *backButton = static_cast<UIButton*>(pUILayer->getWidgetByName("buttonClose"));
	backButton->addTouchEventListener(this, toucheventselector(PopDialogBox::onMenuBackWithReadMsg));

	//创建密码容器////////////////////////////////////////////////////////////////////////
	pPanelCreatePassword = static_cast<UIPanel*>(pUILayer->getWidgetByName("PanelCreatePassword"));
	//密码输入框
	pTFCreatePassword0 = static_cast<UITextField*>(pUILayer->getWidgetByName("TextFieldCreatePassword0"));
	pTFCreatePassword1 = static_cast<UITextField*>(pUILayer->getWidgetByName("TextFieldCreatePassword1"));
	//创建密码保存按键
	UIButton *pBSave = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonCreatePassword"));
	pBSave->addTouchEventListener(this, toucheventselector(PopDialogBoxBank::onMenuCreatePassword));

	//输入密码容器////////////////////////////////////////////////////////////////////////
	pPanelInputPassword = static_cast<UIPanel*>(pUILayer->getWidgetByName("PanelEnterPassword"));
	//输入密码框
	pTFInputPassword = static_cast<UITextField*>(pUILayer->getWidgetByName("TextFieldEnterPassword"));
	//输入密码保存按键
	UIButton *pBInput = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonEnterPassword"));
	pBInput->addTouchEventListener(this, toucheventselector(PopDialogBoxBank::onMenuInputPassword));
	
	//存取钱容器////////////////////////////////////////////////////////////////////////
	pPanelOperationMoney = static_cast<UIPanel*>(pUILayer->getWidgetByName("PanelOperationMoney"));
	pLBankMoney = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelBankMoney"));
	pBTakeOut = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonDrawMoney"));
	pBTakeOut->addTouchEventListener(this, SEL_TouchEvent(&PopDialogBoxBank::onMenuChangeOperationType));
	pBTakeOut->setBright(false);

	pBDeposit = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonSaveMoney"));
	pBDeposit->addTouchEventListener(this, SEL_TouchEvent(&PopDialogBoxBank::onMenuChangeOperationType));

	//
	pLInput = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelInput"));
	pLOutput = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelOutputMoney"));

	pBOperationMoney = static_cast<UIButton*>(pUILayer->getWidgetByName("ButtonOperation")); 
	pBOperationMoney->addTouchEventListener(this, SEL_TouchEvent(&PopDialogBoxBank::onMenuOperationMoney));
	//存款/取款金币数
	pTFInputMoney = static_cast<UITextField*>(pUILayer->getWidgetByName("TextFieldInputMoney"));
	//标题
	pLTitle0 = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelTitle0"));
	pLTitle1 = static_cast<UILabel*>(pUILayer->getWidgetByName("LabelTitle1"));
	if (DataModel::sharedDataModel()->cbInsurePwd)
	{
		setTitle("输入银行密码");
		setBankState(BANK_STATE_ENTER);
		pPanelOperationMoney->setEnabled(false);
		pPanelCreatePassword->setEnabled(false);
		pPanelInputPassword->setEnabled(true);
		pPanelInputPassword->setVisible(true);
	}
	else
	{
		setTitle("创建银行密码");
		setBankState(BANK_STATE_CREATE);
		pPanelOperationMoney->setEnabled(false);
		pPanelInputPassword->setEnabled(false);
		pPanelCreatePassword->setEnabled(true);
		pPanelCreatePassword->setVisible(true);
	}
	//初始化快捷选择款按键
	initQuickSelectMoney();
	setLobbyReadMessage(false);
	playAnimation();
}
void PopDialogBoxBank::onExit(){
	CCLayer::onExit();
}
//初始化快捷选择款按键
void PopDialogBoxBank::initQuickSelectMoney(){
	for (int i = 0; i < MAX_QUICK_BUTTON_COUNT; i++)
	{
		pBQuickSelectMoney[i] = static_cast<UIButton*>(pUILayer->getWidgetByName(CCString::createWithFormat("ButtonItem%d", i)->getCString()));
		pBQuickSelectMoney[i]->setBright(false);
		pBQuickSelectMoney[i]->addTouchEventListener(this, SEL_TouchEvent(&PopDialogBoxBank::onMenuQuickSelectMoney));
		UILabel *pLMoney = static_cast<UILabel*>(pBQuickSelectMoney[i]->getChildByName("LabelMoneyNum"));
		pLMoney->setColor(ccc3(114,61,29));
	}
}
//设置标题
void PopDialogBoxBank::setTitle(const char * sTitle){
	pLTitle0->setText((sTitle));
	pLTitle1->setText((sTitle));
}
//创建密码菜单////////////////////////////////////////////////////////////////////////
void PopDialogBoxBank::onMenuCreatePassword(CCObject *object, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		string password0 = pTFCreatePassword0->getStringValue();
		string password1 = pTFCreatePassword1->getStringValue();

		if (strcmp(pTFCreatePassword0->getStringValue(), "") == 0 || strcmp(pTFCreatePassword1->getStringValue(), "") == 0)
		{
			showTipInfo(BaseAttributes::sharedAttributes()->sPasswordEmpty.c_str());
		}
		else if (strcmp(pTFCreatePassword0->getStringValue(), pTFCreatePassword1->getStringValue()) != 0)
		{
			showTipInfo(BaseAttributes::sharedAttributes()->sPasswordInconsistent.c_str());
		}
		else if (strlen(pTFCreatePassword0->getStringValue())<8 || strlen(pTFCreatePassword1->getStringValue())<8)
		{
			showTipInfo(BaseAttributes::sharedAttributes()->sInsurePasswordLeng.c_str());
		}
		else if (password0.find_first_not_of("1234567890") == string::npos){
			showTipInfo(BaseAttributes::sharedAttributes()->sInsurePasswordNum.c_str());
		}
		else
		{
			connectServer(SOCKET_BANK);
		}
		
	}
		break;
	default:
		break;
	}
}
//输入密码菜单
void PopDialogBoxBank::onMenuInputPassword(CCObject *object, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		if (strcmp(pTFInputPassword->getStringValue(), "") == 0 )
		{
			showTipInfo(BaseAttributes::sharedAttributes()->sPasswordEmpty.c_str());
		}
		else
		{
			setBankState(BANK_STATE_ENTER);
			connectServer(SOCKET_BANK);
		}
	}
	break;
	default:
		break;
	}
}
void PopDialogBoxBank::onMenuChangeOperationType(CCObject *object, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		UIButton *pBTemp = (UIButton*)object;
		if (strcmp(pBTemp->getName(),"ButtonDrawMoney")==0)
		{
			pBTemp->setBright(false);
			pBDeposit->setBright(true);

			pBTakeOut->setTitleColor(ccc3(255, 255, 255));
			pBDeposit->setTitleColor(ccc3(114, 61, 29));

			pLBankMoney->setText(CCString::createWithFormat("%lld", DataModel::sharedDataModel()->userInfo->lInsure)->getCString());
			pLInput->setText("银行存款:");
			pLOutput->setText("取出金币:");
			pBOperationMoney->setTitleText("取出");
			setBankState(BANK_STATE_TAKE_OUT);
		}
		else if (strcmp(pBTemp->getName(), "ButtonSaveMoney") == 0)
		{
			pBTemp->setBright(false);
			pBTakeOut->setBright(true);

			pBDeposit->setTitleColor(ccc3(255, 255, 255));
			pBTakeOut->setTitleColor(ccc3(114, 61, 29));

			pLBankMoney->setText(CCString::createWithFormat("%lld", DataModel::sharedDataModel()->userInfo->lScore)->getCString());
			pLInput->setText("我的金币:");
			pLOutput->setText("存入金币:");
			pBOperationMoney->setTitleText("存入");
			setBankState(BANK_STATE_SAVE);
		}
		updateQuickButton();
	}
	break;
	default:
		break;
	}
}
//存款/取款按键
void PopDialogBoxBank::onMenuOperationMoney(CCObject *object, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		switch (getBankState())
		{
		case BANK_STATE_TAKE_OUT:
		{
			long long llTempNum = strtoll(pTFInputMoney->getStringValue(), NULL, 10);
			if (strcmp(pTFInputMoney->getStringValue(), "") == 0 || llTempNum==0)
			{
				showTipInfo(BaseAttributes::sharedAttributes()->sTakeOutLimit.c_str());
			}
			else if (llTempNum>DataModel::sharedDataModel()->userInfo->lInsure)
			{
				showTipInfo(BaseAttributes::sharedAttributes()->sInsureNotEnough.c_str());
			}
			else{
				connectServer(SOCKET_BANK);
			}
		}
		break;
		case BANK_STATE_SAVE:
		{
			long long llTempNum = strtoll(pTFInputMoney->getStringValue(), NULL, 10);
			if (strcmp(pTFInputMoney->getStringValue(), "") == 0 || llTempNum == 0)
			{
				showTipInfo(BaseAttributes::sharedAttributes()->sSaveLimit.c_str());
			}
			else if (llTempNum > DataModel::sharedDataModel()->userInfo->lScore)
			{
				showTipInfo(BaseAttributes::sharedAttributes()->sScoreNotEnough.c_str());
			}
			else{
				connectServer(SOCKET_BANK);
			}
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
}
//快捷选择款项按键
void PopDialogBoxBank::onMenuQuickSelectMoney(CCObject *object, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		UIButton *pBTempQuick = (UIButton*)object;
		for (int i = 0; i < MAX_QUICK_BUTTON_COUNT; i++)
		{
			if (!pBQuickSelectMoney[i]->isTouchEnabled())
			{
				continue;
			}
			UILabel *pTempLabel = static_cast<UILabel*>(pBQuickSelectMoney[i]->getChildByName("LabelMoneyNum"));

			if (pBQuickSelectMoney[i]->getTag() == pBTempQuick->getTag())
			{
				pTempLabel->setColor(ccc3(255, 255, 255));
				pBQuickSelectMoney[i]->setBright(true);
				if (i==0)
				{
					long long llTempQuick = 0;
					if (getBankState()==BANK_STATE_TAKE_OUT)
					{
						llTempQuick = DataModel::sharedDataModel()->userInfo->lInsure;
					}
					else if (getBankState() == BANK_STATE_SAVE)
					{
						llTempQuick = DataModel::sharedDataModel()->userInfo->lScore;
					}
					pTFInputMoney->setText(CCString::createWithFormat("%lld", llTempQuick)->getCString());
				}
				else
				{
					pTFInputMoney->setText(CCString::createWithFormat("%lld",llQuickLimitNum[i])->getCString());
				}
			}
			else
			{
				pTempLabel->setColor(ccc3(114, 61, 29));
				pBQuickSelectMoney[i]->setBright(false);
			}
		}
	}
	break;
	default:
		break;
	}
}
//更新快捷款项选择键
void PopDialogBoxBank::updateQuickButton(){
	pTFInputMoney->setText("");
	long long llTempNum = 0;

	switch (bankState)
	{
	case PopDialogBoxBank::BANK_STATE_TAKE_OUT:
	{
		llTempNum = DataModel::sharedDataModel()->userInfo->lInsure;
	}
	break;
	case PopDialogBoxBank::BANK_STATE_SAVE:
	{
		llTempNum = DataModel::sharedDataModel()->userInfo->lScore;
	}
	break;
	default:
		break;
	}
	pLBankMoney->setText(CCString::createWithFormat("%lld", llTempNum)->getCString());

	for (int i = 0; i < MAX_QUICK_BUTTON_COUNT; i++)
	{
		UILabel *pTempLabel = static_cast<UILabel*>(pBQuickSelectMoney[i]->getChildByName("LabelMoneyNum"));
		pBQuickSelectMoney[i]->setBright(false);
		
		if (llTempNum >= llQuickLimitNum[i])
		{
			pTempLabel->setColor(ccc3(114, 61, 29));
			pBQuickSelectMoney[i]->setTouchEnabled(true);
		}
		else
		{
			pTempLabel->setColor(ccc3(182, 136, 91));
			pBQuickSelectMoney[i]->setTouchEnabled(false);
		}

	}
}
//更新
void PopDialogBoxBank::update(float delta){
	MessageQueue::update(delta);
}
//读取网络消息回调
void PopDialogBoxBank::onEventReadMessage(WORD wMainCmdID, WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wMainCmdID)
	{
	case MDM_MB_SOCKET://socket连接成功
	{
		switch (bankState)
		{
		case PopDialogBoxBank::BANK_STATE_CREATE://创建密码
		{
			//变量定义
			CMD_GP_ModifyInsurePass ModifyInsurePass;

			//加密密码
			std::string szSrcPassword = DataModel::sharedDataModel()->sLogonPassword;
			std::string szDesPassword = pTFCreatePassword1->getStringValue();


			

			//构造数据
			ModifyInsurePass.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

			MD5 m;
			m.ComputMd5(szSrcPassword.c_str(), szSrcPassword.length());
			std::string md5PassWord = m.GetMd5();
			strcpy(ModifyInsurePass.szScrPassword, md5PassWord.c_str());

			m.ComputMd5(szDesPassword.c_str(), szDesPassword.length());
			md5PassWord = m.GetMd5();
			strcpy(ModifyInsurePass.szDesPassword, md5PassWord.c_str());

			
			//发送数据
			getSocket()->SendData(MDM_GP_USER_SERVICE, SUB_GP_MODIFY_INSURE_PASS, &ModifyInsurePass, sizeof(ModifyInsurePass));
		}
		break;
		case BANK_STATE_ENTER://输入密码
		{
			/*//验证保险柜密码
			struct CMD_GP_VERIFY_INSUREPASS
			{
				DWORD                           dwUserID;
				TCHAR							szInsurePass[LEN_MD5];			    //保险柜密码
				TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
			};*/
			CMD_GP_VERIFY_INSUREPASS insurePass;
			insurePass.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

			//密码
			std::string sPassword = pTFInputPassword->getStringValue();
			
			MD5 m;
			m.ComputMd5(sPassword.c_str(), sPassword.length());
			std::string md5PassWord = m.GetMd5();
			strcpy(insurePass.szInsurePass, md5PassWord.c_str());
			sTempPassword = md5PassWord;

			strcpy(insurePass.szMachineID, "12");
			//发送数据
			getSocket()->SendData(MDM_GP_USER_SERVICE, SUB_GP_VERIFY_INSURE_PASS, &insurePass, sizeof(insurePass));
		}
			break;
		case BANK_STATE_GET:
		{
			CMD_GP_QueryInsureInfo queryBank;
			queryBank.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

			//密码
			std::string sPassword = DataModel::sharedDataModel()->sLogonPassword;
			MD5 m;
			m.ComputMd5(sPassword.c_str(), sPassword.length());
			std::string md5PassWord = m.GetMd5();
			strcpy(queryBank.szPassword, md5PassWord.c_str());
			//发送数据
			getSocket()->SendData(MDM_GP_USER_SERVICE, SUB_GP_QUERY_INSURE_INFO, &queryBank, sizeof(queryBank));
		}
			break;
		case BANK_STATE_TAKE_OUT://取款
		{
			CMD_GP_UserTakeScore takeScore;
			takeScore.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;
			takeScore.lTakeScore = strtoll(pTFInputMoney->getStringValue(), NULL, 10);

			//密码
			strcpy(takeScore.szPassword, sTempPassword.c_str());

			strcpy(takeScore.szMachineID, "12");
			//发送数据
			getSocket()->SendData(MDM_GP_USER_SERVICE, SUB_GP_USER_TAKE_SCORE, &takeScore, sizeof(takeScore));
		}
			break;
		case BANK_STATE_SAVE://存款
		{
			CMD_GP_UserSaveScore saveScore;
			saveScore.dwUserID = DataModel::sharedDataModel()->userInfo->dwUserID;

			saveScore.lSaveScore = strtoll(pTFInputMoney->getStringValue(), NULL, 10);
			strcpy(saveScore.szMachineID, "12");
			//发送数据
			getSocket()->SendData(MDM_GP_USER_SERVICE, SUB_GP_USER_SAVE_SCORE, &saveScore, sizeof(saveScore));

		}
			break;
		default:
			break;
		}
	}
	break;
	case MDM_GP_USER_SERVICE://用户服务
	{
		onEventUserService(wSubCmdID, pDataBuffer, wDataSize);

		//移除loading
		this->getChildByTag(TAG_LOADING)->removeFromParentAndCleanup(true);
		//关闭网络
		TCPSocketControl::sharedTCPSocketControl()->stopSocket(SOCKET_BANK);

		if (getBankState() == BANK_STATE_ENTER || getBankState() == BANK_STATE_CREATE)
		{
			if (isGetBankInfo)
			{
				isGetBankInfo = false;
				setBankState(BANK_STATE_GET);
				connectServer(SOCKET_BANK);
			}
		}
		
	}
	break;
	default:
		CCLog("other:%d   %d<<%s>>", wMainCmdID, wSubCmdID, __FUNCTION__);
		break;
	}
}
//用户服务
void PopDialogBoxBank::onEventUserService(WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GP_OPERATE_SUCCESS:
	{
		CMD_GP_OperateSuccess * pOperateSuccess = (CMD_GP_OperateSuccess *)pDataBuffer;
		//效验数据
		assert(wDataSize >= (sizeof(CMD_GP_OperateSuccess) - sizeof(pOperateSuccess->szDescribeString)));
		if (wDataSize < (sizeof(CMD_GP_OperateSuccess) - sizeof(pOperateSuccess->szDescribeString))) return ;
		if (pOperateSuccess->lResultCode!=0)
		{
			if (getBankState()==BANK_STATE_CREATE)
			{
				isGetBankInfo = true;
				DataModel::sharedDataModel()->cbInsurePwd = true;
			}
			//showTipInfo(GBKToUTF8(pOperateSuccess->szDescribeString));
		}
		else
		{
			isGetBankInfo = true;
		}
		
	}
		break;
	case SUB_GP_OPERATE_FAILURE:
	{
		//效验参数
		CMD_GP_OperateFailure * pOperateFailure = (CMD_GP_OperateFailure *)pDataBuffer;
		assert(wDataSize >= (sizeof(CMD_GP_OperateFailure) - sizeof(pOperateFailure->szDescribeString)));
		if (wDataSize < (sizeof(CMD_GP_OperateFailure) - sizeof(pOperateFailure->szDescribeString))) return;
		
		showTipInfo(GBKToUTF8(pOperateFailure->szDescribeString));
	}
		break;
	case SUB_GP_USER_INSURE_INFO:
	{
		assert(wDataSize == sizeof(CMD_GP_UserInsureInfo));
		if (wDataSize < sizeof(CMD_GP_UserInsureInfo)) return ;
		//变量定义
		CMD_GP_UserInsureInfo * pUserInsureInfo = (CMD_GP_UserInsureInfo *)pDataBuffer;
	
		setTitle("我的银行");
		pPanelInputPassword->setEnabled(false);
		pPanelCreatePassword->setEnabled(false);
		pPanelOperationMoney->setEnabled(true);

		setBankState(BANK_STATE_TAKE_OUT);
		DataModel::sharedDataModel()->userInfo->lScore = pUserInsureInfo->lUserScore;
		DataModel::sharedDataModel()->userInfo->lInsure = pUserInsureInfo->lUserInsure;
		pLBankMoney->setText(CCString::createWithFormat("%lld", DataModel::sharedDataModel()->userInfo->lInsure)->getCString());

		onMenuChangeOperationType(pBTakeOut, TOUCH_EVENT_ENDED);
	}
		break;
	case SUB_GP_USER_INSURE_SUCCESS://银行成功
	{
		//效验参数
		CMD_GP_UserInsureSuccess * pUserInsureSuccess = (CMD_GP_UserInsureSuccess *)pDataBuffer;
		assert(wDataSize >= (sizeof(CMD_GP_UserInsureSuccess) - sizeof(pUserInsureSuccess->szDescribeString)));
		if (wDataSize < (sizeof(CMD_GP_UserInsureSuccess) - sizeof(pUserInsureSuccess->szDescribeString))) return ;

		DataModel::sharedDataModel()->userInfo->lInsure = pUserInsureSuccess->lUserInsure;
		DataModel::sharedDataModel()->userInfo->lScore = pUserInsureSuccess->lUserScore;
		showTipInfo(GBKToUTF8(pUserInsureSuccess->szDescribeString));
		
		((BaseLobbyScene*)this->getParent())->pLabelGoldCount->setText(CCString::createWithFormat("%lld", DataModel::sharedDataModel()->userInfo->lScore)->getCString());

		updateQuickButton();
	}
		break;
	case SUB_GP_USER_INSURE_FAILURE://银行失败
	{
		//效验参数
		CMD_GP_UserInsureFailure * pUserInsureFailure = (CMD_GP_UserInsureFailure *)pDataBuffer;
		assert(wDataSize >= (sizeof(CMD_GP_UserInsureFailure) - sizeof(pUserInsureFailure->szDescribeString)));
		if (wDataSize < (sizeof(CMD_GP_UserInsureFailure) - sizeof(pUserInsureFailure->szDescribeString))) return ;

		showTipInfo(GBKToUTF8(pUserInsureFailure->szDescribeString));
		
	}
		break;
	default:
		CCLog("--------------sub:%d <<%s>>",wSubCmdID, __FUNCTION__);
		break;
	}
}
