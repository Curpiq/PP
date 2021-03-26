// lab3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <tchar.h>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#pragma comment( lib, "winmm.lib") 

using namespace std;

struct ThreadParams
{
    size_t threadNumber;
    std::ofstream* output;
    DWORD startTime;
};

DWORD WINAPI ThreadProc(const LPVOID lpParam)
{
    ThreadParams* threadParams = (ThreadParams*)lpParam;

    for (size_t i = 0; i < 1000000; i++)
    {
        DWORD dwDuration = timeGetTime() - threadParams->startTime;
        stringstream ssDuration;
        ssDuration << dwDuration;
        *threadParams->output << std::to_string(threadParams->threadNumber) + "|" + ssDuration.str() + "\n";
    }

    delete threadParams;

    ExitThread(0);
}


int main(int argc, char* argv[])
{
    size_t threadCount = 2;
    const DWORD startTime = timeGetTime();
    HANDLE* handles = new HANDLE[threadCount];
    ofstream* files = new ofstream[threadCount];
    files[0] = std::ofstream("output1.txt");
    files[1] = std::ofstream("output2.txt");
    for (size_t i = 0; i < threadCount; i++)
    {
        handles[i] = CreateThread(NULL, 0, &ThreadProc, new ThreadParams{ i+1, &files[i], startTime }, CREATE_SUSPENDED, NULL);
    }

    for (size_t i = 0; i < threadCount; i++)
    {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(threadCount, handles, true, INFINITE);

    delete[] files;


    return 0;
}
