/***********************************************************************************
 *  SnoozeAudio.h
 *  Teensy 3.6
 *
 * Purpose: Audio Driver
 *
 **********************************************************************************/
#if defined(__MK66FX1M0__)

#include "SnoozeAudio.h"
/***********************************************************************************/
#define PDB_CONFIG (PDB_SC_TRGSEL( 15 ) | PDB_SC_PDBEN | PDB_SC_CONT | PDB_SC_PDBIE | PDB_SC_DMAEN)
/*******************************************************************************
 *  disable audio features
 *******************************************************************************/
void SnoozeAudio::disableDriver( uint8_t type ) {
    if ( audioADC || audioDAC ) {
        PDB0_SC = 0;
    }
}

/*******************************************************************************
 *  enable audio features
 *******************************************************************************/
void SnoozeAudio::enableDriver( uint8_t type ) {
    if ( audioADC || audioDAC ) {
        PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
        PDB0_SC = PDB_CONFIG | PDB_SC_SWTRIG;
    }
}
/*******************************************************************************
 *  adc uses pdb
 *******************************************************************************/
void SnoozeAudio::usingADC( uint8_t ADC_pin ) {
    audioADC = true;
}
/*******************************************************************************
 *  dac uses pdb
 *******************************************************************************/
void SnoozeAudio::usingDAC( uint8_t DAC_pin ) {
    audioDAC = true;
}
#endif /* __MK66FX1M0__ */
