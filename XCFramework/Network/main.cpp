/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "NetworkPrecompiledHeader.h"

#if defined(UNUSED)

#include <stdio.h>
#include <conio.h>

#include "NetworkManager.h"
#include "Server.h"
#include "Client.h"


static const i32 MAX_IMAGE_SIZE = 200 * 1024;       //200 kb

void sendImageData(NetworkManager& netMgr);


i32 main(i32 argc, char* argv[])
{
    NetworkManager netMgr;
    
    INetPeer* peer = nullptr;

    const char* ipAddress = IP_ADDRESS;
    i32 port = DEFAULT_PORT;

    if (argc > 0)
    {
        if (strcmp(argv[1], "server") == 0)
        {
            peer = new Server();
        }
        else if (strcmp(argv[1], "client") == 0)
        {
            peer = new Client();
        }

        if (argc > 1 && argv[2])
        {
            ipAddress = argv[2];
        }

        if (argc > 2 && argv[3])
        {
            port = atoi(argv[3]);
        }

        peer->Init(ipAddress, port);
    }

    netMgr.AddNetPeer(peer);

    bool isDone = false;

    while (!isDone)
    {
        //Loop until end
        if (_kbhit())
        {
            char ch = getch();
            
            if (ch == '1')
            {
                isDone = true;
            }
            else if (ch == '2')
            {
                sendImageData(netMgr);
                //netMgr.NetworkSendData("Hi! I am sending data\n");
            }
        }

        netMgr.NetworkReceiveData();
    }

    netMgr.Destroy();

    getchar();
    return 0;
}


void sendImageData(NetworkManager& netMgr)
{
    FILE* fp = fopen("out.bmp", "rb");
    u8* pImageData = new u8[256 * 256 * 4];
    fread(pImageData, sizeof(u8), 256 * 256 * 4, fp);
    fclose(fp);

    netMgr.NetworkSendData("S", 1);

    for (i32 index = 0; index < 256 * 256 * 4; index++)
    {
        if (pImageData[index] == '\n' || pImageData[index] == 0)
        {
            pImageData[index] = 1;
        }
        else if (pImageData[index] == '\r')
        {
            pImageData[index] = 1;
        }
    }

    for (i32 index = 0; index < 128; index += 4)
    {
        printf("[Pixel %d] R : %d  G: %d B : %d A : %d\n", index, pImageData[index], pImageData[index + 1], pImageData[index + 2], pImageData[index + 3]);
    }

    //pImageData[256 * 256 * 4] = '\n';
    //pImageData[256 * 256 * 4 + 1] = '0';    //Mark end of stream

    netMgr.NetworkSendData((char*)pImageData, 256 * 256 * 4);

    delete(pImageData);
    fclose(fp);
}

#endif