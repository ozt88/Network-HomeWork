#pragma once
#include "CustomBuffer.h"
#include "Util.h"
#define MAX_NAME_SIZE 20

class ClientSession
{
public:
	ClientSession();
	ClientSession(SOCKET sock);
	~ClientSession();

	void			PacketHandling(DWORD bytesTrans);
	bool			Connect(const SOCKADDR_IN& clntAddr);
	void			Disconnect();
	bool			SendToClient(char* message, DWORD bytesTrans);
	bool			RecvFromClient();

private:
	SOCKET			m_Socket;
	SOCKADDR_IN		m_Addr;
	char			m_NickName[MAX_NAME_SIZE];
	CustomBuffer*	m_Buffer;
	bool			m_IsConnected;
};

