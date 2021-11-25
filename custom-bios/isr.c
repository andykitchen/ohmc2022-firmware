// This file is Copyright (c) 2020 Florent Kermarrec <florent@enjoy-digital.fr>
// License: BSD

#include <generated/csr.h>
#include <generated/soc.h>
#include <irq.h>

void isr(void);

#ifdef CONFIG_CPU_HAS_INTERRUPT

void isr(void)
{
	__attribute__((unused)) unsigned int irqs;

	irqs = irq_pending() & irq_getmask();
}

#else

/* This is really only triggered on a trap,
   probably an illegal instruction or bus error? */
void isr(void)
{
	/* give some visual feedback by trying to set the LEDs to RED */
	rgb_addr_write(0x01);
	rgb_dat_write(0xff);
	rgb_addr_write(0x02);
	rgb_dat_write(0x00);
	rgb_addr_write(0x03);
	rgb_dat_write(0x00);

	/* loop forever */
	while (1) { }
}

#endif
