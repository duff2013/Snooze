/*
 *  mcg.h
 *  Teensy3
 *
 */

#ifndef __MCG_H__
#define __MCG_H__
/********************************************************************/
#include "kinetis.h"
#include "kinetis_ext.h"
/********************************************************************/
/* MCG Modes */
typedef enum {
    BLPI = 1,
    FBI  = 2,
    FEI  = 3,
    FEE  = 4,
    FBE  = 5,
    BLPE = 6,
    PBE  = 7,
    PEE  = 8
} CLOCK_MODE;

/* CPU SPeeds */
typedef enum {
    TWO_MHZ                 = 2000000,
    FOUR_MHZ                = 4000000,
    EIGHT_MHZ               = 8000000,
    SIXTEEN_MHZ             = 16000000,
    TWENTY_FOUR_MHZ         = 24000000,
    FORTY_EIGHT_MHZ         = 48000000,
    SEVENTY_TWO_MHZ         = 72000000,
    NINETY_SIX_MHZ          = 96000000,
    HUNDRED_TWENTY_MHZ      = 120000000,
#if defined(__MK20DX256__)
    HUNDRED_FORTY_FOUR_MHZ  = 144000000,
    HUNDRED_SIXTY_EIGHT_MHZ = 168000000,
#endif
    _F_CPU                  = F_CPU
} SPEED;

//typedef struct {
    //extern uint8_t BLPI_BLPE[4] = {1,1,1,1};//{BLPI, FBI, FBE, BLPE};
//}mcg_transition_t;

typedef struct {
    SPEED cpu;
    CLOCK_MODE mode;
}mcg_mask_t;

//extern mcg_mask_t CLK_MODE;
/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    // function prototypes
    /* transistion into different modes */
    void pee_blpi( void );
    void blpi_pee( void );
    void pee_blpe( void );
    void blpe_pee( void );
    void pbe_pee ( void );
    void blpi_blpe( void );
    void blpe_blpi( void );
    CLOCK_MODE mcg_mode( void );
    void mcg_div( uint8_t cpu, uint8_t bus, uint8_t mem, uint32_t syst );
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __MCG_H__ */
