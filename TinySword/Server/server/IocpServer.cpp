#pragma warning (disable:4127)

#ifdef _IA64_
#pragma warning(disable:4267)
#endif // _IA64_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif // !WIN32_LEAN_AND_MEAN

#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(p)   HeapFree(GetProcessHeap(),0,(p))



#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>

#include "IocpServer.h"
//#include "DatabaseManager.h"

#include "Protocol.h"

#include <map>
#include <iostream>

#include "Manager.h"
#include "BombManager.h"



const char* g_Port = DEFAULT_PORT;
BOOL g_bEndServer = FALSE; 
BOOL g_bRestart = TRUE; 
BOOL g_bVerbose = FALSE; 
DWORD g_dwThreadCount = 0; 
HANDLE g_hIOCP = INVALID_HANDLE_VALUE; 
SOCKET g_sdListen = INVALID_SOCKET; 
HANDLE g_ThreadHandles[MAX_WORKER_THREAD]; 
PPER_SOCKET_CONTEXT g_pCtxtList = NULL; 

CRITICAL_SECTION g_CriticalSection; 

int myprintf(const char* lpFormat, ...); 

short PlayerCnt = 0; 
extern void GameReadyExecute();




PlayerObject* PlayerObjectFactory(SOCKET sdAccept)
{
	return new PlayerObject(sdAccept, 100);						// Vector 기본값 전달 가능
}

// DB
//bool success = DatabaseManager::GetInstance().Connect("127.0.0.1", "sa", "sotptkd", "tinysworddb");




