/* 
 * File:   DefaultListerner.h
 * 
 */
#pragma once
#include "SocketListerner.h"
#include "cocos2d.h"
USING_NS_CC;
class LobbyGameListerner : public SocketListerner,public CCNode {
public:
    
    LobbyGameListerner();
    virtual ~LobbyGameListerner();
    void OnClose(TCPSocket* so, bool fromRemote);
    void OnError(TCPSocket* so, const char* e);
    void OnIdle(TCPSocket* so);
    bool OnMessage(TCPSocket* so,unsigned short	wSocketID, TCP_Command tCommand, void * pDataBuffer, unsigned short wDataSize);
    void OnOpen(TCPSocket* so);
};

