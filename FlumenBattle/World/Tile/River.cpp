#include "River.h"

using namespace world::tile;

void River::Initialize()
{
    segments.Clear();
}

void River::AddSegment(WorldEdge *segment)
{   
    *segments.Add() = segment;
}