void __cdecl main(int argc, char* argv[]) {

	SYSTEM_INFO systemInfo; 
	WSADATA wsaData; 
	SOCKET sdAccept = INVALID_SOCKET; 
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL; 
	DWORD dwRecvNumBytes = 0; 
	DWORD dwFlags = 0; 
	int nRet = 0; 

	int deadPlayerCnt = 0;
	int deadCastleCnt = 0;

	//myprintf("DB Connect Success?: %d", success);

	////////////
	std::cout << ">>>>>>>>> Starting GameReadyExecute...<<<<<<<<<<<<<<<" << std::endl;
	GameReadyExecute();

	/*std::cout << "\t>> CASTLE CNT : " << castleManager->GetCastleEntry().size() <<
		"\n\t>> SHEEP CNT : " << sheepManager->GetSheepEntry().size() <<
		"\n\t>> GOLDMINE CNT : " << goldmineManager->GetGoldmineEntry().size() << std::endl;*/
	//////////////////

	myprintf("Castle CNT: %d\n", castleManager->GetCastleEntry().size());
	myprintf("GodlMine CNT: %d\n", goldmineManager->GetGoldmineEntry().size());
	myprintf("Sheep CNT: %d\n", sheepManager->GetSheepEntry().size());

	////////////////////////////////////////////////////////////////////



	for (int i = 0; i < MAX_WORKER_THREAD; i++) {
		g_ThreadHandles[i] = INVALID_HANDLE_VALUE; 
	}

	if (!ValidOptions(argc, argv))
		return;

	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		myprintf("SetConsoleCtrlHandler() failed to install console handler: %d\n", GetLastError());
		return; 
	}

	GetSystemInfo(&systemInfo); 
	g_dwThreadCount = systemInfo.dwNumberOfProcessors * 2; 

	if ((nRet = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		myprintf("WSAStartup() failed: %d\n", nRet);
		SetConsoleCtrlHandler(CtrlHandler, FALSE); 
		return; 
	}

	__try
	{
		InitializeCriticalSection(&g_CriticalSection); 
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("InitializeCriticalSection raised exception.\n"); 
		return; 
	}

	

	while (g_bRestart)
	{
		g_bRestart = FALSE; 
		g_bEndServer = FALSE; 

		__try {
			g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
			if (g_hIOCP == NULL) {
				myprintf("CreateIoCompletionPort() failed to create I/O completion port: %d\n", GetLastError());
				__leave;
			}

			for (DWORD dwCPU = 0; dwCPU < g_dwThreadCount; dwCPU++) {
				HANDLE hThread = INVALID_HANDLE_VALUE; 
				DWORD dwThreadId = 0; 

				hThread = CreateThread(NULL, 0, WorkerThread, g_hIOCP, 0, &dwThreadId); 
				if (hThread == NULL) {
					myprintf("CreateThread() failed to create worker thread: %d\n", GetLastError());
					__leave; 
				}
				g_ThreadHandles[dwCPU] = hThread; 
				hThread = INVALID_HANDLE_VALUE; 
			}

			if (!CreateListenSocket())
				__leave; 

			while (TRUE) {
				sdAccept = WSAAccept(g_sdListen, NULL, NULL, NULL, 0);
				if (sdAccept == SOCKET_ERROR) {
					myprintf("WSAAcept() failed: %d\n", WSAGetLastError()); 
					__leave; 
				}
				
				/////////////////
				BOOL bNoDelay = TRUE;
				int result = setsockopt(sdAccept, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay));
				if (result == SOCKET_ERROR) {
					myprintf("setsockopt(TCP_NODELAY) failed: %d\n", WSAGetLastError());
				}
				////////////////////

				lpPerSocketContext = UpdateCompletionPort(sdAccept, ClientIoRead, TRUE); 
				if (lpPerSocketContext == NULL)
					__leave; 

				if (g_bEndServer)
					break;

				nRet = WSARecv(sdAccept, &(lpPerSocketContext->pIOContext->wsabuf),
					1, &dwRecvNumBytes, &dwFlags,
					&(lpPerSocketContext->pIOContext->Overlapped), NULL);
				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
					myprintf("WSARecv() Failed: %d\n", WSAGetLastError()); 
					CloseClient(lpPerSocketContext, FALSE);
				}

				// Accept 후 플레이어 생성 
				PlayerObject* tmpPlayer = PlayerObjectFactory(sdAccept);
				IBindObject* bindObj = static_cast<IBindObject*>(tmpPlayer); //reinterpret_cast
				CtxtBindTo(sdAccept, bindObj);
				


			} // while
		}

		__finally {
			g_bEndServer = TRUE; 

			if (g_hIOCP) {
				for (DWORD i = 0; i < g_dwThreadCount; i++)
					PostQueuedCompletionStatus(g_hIOCP, 0, 0, NULL);
			}

			// MAKE SURE WORKER THREAD EXITS.
			if (WAIT_OBJECT_0 != WaitForMultipleObjects(g_dwThreadCount, g_ThreadHandles, TRUE, 1000))
				myprintf("WaitForMultipleObjects() failed: %d\n", GetLastError()); 
			else
				for (DWORD i = 0; i < g_dwThreadCount; i++) {
					if (g_ThreadHandles[i] != INVALID_HANDLE_VALUE) CloseHandle(g_ThreadHandles[i]);
					g_ThreadHandles[i] = INVALID_HANDLE_VALUE;
				}

			CtxtListFree();

			if (g_hIOCP) {
				CloseHandle(g_hIOCP); 
				g_hIOCP = NULL; 
			}

			if (g_sdListen != INVALID_SOCKET) {
				closesocket(g_sdListen);
				g_sdListen = INVALID_SOCKET;
			}

			if (sdAccept != INVALID_SOCKET) {
				closesocket(sdAccept); 
				sdAccept = INVALID_SOCKET; 
			}
		} // FINALLY

		if (g_bRestart) {
			myprintf("\niocpserver is restarting...]n");
		}
		else
			myprintf("\niocpserver is exting...\n");
	} // while (g_bRestart)

	DeleteCriticalSection(&g_CriticalSection); 
	WSACleanup();
	SetConsoleCtrlHandler(CtrlHandler, FALSE);
} // main


