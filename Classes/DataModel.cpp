/**
 *数据模型
 *
 *
 */
#include "DataModel.h"
#include "Tools.h"
#include "GameConfig.h"
bool DataModel::isSound = true;
bool DataModel::isMusic = true;
//int DataModel::gameChapter = 0;
//int DataModel::gameCustomsPass = 0;
DataModel* _sharedContext;
DataModel::DataModel()
:isSit(false)
{
	DataModel::isMusic = Tools::getBoolByRMS(RMS_IS_MUSIC);
	DataModel::isSound = Tools::getBoolByRMS(RMS_IS_SOUND);

	deviceSize = CCDirector::sharedDirector()->getWinSize();
	gameLogic = new GameLogic();
	logonSuccessUserInfo=new CMD_MB_LogonSuccess();
}
DataModel::~DataModel() {
	CCLog("~ <<%s>>",__FUNCTION__);
	m_aTagGameKind->removeAllObjects();
	m_aTagGameKind->release();

	CC_SAFE_DELETE(gameLogic);
	CC_SAFE_DELETE(logonSuccessUserInfo);

	for (int i=0;i<tagGameServerList.size();i++)
	{
		CC_SAFE_DELETE(tagGameServerList[i]);
	}
	tagGameServerList.resize(0);
	/*
	_outputEnemysData->removeAllObjects();
	_outputEnemysData->release();
	_enemys->removeAllObjects();
	_enemys->release();
	_lineDatas->removeAllObjects();
	_lineDatas->release();
	_tMapTilesetInfo->removeAllObjects();
	_tMapTilesetInfo->release();
	_goods->removeAllObjects();
	_goods->release();
	_effectSprite->removeAllObjects();
	_effectSprite->release();
	

	_pDicSoundName->removeAllObjects();
	_pDicSoundName->release();
	_pDicSoundName = NULL;

	_pDicSound->removeAllObjects();
	_pDicSound->release();
	_pDicSound = NULL;*/
	/*
	_partners->removeAllObjects();
	_partners->release();
	
	_bulletMagicWands->removeAllObjects();
	_bulletMagicWands->release();

	
	*/
	_sharedContext = NULL;
}
DataModel* DataModel::sharedDataModel()
{
	if (_sharedContext == NULL) {
		_sharedContext = new DataModel();
		_sharedContext->initDataModel();
		return _sharedContext;
	}
	return _sharedContext;
}
void DataModel::initDataModel(){

	m_aTagGameKind = CCArray::create();
	m_aTagGameKind->retain();
	/*
	
	_outputEnemysData = CCArray::create();
	_outputEnemysData->retain();
	_enemys = CCArray::create();
	_enemys->retain();
	_lineDatas = CCArray::create();
	_lineDatas->retain();
	_tMapTilesetInfo = CCArray::create();
	_tMapTilesetInfo->retain();
	_goods = CCArray::create();
	_goods->retain();
	_effectSprite = CCArray::create();
	_effectSprite->retain();
	
	
	_pDicSoundName = CCDictionary::create();
	_pDicSoundName->retain();

	_pDicSound = CCDictionary::create();
	_pDicSound->retain();
	//_partners = CCArray::create();
	//_partners->retain();
	//
	//_bulletMagicWands = CCArray::create();
	//_bulletMagicWands->retain();*/
}
