#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "etherbone.h"
#include "generated/csr.h"

#include "rgb.h"
#include "i2c.h"
#include "dac.h"
#include "codec.h"

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
    fprintf(stderr, "ctrl_scratch: 0x%08x\n", ctrl_scratch_read());

    rgb_init();

    fprintf(stderr, "rgb init..\n");

    i2c_init();

    fprintf(stderr, "i2c init...\n");

    rgb_set(0xa0, 0x30, 0x60);

    fprintf(stderr, "i2c general call reset... ");
    int ret = i2c_general_call_reset();
    if (ret < 0) {
        fprintf(stderr, "failed!\n");
    } else {
        fprintf(stderr, "sent\n");
    }

    int rx, status;
    rx = i2c_read_txn(DAC_I2C_ADDR, DAC_ID_CMD, -1, &status);
    fprintf(stderr, "DAC: 0x%04x\n", rx);
    rx = i2c_read_txn(CODEC_I2C_ADDR, CODEC_ID_CMD1, CODEC_ID_CMD2, &status);
    fprintf(stderr, "CODEC: 0x%04x\n", rx);

    return 0;
}