BOOL ValidOptions(int argc, char* argv[]) {

	BOOL bRet = TRUE; 

	for (int i = 1; i < argc; i++) {
		if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
			switch (tolower(argv[i][1]) )
			{
			case 'e':
				if (strlen(argv[i]) > 3)
					g_Port = &argv[i][3];
				break;

			case 'v':
				g_bVerbose = TRUE; 
				break;

			case '?':
				myprintf("Usage: \n	iocpserver [-p:port] [-v] [-?]\n");
				myprintf("	-e:port\tSpecify echoing port number\n"); 
				myprintf("	-v:\t\tVerbose\n");
				myprintf("	-?:\t\tDisplay this help\n");
				bRet = FALSE; 
				break;

			default:
				myprintf("Unknown options flag %s\n", argv[i]);
				bRet = FALSE; 
				break;
			}
		}
	}
	return (bRet);
}


BOOL WINAPI CtrlHandler(DWORD dwEvent) {

	SOCKET sockTemp = INVALID_SOCKET; 

	switch (dwEvent) 
	{
	case CTRL_BREAK_EVENT:
		g_bRestart = TRUE; 
	case CTRL_C_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_CLOSE_EVENT:
		if (g_bVerbose)
			myprintf("CtrlHandler: closing listening socket\n");

		sockTemp = g_sdListen; 
		g_sdListen = INVALID_SOCKET;
		g_bEndServer = TRUE; 
		closesocket(sockTemp); 
		sockTemp = INVALID_SOCKET;
		break;

	default:
		return(FALSE);
	}
	return(TRUE);
}

