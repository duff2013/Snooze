/*
 *  module.h
 *  Teensy 3.x/LC
 *
 */
#ifndef __MODULE_H__
#define __MODULE_H__
#include "kinetis.h"
#include "kinetis_ext.h"
#include "bitband.h"
/********************************************************************/
#define SIM_SCGC4_I2C0_BIT      6  //0x06
#define SIM_SCGC4_UART0_BIT     10 //0x0A
#define SIM_SCGC4_UART1_BIT     11 //0x0B
#define SIM_SCGC4_UART2_BIT     12 //0x0C
#define SIM_SCGC4_USBOTG_BIT    18 //0x12
#define SIM_SCGC4_CMP_BIT       19 //0x13
#define SIM_SCGC4_VREF_BIT      20 //0x14

#define SIM_SCGC5_LPTIMER_BIT   0  //0x00
#define SIM_SCGC5_TSI_BIT       5  //0x05

#define SIM_SCGC6_SPI0_BIT      12 //0x0C
#define SIM_SCGC6_I2S_BIT       15 //0x0F
#define SIM_SCGC6_PDB_BIT       22 //0x16
#define SIM_SCGC6_ADC0_BIT      27 //0x1B
#define SIM_SCGC6_RTC_BIT       29 //0x1D
//-------------------------------------
typedef enum {
    CMP_OFF      = SIM_SCGC4_CMP,
    I2C0_OFF     = SIM_SCGC4_I2C0,
    I2C1_OFF     = SIM_SCGC4_I2C1,
    UART2_OFF    = SIM_SCGC4_UART0,
    UART1_OFF    = SIM_SCGC4_UART1,
    UART3_OFF    = SIM_SCGC4_UART2,
    USB_OFF      = SIM_SCGC4_USBOTG,
    VREF_OFF     = SIM_SCGC4_VREF,
} SCGC4_OFF_t;

typedef enum {
    CMP_ON      = SIM_SCGC4_CMP,
    I2C0_ON     = SIM_SCGC4_I2C0,
    I2C1_ON     = SIM_SCGC4_I2C1,
    UART2_ON    = SIM_SCGC4_UART0,
    UART1_ON    = SIM_SCGC4_UART1,
    UART3_ON    = SIM_SCGC4_UART2,
    USB_ON      = SIM_SCGC4_USBOTG,
    VREF_ON     = SIM_SCGC4_VREF,
} SCGC4_ON_t;
//-------------------------------------
typedef enum {
    LPTMR_OFF    = SIM_SCGC5_LPTIMER,
    TSI_OFF      = SIM_SCGC5_TSI,
} SCGC5_OFF_t;

typedef enum {
    LPTMR_ON    = SIM_SCGC5_LPTIMER,
    TSI_ON      = SIM_SCGC5_TSI,
} SCGC5_ON_t;
//-------------------------------------
typedef enum {
    ADC_OFF      = SIM_SCGC6_ADC0,
    I2S_OFF      = SIM_SCGC6_I2S,
    PDB_OFF      = SIM_SCGC6_PDB,
#if defined(__MK20DX128__) || defined(__MK20DX256__)
    SPI0_OFF     = SIM_SCGC6_SPI0,
    SPI1_OFF     = SIM_SCGC6_SPI1,
#elif defined(__MKL26Z64__)
    
#endif
    RTC_OFF      = SIM_SCGC6_RTC,
} SCGC6_OFF_t;

typedef enum {
    ADC_ON      = SIM_SCGC6_ADC0,
    I2S_ON      = SIM_SCGC6_I2S,
    PDB_ON      = SIM_SCGC6_PDB,
#if defined(__MK20DX128__) || defined(__MK20DX256__)
    SPI0_ON     = SIM_SCGC6_SPI0,
    SPI1_ON     = SIM_SCGC6_SPI1,
#elif defined(__MKL26Z64__)
    
#endif
    RTC_ON      = SIM_SCGC6_RTC,
} SCGC6_ON_t;
//-------------------------------------

