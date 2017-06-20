#include <windows.h>  
#include <stdio.h> 
#include <process.h>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>
using namespace std;

bool bUseMultiWork = true;
int maxAssistant = 4;
HANDLE* assistant;
HANDLE server_event;
HANDLE* client_event;
int tcount = 0;

std::map<int,int> mWorkList;
int workCount = 1000000;
int haveWorkCount = 0;
void testWork()
{
	//haveWorkCount++;
	sqrt(rand());
	return;
}

void multiWork()
{
	int _workCount = workCount / maxAssistant;
	for (int i = 0; i < _workCount; i++)
	{
		testWork();
	}
}

void singleWork()
{
	for (int i = 0; i < workCount; i++)
	{
		testWork();
	}
}



void say_hello(void* args)
{
	//int id = *(int*)args;
	int id = tcount++;

	printf("thread created %d...\n",id);

	while (true)
	{
		WaitForSingleObject(server_event, INFINITE);

		//do something
		//printf("thread %d work...\n", id);

		//Sleep(1000);
		multiWork();

		//printf("thread %d finish...\n", id);
		
		SetEvent(client_event[id]);
	}

	printf("thread %d end...\n", id);
} 

int main()  
{  
	

	client_event = new HANDLE[maxAssistant];
	assistant = new HANDLE[maxAssistant];

	printf("Creating threads...\n");
	
	server_event = CreateEvent(NULL, FALSE, FALSE, NULL); 

	for (int i = 0; i < maxAssistant; i++)
	{
		HANDLE hThread;
		unsigned threadID;
		
		hThread = (HANDLE)_beginthread(say_hello, 0, &i);

		//hThread = (HANDLE)_beginthreadex(NULL, 0, say_hello2, &i, 0, &threadID);

		assistant[i] = hThread;

		client_event[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

		//Sleep(1);
	}

	char command[256];

	LARGE_INTEGER pb,pe,fq;
	double time;
	while (true)
	{
		std::cin.getline(command, 256);
		//Sleep(50);
		if (command[0] == '0')
		{
			bUseMultiWork = false;
		}
		else
		{
			bUseMultiWork = true;
		}

		printf("do command...\n");

		QueryPerformanceCounter(&pb);
		if (bUseMultiWork)
		{
			for (int i = 0; i < maxAssistant; i++)
			{
				SetEvent(server_event);
			}

			WaitForMultipleObjects(maxAssistant, client_event, true, INFINITE);
		}
		else
		{
			singleWork();
		}		

		QueryPerformanceCounter(&pe);
		
		QueryPerformanceFrequency(&fq);

		time = (double)(pe.QuadPart - pb.QuadPart) / (double)fq.QuadPart;
		printf("command finish all %d %f...\n", haveWorkCount, time*1000);
	}
} 
