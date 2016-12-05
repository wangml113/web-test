#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <thread>
#include <chrono>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")  //load ws2_32.dll

#define BUFF_SIZE 100
//clock_t t1[500], t2[500];
double t1[500], t2[500];
class Data
{
public:
	int index;
	double x;
	double y;
	double theta;
};
class Rate
{
private:
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> timeBegin;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> timeEndExpected;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> timeEnd;
	int rate;
	std::chrono::nanoseconds durationNeed;

public:
	Rate(int rate_);
	~Rate();

	void sleep();

};

Rate::Rate(int rate_) {
	rate = rate_;
	int64_t nano = 1e9 / rate;

	durationNeed = std::chrono::nanoseconds(nano);

	timeBegin = std::chrono::high_resolution_clock::now();
	timeEndExpected = timeBegin + durationNeed;

}

Rate::~Rate() {}

void Rate::sleep() {

	timeEnd = std::chrono::high_resolution_clock::now();

	if (timeEnd < timeEndExpected)
		std::this_thread::sleep_until(timeEndExpected);
	else
		timeEndExpected = std::chrono::high_resolution_clock::now();

	timeEndExpected += durationNeed;
}

// TO DO 
void rcvFunc() 
{
	//get data from server
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //fill every element with 0
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("169.254.232.64");
	sockAddr.sin_port = htons(8000);

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	char szBuffer[BUFF_SIZE] = "init";
	int index = 0;
	while (recv(sock, szBuffer, BUFF_SIZE, NULL))
	{
		//double rcv_array[3] = { 1.0, 2.0, 3.0 };
		//memcpy(rcv_array, szBuffer, sizeof(rcv_array));
		Data posData;
		memcpy(&posData, szBuffer, sizeof(posData));
		//index = posData.index;
		t2[index] = double(clock());
		//printf("I get feedback ");
		//for (int j = 0; j < 3; j++)
			//printf("%f ", rcv_array[j]);
		//printf("\n");
		closesocket(sock);

		SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
		char szBuffer[BUFF_SIZE] = "init";
		index++;
	}
	closesocket(sock);
}

void sndFunc()
{
	Rate rate(500);
	//sent request to server 
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //fill every element with 0
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("169.254.232.64");
	sockAddr.sin_port = htons(8080);
	
	for (int i = 0; i < 500; i++) 
	{
		//create sock
		SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
		// decide data to send
		Data odoData;
		odoData.index = i;
		odoData.x = sin(i);
		odoData.y = cos(i);
		odoData.theta = i*0.002;
		//double array[3];
		//array[0] = (double)i;
		//array[1] = (double)i + 0.1;
		//array[2] = (double)i + 0.2;
		//char buffer[sizeof(array)];
		//memcpy(buffer, array, sizeof(array));
		char buffer[sizeof(Data)];
		memcpy(buffer, &odoData, sizeof(odoData));
		// send data
		send(sock, buffer, BUFF_SIZE, NULL);
		//printf("I have sent package %d \n", i);
		//for (int j = 0; j < 3; j++)
			//printf("%f ", array[j]);
		//printf("\n");
		t1[i] = double(clock());
		closesocket(sock);
		rate.sleep();
	}
	
}

int main() {
	
	//init DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::thread threadRecv(&rcvFunc);
	std::thread threadSnd(&sndFunc);
	
	
	//stop using DLL
	WSACleanup();
	for (int i = 0; i < 500; i++)
	{
		printf("sendTime %d = %f \n", i, t1[i]/CLOCKS_PER_SEC);
		printf("recvTime %d = %f \n", i, t2[i]/CLOCKS_PER_SEC);
	}
	system("pause");
	return 0;
}