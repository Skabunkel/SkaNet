/*  Skanet is a c99 copy/paste/implementation of the network portion of https://github.com/SLikeSoft/SLikeNet
*   For more robust networking and coding see that.
*   
*   
*   
*/  

#if !defined(_SKANET_H)
#define _SKANET_H
#if defined(__STDC__) && __STDC_VERSION__ >= 199901L
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
    typedef enum { false, true } bool;
#endif // __STDC__

#if !defined(MAXCONNECTIONS)
    // One could use SOMAXCONN however its definition verries alot depending on what version we use
    #define MAXCONNECTIONS 5
#endif
#if defined(PLATFORM_OS_WINDOWS)
    #pragma comment(lib, "ws2_32.lib")
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    #include <stdio.h>
    #if !defined(WINDOWS_PROTOCOL_1)
        #define WINDOWS_PROTOCOL_VERSION MAKEWORD(2, 2)
    #else
        #define WINDOWS_PROTOCOL_VERSION MAKEWORD(1, 1)
    #endif // WINDOWS_PROTOCOL_1
#else
    #include <stdio.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <stdlib.h>
    #include <arpa/inet.h>
    #include <memory.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>


    
    // defines 
    typedef int SOCKET;    
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif // PLATFORM_OS_WINDOWS

typedef enum 
{
    IPV4 =  AF_INET,
    IPV6 =  AF_INET6
} AddressProtocol;

typedef enum
{
    TCP =  SOCK_STREAM,
    UDP =  SOCK_DGRAM
} TransferProtocol;

int InitNetwork();
int CloseNetwork();

int SetNoneBlocking(SOCKET socket);

int CloseSocket(SOCKET socket);
SOCKET CreateTCPSocket(unsigned short port, AddressProtocol socketFamily, const char *bindAddress, bool isListner);

bool SetSocketReciveBufferSize(SOCKET socket, int bufferSize);
bool SetSocketSendBufferSize(SOCKET socket, int bufferSize);
bool SetSocketHardClose(SOCKET socket);
int SetNoneBlocking(SOCKET socket);
bool SetBroadcast(SOCKET socket);


#endif // _RAYNET_H