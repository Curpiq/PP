// lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <windows.h>
#include <string>
#include <iostream>


DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	std::cout << "thread number " << *(int*)lpParam << " is working" << std::endl;
	ExitThread(0); // функция устанавливает код завершения потока в 0
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		return 1;
	}
	int threadCount = std::stoi(argv[1]);
	HANDLE* handles = new HANDLE[threadCount];
	for (int i = 0; i < threadCount; i++)
	{
		handles[i] = CreateThread(NULL, 0, &ThreadProc, new int(i), CREATE_SUSPENDED, NULL);
		ResumeThread(handles[i]);
	}
	WaitForMultipleObjects(threadCount, handles, true, INFINITE);
	return 0;
}
