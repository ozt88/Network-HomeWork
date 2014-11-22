// ChattingClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Util.h"
#include "CustomBuffer.h"


unsigned int WINAPI SendThreadProc(LPVOID argv);
unsigned int WINAPI RecvThreadProc(LPVOID argv);
int					SendMessage(SOCKET socket, char* message, int length);
int					RecvMessage(SOCKET socket, char* buffer, int length);
unsigned char		GetPacketSize(SOCKET socket);
void				ErrorHandling(char* message, DWORD error);
void				GetInput(char* inputBuffer, unsigned char* inputPointer);
void				Log(char* message);
void				PrintLogs();
void				PrintInput(char* inputBuffer, int length);
void				ErrorHandling(char* message, DWORD error);

std::deque<char*> logs;
CRITICAL_SECTION gCriticalSection;

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	HANDLE hSendThread, hRecvThread;
	DWORD dwSendThreadId, dwRecvThreadId;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	int port = 0;
	const char* ip;
	InitializeCriticalSection(&gCriticalSection);

	if(argc != 2)
	{
#ifndef _DEBUG
		Log("Usage: %s <IP> <PORT>\n", argv[0]);
		exit(1);
#else
		port = 8000;
		ip = "127.0.0.1";
#endif
	}
	else
	{
		ip = (const char*)argv[1];
		port = atoi((const char*) argv[2]);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!", GetLastError());
		exit(1);
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if(hServSock == INVALID_SOCKET)
	{
		ErrorHandling("socket() error!", GetLastError());
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(port);

	if(connect(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!", GetLastError());
		exit(1);
	}
	else
	{
		Log("Connected........");
	}

	hidecursor();
	hSendThread = (HANDLE) _beginthreadex(NULL, 0, SendThreadProc, (LPVOID)hServSock, 0, (unsigned*) &dwSendThreadId);
	if(hSendThread == INVALID_HANDLE_VALUE)
	{
		ErrorHandling("_beginthreadex() : SendThread error!", GetLastError());
		exit(1);
	}

	hRecvThread = (HANDLE) _beginthreadex(NULL, 0, RecvThreadProc, (LPVOID) hServSock, 0, (unsigned*) &dwRecvThreadId);
	if(hRecvThread == INVALID_HANDLE_VALUE)
	{
		ErrorHandling("_beginthreadex() : RecvThread error!", GetLastError());
		exit(1);
	}

	WaitForSingleObject(hSendThread, INFINITE);
	WaitForSingleObject(hRecvThread, INFINITE);
	_endthreadex(dwSendThreadId);
	_endthreadex(dwRecvThreadId);
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

int SendMessage(SOCKET socket, char* message, int length)
{
	int ret = 0;
	int sentBytes = 0;
	while(sentBytes < length)
	{
		ret = send(socket, message + sentBytes, length - sentBytes, NULL);
		if(ret == SOCKET_ERROR)
		{
			ErrorHandling("send() error!", GetLastError());
			return ret;
		}
		sentBytes += ret;
	}
	return sentBytes;
}

int RecvMessage(SOCKET socket, char* buffer, int length)
{
	int ret = 0;
	int recvBytes = 0;
	while(recvBytes < length)
	{
		ret = recv(socket, buffer + recvBytes, length - recvBytes, NULL);
		if(ret == SOCKET_ERROR)
		{
			ErrorHandling("recv() error!", GetLastError()); 
			return ret;
		}
		else if(ret == 0)
		{
			break;
		}
		recvBytes += ret;
	}
	return recvBytes;
}

unsigned int WINAPI SendThreadProc(LPVOID argv)
{
	SOCKET hServSock = (SOCKET) argv;
	char inputBuffer[BUF_SIZE] = {0, };
	char message[BUF_SIZE] = {0, };
	unsigned char messageSize = 0;
	unsigned char inputSize = 0;

	while(true)
	{
		memset(message, 0, sizeof(char) * BUF_SIZE);
		memset(inputBuffer, 0, sizeof(char)*BUF_SIZE);
		inputSize = 0;
		GetInput(inputBuffer, &inputSize);
		messageSize = inputSize + 2;
		MakeMessageHeader(inputBuffer, messageSize, PACKET_CHAT, message);
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			break;
		}
		if(SendMessage(hServSock, message, messageSize) == SOCKET_ERROR)
		{
			ErrorHandling("sendMessage() error!", GetLastError());
			break;
		}
	}

	return 0;
}

unsigned int WINAPI RecvThreadProc(LPVOID argv)
{
	SOCKET hServSock = (SOCKET) argv;
	CustomBuffer packetBuffer(BUF_SIZE);
	char* message = new char[BUF_SIZE];
	unsigned short packetSize = 0;
	unsigned short messageLength = 0;
	PacketType packetType = PACKET_NONE;

	while(true)
	{
		memset(message, 0, sizeof(char)*BUF_SIZE);
		packetSize = GetPacketSize(hServSock);
		if(RecvMessage(hServSock, packetBuffer.GetBuffer(), packetSize) == SOCKET_ERROR)
		{
			ErrorHandling("RecvMessage(): error!", GetLastError());
			getchar();
			exit(1);
		}
		packetBuffer.Commit(packetSize);
		packetBuffer.Read((char*)&packetType, 1);
		packetBuffer.Read(message, packetSize - 1);
		switch(packetType)
		{
			case PACKET_ENTER:
				Log("Enter CSTalk");
				break;
			case PACKET_CHAT:
				Log(message);
				break;
			case PACKET_EXIT:
				break;
			default:
				break;
		}
	}
	return 0;
}

void GetInput(char* inputBuffer, unsigned char* inputPointer)
{
	char inputChar = NULL;
	int currentPointer = *inputPointer;
	while(true)
	{
		PrintInput(inputBuffer, currentPointer);
		inputChar = _getch();
		if(currentPointer > MAX_MSG_LENGTH)
		{
			currentPointer--;
			continue;
		}
		else if(0 < currentPointer && inputChar == 8) // backspace
		{
			inputBuffer[--currentPointer] = '\0';
		}
		else if(inputChar == '\r')
		{
			clearLine(MAX_PRINT_LINE);
			inputBuffer[currentPointer] = '\0';
			*inputPointer = currentPointer;
			break;
		}
		else 
		{
			if(currentPointer != 0 && currentPointer%MAX_LENGTH_BY_LINE == 0)
			{
				inputBuffer[currentPointer++] = '\n';
			}
			inputBuffer[currentPointer++] = inputChar;
		}
	}
}

void PrintInput(char* inputBuffer, int length)
{
	Lock lock;
	gotoxy(1, MAX_PRINT_LINE);
	printf_s("ME> %s%50c", inputBuffer, ' ', length);
}

void PrintLogs()
{
	Lock lock;
	for(int i = 0; i < logs.size(); ++i)
	{
		clearLine(i + 1);
		gotoxy(0, i + 1);
		printf_s("%s", logs[i], strlen(logs[i]));
	}
}

void Log(char* message)
{
	int length = strlen(message);
	char* log = new char[length + 1];
	memset(log, 0, sizeof(char)*(length + 1));
	memcpy(log, message, length);
	Lock lock;
	logs.push_back(log);
	if(logs.size() > MAX_PRINT_LINE)
	{
		char* delNode = logs.front();
		logs.pop_front();
		SafeDelete<char*>(delNode);
	}
	PrintLogs();
}

unsigned char GetPacketSize(SOCKET socket)
{
	unsigned char packetSize = 0;
	if(RecvMessage(socket, (char*) &packetSize, 1) == SOCKET_ERROR)
	{
		ErrorHandling("GetPacketSize(): error!", GetLastError());
		getchar();
		exit(1);
	}
	return packetSize - 1;
}

void ErrorHandling(char* message, DWORD error)
{
	char errorMessage[BUF_SIZE] = {0, };
	sprintf_s(errorMessage, "%s error No: %d", message, error);
	Log(errorMessage);
}