//
// Created by adirt on 26/05/2022.
//
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream> //TODO DELETE THIS IMPORT AND REMOVE PRINTS
#include <cassert>
#define ROOT_ADDR 0
#define MAX_USED_FRAME_INDEX 0
#define EMPTY_TABLE_INDEX 1
#define MAX_DISTANCE_PAGE 2
#define MAX_DISTANCE_FRAME 3
#define DISTANCE 4
using namespace std;
uint64_t translateAddress(uint64_t virtualAddress);
void dfs(uint64_t node, uint64_t candidates[3], uint64_t depth, uint64_t parentFrame, uint64_t targetPage, uint64_t pageIdx);
bool isFrameEmptyTable(uint64_t node);
uint64_t getDistance(uint64_t pageIdx, uint64_t targetPage);
uint64_t findFrame(uint64_t parentFrame, uint64_t targetPage);
void initFrame(uint64_t frameAddress);
typedef struct BestFrameCandidates {
    uint64_t maxFrameIndex = ROOT_ADDR;
    uint64_t emptyTableFrameIndex = ROOT_ADDR;
    uint64_t maxDistancePageIndex = ROOT_ADDR;
    uint64_t maxDistanceFrameIndex = ROOT_ADDR;
    uint64_t maxDistance = ROOT_ADDR;
    uint64_t linkRemovalAddress = ROOT_ADDR;
} BestFrameCandidates;

void VMinitialize(){
    initFrame(0);
}

void initFrame(uint64_t frameAddress){
    for(int i = 0; i < PAGE_SIZE; i++){
        PMwrite(frameAddress * PAGE_SIZE + i, 0);
    }
}

int VMread(uint64_t virtualAddress, word_t* value){
    uint64_t physicalAddress = translateAddress(virtualAddress);
    PMread(physicalAddress, value);
	return 0;
}

/* Writes a word to the given virtual address.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMwrite(uint64_t virtualAddress, word_t value){
	uint64_t physicalAddress = translateAddress(virtualAddress);
    PMwrite(physicalAddress, value);
	return 0;
}

uint64_t recTranslateAddress(uint64_t frame, uint64_t virtualAddress, uint64_t targetPage, int depth){
	if(depth == 0){
		return frame * PAGE_SIZE + virtualAddress;
	}
	word_t nextFrame;
	int irrelevantBits = (OFFSET_WIDTH * depth);
	uint64_t offset = virtualAddress >> irrelevantBits;
	PMread(frame * PAGE_SIZE + offset, &nextFrame);
	if(nextFrame == 0){
		nextFrame = (word_t)findFrame(frame, targetPage);
        if(depth == 1){
            PMrestore(nextFrame, targetPage);
        }else{
            initFrame(nextFrame);
        }
		PMwrite(frame * PAGE_SIZE + offset, nextFrame);
	}
	uint64_t updatedVirtualAddress = virtualAddress & ((1LL << irrelevantBits) - 1);
	return recTranslateAddress(nextFrame, updatedVirtualAddress, targetPage, depth -1);
}

uint64_t translateAddress(uint64_t virtualAddress){
    uint64_t targetPage = virtualAddress >> OFFSET_WIDTH;
	int noRootBits = (OFFSET_WIDTH * TABLES_DEPTH);
	uint64_t rootOffset = virtualAddress >> noRootBits;
	word_t firstFrame;
	PMread(ROOT_ADDR + rootOffset, &firstFrame);
	if(firstFrame == 0){
		firstFrame = (word_t)findFrame(ROOT_ADDR, targetPage);
		initFrame(firstFrame);
		PMwrite(ROOT_ADDR + rootOffset, firstFrame);
	}
	uint64_t currentVirtualAddress = virtualAddress & ((1LL << noRootBits) - 1);
    return recTranslateAddress(firstFrame, currentVirtualAddress, targetPage, TABLES_DEPTH - 1);
}

uint64_t findFrame(uint64_t parentFrame, uint64_t targetPage){
    BestFrameCandidates bfc;
    dfs(ROOT_ADDR, bfc, TABLES_DEPTH, parentFrame, targetPage, 0);

    if(candidates[EMPTY_TABLE_INDEX] != ROOT_ADDR) {
        return candidates[EMPTY_TABLE_INDEX];
    }else if(candidates[MAX_USED_FRAME_INDEX] + 1 < NUM_FRAMES){
        return candidates[MAX_USED_FRAME_INDEX] + 1;
    }else{
        assert(candidates[MAX_DISTANCE_FRAME] != 0);
        PMevict(candidates[MAX_DISTANCE_FRAME], candidates[MAX_DISTANCE_PAGE]);
        return candidates[MAX_DISTANCE_FRAME];
    }
}


void dfs(uint64_t node, BestFrameCandidates &candidates, uint64_t depth, uint64_t parentFrame, uint64_t targetPage, uint64_t pageIdx){
    candidates.maxFrameIndex = max( candidates.maxFrameIndex, node);
    pageIdx = pageIdx << OFFSET_WIDTH;
    if(candidates.emptyTableFrameIndex == ROOT_ADDR && parentFrame != node && isFrameEmptyTable(node) ) {
        candidates.emptyTableFrameIndex = node;
    }
    for(int i = 0; i < PAGE_SIZE; i++){
        word_t nextNode = ROOT_ADDR;
        PMread((node * PAGE_SIZE) + i, &nextNode);
        if(nextNode != ROOT_ADDR){
            if(depth == 1){
                //next node is a page. calculate distance and continue.
                uint64_t nextNodePageId = pageIdx + i;
                uint64_t currentDistance = getDistance(nextNodePageId, targetPage);
                if(candidates.maxDistance < currentDistance){
                    candidates.maxDistance = currentDistance;
                    candidates.maxDistancePageIndex = nextNodePageId;
                    candidates.maxDistanceFrameIndex = nextNode;
                    candidates.linkRemovalAddress = (node * PAGE_SIZE) + i;
                }
            }else{
                dfs(nextNode, candidates, depth - 1, parentFrame, targetPage, pageIdx + i);
            }
        }
     }
}

uint64_t getDistance(uint64_t pageIdx, uint64_t targetPage){
    uint64_t abs_p = max((int64_t)(pageIdx - targetPage) ,(int64_t)(targetPage - pageIdx));
    return min(NUM_PAGES - abs_p , abs_p);
}

bool isFrameEmptyTable(uint64_t node){
    for(int i = 0 ; i < PAGE_SIZE; i++){
        word_t value;
        PMread((node * PAGE_SIZE) + i, &value);
        if(value != 0) {
            return false;
        }
    }
    return true;
}
