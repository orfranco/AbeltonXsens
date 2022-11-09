#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>



class StreamAllocator
{
public:
	StreamAllocator(int streamSize);
	int AddStream(std::string id);
	int GetStreamSlot(std::string id);
	//TODO: void RemoveStream(std::string id);
	
	


private:
	std::unordered_map<std::string, int> streamMap;
	std::unordered_set<int> slotSet;
	int streamSize;
};

