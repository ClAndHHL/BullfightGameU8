//
//  GameHUD.cpp
//  BullfightGame
//
//  Created by 张 恒 on 15/3/16.
//
//

#include "GameControlOxOneByOne.h"
#include "../../Tools/DataModel.h"
#include "../../Network/CMD_Server/CMD_Ox_OneByOne.h"
#include "../../Tools/Tools.h"
#include "../../Tools/SoundConfig.h"
GameControlOxOneByOne::GameControlOxOneByOne()
	:isShowAllUserOx(false)
{

}
GameControlOxOneByOne::~GameControlOxOneByOne(){
	DataModel::sharedDataModel()->mTagUserInfo.clear();
}
void GameControlOxOneByOne::onEnter(){
	GameControlBase::onEnter();
	//添加标题 
	getMainScene()->addTitle();
	hideAllActionPanel();
}
void GameControlOxOneByOne::onExit(){
	GameControlBase::onExit();
}
int GameControlOxOneByOne::getChairIndex(int meChairID,int chairID){
	if (meChairID==0)
	{
		if (chairID==0)
		{
			return 3;
		}
	}else
	{
		if (chairID==1)
		{
			return 3;
		}
	}
	return 0;
}
//开牌
void GameControlOxOneByOne::menuOpenCard(CCObject* pSender, TouchEventType type){
	switch (type)
	{
	case TOUCH_EVENT_ENDED:
	{
		hideTimer(true);
		getMainScene()->cardLayer->sortingOx(getMeChairID(),3);
		showActionPrompt(ACTION_PROMPT_OPEN_CARD, CCPointZero);
		pOptOx->setEnabled(false);		
		CMD_C_OxCard OxCard;
		OxCard.bOX=GetOxCard(DataModel::sharedDataModel()->card[getMeChairID()],5);
		//发送信息
		bool isSend = GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.SendData(MDM_GF_GAME, SUB_C_OPEN_CARD, &OxCard, sizeof(OxCard));
		getMainScene()->setGameStateWithUpdate(MainSceneOxTwo::STATE_WAIT);
	}
		break;
	default:
		break;
	}
}
//用户进入
void GameControlOxOneByOne::onUserEnter(){
	//CCLOG("<<%s>>",__FUNCTION__);
	/*std::map<long, tagUserInfo>::iterator iter;
	for (iter = DataModel::sharedDataModel()->mTagUserInfo.begin(); iter != DataModel::sharedDataModel()->mTagUserInfo.end(); iter++)
	{
		if (iter->second.wChairID > 6 || iter->second.wChairID < 1)
		{
			continue;
		}
		CCLOG("server:%d  view位置 :%d   me:%d %s<<%s>>", iter->second.wChairID, getViewChairID(iter->second.wChairID), DataModel::sharedDataModel()->userInfo->wChairID, Tools::GBKToUTF8(iter->second.szNickName), __FUNCTION__);
		getMainScene()->playerLayer->setUserInfo(getViewChairID(iter->second.wChairID), iter->second);

	}*/
	//std::map<long, tagUserInfo> tempTagUserInfo;w
	//memcpy(&tempTagUserInfo, &DataModel::sharedDataModel()->mTagUserInfo, sizeof(DataModel::sharedDataModel()->mTagUserInfo));
	/*if (isChangeChair)
	{
		return;
	}*/


	std::map<long, tagUserInfo> tempTagUserInfo = DataModel::sharedDataModel()->mTagUserInfo;
	//memcpy(&tempTagUserInfo, &DataModel::sharedDataModel()->mTagUserInfo, sizeof(DataModel::sharedDataModel()->mTagUserInfo));
	
	std::map<long, tagUserInfo>::iterator iterUser = tempTagUserInfo.find(DataModel::sharedDataModel()->userInfo->dwUserID);
	if (iterUser!=tempTagUserInfo.end())
	{
		if (iterUser->second.wChairID>5||iterUser->second.wChairID<0)
		{
			return;
		}
		DataModel::sharedDataModel()->userInfo->wChairID = iterUser->second.wChairID;
		DataModel::sharedDataModel()->userInfo->wTableID = iterUser->second.wTableID;
	}

	std::map<long, tagUserInfo>::iterator iter;
	for (iter = tempTagUserInfo.begin(); iter != tempTagUserInfo.end(); iter++)
	{
		if (iter->second.wChairID > 5 || iter->second.wChairID < 0
			//||iter->second.wTableID!=DataModel::sharedDataModel()->userInfo->wTableID
			)
		{
			//CCLOG("------------->6 <1 %d<<%s>>",iter->second.wChairID,__FUNCTION__);
			//tempTagUserInfo.erase(iter++);ww
			continue;
		}

		CCLOG("table:%d server:%d  view位置 :%d   me:%d %s<<%s>>",iter->second.wTableID, iter->second.wChairID, getViewChairID(iter->second.wChairID), DataModel::sharedDataModel()->userInfo->wChairID, Tools::GBKToUTF8(iter->second.szNickName).c_str(), __FUNCTION__);
		getMainScene()->playerLayer->setUserInfo(getViewChairID(iter->second.wChairID), iter->second);
	}
	CCLOG("=======================================<<%s>>",__FUNCTION__);
	//DataModel::sharedDataModel()->mTagUserInfo.clear();
}
/*void GameControlOxOneByOne::onUserEnterWithUpdate(tagUserInfo *user){
	tagUserInfo *tempUser=user;
	//memset(tempUser, 0, sizeof(tagUserInfo));
	memcpy(tempUser,user,sizeof(tagUserInfo));
	if (user->wChairID > 5 || user->wChairID < 0) return;

	getMainScene()->playerLayer->setUserInfo(getViewChairID(tempUser->wChairID), *tempUser);
}*/
//获取视图位置
int GameControlOxOneByOne::getViewChairID(int severChairID){
	int viewChair = 3 - DataModel::sharedDataModel()->userInfo->wChairID;
	if (viewChair<0)
	{
		viewChair += MAX_CHAIR_COUNT;
	}
	viewChair += severChairID;
	viewChair %= MAX_CHAIR_COUNT;
	return viewChair;
}
//游戏场景
bool GameControlOxOneByOne::OnEventSceneMessage(void * pData, WORD wDataSize){
	//@if (IsLookonMode())m_GameClientView.m_bLookOnUser = true;
	switch (m_cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusFree)) return false;
		CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;
		pPanelReady->setEnabled(true);
		isPalySoundWarn = true;
		//设置控件
		//@if (IsLookonMode() == false && GetMeUserItem()->GetUserStatus() != US_READY)
		{
			//@SetGameClock(GetMeChairID(), IDI_START_GAME, TIME_USER_START_GAME);
			//@m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		}
		//@m_GameClientView.lCellScore = pStatusFree->lCellScore;
		return true;
	}
	case GS_TK_SCORE:	//下注状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusScore)) return false;
		CMD_S_StatusScore * pStatusScore = (CMD_S_StatusScore *)pData;
		isPalySoundWarn = false;
		//设置变量
		//@m_lTurnMaxScore = pStatusScore->lTurnMaxScore;
		//@CopyMemory(m_lTableScore, pStatusScore->lTableScore, sizeof(m_lTableScore));
		CopyMemory(m_cbPlayStatus, pStatusScore->cbPlayStatus, sizeof(m_cbPlayStatus));

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//视图位置
			//@m_wViewChairID[i] = SwitchViewChairID(i);
			//@m_GameClientView.SetUserPlayingStatus(m_wViewChairID[i], m_cbPlayStatus[i]);

			//桌面筹码
			//@if (m_lTableScore[i]>0L)m_GameClientView.SetUserTableScore(m_wViewChairID[i], m_lTableScore[i]);

			//获取用户
			//@IClientUserItem * pUserData = GetTableUserItem(i);
			//@if (pUserData == NULL) continue;


			//用户名字
			//@lstrcpyn(m_szNickNames[i], pUserData->GetNickName(), CountArray(m_szNickNames[i]));
		}

		//设置筹码
		//@if (!IsLookonMode() && pStatusScore->lTurnMaxScore > 0L && m_lTableScore[GetMeChairID()] == 0L)
		{
			long long lUserMaxScore[GAME_PLAYER];
			memset(lUserMaxScore, 0,sizeof(lUserMaxScore));
			//@LONGLONG lTemp = m_lTurnMaxScore;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//@if (i>0)lTemp /= 2;
				//@lUserMaxScore[i] = __max(lTemp, 1L);
			}

			//更新控件
			//@UpdateScoreControl(lUserMaxScore, SW_HIDE);

			//实际定时器
			//@SetTimer(IDI_TIME_USER_ADD_SCORE, (TIME_USER_ADD_SCORE)* 1000, NULL);
		}


		//等待标志
		//@m_GameClientView.SetWaitInvest(true);


		//辅助显示中心时钟
		//@SetGameClock(GetMeChairID(), IDI_NULLITY, TIME_USER_ADD_SCORE);

		return true;
	}
	case GS_TK_PLAYING:	//游戏状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusPlay)) return false;
		CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;
		hideAllActionPanel();
		isPalySoundWarn = false;
		//设置变量
		m_lTurnMaxScore = pStatusPlay->lTurnMaxScore;
		CopyMemory(m_lTableScore, pStatusPlay->lTableScore, sizeof(m_lTableScore));
		CopyMemory(m_bUserOxCard, pStatusPlay->bOxCard, sizeof(m_bUserOxCard));
		CopyMemory(m_cbPlayStatus, pStatusPlay->cbPlayStatus, sizeof(m_cbPlayStatus));
		isShowAllUserOx = false;
 		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//视图位置
			//@m_wViewChairID[i] = SwitchViewChairID(i);
			//@m_GameClientView.SetUserPlayingStatus(m_wViewChairID[i], m_cbPlayStatus[i]);

			//桌面筹码
			//@if (m_lTableScore[i]>0L)m_GameClientView.SetUserTableScore(m_wViewChairID[i], m_lTableScore[i]);

			//获取用户
			//@IClientUserItem * pUserData = GetTableUserItem(i);
			//@if (pUserData == NULL) continue;

			//扑克数据
			//@CopyMemory(m_cbHandCardData[i], pStatusPlay->cbHandCardData[i], MAX_COUNT);

			//用户名字
			//@lstrcpyn(m_szNickNames[i], pUserData->GetNickName(), CountArray(m_szNickNames[i]));
		}


		//设置界面
		long long lTableScore = 0L;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//设置位置
			//@WORD wViewChairID = m_wViewChairID[i];

			//设置扑克
			if (m_cbPlayStatus[i] == USEX_PLAYING)
			{
				getMainScene()->cardLayer->canSendCard[getViewChairID(i)] = true;
				
				//显示牌型
				bool bOxSound = false;
				
					//WORD wViewChairID=m_wViewChairID[i];
					if (i == getMeChairID() && !IsLookonMode())
						continue;
					bool isCardError = false;
					for (int j = 0; j < MAX_COUNT; j++)
					{
						//DataModel::sharedDataModel()->card[i][j] = pStatusPlay->cbHandCardData[i][j];
						DataModel::sharedDataModel()->card[i][j] = 80;
						if (pStatusPlay->cbHandCardData[i][j] == 0)
						{
							isCardError = true;
							break;
						}
					}
					if (!isCardError)
					{
						getMainScene()->cardLayer->showCard(false,getViewChairID(i), i);
						//getMainScene()->cardLayer->sortingOx(i, getViewChairID(i));
					}
			}
			else{
				getMainScene()->cardLayer->canSendCard[getViewChairID(i)] = false;
			}
		}
		//hideAllActionPanel();
		//getMainScene()->setGameStateWithUpdate(MainSceneOxTwo::STATE_END);
		
		//@WORD wMeChiarID = GetMeChairID();
		//@WORD wViewChairID = m_wViewChairID[wMeChiarID];
		//@if (!IsLookonMode())m_GameClientView.m_CardControl[wViewChairID].SetPositively(true);
		//@if (IsAllowLookon() || !IsLookonMode())
		{
			//@m_GameClientView.m_CardControl[wViewChairID].SetDisplayItem(true);
		}

		//摊牌标志
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbPlayStatus[i] != USEX_PLAYING) continue;
			if (m_bUserOxCard[i] != 0xff)
			{
				//@m_GameClientView.ShowOpenCard(m_wViewChairID[i]);
			}
		}

		//控件处理
		if (!IsLookonMode())
		{
			//显示控件
			//@if (m_bUserOxCard[wMeChiarID] == 0xff)
			{
				//@OnSendCardFinish(0, 0);
			}
			//@else
			{
				//@m_GameClientView.m_CardControl[wViewChairID].SetPositively(false);
				//@if (m_bUserOxCard[wMeChiarID] == TRUE)//牛牌分类
				{
					//设置变量
					BYTE bTemp[MAX_COUNT];
					//@CopyMemory(bTemp, m_cbHandCardData[wMeChiarID], sizeof(bTemp));

					

					//@BYTE bCardValue = m_GameLogic.GetCardType(bTemp, MAX_COUNT, bTemp);
					//@ASSERT(bCardValue > 0);

					//设置控件
					//@m_GameClientView.m_CardControl[wViewChairID].SetCardData(bTemp, 3);
					//@m_GameClientView.m_CardControlOx[wViewChairID].SetCardData(&bTemp[3], 2);

					//显示点数
					//@m_GameClientView.SetUserOxValue(wViewChairID, bCardValue);
				}
				//@else
				{
					//无牛
					//@m_GameClientView.SetUserOxValue(wViewChairID, 0);
				}
			}
		}
		return true;
		
	}
	default:
		pPanelReady->setEnabled(true);
		break;
	}

	return false;
}
//游戏中
void GameControlOxOneByOne::onEventGameIng(WORD wSubCmdID, void * pDataBuffer, unsigned short wDataSize){
	switch (wSubCmdID)
	{
	case SUB_S_CALL_BANKER:	//用户叫庄
	{
		//消息处理
		OnSubCallBanker(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_GAME_START:	//游戏开始
	{
		Tools::playSound(kSoundStart);
		//消息处理
		OnSubGameStart(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_ADD_SCORE:	//用户下注
	{
		//消息处理
		OnSubAddScore(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_SEND_CARD:	//发牌消息
	{
		//消息处理
		OnSubSendCard(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_OPEN_CARD:	//用户摊牌
	{
		//消息处理
		OnSubOpenCard(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_PLAYER_EXIT:	//用户强退
	{
		//消息处理
		OnSubPlayerExit(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_GAME_END:	//游戏结束
	{
								//结束动画
								//m_GameClientView.FinishDispatchCard();
		//消息处理
		OnSubGameEnd(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_GAME_BASE:
	{
		OnSubGameBase(pDataBuffer, wDataSize);
	}
		break;
	case SUB_S_USER_OPEN:
	{
		onSubUserOpen(pDataBuffer, wDataSize);
	}
		break;
	default:
		CCLOG("--------------------gameIng:%d<<%s>>", wSubCmdID, __FUNCTION__);
		break;
	}
}
//设置基数
bool GameControlOxOneByOne::OnSubGameBase(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_GameBase)) return false;
	CMD_S_GameBase * pGameBase = (CMD_S_GameBase *)pBuffer;

	//m_GameClientView.lCellScore = pGameBase->lCellScore;
	return true;
}
//用户准备
void GameControlOxOneByOne::onUserReady(CMD_GR_UserStatus *info){
	
	int indexPlayer = getViewChairID(info->UserStatus.wChairID);
	if (indexPlayer < 0||isChangeChair)
	{
		return;
	}
	getMainScene()->playerLayer->pPlayerData[indexPlayer]->showActionType(PlayerData::ACTION_READY);
}
//隐藏用户
void GameControlOxOneByOne::hidePlayer(CMD_GR_UserStatus *userInfo){
	std::map<long, tagUserInfo>::iterator iterUser = DataModel::sharedDataModel()->mTagUserInfo.find(userInfo->dwUserID);
	if (iterUser != DataModel::sharedDataModel()->mTagUserInfo.end())
	{
		getMainScene()->playerLayer->pPlayerData[getViewChairID(iterUser->second.wChairID)]->hidePlayer();
		DataModel::sharedDataModel()->mTagUserInfo.erase(userInfo->dwUserID);
	}
}
//用户下注
bool GameControlOxOneByOne::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore = (CMD_S_AddScore *)pBuffer;
	CCLOG("userScore : %lld<<%s>>",pAddScore->lAddScoreCount,__FUNCTION__);
	/*//删除定时器/控制按钮
	if (IsCurrentUser(pAddScore->wAddScoreUser) && m_GameClientView.m_btOneScore.IsWindowVisible() == TRUE)
	{
		KillTimer(IDI_TIME_USER_ADD_SCORE);
		UpdateScoreControl(NULL, SW_HIDE);
	}

	//变量定义
	WORD wAddScoreUser = pAddScore->wAddScoreUser;
	WORD wViewChairID = m_wViewChairID[wAddScoreUser];

	//加注界面
	m_lTableScore[pAddScore->wAddScoreUser] = pAddScore->lAddScoreCount;
	//	m_GameClientView.SetUserTableScore(wViewChairID,pAddScore->lAddScoreCount);

	//播放声音
	//if (!IsCurrentUser(pAddScore->wAddScoreUser) && m_cbPlayStatus[wAddScoreUser]==USEX_PLAYING)
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
	*/
	return true;
}
//游戏开始
bool GameControlOxOneByOne::OnSubGameStart(const void * pBuffer, WORD wDataSize){
	//效验数据
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;
	//隐藏行为类型显示语
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		getMainScene()->playerLayer->pPlayerData[i]->hideActionType();
	}


	/*//用户信息
	for (WORD i = 0; i<GAME_PLAYER; i++)
	{
		//视图位置
		m_wViewChairID[i] = SwitchViewChairID(i);

		//获取用户
		IClientUserItem * pUserData = GetTableUserItem(i);
		if (pUserData == NULL)
		{
			m_cbPlayStatus[i] = USEX_NULL;
		}
		else
		{
			m_cbPlayStatus[i] = USEX_PLAYING;

			//用户名字
			lstrcpyn(m_szNickNames[i], pUserData->GetNickName(), CountArray(m_szNickNames[i]));
		}

		//设置界面
		m_GameClientView.SetUserPlayingStatus(m_wViewChairID[i], m_cbPlayStatus[i]);
	}

	//旁观者清理数据
	if (IsLookonMode()) OnStart(0, 0);
	//删除定时器/按钮
	if (m_GameClientView.m_btIdler.IsWindowVisible() == TRUE)
	{
		KillGameClock(IDI_CALL_BANKER);
		m_GameClientView.m_btIdler.ShowWindow(SW_HIDE);
	}
	*/
	//设置变量
	DataModel::sharedDataModel()->m_lTurnMaxScore = pGameStart->lTurnMaxScore;
	/*//m_wBankerUser=pGameStart->wBankerUser;
	m_GameClientView.SetWaitCall(0xff);

	//设置界面
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//设置筹码
	if (!IsLookonMode() && pGameStart->lTurnMaxScore>0)
	{
		LONGLONG lUserMaxScore[4];
		ZeroMemory(lUserMaxScore, sizeof(lUserMaxScore));
		LONGLONG lTemp = m_lTurnMaxScore;
		for (WORD i = 0; i<4; i++)
		{
			if (i>0)lTemp /= 2;
			lUserMaxScore[i] = __max(lTemp, 1L);
		}

		//更新控件
		//ActiveGameFrame();
		UpdateScoreControl(lUserMaxScore, SW_HIDE);

		//实际定时器
		//SetTimer(IDI_TIME_USER_ADD_SCORE,(TIME_USER_ADD_SCORE)*1000,NULL);
	}



	//等待标志
	m_GameClientView.SetWaitInvest(true);

	//辅助显示中心时钟
	//SetGameClock(GetMeChairID(),IDI_NULLITY,TIME_USER_ADD_SCORE);

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(), TEXT("GAME_START"));

	for (WORD i = 0; i<MAX_COUNT; i++)
	{
		m_GameClientView.m_CardControl[i].m_bShow[i] = false;
		m_GameClientView.m_CardControlOx[i].m_bShow[i] = false;
	}

	//用户信息
	for (WORD i = 0; i<GAME_PLAYER; i++)
	{
		//视图位置
		m_wViewChairID[i] = SwitchViewChairID(i);

		//获取用户
		IClientUserItem * pUserData = GetTableUserItem(i);
		if (pUserData != NULL)
		{
			if (m_cbPlayStatus[i] == USEX_PLAYING)
			{
				m_GameClientView.OnUserAddScore(m_wViewChairID[i], m_lTurnMaxScore, false);
			}
		}
	}
	//	m_GameClientView.PlaceUserJetton(1,100);
	*/
	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore = DataModel::sharedDataModel()->m_lTurnMaxScore;
	GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.SendData(MDM_GF_GAME, SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
	return true;
}
//发牌消息
bool GameControlOxOneByOne::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;
	CCLOG("-----------------------------------发牌");
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		int viewChair = getViewChairID(i);
		for (int j = 0; j < MAX_COUNT; j++)
		{
			DataModel::sharedDataModel()->card[viewChair][j] = pSendCard->cbCardData[viewChair][j];
			if (pSendCard->cbPlayStatus[i] == USEX_PLAYING){
				getMainScene()->cardLayer->canSendCard[viewChair] = true;
			}
			else 
			{
				getMainScene()->cardLayer->canSendCard[viewChair] = false;
			}
		}
		CCLOG("----------static %d<<%s>>", pSendCard->cbPlayStatus[i], __FUNCTION__);
	}
	CCLOG("-++++++++++++++----------------------------------发牌");
	getMainScene()->setServerStateWithUpdate(MainSceneOxTwo::STATE_SEND_CARD);
	/*
	//效验数据
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
	//删除定时器
	KillGameClock(IDI_NULLITY);

	m_GameClientView.SetWaitInvest(false);

	CopyMemory(m_cbHandCardData,pSendCard->cbCardData,sizeof(m_cbHandCardData));

	WORD wMeChiarID=GetMeChairID();
	WORD wViewChairID=m_wViewChairID[wMeChiarID];
	if((IsAllowLookon() || !IsLookonMode()) && m_cbPlayStatus[wMeChiarID]==USEX_PLAYING)
	{
	m_GameClientView.m_CardControl[wViewChairID].SetDisplayItem(true);
	if(m_bAutoOpenCard)
	{
	for(WORD i=0;i<MAX_COUNT;i++)
	{
	m_GameClientView.m_CardControl[wViewChairID].m_bShow[i] = true;
	m_GameClientView.m_CardControlOx[wViewChairID].m_bShow[i] = true;
	}
	}
	else
	{
	for(WORD i=0;i<MAX_COUNT;i++)
	m_GameClientView.m_CardControl[wViewChairID].m_bShow[i] = false;
	}
	}

	//派发扑克
	for(WORD i=0;i<MAX_COUNT;i++)
	{
	for (WORD j=m_wWinUser;j<m_wWinUser+GAME_PLAYER;j++)
	{
	WORD w=j%GAME_PLAYER;
	if (m_cbPlayStatus[w]==USEX_PLAYING)
	{
	if (w==GetMeChairID())
	{
	WORD wViewChairID=m_wViewChairID[w];
	if(pSendCard->cbCardData[w][i]!=0)
	{
	m_GameClientView.DispatchUserCard(wViewChairID,m_cbHandCardData[w][i]);
	}
	}else
	{
	WORD wViewChairID=m_wViewChairID[w];
	if(pSendCard->cbCardData[GetMeChairID()][i]!=0)
	{
	m_GameClientView.DispatchUserCard(wViewChairID,m_cbHandCardData[GetMeChairID()][i]);
	}
	}
	}
	}
	}
	*/

	return true;
}

//用户摊牌
bool GameControlOxOneByOne::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_Open_Card)) return false;
	CMD_S_Open_Card * pOpenCard = (CMD_S_Open_Card *)pBuffer;
	CCLOG("openCardID:%d  bOpen:%d", pOpenCard->wPlayerID, pOpenCard->bOpen);
	/*
	//设置变量
	WORD wMeChairID=GetMeChairID();
	WORD wID=pOpenCard->wPlayerID;
	WORD wViewChairID=m_wViewChairID[wID];
	m_bUserOxCard[wID]=pOpenCard->bOpen;
	m_GameClientView.ShowOpenCard(wViewChairID);

	//摊牌标志
	if(!IsCurrentUser(wID))
	{
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OPEN_CARD"));
	}
	else if(m_GameClientView.m_btOpenCard.IsWindowVisible()==TRUE)
	{
	//删除时间/控件
	KillTimer(IDI_TIME_OPEN_CARD);
	m_GameClientView.m_btOx.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btHintOx.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShortcut.ShowWindow(SW_HIDE);

	//显示不构成牛牛
	if(m_bUserOxCard[wID]==FALSE)
	{
	//			m_GameClientView.m_CardControl[wViewChairID].ShootAllCard(false);
	m_GameClientView.SetUserOxValue(wViewChairID,0);
	}
	else
	{
	//提示再分类牛牌
	OnHintOx(0,0);
	m_GameClientView.m_CardControl[wViewChairID].SetOX(true);
	OnSortCard(0,0);
	}
	m_GameClientView.m_CardControl[wViewChairID].SetPositively(false);
	}

	for(WORD i=0;i<MAX_COUNT;i++)
	{
	m_GameClientView.m_CardControl[wViewChairID].m_bShow[i] = true;
	m_GameClientView.m_CardControlOx[wViewChairID].m_bShow[i] = true;

	// 		m_GameClientView.m_CardControl[wViewChairID].Invalidate();
	// 		m_GameClientView.m_CardControlOx[wViewChairID].Invalidate();
	}

	m_GameClientView.RefreshGameView();
	*/
	return true;
}
//用户开牌
void GameControlOxOneByOne::onSubUserOpen(const void * pBuffer, WORD wDataSize){
	//效验参数
	if (wDataSize != sizeof(CMD_S_PlayerOpen)) return;
	CMD_S_PlayerOpen * pPlayerOpen = (CMD_S_PlayerOpen *)pBuffer;
	showOxAllNum = 0;
	showOxCurNum = 0;
	isShowAllUserOx = true;
	hideActionPrompt();
	//显示牌型
	bool bOxSound = false;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		int index = (i + getMeChairID())%GAME_PLAYER;
		//WORD wViewChairID=m_wViewChairID[i];
		if (index == getMeChairID() && !IsLookonMode())
			continue;
		bool isCardError = false;
		for (int j = 0; j < MAX_COUNT; j++)
		{
			DataModel::sharedDataModel()->card[index][j] = pPlayerOpen->cbCardData[index][j];
			if (pPlayerOpen->cbCardData[index][j] == 0)
			{
				isCardError = true;
				break;
			}
		}
		if (!isCardError)
		{
			showOxAllNum++;

			CCNode *pTempNode = CCNode::create();
			this->addChild(pTempNode);
			pTempNode->setTag(index);

			pTempNode->runAction(CCSequence::create(
				CCDelayTime::create(showOxAllNum)
				,CCCallFuncN::create(this,SEL_CallFuncN(&GameControlOxOneByOne::onUserShowOx))
				,NULL));
		}
	}
	
}
void GameControlOxOneByOne::onUserShowOx(CCNode *pNode){
	int i = pNode->getTag();
	pNode->removeFromParentAndCleanup(true);

	getMainScene()->cardLayer->showCard(true, getViewChairID(i), i);
	getMainScene()->cardLayer->sortingOx(i, getViewChairID(i));
	showOxCurNum++;
	if (showOxCurNum>=showOxAllNum)
	{
		//所有用户牌都显示完了
		GameIngMsgHandler::sharedGameIngMsgHandler()->gameSocket.SendData(MDM_GF_GAME, SUB_C_OPEN_END);
	}
}
//显示结算（）
void GameControlOxOneByOne::showResultAnimation(){
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		long long lGameScore = llGameScore[i];
		if (getMainScene()->cardLayer->canSendCard[getViewChairID(i)])
		{
			if (getViewChairID(i) == 3)
			{
				DataModel::sharedDataModel()->userInfo->lScore += lGameScore;
				if (lGameScore>0)
				{
					Tools::playSound(kSoundWin);
				}
				else if (lGameScore<0)
				{
					Tools::playSound(kSoundLost);
				}
			}
			getMainScene()->playerLayer->showResultAnimation(getViewChairID(i), lGameScore);
		}
	}

	if (m_cbPlayStatus[getMeChairID()] == USEX_DYNAMIC)
	{
		Tools::playSound(kSoundEnd);
	}
	memset(m_cbPlayStatus, 0, sizeof(m_cbPlayStatus));
	isPalySoundWarn = true;
}

//游戏结束
bool GameControlOxOneByOne::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize != sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;
	hideActionPrompt();
	
	CopyMemory(llGameScore, pGameEnd->lGameScore, sizeof(llGameScore));
	/*for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		long long lGameScore = pGameEnd->lGameScore[i];
		if (i == getMeChairID()){
			getMainScene()->playerLayer->showResultAnimation(3, lGameScore);
			if (lGameScore != 0){
				UIPanel *pPlayer0 = getMainScene()->playerLayer->pPlayerData[0]->pPlayerPanel;
				UIPanel *pPlayer3 = getMainScene()->playerLayer->pPlayerData[3]->pPlayerPanel;
				for (int i = 0; i < 60; i++)
				{
					if (lGameScore>0)
					{
						CCPoint begingPos = ccpAdd(pPlayer0->getPosition(), ccp(pPlayer0->getContentSize().width / 2, pPlayer0->getContentSize().height / 2));
						CCPoint endPos = ccpAdd(pPlayer3->getPosition(), ccp(pPlayer3->getContentSize().width / 2, pPlayer3->getContentSize().height / 2));

						//goldJump(i, begingPos, endPos);
					}
					else
					{
						CCPoint endPos = ccpAdd(pPlayer0->getPosition(), ccp(pPlayer0->getContentSize().width / 2, pPlayer0->getContentSize().height / 2));
						CCPoint begingPos = ccpAdd(pPlayer3->getPosition(), ccp(pPlayer3->getContentSize().width / 2, pPlayer3->getContentSize().height / 2));

						//goldJump(i, begingPos, endPos);
					}
				}
			}
		}
		else
		{
			getMainScene()->playerLayer->showResultAnimation(i, lGameScore);
		}
	}*/
	//显示牌型
	if (isShowAllUserOx)
	{
		//显示积分
		showResultAnimation();
		isShowAllUserOx = false;
	}
	else
	{
		bool bOxSound = false;
		iDelayedMaxCount = 0;
		iCurDelayedCount = 0;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			//WORD wViewChairID=m_wViewChairID[i];
			if (i == getMeChairID() && !IsLookonMode())
				continue;
			bool isCardError = false;
			for (int j = 0; j < MAX_COUNT; j++)
			{
				DataModel::sharedDataModel()->card[i][j] = pGameEnd->cbCardData[i][j];
				if (pGameEnd->cbCardData[i][j] == 0)
				{
					isCardError = true;
					break;
				}
			}
			if (!isCardError)
			{
				iDelayedMaxCount++;

				CCNode *pTempNode = CCNode::create();
				this->addChild(pTempNode);
				pTempNode->setTag(i);


				pTempNode->runAction(CCSequence::create(
					CCDelayTime::create(iDelayedMaxCount)
					, CCCallFuncN::create(this, SEL_CallFuncN(&GameControlBase::delayedShowOx))
					, NULL));
				//getMainScene()->cardLayer->showCard(true, getViewChairID(i), i);
				//getMainScene()->cardLayer->sortingOx(i, getViewChairID(i));
			}
		}
	}
	


	/*pEndLayer = GameEndLayer::create();
	this->addChild(pEndLayer);
	pEndLayer->showEnd(pGameEnd->lGameScore[getMeChairID()] >= 0);*/
	getMainScene()->setGameStateWithUpdate(MainSceneOxTwo::STATE_END);
	/*
	CopyMemory(m_cbHandCardData,pGameEnd->cbCardData,sizeof(m_cbHandCardData));

	for (int i=0;i<GAME_PLAYER;i++)
	{
	if (i!=GetMeChairID())
	{
	WORD wViewchair=m_wViewChairID[i];
	m_GameClientView.m_CardControl[wViewchair].SetCardData(NULL,0);
	}
	}

	for(WORD i=0;i<MAX_COUNT;i++)
	{
	for (WORD j=m_wBankerUser;j<m_wBankerUser+GAME_PLAYER;j++)
	{
	WORD w=j%GAME_PLAYER;
	if (m_cbPlayStatus[w]==TRUE)
	{
	WORD wViewChairID=m_wViewChairID[w];
	if(pGameEnd->cbCardData[w][i]!=0&&w!=GetMeChairID())
	{
	BYTE cbCardData[MAX_COUNT];
	//BYTE cbChangeCardData[MAX_C_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID].GetCardData(cbCardData,CountArray(cbCardData));

	cbCardData[cbCardCount++]=m_cbHandCardData[w][i];
	m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
	}
	}
	}
	}

	//删除定时器
	KillGameClock(IDI_NULLITY);
	KillGameClock(IDI_CALL_BANKER);
	KillTimer(IDI_TIME_USER_ADD_SCORE);
	KillTimer(IDI_TIME_OPEN_CARD);

	//清理数据
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
	m_GameClientView.ShowOpenCard(i,FALSE);
	}
	m_GameClientView.SetWaitInvest(false);
	UpdateScoreControl(NULL,SW_HIDE);
	//m_GameClientView.m_btOx.ShowWindow(SW_HIDE);
	m_GameClientView.m_btHintOx.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btReSort.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShortcut.ShowWindow(SW_HIDE);
	m_GameClientView.m_btBanker.ShowWindow(SW_HIDE);
	m_GameClientView.m_btIdler.ShowWindow(SW_HIDE);
	m_GameClientView.SetWaitCall(0xff);
	m_GameClientView.m_bOpenCard=false;

	//状态设置
	SetGameStatus(GAME_STATUS_FREE);

	//播放声音
	if (IsLookonMode()==false)
	{
	if (pGameEnd->lGameScore[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//显示积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
	if(GetTableUserItem(i)!=NULL)m_GameClientView.ShowScore(m_wViewChairID[i],true);
	m_GameClientView.SetUserTableScore(m_wViewChairID[i],pGameEnd->lGameScore[i]);
	m_GameClientView.SetUserTotalScore(m_wViewChairID[i],pGameEnd->lGameScore[i]);
	}
	m_GameClientView.m_ScoreView.ResetScore();
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
	//设置信息
	if (pGameEnd->lGameScore[i]!=0L || i==m_wBankerUser)
	{
	m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax[i],i);
	if (m_szNickName[i][0]=='#') m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameScore[i]);
	else m_GameClientView.m_ScoreView.SetGameScore(i,m_szNickName[i],pGameEnd->lGameScore[i]);
	}
	}
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//显示牌型
	bool bOxSound=false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
	WORD wViewChairID=m_wViewChairID[i];
	if(i==GetMeChairID() && !IsLookonMode())
	continue;

	m_GameClientView.m_CardControl[wViewChairID].SetDisplayFlag(true);

	//牛牛牌型
	if(m_cbHandCardData[i][0]>0)
	{
	if(m_bUserOxCard[i]==TRUE)
	{
	//扑克数据
	BYTE bCardData[MAX_COUNT];
	CopyMemory(bCardData,m_cbHandCardData[i],sizeof(bCardData));

	//获取牛牛数据
	bool bOx=m_GameLogic.GetOxCard(bCardData,MAX_COUNT);
	ASSERT(bOx);

	//加载数据
	m_GameClientView.m_CardControl[wViewChairID].SetCardData(bCardData,3);
	m_GameClientView.m_CardControlOx[wViewChairID].SetCardData(&bCardData[3],2);

	//显示点数
	BYTE bValue=m_GameLogic.GetCardLogicValue(bCardData[3])+m_GameLogic.GetCardLogicValue(bCardData[4]);
	if(bValue>10)bValue-=10;
	ASSERT(bValue>0);
	if(bValue>=10)bOxSound=true;
	m_GameClientView.SetUserOxValue(wViewChairID,bValue);
	}
	else
	{
	//无牛
	m_GameClientView.SetUserOxValue(wViewChairID,0);
	}
	}
	}
	m_GameClientView.ViewDisplayType(true);

	//牛牛声音
	if(bOxSound)
	{
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_OXOX"));
	}

	//开始按钮
	if (IsLookonMode()==false)
	{
	m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_USER_START_GAME);
	}

	//成绩显示在即时聊天对话框
	TCHAR szBuffer[512]=TEXT("");
	myprintf(szBuffer,CountArray(szBuffer),TEXT("本次对局结果:"));
	m_pIStringMessage->InsertNormalString(szBuffer);
	myprintf(szBuffer,CountArray(szBuffer),TEXT("用户昵称\t成绩"));
	m_pIStringMessage->InsertNormalString(szBuffer);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
	//变量定义
	IClientUserItem * pUserData=GetTableUserItem(i);
	//成绩输出
	if (pUserData!=NULL)
	{
	if(mystrlen(pUserData->GetNickName())<8)
	myprintf(szBuffer,CountArray(szBuffer),TEXT("%s:\t\t%+d"),pUserData->GetNickName(),pGameEnd->lGameScore[i]);
	else
	myprintf(szBuffer,CountArray(szBuffer),TEXT("%s:\t%+d"),pUserData->GetNickName(),pGameEnd->lGameScore[i]);
	m_pIStringMessage->InsertNormalString(szBuffer);
	}
	else if(m_szNickName[i][0]=='#')
	{
	myprintf(szBuffer,CountArray(szBuffer),TEXT("%s:%+d"),TEXT("用户离开"),pGameEnd->lGameScore[i]);
	m_pIStringMessage->InsertNormalString(szBuffer);
	}
	}
	myprintf(szBuffer,CountArray(szBuffer),TEXT("本局游戏结束。\n--------------------"));
	m_pIStringMessage->InsertNormalString(szBuffer);

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szNickName,sizeof(m_szNickName));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	*/
	
	return true;
}
