#pragma once
#include "ITask.h"

class IWorker
{
public:
	virtual void Execute() = 0;
	virtual bool IsBusy() = 0;
	virtual ~IWorker() {};
};