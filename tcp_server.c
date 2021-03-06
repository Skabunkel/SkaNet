#include "skanet.h"
#include <stdio.h>

typedef struct tcpConnections
{
SOCKET sockets[MAXCONNECTIONS];
int socketCount;
} tcpConnections;

#include <stdint.h>

bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

int main()
{
    int status = InitNetwork();

    printf("%s\n", is_big_endian() ? "true" : "false");

    struct tcpConnections connections;
    connections.socketCount = MAXCONNECTIONS;

    fd_set masterSet;
    fd_set tmpSet;

    
    struct sockaddr client;
    int clientSize = sizeof(client);
    memset(&client, 0, clientSize);
    
    FD_ZERO(&masterSet);

    if(status != 0)
    {
        printf("Something went wrong\n");
        return 1;
    }
    AddressProtocol protocol = IPV4;

    SOCKET listner = CreateTCPSocket(15360, protocol, "localhost", true);

    if(listner == INVALID_SOCKET)
    {
        printf("Something went wrong\n");
        return 1;
    }
    else
    {
        printf("Success\n");
    }

    bool run = true;

    for(int i = 0; i < MAXCONNECTIONS; i++)
    {
        connections.sockets[i] = 0;
    }
    int recvCount = 0;
    int peersock = 0;
    FD_SET(listner, &masterSet);

    const int RECVBUFFCOUNT = 512;
    uint8_t recvBuff[RECVBUFFCOUNT+1];

    const int addressBuffer = 256;
    char addrBuff[addressBuffer];
    memset(&addrBuff, 0, addressBuffer);
    
    memset(recvBuff, 0, RECVBUFFCOUNT);

    bool found = false;
    int maxDf = listner;
    
    while(run)
    {
        tmpSet = masterSet;
        status = select(maxDf+1, &tmpSet, NULL, NULL, NULL);
        if(status < 0)
        {
            printf("Select went wrong\n");
            return 1;
        }
        else
        {
            if(FD_ISSET(listner, &tmpSet))
            {
                found = false;
                int peersock = accept(listner, NULL, NULL);
                if(maxDf < peersock)
                {
                    maxDf = peersock;
                }
                for(int i = 0; i < MAXCONNECTIONS; i++)
                {
                    if(connections.sockets[i] == 0)
                    {
                        connections.sockets[i] = peersock;
                        found = true;
                        SetNoneBlocking(peersock);
                        FD_SET(peersock, &masterSet);
                        break;
                    }
                }
                if(!found)
                {
                    CloseSocket(peersock);
                }
            }
            for(int i = 0; i < MAXCONNECTIONS; i++)
            {
                if(connections.sockets[i] != 0)
                {
                    if(FD_ISSET(connections.sockets[i], &tmpSet))
                    {
                        status = RecvFrom(connections.sockets[i], recvBuff, RECVBUFFCOUNT, 0, &client, &clientSize);
                        if(status > 0)
                        {
                            /*if(protocol == IPV4)
                            {   TODO a function to get the IP address of a connected party.
                                struct sockaddr_in *addr_in = (struct sockaddr_in *)&client;
                                inet_ntop(protocol, &addr_in->sin_addr, addrBuff, addressBuffer);
                            }
                            else
                            {
                                struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)&client;
                                inet_ntop(protocol, &addr_in6->sin6_addr, addrBuff, addressBuffer);
                            }*/
                            //printf("%d %s\n %s\n", status, addrBuff, recvBuff);
                            for(int i = 0; i < status; i++)
                            {
                                printf("0x%02x ", recvBuff[i]);
                            }
                            printf("\n");
                            memset(recvBuff, 0, status+1);
                        }
                        else
                        {
                            FD_CLR(connections.sockets[i], &masterSet);
                            connections.sockets[i] = 0;
                        }
                    }
                }
            }
        }
    }

    CloseNetwork();
    return 0;
}
