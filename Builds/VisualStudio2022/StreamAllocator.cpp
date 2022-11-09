#include "StreamAllocator.h"


StreamAllocator::StreamAllocator(int streamSize) {
	for (int i = 0; i < streamSize; ++i)
	{
		this->streamSize = streamSize;
	}
}

int StreamAllocator::IdToSlot(std::string id) {
	/* returns the slot that the stream was allocated to.
		if all slots allocated, returns -1.
	*/
	if (streamMap.find(id) != streamMap.end()) {
		return streamMap[id];
	}

	// extracts the first value between 0 to streamSize
	// that is not allocated yet.
	int minFreeSlot = -1;
	for (int i = 0; i < streamSize; ++i) {
		if (slotSet.count(i) == 0) {
			minFreeSlot = i;
			break;
		}
	}
	// if all slots allocated, do not allocate.
	if (minFreeSlot != -1) {
		streamMap[id] = minFreeSlot;
		slotSet.insert(minFreeSlot);
	}
	return minFreeSlot;
}

//int StreamAllocator::GetStreamSlot(std::string id) {
//	if (streamMap.find(id) == streamMap.end()) {
//		return -1;
//	}
//	return streamMap[id];
//}