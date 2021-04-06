
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <vector>

extern "C" {


// This macro allows us to prefix strings so that they are less likely to
// conflict with existing symbol names in the examined programs.
// e.g. TOLERATE(entry) yields ToLeRaToR_entry
#define TOLERATE(X) ToLeRaToR_##X

std::map<int64_t, int64_t> addressMap;

void
TOLERATE(helloworld)() {
  printf("==============================\n"
         "\tHello, World!\n"
         "==============================\n");
}

void
TOLERATE(goodbyeworld)() {
  printf("==============================\n"
         "\tGoodbye, World!\n"
         "==============================\n");
}

void TOLERATE(saveRange)(int64_t startAddress, int64_t elements) {
  addressMap[startAddress] = elements;
}

bool TOLERATE(loadValidate)(int64_t address, int64_t size) {
  auto it = addressMap.lower_bound(address);
  int64_t base = (*it).first;
  int64_t allocatedSize = (*it).second;
  if(base <= address && address+size <= base+allocatedSize) {
    return true;
  }
  printf("FOUND: Invalid read from memory\n");
  exit(0);
}

}
