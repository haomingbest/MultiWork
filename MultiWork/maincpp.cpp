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

std::vector<int> mWorkList;
int workCount = 10000000;
int haveWorkCount = 0;
int haveWorkCount0 = 0;
int haveWorkCount1 = 0;
int haveWorkCount2 = 0;
int haveWorkCount3 = 0;


///
//多线程的写操作一旦进入公有区域 不管这个内容有没有竞争 都会使运行速度大为下降
//最好的办法是定义一个局部的“结果数据” 把运算内容放在这个“结果数据”内 才能发挥多线程的最大功效
//公有区域的读操作是没有任何问题的
//
void testWork(int& id)
{
	//全局变量的"写"默认是有“锁”的
	//haveWorkCount++;	
	/*if (id == 0)
		haveWorkCount0++;
	if (id == 1)
		haveWorkCount1++;
	if (id == 2)
		haveWorkCount2++;
	if (id == 3)
		haveWorkCount3++;
	if (id == -1)
		haveWorkCount++;*/

	id++;
	sqrt(rand());
	return;
}

void multiWork(int id)
{	
	//if (id == 0)
	//	haveWorkCount0 = 0;
	//if (id == 1)
	//	haveWorkCount1 = 0;
	//if (id == 2)
	//	haveWorkCount2 = 0;
	//if (id == 3)
	//	haveWorkCount3 = 0;
	//

	//多线程的写操作一旦进入公有区域 不管这个内容有没有竞争 都会使运行速度大为下降
	//最好的办法是定义一个局部的“结果数据” 把运算内容放在这个“结果数据”内 才能发挥多线程的最大功效
	int c = 0;
	//如果把运算结果放到公有区域内 比如haveWorkCount0 haveWorkCount1 haveWorkCount2 之类 都会极大降低速度 

	int _workCount = workCount / maxAssistant;
	for (int i = 0; i < _workCount; i++)
	{
		testWork(c);	
		//haveWorkCount++;
		//c++;
	}
	haveWorkCount += c;
}

void singleWork()
{
	haveWorkCount = 0;
	for (int i = 0; i < workCount; i++)
	{
		testWork(haveWorkCount);
		haveWorkCount++;
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
		multiWork(id);

		//printf("thread %d finish...\n", id);
		
		SetEvent(client_event[id]);
	}

	printf("thread %d end...\n", id);
} 

int main()  
{  
	
	mWorkList.resize(maxAssistant);
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

		int _count;
		/*if (bUseMultiWork)
			_count = haveWorkCount0 + haveWorkCount1 + haveWorkCount2 + haveWorkCount3;
		else*/
		{
			_count = haveWorkCount;
		}
		printf("command finish all %d %f...\n", _count, time*1000);
	}
} 
