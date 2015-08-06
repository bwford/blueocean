#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

#define MAX_RESPONSES 255

int main(int Argc, char **Argv)
{
    int32_t DeviceId = hci_get_route(NULL);
    int32_t Socket = hci_open_dev(DeviceId);
    if(DeviceId < 0 || Socket < 0) exit(1);

    char Name[248];
    char AddressString[19];
    inquiry_info* pInfo = (inquiry_info*)malloc(MAX_RESPONSES * sizeof(inquiry_info));
    int32_t Responses = hci_inquiry(DeviceId, 8, MAX_RESPONSES, NULL, &pInfo, IREQ_CACHE_FLUSH);
    for(int i = 0; i < Responses; i++)
    {
        ba2str(&(pInfo+i)->bdaddr, AddressString);
        memset(Name, 0, sizeof(Name));
        if(hci_read_remote_name(Socket, &(pInfo+i)->bdaddr, sizeof(Name), Name, 0) < 0) strcpy(Name, "UNKNOWN");
        printf("%s  %s\n", AddressString, Name);
    }

    free(pInfo);

    printf("Ready!\n");
    bool Quit = false;
    while(!Quit)
    {
        printf("blue ocean>");

        char Buffer[256];
        memset(Buffer, '\0', 256);
        fgets(Buffer, 256, stdin);
        if(!strcmp(Buffer, "exit\n")) exit(1);
        Buffer[strlen(Buffer)-1] = '\0';

        struct sockaddr_rc AddressStruct;
        AddressStruct.rc_family = AF_BLUETOOTH;
        AddressStruct.rc_channel = (uint8_t)1;
        str2ba(AddressString, &AddressStruct.rc_bdaddr);

        int32_t Socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        int32_t Status = connect(Socket, (struct sockaddr *)&AddressStruct, sizeof(AddressStruct));
        if(Status == 0)
        {
            Status = write(Socket, Buffer, strlen(Buffer));
        }

        if(Status < 0) exit(1);
        sleep(1);
    }

    close(Socket);

    return 0;
}
