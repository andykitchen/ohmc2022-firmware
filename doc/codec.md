Audio CODEC Driver
==================

[SGTL500 Datasheet Link](https://www.nxp.com/docs/en/data-sheet/SGTL5000.pdf)

The audio codec is a multi-purpose audio processing chip.
Most importantly it can turn digital samples into
an analog output that can drive headphones or a speaker
after amplification.

Unfortunately the datasheet for the SGTL5000 is complicated and
often confusing. The puspose of this document is to serve as
as starting point for someone tring to figure out how the
driver is actually working in case they need to make modifications,
this document is not stand alone and should be read together
with the codec driver C source and the SGTL5000 datasheet.

Initialisation
--------------

Just initialising the chip takes some doing because
of all the flexible options the chip supports.
There are many questions that need to be answered in the
data sheet in a kind of "choose your own adventure style",
Here is the initialisation sequence with the pertinent questions
and their answers (as far as I know) and the consequences:

1.  Is VDDD driven externally? Yes, at 3V3
    -   Internal regulator is not powered up
    -   Startup power supplies are turned off:
        CHIP_ANA_POWER bits 12 (LINREG_SIMPLE_POWERUP) and 13 (STARTUP_POWERUP) should be cleared
2.  Are VDDA and VDDIO less than 3V1? No, at 3V3
    -   Oscillator charge pump is not enabled.
        CHIP_CLK_TOP_CTRL bit 11 is left cleared (ENABLE_INT_OSC)
3.  Unconditionally set CHIP_ANA_POWER bit 11 (VDDC_CHRGPMP_POWERUP)
    -   (This should be revisited, because it may not need to be enabled
        given we are using I2S)
4.  Given VDDA and VDDIO are greater than 3V1.
    -   Configure the charge pump to use the VDDIO rail,
        CHIP_LINREG_CTRL set bits 5 (VDDC_ASSN_OVRD) and 6 (VDDC_MAN_ASSN)
5.  Ground/ADC/DAC reference voltage needs to be set to half of VDDA
    -   Target reference voltage 3V3 / 2 = 1V65
    -   Calculation is (V/2 - 800mV)/25mV = 0x22 but the
        field is only a 5 bit field so the max value is 0x1F
    -   CHIP_REF_CTRL bits 8:4 (VAG_VAL) set to 0x1F
6.  Set LINEOUT voltage reference to half of VDDIO
    -   Target reference voltage 3V3 / 2 = 1V65
    -   CHIP_LINE_OUT_CTRL bits 0:5  (LO_VAGCNTRL) set to 0x22 (1V65)
7.  Set LINEOUT bias current
    -   CHIP_LINE_OUT_CTRL bits 8:11 (OUT_CURRENT) set to 0x03 (0.36mA)
8.  ...