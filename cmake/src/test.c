#include "dummy.h"

volatile unsigned int global_for_bss=0;
unsigned int global_for_data=0x42;

int main(void) {
    while (1) {
        global_for_data++;
        global_for_bss=global_for_data;
        dummy_fn();
    }
}
