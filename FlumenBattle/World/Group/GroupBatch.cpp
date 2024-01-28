#include <mutex>

#include "GroupBatch.h"

using namespace world::group;

void GroupBatch::Add(Group *group)
{
    static std::mutex mutex;

    mutex.lock();

    *groups.Add() = group;

    mutex.unlock();
}

void GroupBatch::Remove(Group *group)
{
    static std::mutex mutex;

    mutex.lock();
    
    groups.Remove(group);

    mutex.unlock();
}