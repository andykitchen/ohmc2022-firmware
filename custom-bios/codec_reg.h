#ifndef ROCKLING_REG_H_
#define ROCKLING_REG_H_

#define CHIP_ID             0x0000
#define CHIP_ANA_ADC_CTRL   0x0020
#define CHIP_LINREG_CTRL    0x0026
#define CHIP_REF_CTRL       0x0028
#define CHIP_LINE_OUT_CTRL  0x002C
#define CHIP_ANA_POWER      0x0030
#define CHIP_CLK_TOP_CTRL   0x0034
#define CHIP_SHORT_CTRL     0x003C

#define BIT(n) (1<<n)

/* CHIP_LINREG_CTRL bits */
#define VDDC_ASSN_OVRD        BIT(5)
#define VDDC_MAN_ASSN         BIT(6)

/* CHIP_ANA_POWER bits */
#define VDDC_CHRGPMP_POWERUP  BIT(11)
#define LINREG_SIMPLE_POWERUP BIT(12)
#define STARTUP_POWERUP       BIT(13)

/* CHIP_CLK_TOP_CTRL bits */
#define ENABLE_INT_OSC        BIT(11)

#endif
