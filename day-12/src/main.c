#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MB 1024 * 1024

void Munge8(void *data, uint32_t size) {
    uint8_t *data8 = (uint8_t *)data;
    uint8_t *data8End = data8 + size;

    while (data8 != data8End) {
        *data8++ = -*data8;
    }
};

void Munge16(void *data, uint32_t size) {
    uint16_t *data16 = (uint16_t *)data;
    uint16_t *data16End = data16 + (size >> 1); /* Divide size by 2. */
    uint8_t *data8 = (uint8_t *)data16End;
    uint8_t *data8End = data8 + (size & 0x00000001); /* Strip upper 31 bits. */

    while (data16 != data16End) {
        *data16++ = -*data16;
    }
    while (data8 != data8End) {
        *data8++ = -*data8;
    }
};

void Munge64(void *data, uint32_t size) {
    double *data64 = (double *)data;
    double *data64End = data64 + (size >> 3); /* Divide size by 8. */
    uint8_t *data8 = (uint8_t *)data64End;
    uint8_t *data8End = data8 + (size & 0x00000007); /* Strip upper 29 bits. */
    while (data64 != data64End) {
        *data64++ = -*data64;
    }
    while (data8 != data8End) {
        *data8++ = -*data8;
    }
};

int main() {

    uint32_t buffer_size = 10 * MB;
    int offset = 0;
    void *data = aligned_alloc(offset, buffer_size);

    clock_t begin = clock();
    Munge8(data, buffer_size);
    clock_t end = clock();
    printf("Elapsed: %f seconds for Munge8\n", (double)(end - begin) / CLOCKS_PER_SEC);

    clock_t begin1 = clock();
    Munge16(data, buffer_size);
    clock_t end1 = clock();
    printf("Elapsed: %f seconds for Munge16\n", (double)(end1 - begin1) / CLOCKS_PER_SEC);

    clock_t begin2 = clock();
    Munge64(data, buffer_size);
    clock_t end2 = clock();
    printf("Elapsed: %f seconds for Munge64\n", (double)(end2 - begin2) / CLOCKS_PER_SEC);
    return 0;
}
