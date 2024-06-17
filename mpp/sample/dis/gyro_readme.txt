1. Before using the gyro sensor,Modify file pin_mux.c and pin_mux.h in directory smp/a55_linux/interdrv/sysconfig/
1.1 pin_mux.h:SPI_EN
#define SPI_EN 1

1.2 pin_mux.c:spi1_pin_mux
static void spi1_pin_mux(void)
{
    void *iocfg2_base = sys_config_get_reg_iocfg2();

    sys_writel(iocfg2_base + 0x0164, 0x1261); /* SPI1_SCLK */
    sys_writel(iocfg2_base + 0x0160, 0x1051); /* SPI1_SDO */
    sys_writel(iocfg2_base + 0x015C, 0x1251); /* SPI1_SDI */
    sys_writel(iocfg2_base + 0x0158, 0x1241); /* SPI1_CSN0 */
    sys_writel(iocfg2_base + 0x0154, 0x1241); /* SPI1_CSN1 */
}

1.3 pin_mux.c:pin_mux
void pin_mux(void)
{

    /* ... */

    spi_pin_mux(PIN_MUX_SPI_1);
}

Rebuild and get the sys_config.ko

2. update the file: smp/a55_linux/mpp/out/ko/load_ss928v100
2.1  insert_gyro
2.2  rmmod_gyro

3. modify the makefile parameter: smp/a55_linux/mpp/sample/Makefile.param.
    ################ open GYRO_DIS sample ########################
	GYRO_DIS ?= y

Rebuild the sample and get the sample_dis.
