#ifndef IOCPSERVER_H
#define IOCPSERVER_H

#include <WinSock2.h>
#include <MSWSock.h>


#define DEFAULT_PORT		"25000"
#define MAX_BUFF_SIZE		8192
#define MAX_WORKER_THREAD	128

#include <map>


extern int deadPlayerCnt;
extern int deadCastleCnt;


typedef enum _IO_OPERATION {	ClientIoAccept, 
	ClientIoRead, 
	ClientIoWrite
} IO_OPERATION, *PIO_OPERATION;

typedef struct _PER_IO_CONTEXT {
	WSAOVERLAPPED		Overlapped;
	char				Buffer[MAX_BUFF_SIZE];
	WSABUF				wsabuf; 
	int					nTotalBytes; 
	int					nSentBytes; 
	IO_OPERATION		IOOperation;
	SOCKET				SocketAccept; 

	struct _PER_IO_CONTEXT* pIOContextForward;
} PER_IO_CONTEXT, *PPER_IO_CONTEXT;


class IBindObject
{
public:
	virtual ~IBindObject() { } //////////////////////
};

typedef struct _PER_SOCKET_CONTEXT {
	SOCKET				Socket; 

	//LPFN_ACCEPTEX		fnAcceptEx;

	IBindObject* pBindObejct;

	// Linked List
	PPER_IO_CONTEXT			pIOContext; 
	struct _PER_SOCKET_CONTEXT* pCtxtBack; 
	struct _PER_SOCKET_CONTEXT* pCtxtForward; 
} PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;


BOOL ValidOptions(int argc, char* argv[]);

BOOL WINAPI CtrlHandler(DWORD dwEvent);

BOOL CreateListenSocket(void);

BOOL CreateAcceptSocket(BOOL fUpdateIOCP);

DWORD WINAPI WorkerThread(LPVOID WorkContext); 

PPER_SOCKET_CONTEXT UpdateCompletionPort(SOCKET s, IO_OPERATION ClientIo, BOOL bAddToList);


VOID CloseClient(PPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful);

PPER_SOCKET_CONTEXT CtxtAllocate(SOCKET s, IO_OPERATION ClientIO);

VOID CtxtListFree();

VOID CtxtListAddTo(PPER_SOCKET_CONTEXT lpPerSocketContext); 

VOID CtxtListDeleteFrom(PPER_SOCKET_CONTEXT lpPerSocketContext);

VOID CtxtBindTo(SOCKET, IBindObject* bindObject); // & 

IBindObject* CtxtBindFrom(SOCKET sd);

int CtxtBindFroms(std::map<SOCKET, IBindObject*>& BindObjects);


#endif