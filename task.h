#pragma once
#include <vector>
#include <string>
#include <thread>
#include <memory>

// overwrite task function
// select task priority function
// job chunk size
// cache specifier
// task resolve function(GlobalTimed, ThreadLocalTimed, Global VeryHigh, LocalVeryHigh, 

enum class TaskOrder
{
	Sequential,
	Parallel
};

enum class TaskLocation
{
	Local,
	Global
};

enum class TaskPriority
{
	VeryHigh,
	High,
	Normal,
	Low,
	VeryLow
};

enum class TaskState
{
	Queued,
	Running,
	Complete,
	Cancelled,
	Fail
};

enum class TaskType
{
	Visibility,
	Database,
	IO
};

enum class TaskDuration
{
	OneOff,
	Periodic,
	Timed,
};

class TaskConfig
{
	TaskPriority priority;
	TaskState state;
	TaskType type;
	TaskDuration duration;
};

class Task
{
	using TaskPtr = std::shared_ptr<Task>;
	
	unsigned int id;
	std::vector<TaskPtr> subTasks;
	TaskConfig config;
	TaskPtr parent;

public:

	void addSubTask(TaskPtr& taskPtr)
	{
		taskPtr->parent = std::make_shared<Task>(*this);
		subTasks.push_back(taskPtr);
	}
	
	TaskPtr getParent()
	{
		return parent;
	}
	
	TaskPtr getRoot()
	{
		if (parent == nullptr) return nullptr;
		auto rootTask = this->getParent();
		while(rootTask)
		{
			auto parentTask = rootTask->getParent();
			if (parentTask) rootTask = rootTask->getParent();
			else return rootTask;
		}
		return rootTask;
	}

	virtual void operator()() = 0;


};

class TaskManager
{
	std::vector<Task> localTasks;
	size_t threadCount;
	size_t fiberCount;
	size_t fiberStackSize;
	static void addLocal();
	static void addGlobal();
	static void detachToGlobal();
	static void detachToLocal();
};

/*
 *add task
 *remove task
 *complete task
 *task complete callbacks
 *task started callbacks
 *task fail callbacks
 */