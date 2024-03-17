#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <exception>

#include <iostream>

#include "Socket.h"
#include "SocketManager.h"
#include "XBox.h"
#include "InputParser.h"

#pragma comment(lib,"WS2_32")


int main(int argc, char** argv)
{
	InputParser input(argc, argv);
	struct ControllerState controllerState, lastStateController;
	CXBOXController* Player1;
	SocketManager* sm = new TCPSocketManager();

	const std::string& p_host = input.getCmdOption("--host");
	const std::string& p_port = input.getCmdOption("--port");
	const std::string& p_deadline = input.getCmdOption("--deadline");
	const char* address;
	if (!p_host.empty()) {
		address = p_host.c_str();
	}
	else
	{
		std::cerr << "--host arg is missing. IP address is set to default value [127.0.0.1]\n";
		address = "127.0.0.1";
	}

	int port;
	if (!p_port.empty()) {
		port = std::stoi(p_port);
	}
	else
	{
		std::cerr << "--port arg is missing. Port is set to default value [8080]\n";
		port = 8080;
	}

	int deadline;
	if (!p_deadline.empty()) {
		deadline = std::stoi(p_deadline);
	}
	else {
		std::cerr << "--deadline arg is missing. Deadline is set to default value [3000]\n";
		deadline = 3000;
	}

	SmartSocket* sc;
	try {
		sc = sm->CreateSocket(address, port);
	}
	catch (std::exception e) {
		return(1);
	}


	Player1 = new CXBOXController(1);
	int left, right;

	char sendbuf[sizeof(struct ControllerState)];

	boolean isStopped = true;
	boolean isDisconected = true;

	using frames = std::chrono::duration<int64_t, std::ratio<1, 10>>;
	auto nextFrame = std::chrono::system_clock::now();
	while (true)
	{
		std::this_thread::sleep_until(nextFrame);
		nextFrame += frames{ 1 };

		if (Player1->IsConnected())
		{
			if (isDisconected) {
				isDisconected = false;
				std::cerr << "Controller connected!\n";
			}
			left = Player1->GetState().Gamepad.sThumbLY;
			right = Player1->GetState().Gamepad.sThumbRY;
			if (abs(left) < deadline)
				left = 0;
			if (abs(right) < deadline)
				right = 0;
			controllerState.leftThumb = (left * 255) / SHRT_MAX;
			controllerState.rightThumb = (right * 255) / SHRT_MAX;
			if (lastStateController != controllerState) {
				memcpy(sendbuf, &controllerState, sizeof(controllerState));
				sc->write(sendbuf, sizeof(struct ControllerState));
				std::cerr << "SEND: " << controllerState.leftThumb << " " << controllerState.rightThumb << "\n";
			}
			lastStateController = controllerState;
		}
		else
		{
			isDisconected = true;
			std::cerr << "ERROR! XBOX 360 Controller Not Found!\n";
			std::cerr << "System will try again after 5 seconds...\n";
			nextFrame += std::chrono::seconds(5);
		}
	}

	delete(Player1);

	return(0);
}

