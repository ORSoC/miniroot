#include <inttypes.h>

enum {
	ADC_REG_INTERRUPT	= 0x000,
	ADC_REG_DRDYOUT_N_CNT	= 0x004,
	ADC_REG_PPS_CNT		= 0x008,
	ADC_REG_INTERRUPT_ENABLE = 0x010,
	ADC_REG_ADC		= 0x040,
	ADC_REG_GBS		= 0x080,
	ADC_REG_WIFI		= 0x0c0,
	ADC_REG_MUX		= 0x100,
};

/* ADC_REG_INTERRUPT & ADC_REG_INTERRUPT_ENABLE */
#define ADC_IRQ_GPS_PPS		(1 << 0)
#define ADC_IRQ_ADC_DRDYOUT_N	(1 << 1)
#define ADC_IRQ_WIFI_INTR	(1 << 2)
#define ADC_IRQ_COMPASS_INT1	(1 << 3)

/* ADC_REG_ADC */
#define ADC_ADC_ENABLE_SYNC_N_TEST		(1 << 0)
#define ADC_ADC_PPS_SYNC_N			(1 << 1)
#define ADC_ADC_SYNC_N_TEST			(1 << 2)
#define ADC_ADC_TEST_ENABLE_DRDYOUT_N_IRQ_TEST	(1 << 3)
#define ADC_ADC_TEST_DRDYOUT_N_IRQ_TEST		(1 << 4)
#define ADC_ADC_OVRFLW_N			(1 << 4)
#define ADC_ADC_PPS_EN				(1 << 5)

/* ADC_REG_GPS */
#define ADC_GPS_RESET_N				(1 << 0)
#define ADC_GPS_EXTINT0				(1 << 1)
#define ADC_GPS_ENABLE_PPS_IRQ_TEST		(1 << 2)
#define ADC_GPS_PPS_IRQ_TEST			(1 << 3)

/* ADC_REG_WIFI */
#define ADC_WIFI_RESET_N			(1 << 0)

/* ADC_REG_MUX */
#define ADC_MUX_CH1				(1 << 0)
#define ADC_MUX_CH2				(1 << 1)
#define ADC_MUX_CH3				(1 << 2)
#define ADC_MUX_CH4				(1 << 3)

