#include "GroupBatch.h"

using namespace world::group;

void GroupBatch::Add(Group *group)
{
    *groups.Add() = group;
}

void GroupBatch::Remove(Group *group)
{
    groups.Remove(group);
}