BOOL CreateListenSocket(void) {

	int nRet = 0; 
	int nZero = 0; 
	struct addrinfo hints = { 0 };
	struct addrinfo* addrlocal = NULL; 


	hints.ai_flags = AI_PASSIVE; 
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_IP; 

	if (getaddrinfo(NULL, g_Port, &hints, &addrlocal) != 0) {
		myprintf("getaddrinfo() failed with error %d\n", WSAGetLastError()); 
		return (FALSE);
	}

	if (addrlocal == NULL) {
		myprintf("getaddrinfo() failed to resolve/convert the interface\n"); 
		return (FALSE);
	}

	g_sdListen = WSASocketW(addrlocal->ai_family, addrlocal->ai_socktype, addrlocal->ai_protocol,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (g_sdListen == INVALID_SOCKET) {
		myprintf("WSASocket(g_sdListen) failed: %d\n", WSAGetLastError()); 
		return(FALSE);
	}

	nRet = bind(g_sdListen, addrlocal->ai_addr, (int)addrlocal->ai_addrlen); 
	if (nRet == SOCKET_ERROR) {
		myprintf("bind() failed: %d\n", WSAGetLastError()); 
		return (FALSE); 
	}

	nRet = listen(g_sdListen, 5); 
	if (nRet == SOCKET_ERROR) {
		myprintf("listen() failed: %d\n", WSAGetLastError()); 
		return(FALSE); 
	}


	nZero = 0; 
	nRet = setsockopt(g_sdListen, SOL_SOCKET, SO_SNDBUF, (char*)&nZero, sizeof(nZero)); 
	if (nRet == SOCKET_ERROR) {
		myprintf("setsockopt(SNDBUF) failed: %d\n", WSAGetLastError()); 
		return (FALSE);
	}

	freeaddrinfo(addrlocal); 
	return (TRUE); 

}

extern struct PROTOCOL_MAPPING PROTOCOL__MAPPING[];

DWORD WINAPI WorkerThread(LPVOID WorkerThreadContext) {

	HANDLE hIOCP = (HANDLE)WorkerThreadContext; 
	BOOL bSuccess = FALSE; 
	int nRet = 0; 
	LPWSAOVERLAPPED lpOverlapped = NULL; 
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL; 
	PPER_IO_CONTEXT lpIOContext = NULL; 
	WSABUF buffRecv; 
	WSABUF buffSend;
	DWORD dwRecvNumBytes = 0; 
	DWORD dwSendNumBytes = 0; 
	DWORD dwFlags = 0; 
	DWORD dwIoSize = 0;

	while (TRUE) {

		bSuccess = GetQueuedCompletionStatus(hIOCP, &dwIoSize,
			(PDWORD_PTR)&lpPerSocketContext,
			(LPOVERLAPPED*)&lpOverlapped,
			INFINITE);

		if (!bSuccess)
			myprintf("GetQueuedCompletionStatus() failed: %d\n", GetLastError()); 

		if (lpPerSocketContext == NULL) {

			return (0);
		}

		if (g_bEndServer)
		{
			return (0);
		}

		//////////////////////////////////////////////////
		if (!bSuccess || (bSuccess && (dwIoSize == 0))) {
			std::cerr << "!bSuccess || bSuccess&&(dwIoSize == 0)" << std::endl;
			CloseClient(lpPerSocketContext, FALSE); ///////////////////////////////
			continue;
		}

		lpIOContext = (PPER_IO_CONTEXT)lpOverlapped; 
		switch (lpIOContext->IOOperation)
		{
		case ClientIoRead:
			struct HEAD* head = (struct HEAD*)(lpIOContext->wsabuf.buf); 
			struct PROTOCOL_MAPPING* mapping = PROTOCOL__MAPPING +1;
			//std::cout << "Packet Number: " << mapping->ID <<"\t Head Number: "<<head->Index << std::endl;
			
			dwIoSize = 0; 
			while (mapping->EXECUTE != nullptr)
			{
				if ((short)mapping->ID == head->Index)
				{
					//std::cout << "Correct mapping with HEAD" << std::endl;
					dwIoSize = mapping->EXECUTE(lpPerSocketContext->Socket, &lpIOContext->wsabuf);
					break;
				}
				mapping++;
			}







			//////////////////////////////////////////////////////////////

			lpIOContext->IOOperation = ClientIoRead; 
			/*lpIOContext->nTotalBytes = dwIoSize; 
			lpIOContext->nSentBytes = 0; 
			lpIOContext->wsabuf.len = dwIoSize; */


			
			lpIOContext->nTotalBytes = 0;
			lpIOContext->nSentBytes = 0;
			lpIOContext->wsabuf.buf = lpIOContext->Buffer;
			lpIOContext->wsabuf.len = sizeof(lpIOContext->Buffer);

			dwFlags = 0; 


			nRet = WSARecv(lpPerSocketContext->Socket, &lpIOContext->wsabuf, 1,
				&dwRecvNumBytes, &dwFlags, &lpIOContext->Overlapped, NULL);

			if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
				myprintf("WSARecv() failed: %d\n", WSAGetLastError()); 
				CloseClient(lpPerSocketContext, FALSE); 
			}
			else if (g_bVerbose) {
				myprintf("WorkerThread %d : socket(%d) Recv Completed (%d Bytes), Send posted\n",
					GetCurrentThreadId(), lpPerSocketContext->Socket, dwIoSize);
			}


			// 데이터 처리 
			/*if (dwIoSize > 0) {
				std::cout << "Receive DATA" << std::endl;
			}*/

			break;

		}
	}
	return (0);
}

PPER_SOCKET_CONTEXT UpdateCompletionPort(SOCKET sd, IO_OPERATION ClientIo, BOOL bAddToList) {

	PPER_SOCKET_CONTEXT lpPerSocketContext; 

	lpPerSocketContext = CtxtAllocate(sd, ClientIo); 
	if (lpPerSocketContext == NULL)
		return (NULL); 

	g_hIOCP = CreateIoCompletionPort((HANDLE)sd, g_hIOCP, (DWORD_PTR)lpPerSocketContext, 0);
	if (g_hIOCP == NULL) {
		myprintf("CreateIoCompletionPort() failed: %d\n", GetLastError()); 
		if (lpPerSocketContext->pIOContext)
			xfree(lpPerSocketContext->pIOContext); 
		xfree(lpPerSocketContext);
		return (NULL); 
	}

	if (bAddToList) CtxtListAddTo(lpPerSocketContext); 

	if (g_bVerbose)
		myprintf("UpdateCompletionPort: Socket(%d) added to IOCP\n", lpPerSocketContext->Socket); 

	return (lpPerSocketContext);
}

