//
//  TCPLogonID.cpp
//  
//
//  Created by on 15/3/16.
//
//

#include "TCPLogonID.h"
#include "StructLogon.h"
#include "MD5.h"
#include "DataModel.h"
pthread_t TCPLogonID::threadLogonID;
TCPLogonID * TCPLogonID::logonID;
TCPLogonID::TCPLogonID():
isReadData(true)
{
	logonID=this;
}
TCPLogonID::~TCPLogonID(){
	CCLog("~ <<%s>>", __FUNCTION__);
}

void TCPLogonID::onEnter(){
	CCNode::onEnter();
}
void TCPLogonID::onExit(){
	CCNode::onExit();
}
int TCPLogonID::startSendThread(){
	int errCode = 0;
	do{
		pthread_attr_t tAttr;
		errCode = pthread_attr_init(&tAttr);

		CC_BREAK_IF(errCode != 0);

		errCode = pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);

		if (errCode != 0) {
			pthread_attr_destroy(&tAttr);
			break;
		}

		errCode = pthread_create(&threadLogonID, &tAttr, networkThread, this);
		pthread_detach(threadLogonID);
	} while (0);
	return errCode;
}
void* TCPLogonID::networkThread(void*){
	logonID->sendData(DataModel::sharedDataModel()->tagGameServerList[0]->szServerAddr,DataModel::sharedDataModel()->tagGameServerList[0]->wServerPort);
	return 0;
}
void TCPLogonID::stopTcpSocket(){
	//pthread_exit(&threadLogon);
	isReadData = false;
	pthread_detach(threadLogonID);
	//Select();
	Close();
}
void TCPLogonID::sendData(const char* ip, unsigned short port){
	Init();
	Create(AF_INET, SOCK_STREAM, 0);
	bool isConnect=Connect(ip, port);
	CCLog("Connect:%d", isConnect);
	if (!isConnect)
	{
		stopTcpSocket();
		return;
	}
	CMD_GR_LogonUserID logonUserID;
	memset(&logonUserID, 0, sizeof(CMD_GR_LogonUserID));
	//logonUserID.dwFrameVersion=17235969;
	//logonUserID.dwPlazaVersion=16777217;
	//logonUserID.dwProcessVersion=16908289;
	logonUserID.dwFrameVersion=16777217;
	logonUserID.dwPlazaVersion=17235969;
	logonUserID.dwProcessVersion= 17104897;
	logonUserID.dwUserID=DataModel::sharedDataModel()->logonSuccessUserInfo->dwUserID;
	strcpy(logonUserID.szMachineID,"12");
	strcpy(logonUserID.szPassPortID,"12");

	MD5 m;
	MD5::char8 str[] = "z12345678";
	m.ComputMd5(str, sizeof(str)-1);
	std::string md5PassWord = m.GetMd5();
	strcpy(logonUserID.szPassword,md5PassWord.c_str());

	strcpy(logonUserID.szPhoneVerifyID,"1");
	logonUserID.wKindID=DataModel::sharedDataModel()->tagGameServerList[0]->wKindID;

	
	int luidSize=sizeof(CMD_GR_LogonUserID);
	bool isSend = SendData(MDM_GP_LOGON, SUB_GR_LOGON_USERID, &logonUserID, sizeof(logonUserID));
	CCLog("send:%d", isSend);
	
	if (!isSend)
	{
		stopTcpSocket();
		return;
	}
	while (isReadData)
	{
		CCLog("begin-------------------------------------------");
		isReadData = OnSocketNotifyRead(0, 0);
		CCLog("read:%d", isReadData);
	}
	stopTcpSocket();
}
bool TCPLogonID::OnEventTCPSocketRead(unsigned short wSocketID, TCP_Command Command, void * pDataBuffer, unsigned short wDataSize){
	if (Command.wMainCmdID == MDM_GP_LOGON)
	{
		if (Command.wSubCmdID == SUB_GP_UPDATE_NOTIFY)
		{
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_UpdateNotify));
			if (wDataSize < sizeof(CMD_GR_UpdateNotify)) return false;

			CMD_GR_UpdateNotify *lf = (CMD_GR_UpdateNotify*)pDataBuffer;
			CCLog("==");
		}
		if (Command.wSubCmdID == SUB_GP_LOGON_SUCCESS)
		{
			int size=sizeof(CMD_GR_LogonSuccess);
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_LogonSuccess));
			if (wDataSize < sizeof(CMD_GR_LogonSuccess)) return false;

			CMD_GR_LogonSuccess *lf = (CMD_GR_LogonSuccess*)pDataBuffer;
			CCLog("==");
		}
	}
	
	return 1;
}