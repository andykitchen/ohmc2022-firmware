#include <generated/csr.h>

#include "i2c.h"

#include "util.h"

#define START 0x1
#define STOP  0x1
#define ACK   0x0
#define NACK  0x1

#define ENACK -1

static void nop_delay(void);
static int i2c_wait_tip(void);
static int i2c_tx1(int tx, int sta);
static int i2c_rx1(int *rx, int nack, int stop);


void NOINLINE i2c_init(void) {
	int flags;

	/*
	setup clock prescaler...
	clock prescaler formula is apparently (clk_htz / (5*i2c_htz)) - 1
	e.g. (12e6 / (5*100e3)) - 1 == 23
	*/
	i2c0_prescale_write(23);

	flags = 0;
	flags = i2c0_control_ien_replace(flags, 0);  /* disable interrupts from the core */
	flags = i2c0_control_en_replace(flags, 1);   /* enable the core */

	/* write control flags */
	i2c0_control_write(flags);
}

/* TODO handle loss of arbitration */
int NOINLINE i2c_read_char(int addr, int tx, int *rx) {
	int status;
	int d0, d1;

	addr &= 0x7f; /* mask off only 7 low bits of address */

	/* send address with START */
	status = i2c_tx1(addr<<1, START);

	/* if we got a NACK return an error */
	if (i2c0_status_rxack_extract(status))
		return ENACK;

	status = i2c_tx1(tx & 0xff, 0); /* send first data byte */

	if (i2c0_status_rxack_extract(status))
		return ENACK;

	/* send address with read mode bit and RESTART */
	status = i2c_tx1((addr<<1) + 1, START);

	if (i2c0_status_rxack_extract(status))
		return ENACK;

	/* receive with ACK */
	status = i2c_rx1(&d0, ACK, 0);

	/* receive with NACK and STOP */
	status = i2c_rx1(&d1, NACK, STOP);

	*rx = (d1<<8) + d0;
	return 0;
}

static void nop_delay(void) {
	for (int i = 23*8; i >= 0; i--)
		asm volatile ( "nop" );
}

static int i2c_wait_tip(void) {
	int status;

	do {
		nop_delay();
		status = i2c0_status_read();
	}
	while (i2c0_status_tip_extract(status));

	return status;
}

static int NOINLINE i2c_tx1(int tx, int sta) {
	int cmd;
	int status;

	i2c0_txr_write(tx & 0xff);                /* setup tx data register */

	cmd = 0;
	cmd = i2c0_command_wr_replace(cmd, 1);    /* write command bit */
	cmd = i2c0_command_sta_replace(cmd, sta); /* generate START condition? */
	i2c0_command_write(cmd);                  /* send address byte */

	/* wait until transfer in progress is clear */
	status = i2c_wait_tip();

	return status;
}

static int NOINLINE i2c_rx1(int *rx, int nack, int stop) {
	int cmd;
	int status;

	cmd = 0;
	cmd = i2c0_command_rd_replace(cmd, 1);     /* read command bit */
	cmd = i2c0_command_ack_replace(cmd, nack); /* generate ACK? */
	cmd = i2c0_command_sto_replace(cmd, stop); /* generate STOP condition? */
	i2c0_command_write(cmd);                   /* send address byte */

	status = i2c_wait_tip();

	*rx = i2c0_rxr_read();

	return status;
}
