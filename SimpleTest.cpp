#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>
#include <cassert>

int main(int argc, char **argv) {
    VMinitialize();
//    VMwrite(0,1);
//    int v;
//    VMread(0,&v);
//    printf("%d",v);
    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
        printf("writing to vm address %llu the value %llu\n", (5 * i * PAGE_SIZE), (long long int) i);
        VMwrite(5 * i * PAGE_SIZE, i);
    }
//    print_ram();
    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
        word_t value;
        VMread(5 * i * PAGE_SIZE, &value);
        printf("reading from %llu %d\n", (long long int) i, value);
        assert(uint64_t(value) == i);
    }
    printf("success\n");

    return 0;
}
