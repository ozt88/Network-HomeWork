// HTTPServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#pragma comment(lib,"ws2_32.lib")
#include "Util.h"


unsigned WINAPI RequestHandler(void* argv);
char* ContentType(char* file);
void SendData(SOCKET sock, char* ct, char* fileName);
void SendErrorMSG(SOCKET sock, ErrorCode errorCode);
void ErrorHandling(char* message);

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;
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
		port = atoi((const char*) argv[1]);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!", GetLastError());
		exit(1);
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
		int clntAddrLen = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &clntAddrLen);
		if(hClntSock == INVALID_SOCKET)
		{
			ErrorHandling("accept() error!", GetLastError());
			continue;
		}
		printf_s("Client Connected: IP=%s, PORT=%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));

		hThread = (HANDLE) _beginthreadex(NULL, 0, RequestHandler, (LPVOID)hClntSock, 0, (unsigned*)&dwThreadId);
		if(hThread == INVALID_HANDLE_VALUE)
		{
			ErrorHandling("_beginthreadex() error!", GetLastError());
		}
	}

	closesocket(hServSock);
	WSACleanup();
	return 0;
}

unsigned int WINAPI RequestHandler(void* argv)
{
	SOCKET hClntSock = (SOCKET) argv;

	char buf[BUF_SIZE] = {0, };
	char method[BUF_SMALL] = {0, };
	char ct[BUF_SMALL] = {0, };
	char fileName[BUF_SMALL] = {0, };
	DWORD ret = 0;
	if(ret = recv(hClntSock, buf, BUF_SIZE, 0) == SOCKET_ERROR)
	{
		ErrorHandling("recv() error!", GetLastError());
		return 1;
	}
	puts("recv() success");
	if(strstr(buf, "HTTP/") == NULL)
	{
		SendErrorMSG(hClntSock, ERROR_400_BAD_REQUEST);
		closesocket(hClntSock);
		return 1;
	}

	strcpy(method, strtok(buf, " "));
	if(strcmp(method, "GET"))
	{
		SendErrorMSG(hClntSock, ERROR_400_BAD_REQUEST);
	}

	strcpy(fileName, strtok(NULL, " "));
	memmove(fileName + 1, fileName, strlen(fileName));
	fileName[0] = '.';
	for(int i = 0; i < strlen(fileName) + 1; ++i)
	{
		if(fileName[i] == '/')
		{
			fileName[i] = '\\';
		}
	}

	char* type = ContentType(fileName);
	if(type == NULL)
	{
		SendErrorMSG(hClntSock, ERROR_404_NOT_FOUND);
		closesocket(hClntSock);
		return 0;
	}

	strcpy(ct, ContentType(fileName));
	SendData(hClntSock, ct, fileName);
	return 0;
}

void SendData(SOCKET socket, char* ct, char* fileName)
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char servName[] = "Server:simple web server\r\n";
	char cntLen[BUF_SMALL];
	char cntType[BUF_SMALL] = {0, };
	char buf[BUF_SIZE] = {0, };
	FILE* sendFile;

	sprintf_s(cntType, "Content-type:%s\r\n\r\n", ct);
	if(( sendFile = fopen(fileName, "rb") ) == NULL)
	{
		SendErrorMSG(socket, ERROR_404_NOT_FOUND);
		return;
	}

	int fileSize;
	fseek(sendFile, 0, SEEK_END);
	fileSize = ftell(sendFile);
	fseek(sendFile, 0, SEEK_SET);

	sprintf_s(cntLen, "Content-length : %d\r\n", fileSize);
	if(send(socket, protocol, strlen(protocol), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send(): protocol error!", GetLastError());
		return; 
	}
	if(send(socket, servName, strlen(servName), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():servName error!", GetLastError());
		return;
	}
	if(send(socket, cntType, strlen(cntType), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():cntType error!", GetLastError());
		return;
	}

	while(!feof(sendFile))
	{
		int read = fread(buf, 1, BUF_SIZE, sendFile);
		send(socket, buf, read, 0);
	}

	closesocket(socket);
	fclose(sendFile);
}

void SendErrorMSG(SOCKET socket, ErrorCode errorCode )
{
	char protocol[BUF_SMALL] = {0, };
	char cntLen[BUF_SMALL] = {0, };
	char content[BUF_SIZE] = {0, };
	char errorMessage[BUF_SMALL] = {0, };
	switch(errorCode)
	{
		case ERROR_400_BAD_REQUEST:
			strcpy(errorMessage, "400 Bad Request\r\n");
			break;
		case ERROR_404_NOT_FOUND:
			strcpy(errorMessage, "404 Not Found\r\n");
			break;
		default:
			strcpy(errorMessage, "400 Bad Request\r\n");
			break;
	}
	char servName[] = "Server : simple web server\r\n";
	char cntType[] = "Content-type:text/html\r\n\r\n";
	sprintf_s(protocol, "HTTP/1.0 %s", errorMessage);
	sprintf_s(content, "<html><head><title>NETWORK</title></head>"
			  "<body><font size=+2><br>%s"
			  "</font></body></html>", errorMessage);
	sprintf_s(cntLen, "Content-length : %d\r\n", strlen(content));


	if(send(socket, protocol, strlen(protocol), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():protocol error!", GetLastError());
		return;
	}
	if(send(socket, servName, strlen(servName), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():cntType error!", GetLastError());
		return;
	}
	if(send(socket, cntLen, strlen(cntLen), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():cntLen error!", GetLastError());
		return;
	}
	if(send(socket, cntType, strlen(cntType), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():cntType error!", GetLastError());
		return;
	}
	if(send(socket, content, strlen(content), 0) == SOCKET_ERROR)
	{
		ErrorHandling("send():content error!", GetLastError());
		return;
	}

	closesocket(socket);
}

char* ContentType(char* file)
{
	char extension[BUF_SIZE] = {0, };
	char fileName[BUF_SMALL] = {0, };
	char* ret = nullptr;
	strcpy(fileName, file);
	strtok(fileName, ".");
	ret = strtok(NULL, ".");
	if(ret == nullptr)
	{
		return ret;
	}

	strcpy(extension, ret);
	ret = (strcmp(extension, "html") == 0 || strcmp(extension, "htm") == 0) ? "text/html" : "text/plain";
	return ret;
}


