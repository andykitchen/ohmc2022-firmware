#ifndef ROCKLING_REG_H_
#define ROCKLING_REG_H_

#define BIT(n) (1<<n)

/* ----------------- */
/* Register addreses */
/* ----------------- */

#define CHIP_ID             0x0000
#define CHIP_ANA_ADC_CTRL   0x0020
#define CHIP_LINREG_CTRL    0x0026
#define CHIP_REF_CTRL       0x0028
#define CHIP_LINE_OUT_CTRL  0x002C
#define CHIP_ANA_POWER      0x0030
#define CHIP_CLK_TOP_CTRL   0x0034
#define CHIP_SHORT_CTRL     0x003C

/* ------------------------- */
/* CHIP_LINREG_CTRL register */
/* ------------------------- */

#define VDDC_ASSN_OVRD            BIT(5)
#define VDDC_MAN_ASSN             BIT(6)

/* ------------------------------------------------------- */
/* CHIP_ANA_POWER register, Datasheet page 42-43, table 33 */
/* ------------------------------------------------------- */

#define LINEOUT_POWERUP           BIT(0)  // 0:off, 1:on
#define ADC_POWERUP               BIT(1)  // 0:off, 1:on
#define CAPLESS_HEADPHONE_POWERUP BIT(2)  // 0:off, 1:on
#define DAC_POWERUP               BIT(3)  // 0:off, 1:on
#define HEADPHONE_POWERUP         BIT(4)  // 0:off, 1:on

// REFTOP_POWERUP can be off when IC is in sleep state to minimize analog power
#define REFTOP_POWERUP            BIT(5)  // Reference bias current: 1:on

// ADC_MONO mode is only useful when using the microphone input
#define ADC_MONO                  BIT(6)  // 0:mono (left only), 1:stereo

// HEADPHONE_POWERUP and/or LINEOUT_POWERUP should be set before clearing
// VAG_POWERUP and remain set for 200 to 400 ms after VAG_POWERUP cleared
#define VAG_POWERUP               BIT(7)  // Headphone and LINEOUT powerup ramp
#define VCOAMP_POWERUP            BIT(8)  // PLL VCO amplifier, 0:off, 1:on
#define LINREG_D_POWERUP          BIT(9)  // Primary VDDD linear regulator

// CHIP_PLL_CTRL register must be configured before setting PLL_POWERUP
// PLL_POWERUP must be set before CHIP_CLK_CTRL:MCLK_FREQ is programmed to 0x3
#define PLL_POWERUP               BIT(10) // 0:off, 1:on

// If both VDDA ond VDDIO are < 3.0 V, then VDDC_CHRGPMP_POWERUP must be cleared
// before analog blocks are powered up
// VDDC_CHRGPMP_POWERUP requires either PLL_POWERUP or
// CLK_TOP_CTRL:ENABLE_INT_OSC
#define VDDC_CHRGPMP_POWERUP      BIT(11) // Power up the VDDC charge pump block

// STARTUP_POWERUP can be cleared after reest, if VDDD externally powered
#define STARTUP_POWERUP           BIT(12) // 0:off, 1:on (default)

// STARTUP_POWERUP can be cleared after reest, if VDDD externally powered or
// the primary digital linear regulator is enabled with LINREG_D_POWERUP
#define LINREG_SIMPLE_POWERUP     BIT(13) // 0:off, 1:on (default)

// While DAC_POWERUP is set, allows the DAC to be left only mono to save power
#define DAC_MONO                  BIT(14) // 0:Mono (left), 1:Stereo (default)
//      RESERVED                  BIT(15)

/* -------------------------- */
/* CHIP_CLK_TOP_CTRL register */
/* -------------------------- */

#define ENABLE_INT_OSC        BIT(11)

#endif
