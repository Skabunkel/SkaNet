#include "skanet.h"
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#define DATASIZE 16

int main()
{
    int status = InitNetwork();

    if(status != 0)
    {
        printf("Cannot initate networking\n");
        return 1;
    }
    AddressProtocol protocol = IPV4;
    int socket = CreateTCPSocket(15361, protocol, "localhost", false);

    if(socket == INVALID_SOCKET)
    {
        printf("Cannot create socket\n");
        return 1;
    }
    struct sockaddr clientService;

    Resolve(&clientService, protocol, "127.0.0.1", "15360");

    status = connect(socket, &clientService,  sizeof(clientService));
    if(status == SOCKET_ERROR)
    {
        printf("Cannot Connect %s\n", strerror(errno));
        return 1;
    }

    uint8_t data[DATASIZE] = { 0x00, 0xf6, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f  };

    bool sent = SendTo(socket, (uint8_t*)data, DATASIZE, 0, NULL, 0);

    if(!sent)
    {
        printf("Cannot send all data\n");
        return 1;
    }

    CloseSocket(socket);

    CloseNetwork();
    return 0;
}
