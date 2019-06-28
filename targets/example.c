#include <stdint.h>
#include <math.h>

double example2(float v3, double v4) {
    return v3 * v4;
}


uint32_t example(uint32_t v0, uint64_t v1, float v2, double v3) {

    uint64_t product0 = v0 * v1;

    uint32_t product1 = v0 * 128;

    double product3 = example2(v2, v3);

    return (uint64_t)floor(product3) + product0 + product1;
    
}


volatile uint64_t test_r0;
volatile uint32_t test_v0;
volatile uint64_t test_v1;
volatile float    test_v2;
volatile double   test_v3;

void main(void) {
    test_r0 = example(test_v0,
                      test_v1,
                      test_v2,
                      test_v3);
    while (1);
}
