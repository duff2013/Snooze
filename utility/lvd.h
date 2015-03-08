/*******************************************************************************
 *  lvd.h
 *  Teensy 3.x/LC
 *
 * Purpose: Low Voltage Detect
 *
 *******************************************************************************/
#ifndef __LVD_H__
#define __LVD_H__
/*******************************************************************************/
typedef struct {
    float threshold = 0.00;
} lvd_mask_t;
/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    /*******************************************************************************
     *
     *       lvd_configure_pin_mask
     *
     *******************************************************************************/
    static inline
    void lvd_configure_pin_mask( float threshold, lvd_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void lvd_configure_pin_mask( float threshold, lvd_mask_t *mask ) {
        mask->threshold = threshold;
    }
    /*******************************************************************************
     *
     *       lvdISR
     *
     *******************************************************************************/
    static inline
    void lvdISR( void )
    __attribute__((always_inline, unused));
    
    static inline
    void lvdISR( void ) {
        
    }
    /*******************************************************************************
     *
     *       lvd_set
     *
     *******************************************************************************/
    static inline
    void lvd_set( lvd_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void lvd_set( lvd_mask_t *mask ) {
        
    }
    /*******************************************************************************
     *
     *       lvd_disable
     *
     *******************************************************************************/
    static inline
    void lvd_disable( lvd_mask_t *mask )
    __attribute__((always_inline, unused));
    
    static inline
    void lvd_disable( lvd_mask_t *mask ) {

    }
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __LVD_H__ */