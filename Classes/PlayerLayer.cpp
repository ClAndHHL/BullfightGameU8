

#include "PlayerLayer.h"
#include "GameConfig.h"
#include "DataModel.h"
#include "Tools.h"
PlayerLayer::PlayerLayer()
{
}
PlayerLayer::~PlayerLayer() {
}
void PlayerLayer::onEnter(){
	CCLayer::onEnter();
	initPlayerInfo();
	hidePlayer(pPlayerData[1]->pPlayerPanel);
	hidePlayer(pPlayerData[2]->pPlayerPanel);
	hidePlayer(pPlayerData[4]->pPlayerPanel);
	hidePlayer(pPlayerData[5]->pPlayerPanel);
}
void PlayerLayer::onExit(){

}
void PlayerLayer::initPlayerInfo(){
	UILayer *m_pWidget = UILayer::create();
	this->addChild(m_pWidget);

	UILayout *pWidget = dynamic_cast<UILayout*>(GUIReader::shareReader()->widgetFromJsonFile(CCS_PATH_SCENE(UIPalyer.ExportJson)));
	m_pWidget->addWidget(pWidget);

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		pPlayerData[i]=PlayerData::create();
		this->addChild(pPlayerData[i]);
		pPlayerData[i]->pPlayerPanel = static_cast<UIPanel*>(m_pWidget->getWidgetByName(CCString::createWithFormat("player%d",i)->getCString()));
		//��������
		pPlayerData[i]->pLResult=static_cast<UILabelAtlas*>(pPlayerData[i]->pPlayerPanel->getChildByName("AtlasLabelResult"));
		pPlayerData[i]->pLResult->setVisible(false);
		//�û��ǳ�
		pPlayerData[i]->pLUserName=static_cast<UILabel*>(pPlayerData[i]->pPlayerPanel->getChildByName("userName"));
		//�û����
		pPlayerData[i]->pLGoldCount=static_cast<UILabel*>(pPlayerData[i]->pPlayerPanel->getChildByName("goldCount"));
		//ׯ�ұ�ʶ
		pPlayerData[i]->pIBankIcon=static_cast<UIImageView*>(pPlayerData[i]->pPlayerPanel->getChildByName("ImageBankIcon"));
		pPlayerData[i]->pIBankIcon->setVisible(false);
		//�����߹��
		pPlayerData[i]->pILight=static_cast<UIImageView*>(pPlayerData[i]->pPlayerPanel->getChildByName("ImageLight"));
		pPlayerData[i]->pILight->setVisible(false);
		//�û�ͷ��
		pPlayerData[i]->pIPlayerIcon=static_cast<UIImageView*>(pPlayerData[i]->pPlayerPanel->getChildByName("headPortrait"));
		//�����û�
		pPlayerData[i]->hidePlayer();
	}
}
void PlayerLayer::resetPlayerData(){
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		pPlayerData[i]->pIBankIcon->setVisible(false);
		pPlayerData[i]->pILight->setVisible(false);
	}
}
void PlayerLayer::hidePlayer(UIPanel *panel){
	panel->setVisible(false);
}
void PlayerLayer::updateState(){
	switch (DataModel::sharedDataModel()->getMainScene()->getGameState())
	{
	case MainScene::STATE_READY:
		{
			resetPlayerData();
		}
		break;
	default:
		break;
	}
}
//��ʾ���㶯��
void PlayerLayer::showResultAnimation(int iPanelIndex,long long lGameScore){
	pPlayerData[iPanelIndex]->showResultAnimation(lGameScore);
}

//�����û���Ϣ
void PlayerLayer::setUserInfo(int panelIndex,tagUserInfo tagUser){
	pPlayerData[panelIndex]->setUserInfo(tagUser);
}
void PlayerLayer::setUserGold(int panelIndex,long long goldCount){
	//DataModel::sharedDataModel()->
}
void PlayerLayer::setBankLight(int userIndex){
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (i==userIndex)
			{
				pPlayerData[i]->pILight->setVisible(true);
			}else
			{
				pPlayerData[i]->pILight->setVisible(false);
			}
		}
}
void PlayerLayer::setBankIcon(int bankIndex){
	setBankLight(bankIndex);

	pPlayerData[bankIndex]->setBankIcon();
}