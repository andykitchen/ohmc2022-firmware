#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "etherbone.h"
#include "generated/csr.h"

#include "rgb.h"
#include "i2c.h"
#include "dac.h"
#include "codec.h"
#include "codec_reg.h"

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

    dac_init();

    fprintf(stderr, "dac init...\n");

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
    rx = dac_read_txn(DAC_ID_REG);
    fprintf(stderr, "DAC addr: 0x%04x\n", rx);

    rx = dac_read_txn(DAC_STATUS_REG);
    fprintf(stderr, "DAC status: 0x%04x\n", rx);

    rx = dac_read_txn(0x00);
    fprintf(stderr, "DAC0: 0x%04x\n", rx);

    rx = dac_read_txn(0x01);
    fprintf(stderr, "DAC1: 0x%04x\n", rx);

    fprintf(stderr, "writing DAC0 register... ");
    rx = dac_write_txn(0x00, 0x006e);
    if (rx < 0) {
        fprintf(stderr, "failed!\n");
    } else {
        fprintf(stderr, "done\n");
    }

    fprintf(stderr, "writing DAC0 register... ");
    rx = dac_write_txn(0x00, 0x00ff);
    if (rx < 0) {
        fprintf(stderr, "failed!\n");
    } else {
        fprintf(stderr, "done\n");
    }

    rx = dac_read_txn(0x00);
    fprintf(stderr, "DAC0: 0x%04x\n", rx);

    rx = dac_read_txn(0x01);
    fprintf(stderr, "DAC1: 0x%04x\n", rx);

    rx = codec_read_txn(CHIP_ID);
    fprintf(stderr, "CODEC ID: 0x%04x\n", rx);

    rx = codec_read_txn(CHIP_ANA_ADC_CTRL);
    fprintf(stderr, "CHIP_ANA_ADC_CTRL: 0x%04x\n", rx);

    fprintf(stderr, "writing CODEC CHIP_ANA_ADC_CTRL register... ");
    rx = codec_write_txn(CHIP_ANA_ADC_CTRL, 0x0001);
    if (rx < 0) {
        fprintf(stderr, "failed!\n");
    } else {
        fprintf(stderr, "done\n");
    }

    rx = codec_read_txn(CHIP_ANA_ADC_CTRL);
    fprintf(stderr, "CHIP_ANA_ADC_CTRL: 0x%04x\n", rx);

    fprintf(stderr, "writing zero to CODEC CHIP_ANA_ADC_CTRL register... ");
    rx = codec_write_txn(CHIP_ANA_ADC_CTRL, 0x0000);
    if (rx < 0) {
        fprintf(stderr, "failed!\n");
    } else {
        fprintf(stderr, "done\n");
    }

    fprintf(stderr, "LED PWM seems to cause noise on OSC 1... turing LEDs off\n");
    rgb_set(0x00, 0x00, 0x00);

    for (int i = 0; i < 10; i++) {
        fprintf(stderr, "setting DAC voltage 1\n");
        dac_set_val(0x00, -1);

        fprintf(stderr, "sleeping 2 seconds\n");
        sleep(2);

        fprintf(stderr, "setting DAC voltage 2\n");
        dac_set_val(0xff, -1);

        fprintf(stderr, "sleeping 2 seconds\n");
        sleep(2);
    }

    return 0;
}
