/*
 * define file about portable socket class.
 * description:this sock is suit both windows and linux
 * design:odison
 * e-mail:odison@126.com>
 *
 */

#ifndef _ODSOCKET_H_
#define _ODSOCKET_H_

#ifdef WIN32
#include <winsock2.h>
typedef int				socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
typedef int				SOCKET;

//#pragma region define win32 const variable in linux
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
//#pragma endregion
#endif

//���ȶ���
#define SOCKET_TCP_BUFFER			16384								//���绺��
#define SOCKET_TCP_PACKET			(SOCKET_TCP_BUFFER-sizeof(TCP_Head))//���绺��

class TCPSocket {
    
public:
    TCPSocket(SOCKET sock = INVALID_SOCKET);
    ~TCPSocket();
    
    // Create socket object for snd/recv data
    bool Create(int af, int type, int protocol = 0);
    
    // Connect socket
    bool Connect(const char* ip, unsigned short port);
    //#region server
    // Bind socket
    bool Bind(unsigned short port);
    
    // Listen socket
    bool Listen(int backlog = 5);
    
    // Accept socket
    bool Accept(TCPSocket& s, char* fromip = NULL);
    //#endregion
    int Select();
    // Send socket
    int Send(const char* buf, int len, int flags = 0);
    
    // Recv socket
    int Recv(char* buf, int len, int flags = 0);
    
    // Close socket
    int Close();
    
    // Get errno
    int GetError();
    
    //#pragma region just for win32
    // Init winsock DLL
    static int Init();
    // Clean winsock DLL
    static int Clean();
    //#pragma endregion
    
    // Domain parse
    static bool DnsParse(const char* domain, char* ip);
    
    TCPSocket& operator = (SOCKET s);
    
    operator SOCKET ();
    
protected:
    SOCKET m_sock;
    fd_set  fdR;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	//���ձ���
protected:
	WORD							m_wRecvSize;						//���ճ���
	BYTE							m_cbRecvBuf[SOCKET_TCP_BUFFER * 10];		//���ջ���
	//��������
protected:
	BYTE							m_cbSendRound;						//�ֽ�ӳ��
	BYTE							m_cbRecvRound;						//�ֽ�ӳ��
	DWORD							m_dwSendXorKey;						//������Կ
	DWORD							m_dwRecvXorKey;						//������Կ

	BYTE static						g_SendByteMap[];
	BYTE static						g_RecvByteMap[];
	//��������
protected:
	DWORD							m_dwSendTickCount;					//����ʱ��
	DWORD							m_dwRecvTickCount;					//����ʱ��
	DWORD							m_dwSendPacketCount;				//���ͼ���
	DWORD							m_dwRecvPacketCount;				//���ܼ���

	//���ܺ���
protected:
	//��������
	WORD CrevasseBuffer(BYTE cbDataBuffer[], WORD wDataSize);
	//��������
	WORD EncryptBuffer(BYTE cbDataBuffer[], WORD wDataSize, WORD wBufferSize);

	//��������
private:
	//�ֽ�ӳ��
	inline WORD SeedRandMap(WORD wSeed);
	//����ӳ��
	inline BYTE MapSendByte(BYTE cbData);
	//����ӳ��
	inline BYTE MapRecvByte(BYTE cbData);

	//�����ӿ�
public:
	//���ͺ���
	virtual DWORD __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	virtual DWORD __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//��������
protected:
	//�ر�����
	//VOID CloseSocket(BYTE cbShutReason);
	//��������
	//VOID AmortizeBuffer(VOID * pData, WORD wDataSize);
	//��������
	DWORD SendDataBuffer(VOID * pBuffer, WORD wSendSize);

	//��Ϣ����
public:
	//�����ȡ
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
};


#endif
