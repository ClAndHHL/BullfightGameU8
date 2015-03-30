//
//  GameLobbyScene.cpp
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//

#include "GameLobbyScene.h"
#include "Tools.h"
#include "GameConfig.h"
#include "PopDialogBoxUserInfo.h"
#include "PopDialogBoxLoading.h"
#include "ClassicLobbyScene.h"
#include "DataModel.h"
#include "SocketThread.h"
#include "DefaultListerner.h"
//#include <stdio.h>
#include "MD5.h"
#include "TCPSocketControl.h"

#ifdef WIN32
#define UTEXT(str) GBKToUTF8(str)
#else
#define UTEXT(str) gb23122utf8(str)
#endif

#ifdef WIN32
#include "platform/third_party/win32/iconv/iconv.h"

static char g_GBKConvUTF8Buf[5000] = {0};
const char* GBKToUTF8(const char *strChar)
{

	iconv_t iconvH;
	iconvH = iconv_open("utf-8","gb2312");
	if (iconvH == 0)
	{
		return NULL;
	}
	size_t strLength = strlen(strChar);
	size_t outLength = strLength<<2;
	size_t copyLength = outLength;
	memset(g_GBKConvUTF8Buf, 0, 5000);

	char* outbuf = (char*) malloc(outLength);
	char* pBuff = outbuf;
	memset( outbuf, 0, outLength);

	if (-1 == iconv(iconvH, &strChar, &strLength, &outbuf, &outLength))
	{
		iconv_close(iconvH);
		return NULL;
	}
	memcpy(g_GBKConvUTF8Buf,pBuff,copyLength);
	free(pBuff);
	iconv_close(iconvH);
	return g_GBKConvUTF8Buf;
}
#else
#include <dlfcn.h>
void (*ucnv_convert)(const char *, const char *, char * , int32_t , const char *, int32_t,int32_t*) = 0;
bool openIcuuc()
{
	void* libFile = dlopen("/system/lib/libicuuc.so", RTLD_LAZY);
	if (libFile)
	{
		ucnv_convert = (void (*)(const char *, const char *, char * , int32_t , const char *, int32_t,int32_t*))dlsym(libFile, "ucnv_convert_3_8");

		int index = 0;
		char fun_name[64];
		while (ucnv_convert == NULL)
		{
			sprintf(fun_name, "ucnv_convert_4%d", index++);
			ucnv_convert = (void (*)(const char *, const char *, char * , int32_t , const char *, int32_t,int32_t*))dlsym(libFile, fun_name);
			if (ucnv_convert)
				return true;
			if (++index > 11)
				break;
		}
		dlclose(libFile);
	}
	return false;
}
const char* gb23122utf8(const char * gb2312)
{
	if (ucnv_convert == NULL)
	{
		openIcuuc();
	}
	if (ucnv_convert)
	{
		int err_code = 0;
		int len = strlen(gb2312);
		char* str = new char[len * 2 + 10];
		memset(str, 0, len * 2 + 10);
		ucnv_convert("utf-8", "gb2312", str, len * 2 + 10, gb2312, len, &err_code);
		if (err_code == 0)
		{
			return str;
		}
	}
	char test[256] = "gb23122utf8 error";
	char* str = new char[30];
	strcpy(str, test);
	return str;
}
#endif

GameLobbyScene::GameLobbyScene()
:deleteSocket(false){
	scheduleUpdate();
}
GameLobbyScene::~GameLobbyScene(){
	CCLog("~ <<%s>>", __FUNCTION__);
	unscheduleUpdate();
	
	//移除对象
	this->removeAllChildrenWithCleanup(true);
	//清理数据
	GUIReader::purge();
	CCArmatureDataManager::purge();
	//清理纹理
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}
CCScene* GameLobbyScene::scene()
{
    CCScene *scene = CCScene::create();
    GameLobbyScene *layer = GameLobbyScene::create();
    scene->addChild(layer);
    return scene;
}
void GameLobbyScene::onEnter(){
	CCLayer::onEnter();
	//contentScaleFactor
	CCSize deviceSize=DataModel::sharedDataModel()->deviceSize;
	//float scale=SCENE_SIZE.height/deviceSize.height;

	CCSprite *spriteBg=CCSprite::create("res/main_bg.jpg");
	this->addChild(spriteBg);
	spriteBg->setPosition(ccp(deviceSize.width/2,deviceSize.height/2));
	float scale=deviceSize.height/spriteBg->getContentSize().height;
	spriteBg->setScale(scale);
	
	UILayer *m_pWidget = UILayer::create();
	this->addChild(m_pWidget);


	UILayout *pWidget = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIGameLobby.ExportJson)));
	m_pWidget->addWidget(pWidget);

	UIButton* button = static_cast<UIButton*>(m_pWidget->getWidgetByName("buttonUser"));
	button->addTouchEventListener(this, SEL_TouchEvent(&GameLobbyScene::menuResetUser));
	for (int i = 0; i < 3;i++)
	{
		button = static_cast<UIButton*>(m_pWidget->getWidgetByName(CCString::createWithFormat("buttonMode%d",i+1)->getCString()));
		button->addTouchEventListener(this, SEL_TouchEvent(&GameLobbyScene::menuSelectMode));
	}
	scroll=static_cast<UIScrollView*>(m_pWidget->getWidgetByName("ScrollView"));
	scroll->setInnerContainerSize(scroll->getContentSize());
	//用户名
	userName=static_cast<UILabel*>(m_pWidget->getWidgetByName("labelUserName"));
	//金币
	pLabelGoldCount=static_cast<UILabel*>(m_pWidget->getWidgetByName("LabelGoldCount"));
	//添加监听事件
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,callfuncO_selector(GameLobbyScene::callbackData),LISTENER_LOGON,NULL);
	initTCPLogon();
}
void GameLobbyScene::callbackData(CCObject *obj){
	//CMD_MB_LogonSuccess *ls = (CMD_MB_LogonSuccess*)obj;
	PopDialogBoxLoading *pdb = (PopDialogBoxLoading*)this->getChildByTag(189);
	pdb->removeFromParentAndCleanup(true);
	//////////////////////////////////////////////////////////////////////////
	//设置用户名
	//char *name=DataModel::sharedDataModel()->logonSuccessUserInfo->szNickName;
	//userName->setText(UTEXT(name));
	//userName->setText("dsdsg");
	deleteSocket=true;
}
void GameLobbyScene::update(float dt){
	char *name=DataModel::sharedDataModel()->logonSuccessUserInfo->szNickName;
    if(strcmp(userName->getStringValue(),"游客")==0&&strcmp(name, "") != 0)
	{
		userName->setText(UTEXT(name));
		pLabelGoldCount->setText(CCString::createWithFormat("%ld",DataModel::sharedDataModel()->logonSuccessUserInfo->dwExperience)->getCString());
		CCLog("333333333333");
	}
	if (deleteSocket)
	{
		TCPSocketControl::sharedTCPSocketControl()->stopSocket();
		deleteSocket=false;
		CCLog("2222222222222");
	}
}
void GameLobbyScene::onExit(){
	//移除监听事件 
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, LISTENER_LOGON); 

	CCLayer::onExit();
}
void GameLobbyScene::menuResetUser(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
		popDialogBoxUserInfo();
		break;
	default:
		break;
	}
}
void GameLobbyScene::menuSelectMode(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		UIButton *button = (UIButton*)pSender;
		enterLobbyByMode(button->getTag());
	}
		break;
	default:
		break;
	}
}

