#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <cstdio>
#include <cstdlib>
#include <thread>

#include <iostream>

#include "Socket.h"
#include "SocketManager.h"
#include "XBox.h"

#pragma comment(lib,"WS2_32")


int main()
{
	struct ControllerState controller;
	CXBOXController* Player1;
	SocketManager* sm = new TCPSocketManager();

	//SmartSocket* sc = sm->CreateSocket("127.0.0.1", 8080);
	SmartSocket* sc = sm->CreateSocket("83.6.65.43", 8080);	
	

    Player1 = new CXBOXController(1);
	int left, right;
	int deadline = 3000;
	char sendbuf[sizeof(struct ControllerState)];


	using frames = std::chrono::duration<int64_t, std::ratio<1, 10>>;
	auto nextFrame = std::chrono::system_clock::now();
    while (true)
    {
		std::this_thread::sleep_until(nextFrame);
		nextFrame += frames{ 1 };

        if (Player1->IsConnected())
        {
			left = Player1->GetState().Gamepad.sThumbLY;
			right = Player1->GetState().Gamepad.sThumbRY;
			if (abs(left) > deadline || abs(right) > deadline) {
				controller.leftThumb = (left * 255) / SHRT_MAX;
				controller.rightThumb = (right * 255) / SHRT_MAX;
				memcpy(sendbuf, &controller, sizeof(controller));
				sc->write(sendbuf, sizeof(struct ControllerState));
			}
        }
        else
        {
            std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
            std::cout << "Press Any Key To Exit.";
            std::cin.get();
            break;
        }
    }

    delete(Player1);

    return(0);
}

