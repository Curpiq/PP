// lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>


DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	std::stringstream ss;
	ss << "Поток №" << *(int*)lpParam << " выполняет свою работу\n";
	std::cout << ss.str();
	ExitThread(0); // функция устанавливает код завершения потока в 0
}


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");
	if (argc != 2)
	{
		return 1;
	}
	int threadCount = std::stoi(argv[1]);
	HANDLE* handles = new HANDLE[threadCount];
	for (int i = 0; i < threadCount; i++)
	{
		handles[i] = CreateThread(NULL, 0, &ThreadProc, new int(i + 1), CREATE_SUSPENDED, NULL);
		ResumeThread(handles[i]);
	}
	WaitForMultipleObjects(threadCount, handles, true, INFINITE);
	return 0;
}
