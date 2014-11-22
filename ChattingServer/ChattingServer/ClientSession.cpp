#include "stdafx.h"
#include "ClientSession.h"
#include "ClientManager.h"

ClientSession::ClientSession()
	:m_Socket(INVALID_SOCKET), m_IsConnected(false)
{
	m_Buffer = new CustomBuffer(BUF_SIZE);
	memset(&m_Addr, 0, sizeof(SOCKADDR_IN));
	strcpy(m_NickName, "anonymous");
}

ClientSession::ClientSession(SOCKET sock)
	: m_Socket(sock), m_IsConnected(false)
{
	m_Buffer = new CustomBuffer(BUF_SIZE);
	memset(&m_Addr, 0, sizeof(SOCKADDR_IN));
	strcpy(m_NickName, "anonymous");
}

ClientSession::~ClientSession()
{
}

bool ClientSession::Connect(const SOCKADDR_IN& clntAddr)
{
	memcpy(&m_Addr, &clntAddr, sizeof(SOCKADDR_IN));
	char message[BUF_SIZE] = {0, };

	sprintf_s(message, "Client Connected: IP=%s, PORT=%d\n", inet_ntoa(m_Addr.sin_addr), ntohs(m_Addr.sin_port));
	puts(message);
	m_IsConnected = true;

	return RecvFromClient();
}

void ClientSession::Disconnect()
{
	if(!m_IsConnected)
	{
		return;
	}
	char message[BUF_SIZE] = {0, };

	sprintf_s(message, "User [ %s ] Disconnected.\n", m_NickName);
	ClientManager::GetInstance()->SendToAllClient(message, strlen(message));
	puts(message);
	m_IsConnected = false;

	//즉각 해제
	LINGER lingerOption;
	lingerOption.l_onoff = 1;
	lingerOption.l_linger = 0;
	if(SOCKET_ERROR == setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*) &lingerOption, sizeof(LINGER)))
	{
		ErrorHandling("setsockopt():linger option error", GetLastError());
		return;
	}

	closesocket(m_Socket);
}

void ClientSession::PacketHandling(DWORD bytesTrans)
{
	m_Buffer->Commit(bytesTrans);
	/// 패킷 파싱하고 처리
	while(true)
	{
		/// 패킷 헤더 크기 만큼 읽어와 보기
		unsigned char messageSize;
		char* message;
		if(false == m_Buffer->Peek((char*) &messageSize, sizeof(char)))
			return;

		/// 패킷 완성이 되는가? 
		if(m_Buffer->GetStoredSize() < (size_t) messageSize)
			return;

		//chat
		message = new char[messageSize];
		m_Buffer->Read(message, (size_t)messageSize);
		ClientManager::GetInstance()->SendToAllClient(message, (DWORD)messageSize);
	}
}

bool ClientSession::SendToClient(char* message, DWORD bytesTrans)
{
	IOData* ioData = new IOData();
	WSABUF wsa_buf;
	ioData->m_Mode = MODE_SEND;
	wsa_buf.buf = message;
	wsa_buf.len = bytesTrans;
	printf("send to %d, message: %s(%d)\n", this->m_NickName, wsa_buf.buf, wsa_buf.len);
	if(WSASend(m_Socket, &wsa_buf, 1, NULL, 0, &ioData->m_Overlapped, NULL) == SOCKET_ERROR)
	{
		DWORD error = WSAGetLastError();
		if(error != WSA_IO_PENDING)
		{
			ErrorHandling("WSASend() error!", error);
			return false;
		}
	}
	
	return true;
}

bool ClientSession::RecvFromClient()
{
	DWORD flag = 0;
	IOData* ioData = new IOData();
	WSABUF wsa_buf;
	ioData->m_Mode = MODE_RECV;
	wsa_buf.buf = (char*)m_Buffer->GetBuffer();
	wsa_buf.len = (ULONG)m_Buffer->GetAvailableSize();

	if(WSARecv(m_Socket, &wsa_buf, 1, NULL, &flag, &ioData->m_Overlapped, NULL) == SOCKET_ERROR)
	{
		DWORD error = WSAGetLastError();
		if(error != WSA_IO_PENDING)
		{
			ErrorHandling("WSARecv() error!", error);
			return false;
		}
	}
	
	return true;
}


