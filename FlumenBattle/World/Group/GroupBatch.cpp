#include <mutex>

#include "GroupBatch.h"

using namespace world::group;

void GroupBatch::Add(GroupCore *group)
{
    *groups.Add() = group;
}

void GroupBatch::Remove(GroupCore *group)
{
    groups.Remove(group);
}