#include <generated/csr.h>

#include "i2c.h"

#include "util.h"

#define START 0x1
#define STOP  0x1
#define ACK   0x0
#define NACK  0x1

#define ENACK -1

static void nop_delay(void);
static void i2c_wait_tip(int *status);
static void i2c_tx_char(int *status, int tx, int sta);
static int  i2c_rx_char(int *status, int nack, int stop);


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
int NOINLINE i2c_read_txn(int addr, int tx0, int tx1, int *status) {
	int rx0, rx1;

	/* send address in write mode with START */
	i2c_tx_char(status, addr<<1, START);

	/* if we got a NACK return an error */
	if (i2c0_status_rxack_extract(*status))
		return ENACK;

	i2c_tx_char(status, tx0, 0); /* send first data byte */

	if (i2c0_status_rxack_extract(*status))
		return ENACK;

	if (!(tx1 < 0)) { /* optionally second data byte if non-negative */
		i2c_tx_char(status, tx1, 0);

		if (i2c0_status_rxack_extract(*status))
			return ENACK;
	}

	/* send address in read mode and RESTART */
	i2c_tx_char(status, (addr<<1) + 1, START);

	if (i2c0_status_rxack_extract(*status))
		return ENACK;

	/* receive with ACK */
	rx0 = i2c_rx_char(status, ACK, 0);

	/* receive with NACK and STOP */
	rx1 = i2c_rx_char(status, NACK, STOP);

	/* the i2c devices we are dealing with send 16-bit values data big-endian */
	return (rx0<<8) + rx1;
}

static void nop_delay(void) {
	for (int i = 23*8; i >= 0; i--)
		asm volatile ( "nop" );
}

static void i2c_wait_tip(int *status) {
	int s;

	do {
		nop_delay();
		s = i2c0_status_read();
	}
	while (i2c0_status_tip_extract(s));

	*status = s;
}

static void i2c_tx_char(int *status, int tx, int sta) {
	int cmd;

	tx &= 0xff;  /* mask out only 8 low bits of tx data */

	i2c0_txr_write(tx);  /* setup tx data register */

	cmd = 0;
	cmd = i2c0_command_wr_replace(cmd, 1);    /* write command bit */
	cmd = i2c0_command_sta_replace(cmd, sta); /* generate START condition? */
	i2c0_command_write(cmd);                  /* send address byte */

	/* wait until transfer in progress is complete */
	i2c_wait_tip(status);
}

static int i2c_rx_char(int *status, int nack, int stop) {
	int cmd;

	cmd = 0;
	cmd = i2c0_command_rd_replace(cmd, 1);     /* read command bit */
	cmd = i2c0_command_ack_replace(cmd, nack); /* generate ACK? */
	cmd = i2c0_command_sto_replace(cmd, stop); /* generate STOP condition? */
	i2c0_command_write(cmd);                   /* send address byte */

	/* wait until transfer in progress is complete */
	i2c_wait_tip(status);

	/* return value in read register */
	return i2c0_rxr_read();
}
