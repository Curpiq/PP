#pragma once

#include "BlurTask.h"
#include "IWorker.h"
#include <Windows.h>
#include <memory>
#include <vector>

const int MAX_THREADS = 4;

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	ITask* task = (ITask*)lpParam;
	task->Execute();
	ExitThread(0);
}

class ThreadsPool : public IWorker
{
private:
	std::vector<HANDLE> m_threads;
	int m_threadNumber;

public:
	ThreadsPool(std::vector<ITask*>& tasks, int threadNumber)
		:m_threadNumber(threadNumber)
	{
		for (size_t i = 0; i < m_threadNumber; i++)
		{
			m_threads.push_back(CreateThread(NULL, i, &ThreadProc, tasks[i], CREATE_SUSPENDED, NULL));
		}
	}

	void Execute() override
	{
		int counter = 0;
		for (size_t i = 0; i < m_threadNumber; i++)
		{
			ResumeThread(m_threads[i]);
			counter++;
			if (counter == MAX_THREADS)
			{
				WaitForMultipleObjects(i+1, m_threads.data(), true, INFINITE);
				counter = 0;
			}
		}
		WaitForMultipleObjects(m_threadNumber, m_threads.data(), true, INFINITE);
	}

	bool IsBusy() override
	{
		DWORD statusCode;
		for (int i = 0; i < m_threads.size(); i++)
		{
			GetExitCodeThread(m_threads[i], &statusCode);
			if (statusCode == STILL_ACTIVE)
			{
				return true;
			}
		}
	}

	~ThreadsPool()
	{
		while (IsBusy())
		{
		}
	}
};