typedef struct {
    //uint32_t modules;
    uint32_t SCGC4;
    uint32_t SCGC5;
    uint32_t SCGC6;
} peripheral_mask_t;
//-------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       rtc_configure_alarm_mask
     *
     *******************************************************************************/
    static inline void peripheral_configure_scgc4_mask( uint32_t unit, peripheral_mask_t *mask ) __attribute__((always_inline, unused));
    static inline void peripheral_configure_scgc4_mask( uint32_t unit, peripheral_mask_t *mask ) {
        mask->SCGC4 |= unit;
    }
    // ---------------------------------------------------------------------------------
    static inline void peripheral_configure_scgc5_mask( uint32_t unit, peripheral_mask_t *mask ) __attribute__((always_inline, unused));
    static inline void peripheral_configure_scgc5_mask( uint32_t unit, peripheral_mask_t *mask ) {
        mask->SCGC5 |= unit;
    }
    // ---------------------------------------------------------------------------------
    static inline void peripheral_configure_scgc6_mask( uint32_t unit, peripheral_mask_t *mask ) __attribute__((always_inline, unused));
    static inline void peripheral_configure_scgc6_mask( uint32_t unit, peripheral_mask_t *mask ) {
        mask->SCGC6 |= unit;
    }
    /*******************************************************************************
     *
     *       peripheral_set
     *
     *******************************************************************************/
    static inline void peripheral_set( peripheral_mask_t *mask ) __attribute__((always_inline, unused));
    static inline void peripheral_set( peripheral_mask_t *mask ) {
        SIM_SCGC4 |= mask->SCGC4;
        SIM_SCGC5 |= mask->SCGC5;
        SIM_SCGC6 |= mask->SCGC6;
    }
    /*******************************************************************************
     *
     *       peripheral_stop
     *
     *******************************************************************************/
    static inline void peripheral_disable( peripheral_mask_t *mask ) __attribute__((always_inline, unused));
    static inline void peripheral_disable( peripheral_mask_t *mask ) {
        SIM_SCGC4 &= ~mask->SCGC4;
        SIM_SCGC5 &= ~mask->SCGC5;
        SIM_SCGC6 &= ~mask->SCGC6;
    }
    /*******************************************************************************
     *
     *       adcEnable
     *
     *******************************************************************************/
    static inline void adcEnable( void ) __attribute__((always_inline, unused));
    static inline void adcEnable( void ) {
        if ((SIM_SCGC6 & SIM_SCGC6_ADC0) == 0x00) {
        //if ( (BITBAND_U32(SIM_SCGC6, SIM_SCGC6_ADC0_BIT) ) == 0x00 ) {
            //BITBAND_U32( SIM_SCGC6, SIM_SCGC6_ADC0_BIT ) = 0x01;
            SIM_SCGC6 |= SIM_SCGC6_ADC0;// enable ADC clock
            ADC0_SC1A |= ADC_SC1_ADCH( 0x05 );// enable ADC
        }
    }
    /*******************************************************************************
     *
     *       adcDisable
     *
     *******************************************************************************/
    static inline void adcDisable( void ) __attribute__((always_inline, unused));
    static inline void adcDisable( void ) {
        if (SIM_SCGC6 & SIM_SCGC6_ADC0) {
        //if ( (BITBAND_U32( SIM_SCGC6, SIM_SCGC6_ADC0_BIT ) ) == 0x01 ) {
            ADC0_SC1A |= ADC_SC1_ADCH( 0x1F );// disable ADC
            //BITBAND_U32( SIM_SCGC6, SIM_SCGC6_ADC0_BIT ) = 0x00;
            SIM_SCGC6 &= ~SIM_SCGC6_ADC0;// disable ADC clock
        }
    }
    /*******************************************************************************
     *
     *       cmp0Enable
     *
     *******************************************************************************/
    static inline void cmp0Enable( void ) __attribute__((always_inline, unused));
    static inline void cmp0Enable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_CMP) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_CMP;// enable CMP0 clock
            CMP0_CR1 |= CMP_CR1_EN_MASK;// enable CMP0
        }else if ((CMP0_CR1 & CMP_CR1_EN_MASK) == 0x00) CMP0_CR1 |= CMP_CR1_EN_MASK;
    }
    /*******************************************************************************
     *
     *       cmp0Disable
     *
     *******************************************************************************/
    static inline void cmp0Disable( void ) __attribute__((always_inline, unused));
    static inline void cmp0Disable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_CMP) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT)) == 0x01) {
            CMP0_CR1 &= ~CMP_CR1_EN_MASK;// disable CMP
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_CMP;// disable CMP0 clock
        }
    }
    /*******************************************************************************
     *
     *       cmp1Enable
     *
     *******************************************************************************/
    static inline void cmp1Enable( void ) __attribute__((always_inline, unused));
    static inline void cmp1Enable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_CMP) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_CMP;// enable CMP1 clock
            CMP1_CR1 |= CMP_CR1_EN_MASK;// enable CMP1
        }else if ((CMP1_CR1 & CMP_CR1_EN_MASK) == 0x00) CMP1_CR1 |= CMP_CR1_EN_MASK;
    }
    /*******************************************************************************
     *
     *       cmp1Disable
     *
     *******************************************************************************/
    static inline void cmp1Disable( void ) __attribute__((always_inline, unused));
    static inline void cmp1Disable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_CMP) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT)) == 0x01) {
            CMP1_CR1 &= ~CMP_CR1_EN_MASK;// disable CMP1
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_CMP_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_CMP;// disable CMP1 clock
        }
    }
    /*******************************************************************************
     *
     *       dmaEnable
     *
     *******************************************************************************/
    static inline void dmaEnable( void ) __attribute__((always_inline, unused));
    static inline void dmaEnable( void ) {
        
    }
    /*******************************************************************************
     *
     *       dmaDisable
     *
     *******************************************************************************/
    static inline void dmaDisable( void ) __attribute__((always_inline, unused));
    static inline void dmaDisable( void ) {
        
    }
    /*******************************************************************************
     *
     *       i2cEnable
     *
     *******************************************************************************/
    static inline void i2cEnable( void ) __attribute__((always_inline, unused));
    static inline void i2cEnable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_I2C0) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_I2C0_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_I2C0_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_I2C0;// enable I2C clock
            I2C0_C1 |= I2C_C1_IICEN;// enable I2C
        }
    }
    /*******************************************************************************
     *
     *       i2cDisable
     *
     *******************************************************************************/
    static inline void i2cDisable( void ) __attribute__((always_inline, unused));
    static inline void i2cDisable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_I2C0) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_I2C0_BIT)) == 0x01) {
            I2C0_C1 &= ~I2C_C1_IICEN;// disable I2C
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_I2C0_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_I2C0;// disable I2C clock
        }
    }
    /*******************************************************************************
     *
     *       i2sEnable
     *
     *******************************************************************************/
    static inline void i2sEnable( void ) __attribute__((always_inline, unused));
    static inline void i2sEnable( void ) {
        if ((SIM_SCGC6 & SIM_SCGC6_I2S) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_I2S_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_I2S_BIT) = 0x01;
            SIM_SCGC6 |= SIM_SCGC6_I2S;// enable I2S clock
            I2S0_TCSR |= I2S_TCSR_TE_MASK;// enable I2S
        }
    }
    /*******************************************************************************
     *
     *       i2sDisable
     *
     *******************************************************************************/
    static inline void i2sDisable( void ) __attribute__((always_inline, unused));
    static inline void i2sDisable( void ) {
        if (SIM_SCGC6 & SIM_SCGC6_I2S) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_I2S_BIT)) == 0x01) {
            I2S0_TCSR &= ~I2S_TCSR_TE_MASK;// disable I2S
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_I2S_BIT) = 0x00;
            SIM_SCGC6 &= ~SIM_SCGC6_I2S;// disable I2S clock
        }
    }
    /*******************************************************************************
     *
     *       lptmrEnable
     *
     *******************************************************************************/
    static inline void lptmrEnable( void ) __attribute__((always_inline, unused));
    static inline void lptmrEnable( void ) {
        if (!(SIM_SCGC5 & SIM_SCGC5_LPTIMER) || !(LPTMR0_CSR & LPTMR_CSR_TEN_MASK)) {
        //if ((BITBAND_U32(SIM_SCGC5, SIM_SCGC5_LPTIMER_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC5, SIM_SCGC5_LPTIMER_BIT) = 0x01;
            SIM_SCGC5 |= SIM_SCGC5_LPTIMER;// enable LPTMR clock
            LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;// enable LPTMR
        }
    }
    /*******************************************************************************
     *
     *       lptmrDisable
     *
     *******************************************************************************/
    static inline void lptmrDisable( void ) __attribute__((always_inline, unused));
    static inline void lptmrDisable( void ) {
        if (SIM_SCGC5 & SIM_SCGC5_LPTIMER) {
        //if ((BITBAND_U32(SIM_SCGC5, SIM_SCGC5_LPTIMER_BIT)) == 0x01) {
            //LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;
            LPTMR0_CSR &= LPTMR_CSR_TEN_MASK;// disable LPTMR
            //BITBAND_U32(SIM_SCGC5, SIM_SCGC5_LPTIMER_BIT) = 0x00;
            SIM_SCGC5 &= ~SIM_SCGC5_LPTIMER;// disable LPTIMER clock
        }
    }
    /*******************************************************************************
     *
     *       pdbEnable
     *
     *******************************************************************************/
    static inline void pdbEnable( void ) __attribute__((always_inline, unused));
    static inline void pdbEnable( void ) {
        if ((SIM_SCGC6 & SIM_SCGC6_PDB) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_PDB_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_PDB_BIT) = 0x01;
            SIM_SCGC6 |= SIM_SCGC6_PDB;// enable PDB clock
            PDB0_SC |= PDB_SC_PDBEN;// enable PDB
        }
    }
    /*******************************************************************************
     *
     *       pdbDisable
     *
     *******************************************************************************/
    static inline void pdbDisable( void ) __attribute__((always_inline, unused));
    static inline void pdbDisable( void ) {
        if (SIM_SCGC6 & SIM_SCGC6_PDB) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_PDB_BIT)) == 0x01) {
            PDB0_SC &= ~PDB_SC_PDBEN;// disable PDB
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_PDB_BIT) = 0x00;
            SIM_SCGC6 &= ~SIM_SCGC6_PDB;// disable PDB clock
        }
    }
    /*******************************************************************************
     *
     *       spiEnable
     *
     *******************************************************************************/
    static inline void spiEnable( void ) __attribute__((always_inline, unused));
    static inline void spiEnable( void ) {
#ifdef KINETISK
        if ((SIM_SCGC6 & SIM_SCGC6_SPI0) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_SPI0_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_SPI0_BIT) = 0x01;
            SIM_SCGC6 |= SIM_SCGC6_SPI0;// enable SPI clock
            SPI0_MCR |= SPI_MCR_MDIS;// enable SPI
        }
