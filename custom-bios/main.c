#include <generated/csr.h>

#include "util.h"
#include "rgb.h"
#include "i2c.h"

volatile int debug_status;

static const char *greeting = "Hello World!\n";

void counter_loop(void);


static void NOINLINE messible_puts(const char *p) {
	char c;

	while (c = *p++)
		messible_in_write(c);
}

int main(void) {
	int r, g, b;

	debug_status = 1;

	rgb_init();

	r = 0x00;
	g = 0x00;
	b = 0xff;
	rgb_set(r, g, b);

	messible_puts(greeting);

	i2c_init();
	debug_status = 2;

	int addr = 0x62;
	int rd_addr = 0xd6;
	int rx;
	i2c_read_char(addr, rd_addr, &rx);

	debug_status = rx;

	counter_loop();

	return 0;
}
