/* 
* File:   DefaultListerner1.cpp
* Author: beykery
* 
* Created on 2013年12月30日, 下午7:33
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "GameListerner.h"
#include "CMD_GameServer.h"
#include "MD5.h"
#include "DataModel.h"
#include "MTNotificationQueue.h"
#include "Tools.h"
#include "cmd_ox.h"
#include "cocos2d.h"
#include "QueueData.h"
#include "GameConfig.h"
#include "TCPSocketControl.h"
#include "SEvent.h"
#include "PacketAide.h"
using namespace std;

GameListerner::GameListerner()
{
}

GameListerner::~GameListerner()
{
}

void GameListerner::OnClose(TCPSocket* so, bool fromRemote)
{
	//End();
	TCPSocketControl::sharedTCPSocketControl()->deleteControl();
	CCLog("%s\n","-----111111closed");
}

void GameListerner::OnError(TCPSocket* so, const char* e)
{
	CCLog("%s\n","error connection");
}

void GameListerner::OnIdle(TCPSocket* so)
{
	CCLog("%s\n","connection idle");
}

/**
* 有数据到来
* @param so
* @param message
*/
bool GameListerner::OnMessage(TCPSocket* so,unsigned short	wSocketID, TCP_Command Command, void * pDataBuffer, unsigned short wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON://登录
		return logonEvent(so,Command.wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GR_CONFIG://配置
		return configEvent(so,Command.wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GR_USER://用户信息
		return userEvent(so,Command.wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GR_STATUS://状态信息
		return statusEvent(so,Command.wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GF_FRAME://框架命令
		return frameEvent(so,Command.wSubCmdID,pDataBuffer,wDataSize);
		break;
	case MDM_GF_GAME://游戏命令
		return gameEvent(so,Command.wSubCmdID,pDataBuffer,wDataSize);
		break;
	default:
		CCLog("other---------- %d    %d",Command.wMainCmdID,Command.wSubCmdID);
		break;
	}
	return true;
}

void GameListerner::OnOpen(TCPSocket* so)
{
	MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_OPEN,NULL);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool GameListerner::logonEvent(TCPSocket* pSocket,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	if (wSubCmdID == SUB_GR_UPDATE_NOTIFY)
	{
		//效验参数
		assert(wDataSize >= sizeof(CMD_GR_UpdateNotify));
		if (wDataSize < sizeof(CMD_GR_UpdateNotify)) return false;

		CMD_GR_UpdateNotify *lf = (CMD_GR_UpdateNotify*)pDataBuffer;
		CCLog("升级提示");
	}
	if (wSubCmdID == SUB_GR_LOGON_SUCCESS)
	{
		int size=sizeof(CMD_GR_LogonSuccess);
		//效验参数
		assert(wDataSize >= sizeof(CMD_GR_LogonSuccess));
		if (wDataSize < sizeof(CMD_GR_LogonSuccess)) return false;

		CMD_GR_LogonSuccess *lf = (CMD_GR_LogonSuccess*)pDataBuffer;
		CCLog("登录成功");
	}
	if (wSubCmdID==SUB_GR_LOGON_FAILURE)
	{
		//assert(wDataSize >= sizeof(CMD_GR_LogonFailure));
		//if (wDataSize < sizeof(CMD_GR_LogonFailure)) return false;

		CMD_GR_LogonFailure *lf = (CMD_GR_LogonFailure*)pDataBuffer;
		CCLog("登录失败:%s",Tools::GBKToUTF8(lf->szDescribeString));
	}
	return true;
}
bool GameListerner::configEvent(TCPSocket* pSocket,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_CONFIG_COLUMN:
		{
			/*int size=sizeof(tagColumnItem);
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_ConfigColumn));
			if (wDataSize < sizeof(CMD_GR_ConfigColumn)) return false;
			*/
			CMD_GR_ConfigColumn *lf = (CMD_GR_ConfigColumn*)pDataBuffer;
			CCLog("列表配置");
		}
		break;
	case SUB_GR_CONFIG_SERVER:
		{
			int size=sizeof(CMD_GR_ConfigServer);
			//效验参数
			assert(wDataSize >= sizeof(CMD_GR_ConfigServer));
			if (wDataSize < sizeof(CMD_GR_ConfigServer)) return false;

			CMD_GR_ConfigServer *lf = (CMD_GR_ConfigServer*)pDataBuffer;
			CCLog("房间配置");
		}
		break;
	case SUB_GR_CONFIG_PROPERTY:
		{
			CMD_GR_ConfigProperty *lf = (CMD_GR_ConfigProperty*)pDataBuffer;
			CCLog("道具配置");
		}
		break;
	case SUB_GR_CONFIG_FINISH:
		{
			CCLog("配置完成");

			MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_CONFIG_FINISH,NULL);
			/*CMD_GR_UserSitDown sit;
			sit.wTableID=28;
			sit.wChairID=1;
			strcpy(sit.szPassword,"");

			bool isSend=so->SendData(MDM_GR_USER,SUB_GR_USER_SITDOWN,&sit, sizeof(sit));
			CCLog("Classic:%d",isSend);*/
		}
		break;
	default:
		break;
	}
	return true;
}
bool GameListerner::userEvent(TCPSocket* pSocket,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER://用户进入
		{
			OnSocketSubUserEnter(pSocket,pDataBuffer,wDataSize);
		}
		break;
	case SUB_GR_USER_SCORE://用户分数
		{
			CMD_GR_UserScore *userScore=(CMD_GR_UserScore*)pDataBuffer;
			//CCLog("用户分数");
		}
		break;
	case SUB_GR_USER_STATUS://用户状态
		{
			int wSize=wDataSize;
			int size =sizeof(CMD_GR_UserStatus);
			CMD_GR_UserStatus *info= (CMD_GR_UserStatus*)pDataBuffer;
			CCLog("%d <---> %d <<%s>>",info->dwUserID,DataModel::sharedDataModel()->logonSuccessUserInfo->dwUserID,__FUNCTION__);
			if (info->dwUserID==DataModel::sharedDataModel()->logonSuccessUserInfo->dwUserID)
			{
				tagUserStatus state=info->UserStatus;
				if (state.cbUserStatus==US_SIT)
				{
					DataModel::sharedDataModel()->isSit=true;
					CCLog("坐下:table: %d desk:%d",state.wTableID,state.wChairID);
					// CCNotificationCenter::sharedNotificationCenter()->postNotification(LISTENER_LOGON,NULL);
					DataModel::sharedDataModel()->userInfo->wTableID=state.wTableID;
					DataModel::sharedDataModel()->userInfo->wChairID=state.wChairID;

					//构造数据
					CMD_GF_GameOption GameOption;
					GameOption.dwFrameVersion=VERSION_FRAME;
					GameOption.cbAllowLookon=0;
					GameOption.dwClientVersion=VERSION_CLIENT;
					//发送
					bool isSend = pSocket->SendData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, &GameOption, sizeof(GameOption));

				}else if (state.cbUserStatus==US_PLAYING)
				{
					MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_CONFIG_FINISH,NULL);
					//CMD_S_StatusFree *gameStatue=(CMD_S_StatusFree*)pDataBuffer;
					CCLog("游戏状态 ");
				}else if(state.cbUserStatus==US_FREE)
				{
					CCLog("站立");
					MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_US_FREE,NULL);
					//
				}
				CCLog("状态：%d",state.cbUserStatus);
			}
			//CCLog("state %d",info->dwUserID);
		}
		break;

	default:
		break;
	}
	return true;
}
bool GameListerner::statusEvent(TCPSocket* pSocket,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GR_TABLE_INFO:
		{
			CMD_GR_TableInfo *lf = (CMD_GR_TableInfo*)pDataBuffer;
			
			CCLog("桌子信息");
		}
		break;
	default:
		break;
	}
	return true;
}
bool GameListerner::frameEvent(TCPSocket* pSocket,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_STATUS:
		{
			CCLog("游戏状态 ");
		}
		break;
	case SUB_GF_SYSTEM_MESSAGE:
		{
			CCLog("系统消息");
		}
		break;
	case SUB_GF_GAME_SCENE:
		{
			CCLog("游戏场景");
		}
		break;
	default:
		break;
	}
	return true;
}
bool GameListerner::gameEvent(TCPSocket* pSocket,WORD wSubCmdID,void * pDataBuffer, unsigned short wDataSize){
	//创建数据
    QueueData *queueData=new QueueData();
    queueData->wSubCmdID=wSubCmdID;
	queueData->wDataSize=wDataSize;
    memcpy(queueData->sendData.sSendData, pDataBuffer, wDataSize);
	/*QueueData queueData;
	memset(&queueData,0,sizeof(QueueData));
	queueData.wSubCmdID=wSubCmdID;
	queueData.wDataSize=wDataSize;
	 memcpy(queueData.sendData.sSendData, pDataBuffer, wDataSize);*/
	//发送消息
	MTNotificationQueue::sharedNotificationQueue()->postNotification(S_L_GAME_ING,queueData);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*//ascii 转 Unicode
std::wstring Acsi2WideByte(std::string& strascii)
{
	int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<wchar_t> resultstring(widesize);
	int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);


	if (convresult != widesize)
	{
		throw std::exception("La falla!");
	}

	return std::wstring(&resultstring[0]);
}
//Unicode 转 Utf8


