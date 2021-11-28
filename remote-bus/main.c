#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "etherbone.h"

uint32_t csr_read_simple(unsigned long addr);
void csr_write_simple(uint32_t val, unsigned long addr);
#define CSR_ACCESSORS_DEFINED

#include "generated/csr.h"

static struct eb_connection *eb;

uint32_t csr_read_simple(unsigned long addr) {
    return eb_read32(eb, addr);
}

void csr_write_simple(uint32_t val, unsigned long addr) {
    eb_write32(eb, val, addr);
}

int main(int argc, char **argv) {
    eb = eb_connect("127.0.0.1", "1234", 0);
    if (!eb) {
        fprintf(stderr, "Couldn't connect\n");
        exit(1);
    }

    // You can now access registers from csr.h.  E.g.:
    fprintf(stderr, "ctrl_scratch: %d\n", ctrl_scratch_read());
    return 0;
}
