#include "skanet.h"

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

#if defined(PLATFORM_OS_WINDOWS)
    #define _CLOSE(x) closesocket(x)
#else
    #define _CLOSE(x) close(x)
#endif

#if defined(O_NONBLOCK)
    #define NOBLOCK__ O_NONBLOCK
#else
    #define NOBLOCK__ SOCK_NONBLOCK
#endif


int InitNetwork()
{
#if defined(PLATFORM_OS_WINDOWS)
    WORD version = WINDOWS_PROTOCOL_VERSION;
    WSADATA wsaData;
    return WSAStartup(version, &wsaData);
#else
    return 0;
#endif
}

int CloseNetwork()
{
#if defined(PLATFORM_OS_WINDOWS)
    return WSACleanup();
#else
    return 0;
#endif
}

int CloseSocket(SOCKET socket)
{
	return _CLOSE(socket);
}

#define PORTSTRSIZE 32

SOCKET CreateTCPSocket(unsigned short port, AddressProtocol socketFamily, const char *bindAddress, bool isListner)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof (struct addrinfo)); // make sure the struct is empty
	hints.ai_family = socketFamily;     // don't care IPv4 or IPv6
	hints.ai_socktype = TCP; // TCP sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	struct addrinfo *servinfo=0, *aip;  // will point to the results

	char portStr[PORTSTRSIZE];
	snprintf(portStr, PORTSTRSIZE, "%hu", port);
    SOCKET listenSocket;

	getaddrinfo(0, portStr, &hints, &servinfo);
	for (aip = servinfo; aip != NULL; aip = aip->ai_next)
	{
		// Open socket. The address type depends on what
		// getaddrinfo() gave us.
		listenSocket = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
		if (listenSocket != INVALID_SOCKET)
		{
			if(!isListner)
				break;

			int ret = bind( listenSocket, aip->ai_addr, (int) aip->ai_addrlen );
			if (ret>=0)
			{
				break;
			}
			else
			{
                _CLOSE(listenSocket);
				listenSocket=INVALID_SOCKET;
			}
		}
	}

	if (listenSocket!=INVALID_SOCKET && isListner)
		listen(listenSocket, MAXCONNECTIONS);

	free(servinfo);

	return listenSocket;
}

bool SetSocketReciveBufferSize(SOCKET socket, int bufferSize)
{
	return setsockopt(socket, SOL_SOCKET, SO_RCVBUF, ( char * ) & bufferSize, sizeof ( bufferSize ) ) != SOCKET_ERROR;
}

bool SetSocketSendBufferSize(SOCKET socket, int bufferSize)
{
	return setsockopt(socket, SOL_SOCKET, SO_SNDBUF, ( char * ) & bufferSize, sizeof ( bufferSize ) ) != SOCKET_ERROR;
}

bool SetSocketHardClose(SOCKET socket)
{
	int sock_opt=0;
	return setsockopt(socket, SOL_SOCKET, SO_SNDBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) ) != SOCKET_ERROR;
}


int SetNoneBlocking(SOCKET socket)
{
#if defined(PLATFORM_OS_WINDOWS)
    unsigned long nonblocking = 1;
    return ioctlsocket(socket, FIONBIO, &nonblocking);
#else
    return fcntl(socket, F_SETFL, NOBLOCK__ );
#endif
}

bool SetBroadcast(SOCKET socket)
{
    int sock_opt = 1;
    return setsockopt(socket, SOL_SOCKET, SO_BROADCAST, ( char * ) & sock_opt, sizeof( sock_opt ) ) != SOCKET_ERROR;
}

bool Resolve(struct sockaddr *result, AddressProtocol protocol, const char *address, const char *port)
{
    if(result == NULL)
        return false;

    struct addrinfo hints;
    memset(&hints, 0, sizeof (hints));
	hints.ai_family = protocol;
	hints.ai_socktype = TCP;
	hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res = NULL;
    int status = getaddrinfo(address, port, &hints, &res);

    if(status != 0 || res == NULL)
        return false;

    result->sa_family = res[0].ai_family;
    memcpy(&result->sa_data, res[0].ai_addr[0].sa_data, 14);

	free(res);

    return true;
}