std::string Unicode2Utf8(const std::wstring& widestring)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8size == 0)
	{
		throw std::exception("Error in conversion.");
	}

	std::vector<char> resultstring(utf8size);

	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

	if (convresult != utf8size)
	{
		throw std::exception("La falla!");
	}

	return std::string(&resultstring[0]);
}

//ascii 转 Utf8 
std::string ASCII2UTF_8(std::string& strAsciiCode)
{
	std::string strRet("");
	//先把 ascii 转为 unicode
	std::wstring wstr = Acsi2WideByte(strAsciiCode);
	//最后把 unicode 转为 utf8
	strRet = Unicode2Utf8(wstr);
	return strRet;
}*/
//用户进入
bool GameListerner::OnSocketSubUserEnter(TCPSocket* pSocket,void * pDataBuffer, unsigned short wDataSize){
#if (DEBUG_TEST==0)
	//效验参数
	assert(wDataSize>=sizeof(tagUserInfoHead));
	if (wDataSize<sizeof(tagUserInfoHead)) return false;
	//消息处理
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pDataBuffer;

	/*if (pUserInfoHead->wTableID==DataModel::sharedDataModel()->userInfo->wTableID)
	{
	}*/
	int wPacketSize=0;
	//变量定义
	tagUserInfo UserInfo;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET + sizeof(TCP_Head)];
	CopyMemory(cbDataBuffer, pDataBuffer, wDataSize);
	//CCLog("--------------------------");
	wPacketSize+=sizeof(tagUserInfoHead);
	while (true)
	{
		void * pDataBuffer1 = cbDataBuffer + wPacketSize;
		tagDataDescribe *DataDescribe = (tagDataDescribe*)pDataBuffer1;
		wPacketSize+=sizeof(tagDataDescribe);
		switch (DataDescribe->wDataDescribe)
		{
		case DTP_GR_NICK_NAME:		//用户昵称
			{
				CopyMemory(&UserInfo.szNickName, cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szNickName[CountArray(UserInfo.szNickName)-1]=0;
				//std::string name=UserInfo.szNickName;
				//CCLog("gameId:%ld nick:%s",pUserInfoHead->dwGameID,Tools::GBKToUTF8(UserInfo.szNickName));
				//if (strcmp(Tools::GBKToUTF8(UserInfo.szNickName),"(null)")==0)
				{
					//CCLog("null"); 
				}
			}
			break;
		case DTP_GR_GROUP_NAME:
			{
				CCLog("社团");
			}
			break;
		case DTP_GR_UNDER_WRITE:
			{
				CopyMemory(UserInfo.szUnderWrite,cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite)-1]=0;
				CCLog("签名:%s",Tools::GBKToUTF8(UserInfo.szUnderWrite));
			}
			break;
		}
		wPacketSize+=DataDescribe->wDataSize;
		if (wPacketSize>=wDataSize)
		{
			break;
		}
	}
