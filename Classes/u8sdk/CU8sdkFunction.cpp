#include "CU8sdkFunction.h"
#include "../LogonScene/LogonScene.h"
#include "../MTNotificationQueue/LobbyMsgHandler.h"
#include "../Tools/Tools.h"

CU8sdkFunction::CU8sdkFunction()
{
}


CU8sdkFunction::~CU8sdkFunction()
{
}

void CU8sdkFunction::OnInitSuc()
{
	//һ�㲻��Ҫ��ʲô����
}

void CU8sdkFunction::OnLoginSuc(U8LoginResult* result)
{
	CCLog("SDK��¼����֤�ɹ�...%s", result->userId.c_str());
	//this->labelUsername->setString(result->sdkUsername);

	if (result->isSwitchAccount)
	{
		//TODO:�����˻ص���¼���棬����SDK���Ѿ���¼�ɹ�״̬�������ٵ���SDK�ĵ�¼����
		CCLog("U8----result->isSwitchAccount");
	}
	else
	{
		//TODO:��¼��֤�ɹ�����ȡ�������б�������Ϸ��...
		CCLog("U8----loginresult");
		std::string umid = result->sdkUserId;
		std::string username = result->sdkUsername;
		std::string token = result->token;
		std::string userid = result->userId;
		//channelID = result->channelID.c_str();

		CCLog("channelID = %s", channelID.c_str());
		CCLog("umid = %s", umid.c_str());
		CCLog("username = %s", username.c_str());
		CCLog("token = %s", token.c_str());

		LogonScene::pLScene->SDKlgoin(umid.c_str(), token.c_str(),userid.c_str(),username.c_str());
		//__Dictionary*  dic = __Dictionary::create();
		//dic->setObject(__String::create(umid), "umid");
		//dic->setObject(__String::create(username), "username");
		//dic->setObject(__String::create(token), "token");
		//dic->setObject(__String::create(userid), "userid");
		//Director::sharedDirector()->getEventDispatcher()->dispatchCustomEvent(u8LoginMsg, dic);
	}
}

void CU8sdkFunction::OnSwitchLogin()
{
	LobbyMsgHandler::sharedLobbyMsgHandler()->gameSocket.Destroy(true);
	DataModel *m = DataModel::sharedDataModel();
	CC_SAFE_RELEASE_NULL(m);

	Tools::setTransitionAnimation(0, 0, LogonScene::scene());
	//U8SDKInterface::getInstance()->login();
}

void CU8sdkFunction::OnLogout()
{
	CCLOG("SDK�ʺ��˳��ɹ�...");
	//this->labelUsername->setString("U8SDK");
	//TODO:�˻ص���¼���棬������SDK��¼����(����U8SDKInterface::getInstance()->login())
	LobbyMsgHandler::sharedLobbyMsgHandler()->gameSocket.Destroy(true);
	DataModel *m = DataModel::sharedDataModel();
	CC_SAFE_RELEASE_NULL(m);

	Tools::setTransitionAnimation(0, 0, LogonScene::scene());
	//U8SDKInterface::getInstance()->login();
}

void CU8sdkFunction::OnU8sdkInit()
{
	CCLog("OnU8sdkInit");
	U8SDKInterface::getInstance()->initSDK(this);
}

void CU8sdkFunction::OnU8sdkLogin()
{
	CCLog("OnU8sdkLogin");
	U8SDKInterface::getInstance()->login();
}

std::string CU8sdkFunction::OnGetChannelid()
{
	std::string channelid = U8SDKInterface::getInstance()->getchannelId();
	CCLog("OnU8sdkLogin%s", channelid.c_str());

	return channelid;
}

void CU8sdkFunction::OnU8sdkPay(std::string strName, std::string strDesc, int price, std::string tradeNo)
{
	U8PayParams* params = U8PayParams::create();
	std::string productId = "1";
	std::string serverId = "1";

	params->productId = productId;
	params->productName = strName;
	params->productDesc = strDesc;
	params->price = price;
	params->buyNum = 1;
	params->coinNum = 0;
	params->serverId = serverId;
	params->serverName = "";
	params->roleId = tradeNo;
	params->roleName = "";
	params->roleLevel = 0;
	params->vip = "";

	//ȥ��������ȡ��Ϸ����Ʒ�Ķ������Լ���չ����,�������
	params->orderID = tradeNo;
	params->extension = "";

	CCLog("CU8sdkFunction::OnU8sdkPay productName==%s,productDesc==%s,price==%d,orderID==%s", params->productName.c_str()
		, params->productDesc.c_str()
		, params->price
		, params->orderID.c_str());

	U8SDKInterface::getInstance()->pay(params);
}