VOID CloseClient(PPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful) {

	__try
	{
		EnterCriticalSection(&g_CriticalSection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an excpetion\n");
		return;
	}

	if (lpPerSocketContext) {
		if (g_bVerbose)
			myprintf("CloseClient: Socket(%d) connection closing (graceful=%s)\n",
				lpPerSocketContext->Socket, (bGraceful ? "TRUE" : "FALSE"));

		if (!bGraceful) {

			LINGER lingerStruct; 

			lingerStruct.l_onoff = 1; 
			lingerStruct.l_linger = 0; 
			setsockopt(lpPerSocketContext->Socket, SOL_SOCKET, SO_LINGER,
				(char*)&lingerStruct, sizeof(lingerStruct));
		}

		closesocket(lpPerSocketContext->Socket);
		lpPerSocketContext->Socket = INVALID_SOCKET; 
		CtxtListDeleteFrom(lpPerSocketContext);
		lpPerSocketContext = NULL; 
	}
	else {
		myprintf("CloseClient: lpPerSocketContext is NULL\n");
	}

	LeaveCriticalSection(&g_CriticalSection);
	return;
}

PPER_SOCKET_CONTEXT CtxtAllocate(SOCKET sd, IO_OPERATION ClientIO) {
	
	PPER_SOCKET_CONTEXT lpPerSocketContext; 

	__try
	{
		EnterCriticalSection(&g_CriticalSection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n"); 
		return NULL; 
	}

	lpPerSocketContext = (PPER_SOCKET_CONTEXT)xmalloc(sizeof(PER_SOCKET_CONTEXT)); 
	if (lpPerSocketContext) {
		lpPerSocketContext->pIOContext = (PPER_IO_CONTEXT)xmalloc(sizeof(PER_IO_CONTEXT));
		if (lpPerSocketContext->pIOContext) {
			lpPerSocketContext->Socket = sd; 
			lpPerSocketContext->pCtxtBack = NULL; 
			lpPerSocketContext->pCtxtForward = NULL; 

			lpPerSocketContext->pIOContext->Overlapped.Internal = 0; 
			lpPerSocketContext->pIOContext->Overlapped.InternalHigh = 0; 
			lpPerSocketContext->pIOContext->Overlapped.Offset = 0; 
			lpPerSocketContext->pIOContext->Overlapped.OffsetHigh = 0; 
			lpPerSocketContext->pIOContext->Overlapped.hEvent = NULL; 
			lpPerSocketContext->pIOContext->IOOperation = ClientIO; 
			lpPerSocketContext->pIOContext->pIOContextForward = NULL; 
			lpPerSocketContext->pIOContext->nTotalBytes = 0; 
			lpPerSocketContext->pIOContext->nSentBytes = 0; 
			lpPerSocketContext->pIOContext->wsabuf.buf = lpPerSocketContext->pIOContext->Buffer; 
			lpPerSocketContext->pIOContext->wsabuf.len = sizeof(lpPerSocketContext->pIOContext->Buffer);

			ZeroMemory(lpPerSocketContext->pIOContext->wsabuf.buf, lpPerSocketContext->pIOContext->wsabuf.len);
		}
		else {
			xfree(lpPerSocketContext); 
			myprintf("HeapAlloc() PER_IO_CONTEXT failed: %d\n", GetLastError());
		}
	}
	else{
		myprintf("HeapAlloc() PER_SOCKET_CONTEXT failed: %d\n", GetLastError());
	}

	LeaveCriticalSection(&g_CriticalSection);

	return (lpPerSocketContext);
}

VOID CtxtListAddTo(PPER_SOCKET_CONTEXT lpPerSocketContext) {

	PPER_SOCKET_CONTEXT pTemp; 

	__try
	{
		EnterCriticalSection(&g_CriticalSection); 
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n"); 
		return;
	}

	if (g_pCtxtList == NULL) {

		lpPerSocketContext->pCtxtBack = NULL; 
		lpPerSocketContext->pCtxtForward = NULL; 
		g_pCtxtList = lpPerSocketContext; 
	}
	else // 새로운 Socket Context를 연결리스트 가장 앞에 추가 
	{
		pTemp = g_pCtxtList; // 기존 연결 리스트의 헤드 노드 

		g_pCtxtList = lpPerSocketContext;  // 새로운 Socket Context를 헤드로 지정
		lpPerSocketContext->pCtxtBack = pTemp; // 기존 연결리스트를 Back에 연결 
		lpPerSocketContext->pCtxtForward = NULL; 

		// 기존 헤드 노드(pTemp)의 forward를 새로운 Socket Context로 설정 -> 새 노드가 기존 리스트와 양방향 연결
		pTemp->pCtxtForward = lpPerSocketContext; 
	}

	LeaveCriticalSection(&g_CriticalSection); 
	return;
}

VOID CtxtListDeleteFrom(PPER_SOCKET_CONTEXT lpPerSocketContext) {

	PPER_SOCKET_CONTEXT pBack; 
	PPER_SOCKET_CONTEXT pForward; 
	PPER_IO_CONTEXT		pNextIO = NULL; 
	PPER_IO_CONTEXT		pTempIO = NULL;

	__try
	{
		EnterCriticalSection(&g_CriticalSection); 
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n"); 
		return;
	}

	if (lpPerSocketContext) {
		pBack = lpPerSocketContext->pCtxtBack; // 삭제할 노드의 이전 노드
		pForward = lpPerSocketContext->pCtxtForward; // 삭제할 노드의 다음 노드 

		if ((pBack == NULL) && (pForward == NULL)) {
			// 노드 하나 
			g_pCtxtList = NULL; 
		}
		else if ((pBack == NULL) && (pForward != NULL)) {
			// 삭제할 노드가 첫번째 노드 
			// [ㅁ] <-> ㅁ <-> ㅁ
			// back ~ forward
			pForward->pCtxtBack = NULL; 
			g_pCtxtList = pForward; // 헤드로 삭제한 노드의 다음 노드 지정
		}
		else if ((pBack != NULL) && (pForward == NULL)) {
			// 삭제할 노드가 마지막 노드 
			// ㅁ <-> ㅁ <-/-> [ㅁ]
			pBack->pCtxtForward = NULL;  // 연결 끊기 
		}
		else if (pBack&&pForward){
			// 삭제할 노드가 중간 노드 
			// ㅁ <-/-> [ㅁ] <-/-> ㅁ
			// 삭제할 노드의 다음 노드를 삭제할 노드의 다음 노드로 연결
			// 삭제할 노드의 다음 노드를 삭제할 노드의 이전 노드로 연결
			pBack->pCtxtForward = pForward; 
			pForward->pCtxtBack = pBack;
		}

		pTempIO = (PPER_IO_CONTEXT)(lpPerSocketContext->pIOContext); 
		do {
			pNextIO = (PPER_IO_CONTEXT)(pTempIO->pIOContextForward);
			if (pTempIO) {
				
				if (g_bEndServer)
					while (!HasOverlappedIoCompleted((LPOVERLAPPED)pTempIO)) Sleep(0);
				xfree(pTempIO); 
				pTempIO = NULL;
			}
			pTempIO = pNextIO;
		} while (pNextIO);

		xfree(lpPerSocketContext); 
		lpPerSocketContext = NULL;
	}
	else {
		myprintf("CtxtListDeleteFrom: lpPerSocketContext is NULL\n");
	}

	LeaveCriticalSection(&g_CriticalSection);
	return;
}

VOID CtxtListFree() {

	PPER_SOCKET_CONTEXT		pTemp1, pTemp2;

	__try
	{
		EnterCriticalSection(&g_CriticalSection); 
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n");
		return;
	}

	// 헤드 노드부터 하나씩 정리 
	// Linked List의 헤드 노드를 pTemp1에 할당
	pTemp1 = g_pCtxtList; 
	while (pTemp1) {
		// 헤드 노드의 이전 노드를 pTemp2에 저장
		pTemp2 = pTemp1->pCtxtBack;
		// 헤드 노드 닫기 
		CloseClient(pTemp1, FALSE);
		// pTemp1에 이전노드를 저장 
		pTemp1 = pTemp2;
	}

	LeaveCriticalSection(&g_CriticalSection); 
	return;
}


int myprintf(const char* lpFormat, ...) {

	int nLen = 0;
	int nRet = 0;
	char cBuffer[512 + 1];
	va_list arglist;
	HANDLE hOut = NULL;
	HRESULT hRet;

	ZeroMemory(cBuffer, sizeof(cBuffer));

	va_start(arglist, lpFormat);

	nLen = lstrlen(lpFormat);
	hRet = StringCchVPrintf(cBuffer, 512, lpFormat, arglist);

	if (nRet >= nLen || GetLastError() == 0) {
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut != INVALID_HANDLE_VALUE)
			WriteConsole(hOut, cBuffer, lstrlen(cBuffer), (LPDWORD)&nLen, NULL);
	}
	return nLen;
}









////////////////////////////////////////////////////////
/* CUSTOM FUNCTION */
int CtxtBindFroms(std::map<SOCKET, IBindObject*>& BindObjects)
{
	__try
	{
		EnterCriticalSection(&g_CriticalSection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n");
		return 0;
	}

	for (PPER_SOCKET_CONTEXT pSocketContext = g_pCtxtList; pSocketContext != NULL;)
	{
		if (pSocketContext->pBindObejct != NULL)
		{
			BindObjects[pSocketContext->Socket] = pSocketContext->pBindObejct; 
		}
		pSocketContext = pSocketContext->pCtxtBack;
	}

	LeaveCriticalSection(&g_CriticalSection);
	return (int)BindObjects.size();
}

IBindObject* CtxtBindFrom(SOCKET sd)
{
	IBindObject* pBindObject = nullptr;

	__try
	{
		EnterCriticalSection(&g_CriticalSection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n");
		return nullptr;
	}

	for (PPER_SOCKET_CONTEXT pSocketContext = g_pCtxtList; pSocketContext != NULL; )
	{
		if (pSocketContext->Socket == sd)
		{
			pBindObject = pSocketContext->pBindObejct;
			break;
		}
		pSocketContext = pSocketContext->pCtxtBack;
	}

	LeaveCriticalSection(&g_CriticalSection);
	return pBindObject;
}

// 소켓에 오브젝트 바인드 
VOID CtxtBindTo(SOCKET sd, IBindObject* bindObject) ///// &
{

	PPER_SOCKET_CONTEXT pSocketContext;

	__try
	{
		EnterCriticalSection(&g_CriticalSection);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myprintf("EnterCriticalSection raised an exception.\n");
		return;
	}

	pSocketContext = g_pCtxtList;
	while (pSocketContext) {
		if (pSocketContext->Socket == sd)
		{
			if (pSocketContext->pBindObejct != nullptr)
				delete pSocketContext->pBindObejct;

			pSocketContext->pBindObejct = bindObject;
			break;
		}
		pSocketContext = pSocketContext->pCtxtBack;
	}
	LeaveCriticalSection(&g_CriticalSection);
	return;
}
