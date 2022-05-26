//
// Created by adirt on 26/05/2022.
//
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream> //TODO DELETE THIS IMPORT AND REMOVE PRINTS
#define ROOT_ADDR 0
using namespace std;
uint64_t translateAddress(uint64_t virtualAddress);
void VMinitialize(){
	for(int i = 0; i < PAGE_SIZE; i++){
		PMwrite(i, 0);
	}
}

int VMread(uint64_t virtualAddress, word_t* value){
	print_ram();
	return 0;
}

/* Writes a word to the given virtual address.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMwrite(uint64_t virtualAddress, word_t value){
	translateAddress(virtualAddress);
	return 0;
}

uint64_t recTranslateAddress(uint64_t frame, uint64_t virtualAddress,  int depth){
	if(depth == 0){
		return frame * PAGE_SIZE + virtualAddress;
	}
	word_t nextFrame;
	int irrelevantBits = (OFFSET_WIDTH * depth);
	int offset = virtualAddress >> irrelevantBits;
	PMread(frame * PAGE_SIZE + offset, &nextFrame);
	if(nextFrame == 0){
//		nextFrame = findFrame(frame);
//		initNewFrame(nextFrame);
//		PMwrite(frame * PAGE_SIZE + offset, nextFrame);
	}
	uint64_t updatedVirtualAddress = virtualAddress & ((1LL << irrelevantBits) - 1);
	return recTranslateAddress(nextFrame, updatedVirtualAddress, depth -1);
}

uint64_t translateAddress(uint64_t virtualAddress){
	int noRootBits = (OFFSET_WIDTH * TABLES_DEPTH);
	uint64_t rootOffset = virtualAddress >> noRootBits;
	word_t firstFrame;
	PMread(ROOT_ADDR + rootOffset, &firstFrame);
	if(firstFrame == 0){
//		firstFrame = findFrame(frame);
//		initNewFrame(nextFrame);
//		PMwrite(frame * PAGE_SIZE + offset, firstFrame);
	}
	uint64_t currentVirtualAddress = virtualAddress & ((1LL << noRootBits) - 1);
	return recTranslateAddress(firstFrame, currentVirtualAddress, TABLES_DEPTH - 1);
}