#endif
    }
    /*******************************************************************************
     *
     *       spiDisable
     *
     *******************************************************************************/
    static inline void spiDisable( void ) __attribute__((always_inline, unused));
    static inline void spiDisable( void ) {
#ifdef KINETISK
        if (SIM_SCGC6 & SIM_SCGC6_SPI0) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_SPI0_BIT)) == 0x01) {
            SPI0_MCR &= ~SPI_MCR_MDIS;// disable SPI
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_SPI0_BIT) = 0x00;
            SIM_SCGC6 &= ~SIM_SCGC6_SPI0;// disable SPI clock
        }
#endif
    }
    /*******************************************************************************
     *
     *       rtcEnable
     *
     *******************************************************************************/
    static inline void rtcEnable( void ) __attribute__((always_inline, unused));
    static inline void rtcEnable( void ) {
        if ((SIM_SCGC6 & SIM_SCGC6_RTC) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_RTC_BIT)) == 0x00) {
            SIM_SCGC6 |= SIM_SCGC6_RTC;// enable RTC clock
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_RTC_BIT) = 0x01;
            RTC_CR |= RTC_CR_OSCE;// enable RTC
        }
    }
    /*******************************************************************************
     *
     *       rtcDisable
     *
     *******************************************************************************/
    static inline void rtcDisable( void ) __attribute__((always_inline, unused));
    static inline void rtcDisable( void ) {
        if (SIM_SCGC6 & SIM_SCGC6_RTC) {
        //if ((BITBAND_U32(SIM_SCGC6, SIM_SCGC6_RTC_BIT)) == 0x01) {
            RTC_CR &= ~RTC_CR_OSCE;// disable RTC
            //BITBAND_U32(SIM_SCGC6, SIM_SCGC6_RTC_BIT) = 0x00;
            SIM_SCGC6 &= ~SIM_SCGC6_RTC;// disable RTC clock
        }
    }
    /*******************************************************************************
     *
     *       tsiEnable
     *
     *******************************************************************************/
    static inline void tsiEnable( void ) __attribute__((always_inline, unused));
    static inline void tsiEnable( void ) {
        if ((SIM_SCGC5 & SIM_SCGC5_TSI) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC5, SIM_SCGC5_TSI_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC5, SIM_SCGC5_TSI_BIT) = 0x01;
            SIM_SCGC5 |= SIM_SCGC5_TSI;// enable TSI clock
            TSI0_GENCS |= TSI_GENCS_TSIEN;// enable TSI
        }
    }
    /*******************************************************************************
     *
     *       tsiDisable
     *
     *******************************************************************************/
    static inline void tsiDisable( void ) __attribute__((always_inline, unused));
    static inline void tsiDisable( void ) {
        if (SIM_SCGC5 & SIM_SCGC5_TSI) {
        //if ((BITBAND_U32(SIM_SCGC5, SIM_SCGC5_TSI_BIT)) == 0x01) {
            TSI0_GENCS &= ~TSI_GENCS_TSIEN;// disable TSI
            //BITBAND_U32(SIM_SCGC5, SIM_SCGC5_TSI_BIT) = 0x00;
            SIM_SCGC5 &= ~SIM_SCGC5_TSI;// disable TSI clock
        }
    }
    /*******************************************************************************
     *
     *       uart1Enable
     *
     *******************************************************************************/
    static inline void uart1Enable( void ) __attribute__((always_inline, unused));
    static inline void uart1Enable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_UART0) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART0_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART0_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_UART0;// enable UART1 clock
            UART0_C2 |= UART_C2_TE;// enable TX
            UART0_C2 |= UART_C2_RE;// enable RX
        }
    }
    /*******************************************************************************
     *
     *       uart1Disable
     *
     *******************************************************************************/
    static inline void uart1Disable( void ) __attribute__((always_inline, unused));
    static inline void uart1Disable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_UART0) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART0_BIT)) == 0x01) {
            UART0_C2 &= ~UART_C2_TE;// disable UART1 TX
            UART0_C2 &= ~UART_C2_RE;// disable UART1 RX
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART0_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_UART0;// disable UART1 clock
        }
    }
    /*******************************************************************************
     *
     *       uart2Enable
     *
     *******************************************************************************/
    static inline void uart2Enable( void ) __attribute__((always_inline, unused));
    static inline void uart2Enable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_UART1) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART1_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART1_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_UART1;// enable UART2 clock
            UART1_C2 |= UART_C2_TE;// enable TX
            UART1_C2 |= UART_C2_RE;// enable RX
        }
    }
    /*******************************************************************************
     *
     *       uart2Disable
     *
     *******************************************************************************/
    static inline void uart2Disable( void ) __attribute__((always_inline, unused));
    static inline void uart2Disable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_UART1) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART1_BIT)) == 0x01) {
            UART1_C2 &= ~UART_C2_TE;// disable UART2 TX
            UART1_C2 &= ~UART_C2_RE;// disable  UART2 RX
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART1_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_UART1;// disable UART2 clock
        }
    }
    /*******************************************************************************
     *
     *       uart3Enable
     *
     *******************************************************************************/
    static inline void uart3Enable( void ) __attribute__((always_inline, unused));
    static inline void uart3Enable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_UART2) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART2_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART2_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_UART2;// enable UART3 clock
            UART2_C2 |= UART_C2_TE;// enable TX
            UART2_C2 |= UART_C2_RE;// enable RX
        }
    }
    /*******************************************************************************
     *
     *       uart3Disable
     *
     *******************************************************************************/
    static inline void uart3Disable( void ) __attribute__((always_inline, unused));
    static inline void uart3Disable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_UART2) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART2_BIT)) == 0x01) {
            UART2_C2 &= ~UART_C2_TE;// disable UART3 TX
            UART2_C2 &= ~UART_C2_RE;// disable  UART3 RX
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_UART2_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_UART2;// disable UART3 clock
        }
    }
    /*******************************************************************************
     *
     *       usbEnable
     *
     *******************************************************************************/
    static inline void usbEnable( void ) __attribute__((always_inline, unused));
    static inline void usbEnable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_USBOTG) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_USBOTG_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_USBOTG_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_USBOTG;// enable USB clock
            USB0_USBCTRL &= ~USB_USBCTRL_SUSP;// suspended State
            USB0_CTL |= USB_CTL_USBENSOFEN;// enable USB
            while ((USB0_CTL & USB_CTL_USBENSOFEN) != 0x01) ;
            //USB0_USBTRC0 |= 0x40; // undocumented bit
        }
    }
    /*******************************************************************************
     *
     *       usbDisable
     *
     *******************************************************************************/
    static inline void usbDisable( void ) __attribute__((always_inline, unused));
    static inline void usbDisable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_USBOTG) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_USBOTG_BIT)) == 0x01) {
            USB0_USBCTRL |= USB_USBCTRL_SUSP;// suspended State
            USB0_CTL &= ~USB_CTL_USBENSOFEN;// disable USB
            while ((USB0_CTL & USB_CTL_USBENSOFEN) != 0x00) ;
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_USBOTG_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_USBOTG;// disable USB clock
        }
    }
    /*******************************************************************************
     *
     *       vrefEnable
     *
     *******************************************************************************/
    static inline void vrefEnable( void ) __attribute__((always_inline, unused));
    static inline void vrefEnable( void ) {
        if ((SIM_SCGC4 & SIM_SCGC4_VREF) == 0x00) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_VREF_BIT)) == 0x00) {
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_VREF_BIT) = 0x01;
            SIM_SCGC4 |= SIM_SCGC4_VREF;// enable VREF clock
            VREF_SC |= VREF_SC_VREFEN_MASK;// enable VREF
        }
    }
    /*******************************************************************************
     *
     *       vrefDisable
     *
     *******************************************************************************/
    static inline void vrefDisable( void ) __attribute__((always_inline, unused));
    static inline void vrefDisable( void ) {
        if (SIM_SCGC4 & SIM_SCGC4_VREF) {
        //if ((BITBAND_U32(SIM_SCGC4, SIM_SCGC4_VREF_BIT)) == 0x01) {
            VREF_SC &= ~VREF_SC_VREFEN_MASK;// disable VREF
            //BITBAND_U32(SIM_SCGC4, SIM_SCGC4_VREF_BIT) = 0x00;
            SIM_SCGC4 &= ~SIM_SCGC4_VREF;// disable VREF clock
        }
    }
    
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* defined(__MODULE_H__) */
