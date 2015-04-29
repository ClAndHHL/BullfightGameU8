/* 
 * File:   DefaultListerner.h
 * 
 */
#pragma once
#include "SocketListerner.h"
#include "cocos2d.h"
USING_NS_CC;
class LogonGameListerner : public SocketListerner,public CCNode {
public:
    LogonGameListerner();
    virtual ~LogonGameListerner();
    void OnClose(TCPSocket* so, bool fromRemote);
    void OnError(TCPSocket* so, const char* e);
    void OnIdle(TCPSocket* so);
    bool OnMessage(TCPSocket* so,unsigned short	wSocketID, TCP_Command tCommand, void * pDataBuffer, unsigned short wDataSize);
    void OnOpen(TCPSocket* so);
};

