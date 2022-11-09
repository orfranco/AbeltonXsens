#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

/*
this class is responsible on the data stream.
Each id (mac address of the sensor), receives a slot (int index from 0 to streamSize).
the allocation works like this:
if a new mac address is recieved, it gets the min of all unallocated slots.


*/

class StreamAllocator
{
public:
	StreamAllocator(int streamSize);
	int IdToSlot(std::string id);
	//int GetStreamSlot(std::string id);
	//TODO: void RemoveStream(std::string id);

private:
	std::unordered_map<std::string, int> streamMap;
	std::unordered_set<int> slotSet;
	int streamSize;
};