void GameLobbyScene::popDialogBoxUserInfo(){
	PopDialogBox *pdb = PopDialogBoxUserInfo::create();
	this->addChild(pdb);
	pdb->playAnimation();
}
void GameLobbyScene::enterLobbyByMode(int mode){
	switch (mode)
	{
	case MODE_CLASSIC:
		Tools::setTransitionAnimation(0, 0, ClassicLobbyScene::scene());
		break;
	default:
		break;
	}
}
void GameLobbyScene::initTCPLogon(){
	TCPSocketControl *tcp=TCPSocketControl::sharedTCPSocketControl();
	tcp->ip="125.88.145.41";
	tcp->port=8100;
	tcp->listerner=new DefaultListerner();
	tcp->startSendThread();
	//delete tcp->listerner;
	/*CMD_MB_LogonAccounts logonAccounts;
	//memset(&logonAccounts, 0, sizeof(CMD_MB_LogonAccounts));
	logonAccounts.cbDeviceType = 2;
	logonAccounts.dwPlazaVersion = 17235969;


	//_tcscpy(logonAccounts.szPassword, _TEXT("123456"));
	//_tcscpy(logonAccounts.szAccounts, _TEXT("z40144322"));
	strcpy(logonAccounts.szAccounts,"z40144322");

	strcpy(logonAccounts.szMachineID,"12");
	strcpy(logonAccounts.szMobilePhone,"32");
	strcpy(logonAccounts.szPassPortID,"12");
	strcpy(logonAccounts.szPhoneVerifyID,"1");
	//_tcscpy(logonAccounts.szMachineID, _TEXT("12"));
	//_tcscpy(logonAccounts.szMobilePhone, _TEXT("32"));
	//_tcscpy(logonAccounts.szPassPortID, _TEXT("12"));
	//_tcscpy(logonAccounts.szPhoneVerifyID, _TEXT("1"));

	logonAccounts.wModuleID = 210; //210为二人牛牛标示


	MD5 m;
	MD5::char8 str[] = "z12345678";
	m.ComputMd5(str, sizeof(str)-1);
	std::string md5PassWord = m.GetMd5();

	strcpy(logonAccounts.szPassword,md5PassWord.c_str());
	//_tcscpy(logonAccounts.szPassword, _TEXT(md5PassWord.c_str()));

	//bool isSend = so->SendData(MDM_MB_LOGON, SUB_MB_LOGON_ACCOUNTS, &logonAccounts, sizeof(logonAccounts));
	//CCLog("send:%d", isSend);

	bool isSend =TCPSocketControl::sharedTCPSocketControl()->SendData(MDM_MB_LOGON, SUB_MB_LOGON_ACCOUNTS, &logonAccounts, sizeof(logonAccounts));
	CCLog("send:%d", isSend);
	//tcpLogon=TCPLogon::create();
	//this->addChild(tcpLogon);
	//tcpLogon->startSendThread();*/
	/*SocketThread *socketThread=SocketThread::GetInstance();
	//SocketThread::GetInstance()->getSocket().SetListerner(new DefaultListerner());
	socketThread->start();
	*/
	PopDialogBox *pdb = PopDialogBoxLoading::create();
	this->addChild(pdb);
	pdb->setTag(189);
}
bool GameLobbyScene::OnEventTCPSocketRead(unsigned short wSocketID, TCP_Command tCommand, void * pDataBuffer, unsigned short wDataSize){
	CCLog("========================");
	return 1;
}
