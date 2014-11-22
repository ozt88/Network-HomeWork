#include "stdafx.h"
#include "Util.h"
#include "ClientSession.h"
#include "ClientManager.h"

#pragma comment(lib,"ws2_32.lib")

unsigned int WINAPI ThreadProc(LPVOID hComport);
CRITICAL_SECTION gCriticalSection;

int _tmain(int argc, _TCHAR* argv[])
{

	WSADATA wsaData;
	HANDLE hComPort;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	int port = 0;
	SYSTEM_INFO sysInfo;
	DWORD dwThreadId;
	HANDLE hThread;

	if(argc != 2)
	{
#ifndef _DEBUG
		printf("Usage: %s <port>", argv[0]);
		exit(1);
#else
		port = 8000;
#endif
	}
	else
	{
		port = atoi((const char*)argv[1]);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!", GetLastError());
		exit(1);
	}

	InitializeCriticalSection(&gCriticalSection);
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (LPVOID) hComPort, 0, (unsigned*)&dwThreadId);
	if(hThread == INVALID_HANDLE_VALUE)
	{
		ErrorHandling("_beginthreadex() error!", GetLastError());
	}


	hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(hServSock == INVALID_SOCKET)
	{
		ErrorHandling("WSASocket() error!", GetLastError());
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);

	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind() error!", GetLastError());
		exit(1);
	}

	if(listen(hServSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen() error!", GetLastError());
		exit(1);
	}

	while(1)
	{
		SOCKET clntSocket;
		SOCKADDR_IN clntAddr;
		int clntAddrLen = sizeof(clntAddr);
		clntSocket = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrLen);
		if(clntSocket == INVALID_SOCKET)
		{
			ErrorHandling("accept() error!", GetLastError());
			continue;
		}
		ClientSession* newClnt = new ClientSession(clntSocket);
		CreateIoCompletionPort((HANDLE) clntSocket, hComPort, (ULONG_PTR) newClnt, 0);
		newClnt->Connect(clntAddr);
		ClientManager::GetInstance()->AddClient(newClnt);
	}


	return 0;
}


unsigned int WINAPI ThreadProc(LPVOID _hComport)
{
	HANDLE hComport = (HANDLE)_hComport;
	ClientSession* clientSession;
	DWORD bytesTrans = 0;
	IOData* ioData = nullptr;

	while(1)
	{
		if(!GetQueuedCompletionStatus(hComport, &bytesTrans, (PULONG_PTR) &clientSession, (LPOVERLAPPED*) &ioData, INFINITE))
		{
			ErrorHandling("GetQueuedCompletionStatus() error!", GetLastError());
		}
		
		if(ioData->m_Mode == MODE_RECV)
		{
			if(bytesTrans == 0)
			{
				ClientManager::GetInstance()->RemoveClient(clientSession);
			}
			else
			{
				clientSession->PacketHandling(bytesTrans);
				clientSession->RecvFromClient();
			}
		}
		SafeDelete<IOData*>(ioData);
	}

	return 0;
}