#endif
#if (DEBUG_TEST==1)
	//效验参数
	assert(wDataSize>=sizeof(tagMobileUserInfoHead));
	if (wDataSize<sizeof(tagMobileUserInfoHead)) return false;
	int size=sizeof(tagMobileUserInfoHead);
	//消息处理
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)pDataBuffer;
	int wPacketSize=0;
	//变量定义
	tagUserInfo UserInfo;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET + sizeof(TCP_Head)];
	CopyMemory(cbDataBuffer, pDataBuffer, wDataSize);
	CCLog("-------------------------%d",wDataSize);
	wPacketSize+=sizeof(tagMobileUserInfoHead);
	while (true)
	{
		void * pDataBuffer1 = cbDataBuffer + wPacketSize;
		tagDataDescribe *DataDescribe = (tagDataDescribe*)pDataBuffer1;
		wPacketSize+=sizeof(tagDataDescribe);
		switch (DataDescribe->wDataDescribe)
		{
		case DTP_GR_NICK_NAME:		//用户昵称
			{
				CopyMemory(&UserInfo.szNickName, cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szNickName[CountArray(UserInfo.szNickName)-1]=0;
				CCLog("nick:%s  %s",UserInfo.szNickName,Tools::GBKToUTF8(UserInfo.szNickName));
				//if (strcmp(Tools::GBKToUTF8(UserInfo.szNickName),"(null)")==0)
				{
					//CCLog("null"); 
				}
			}
			break;
		case DTP_GR_GROUP_NAME:
			{
				CCLog("社团");
			}
			break;
		case DTP_GR_UNDER_WRITE:
			{
				CopyMemory(UserInfo.szUnderWrite,cbDataBuffer + wPacketSize,DataDescribe->wDataSize);
				UserInfo.szUnderWrite[CountArray(UserInfo.szUnderWrite)-1]=0;
				CCLog("签名:%s",Tools::GBKToUTF8(UserInfo.szUnderWrite));
			}
			break;
		}
		wPacketSize+=DataDescribe->wDataSize;
		if (wPacketSize>=wDataSize)
		{
			break;
		}
	}
#endif
	//


	
	
 	return true;
}
