/***********************************************************************************
 * Low Power Library for Teensy LC/3.x
 * Copyright (c) 2016, Colin Duffy https://github.com/duff2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ************************************************************************************
 *  SnoozeBlock.h
 *  Teensy 3.x/LC
 *
 * Purpose: Connects Driver Classes to a SnoozeBlock. Drivers inherit from SnoozeBlock
 *          which SnoozeBlock will create a linked list of Driver Classes.
 *
 ***********************************************************************************/
#ifndef SnoozeBlock_h
#define SnoozeBlock_h

#include "Arduino.h"
#include "utility/clocks.h"
#include "utility/sleep.h"
#include "utility/wake.h"
/***********************************************************************************/
#define COMPARE_WAKE    34
#define ALARM_WAKE      35
#define TIMER_WAKE      36
#define TOUCH_WAKE      37
/***********************************************************************************
 *  REDUCED_CPU_BLOCK - Configures the Teensy to run at 2MHz inside this block
 *
 *  @param SNOOZE_BLOCK - Connected drivers
 *
 *  @return none
 ***********************************************************************************/
#define SNOOZE_BLOCK SnoozeBlock &configuration
#define TYPE uint8_t
#define REDUCED_CPU_BLOCK( SNOOZE_BLOCK )   \
for ( TYPE __ToDo = SNOOZE_BLOCK.set_runlp( SNOOZE_BLOCK );  __ToDo;  __ToDo = SNOOZE_BLOCK.set_run( SNOOZE_BLOCK ) )
/***********************************************************************************/
typedef enum {
    TSI          = 3,
    CMP          = 4,
} PIN_TYPE;

/***********************************************************************************
 *  Deep Sleep Modes
***********************************************************************************/
typedef enum {
    RUN,
    RUN_LP,
    WAIT,
    VLPW,
    STOP,
    VLPS,
    LLS,
    VLLS3,
    VLLS2,
    VLLS1,
    VLLS0
} SLEEP_MODE;
/***********************************************************************************
 *  Class SnoozeBlock - Connects low power drivers to Snooze.
 ***********************************************************************************/
class SnoozeBlock {
private:
    SnoozeBlock *next_block[8];
    static uint8_t global_block_count;
    int8_t local_block;
    /***********************************************************************************
     *  Driver list to be called after sleep
     *
     *  @param p   Call SnoozeBlock virtual functions
     *  @param idx Array index of the SnoozeBlock list
     ***********************************************************************************/
    void reverseList( SnoozeBlock *p, uint8_t idx ) {
        if ( p != NULL ) {
            reverseList(p->next_block[idx], idx );
            if ( p->isUsed ) {
                p->disableDriver( );
            }
        }
    }
    
    /***********************************************************************************
     *  Driver list to be called before sleep
     *
     *  @param p   Call SnoozeBlock virtual functions
     *  @param idx Array index of the SnoozeBlock
     ***********************************************************************************/
    void forwardList( SnoozeBlock *p, uint8_t idx ) {
        for ( ; p; p = p->next_block[idx] ) {
            if ( p->isUsed ) {
                p->enableDriver( );
            }
        }
    }
protected:
    /***********************************************************************************
     *  Fired after waking from LLS-VLLS sleep modes
     ***********************************************************************************/
    static void wakeupIsr( void ) {
        
        llwuMask.llwuFlag = llwu_clear_flags( );
        pbe_pee( );
        SnoozeBlock *p = SnoozeBlock::root_block[current_block];
        for ( ; p; p = p->next_block[current_block] ) {
            p->clearIsrFlags( );
        }
        
    }
    
    static SnoozeBlock *root_block[8];
    static SnoozeBlock *root_class_address[8];
    static uint8_t current_block;
public:
    /***********************************************************************************
     *  Constructor - Recursively connects drivers to this SnoozeBlock using a linked list
     *
     *  @param head Driver
     *  @param tail Driver
     *
     *  @return nothing
     ***********************************************************************************/
    template<class ...Tail>
    SnoozeBlock ( SnoozeBlock &head, Tail&... tail )  : local_block( -1 ), isUsed( false ), isDriver( false ) {
        
        if ( mode < VLLS3 ) mode = RUN;
        // number of drivers connected to this Snooze Block
        int i = sizeof...( tail );
        // check for duplicate Drivers
        SnoozeBlock *p = SnoozeBlock::root_block[global_block_count];
        for ( ; p; p = p->next_block[global_block_count] ) {
            if ( p == &head ) {
                // last driver that is a duplicate, increment global block count
                if ( i <= 0 ) {
                    global_block_count++;
                    return;
                }
                // get next driver
                SnoozeBlock( tail... );
                return;
            }
        }
        // update linked list
        // set the root block
        if ( root_block[global_block_count] == NULL ) {
            // update registers on first driver installed only once
            if ( global_block_count == 0 ) {
                SIM_SOPT1CFG |= SIM_SOPT1CFG_USSWE;
                SIM_SOPT1 &= ~SIM_SOPT1_USBSSTBY;
                attachInterruptVector( IRQ_LLWU, wakeupIsr );
                NVIC_ENABLE_IRQ( IRQ_LLWU );
            }
            root_class_address[global_block_count] = this;
            root_block[global_block_count] = &head;
        } else {
            p = root_block[global_block_count];
            for ( ; p->next_block[global_block_count]; p = p->next_block[global_block_count] );
            p->next_block[global_block_count] = &head;
        }
        
        next_block[global_block_count] = NULL;
        local_block = global_block_count;
        
        if ( i <= 0 ) {
            global_block_count++;
            return;
        }
        // get next driver
        SnoozeBlock( tail... );
    }
    
    /***********************************************************************************
     *  Constructor - Inherited Drivers call this constructor
     *
     *  @return this
     ***********************************************************************************/
    SnoozeBlock ( void ) :
                local_block( -1 ),
                isUsed( false ),
                isDriver( false )
    {
        
    }
    
    /***********************************************************************************
     *  Deconstructor - Deallocate "this" SnoozeBlock
     ***********************************************************************************/
    ~SnoozeBlock ( void ) {

        if ( local_block == -1 ) return;
        if ( root_class_address[local_block] == this ) {
            SnoozeBlock *u;
            SnoozeBlock *p = SnoozeBlock::root_block[local_block];
            u = p;
            while ( p != NULL ) {
                p = p->next_block[local_block];
                u->next_block[local_block] = NULL;
                u = p;
            }
            root_block[local_block] = NULL;
            root_class_address[local_block] = NULL;
            global_block_count--;
        }
    }
    
    /***********************************************************************************
     *  Combine SnoozeBlock(s)
     *
     *  @param rhs Driver or SnoozeBlock(s) to combine with the lhs
     *
     *  @return this
     ***********************************************************************************/
    SnoozeBlock & operator = ( const SnoozeBlock &rhs ) {
        
        if ( isDriver ) return *this;
        
        if ( local_block == -1 ) local_block = global_block_count++;
        
        SnoozeBlock *u;
        SnoozeBlock *p = SnoozeBlock::root_block[local_block];
        u = p;
        while ( p != NULL ) {
            p = p->next_block[local_block];
            u->next_block[local_block] = NULL;
            u = p;
        }
        
        if ( rhs.isDriver ) {
            p = ( SnoozeBlock * )&rhs;
            root_block[local_block] = p;
        } else {
            root_block[local_block] = root_block[rhs.local_block];
            p = SnoozeBlock::root_block[rhs.local_block];
            u = SnoozeBlock::root_block[local_block];
            for ( ; p; p = p->next_block[rhs.local_block] ) {
                u->next_block[local_block] = p->next_block[rhs.local_block];
                u = u->next_block[local_block];
            }
        }
        
        next_block[local_block] = NULL;
        
        return *this;
    }

    /***********************************************************************************
     *  Remove SnoozeBlock(s)
     *
     *  @param rhs Driver or SnoozeBlock(s) to subtract with the lhs
     *
     *  @return this
     ***********************************************************************************/
    SnoozeBlock & operator -= ( const SnoozeBlock &rhs ) {
        if ( isDriver || local_block == -1 ) return *this;
        
        if ( rhs.isDriver ) {
            uint8_t index = 0;
            SnoozeBlock *p = SnoozeBlock::root_block[local_block];
            SnoozeBlock *u = SnoozeBlock::root_block[local_block];
            for ( ; p; p = p->next_block[local_block] ) {
                if ( p == ( SnoozeBlock * )&rhs ) {
                    if ( index == 0 ) {
                        SnoozeBlock::root_block[local_block] = p->next_block[local_block];
                        break;
                    }
                    p = p->next_block[local_block];
                    u->next_block[local_block] = p;
                    break;
                }
                u = p;
                index++;
            }
            return *this;
        }
        
        SnoozeBlock *rhsBlock = SnoozeBlock::root_block[rhs.local_block];
        for ( ; rhsBlock; rhsBlock = rhsBlock->next_block[rhs.local_block] ) {
            uint8_t index = 0;
            SnoozeBlock *p = SnoozeBlock::root_block[local_block];
            SnoozeBlock *u = SnoozeBlock::root_block[local_block];
            for ( ; p; p = p->next_block[local_block] ) {
                if ( p == rhsBlock  ) {
                    if ( index == 0 ) {
                        SnoozeBlock::root_block[local_block] = p->next_block[local_block];
                        break;
                    }
                    p = p->next_block[local_block];
                    u->next_block[local_block] = p;
                    break;
                }
                u = p;
                index++;
            }
        }
        return *this;
    }
    
    /***********************************************************************************
     *  Add SnoozeBlock(s)
     *
     *  @param rhs Driver or SnoozeBlock(s) to add with the lhs
     *
     *  @return this
     ***********************************************************************************/
    SnoozeBlock & operator += ( const SnoozeBlock &rhs ) {
        
        if ( isDriver ) return *this;
        //int b = local_block;
        if ( local_block == -1 ) local_block = global_block_count++;
        
        if ( rhs.isDriver ) {
            SnoozeBlock *p = SnoozeBlock::root_block[local_block];
            
            for ( ; p; p = p->next_block[local_block] ) if ( p == ( SnoozeBlock * )&rhs ) return *this;
            
            uint8_t idx = local_block;
            p = SnoozeBlock::root_block[idx];
            for ( ; p->next_block[idx]; p = p->next_block[idx] );
            p->next_block[idx] = ( SnoozeBlock * )&rhs;
            next_block[idx] = NULL;
            return *this;
        }
        
        SnoozeBlock *rhsBlock = SnoozeBlock::root_block[rhs.local_block];
        for ( ; rhsBlock; rhsBlock = rhsBlock->next_block[rhs.local_block] ) {
            uint8_t idx = local_block;
            bool flag = false;
            
            if ( root_block[idx] == NULL ) {
                root_block[idx] = rhsBlock;
            } else {
                SnoozeBlock *p;
                for ( p = root_block[idx]; p->next_block[idx]; p = p->next_block[idx] );
                SnoozeBlock *tmpBlock = SnoozeBlock::root_block[idx];
                for ( ; tmpBlock; tmpBlock = tmpBlock->next_block[idx] ) {
                    if ( tmpBlock == rhsBlock ) {
                        flag = true;
                        break;
                    }
                }
                if ( !flag ) p->next_block[idx] = rhsBlock;
            }
            if ( !flag ) next_block[idx] = NULL;
        }
        return *this;
    }
    /***********************************************************************************
     *  Default add SnoozeBlock(s) - does nothing
     *
     *  @param rhs Driver or SnoozeBlock(s) to combine with the lhs
     *
     *  @return this
     ***********************************************************************************/
    SnoozeBlock & operator + ( const SnoozeBlock &rhs ) {
        return *this;
    }
    /***********************************************************************************
     *  Default subtract SnoozeBlock(s) - does nothing
     *
     *  @param rhs Driver or SnoozeBlock(s) to combine with the lhs
     *
     *  @return this
     ***********************************************************************************/
    SnoozeBlock & operator - ( const SnoozeBlock &rhs ) {
        return *this;
    }
    /***********************************************************************************
     *  call drivers enable functions
     ***********************************************************************************/
    virtual void enableDriver ( void ) {
        if ( local_block == -1 ) return;
        current_block = local_block;
        source = -1;
        SnoozeBlock *p = SnoozeBlock::root_block[local_block];
        forwardList( p, local_block );
    }
    
    /***********************************************************************************
     *  call drivers disable functions
     ***********************************************************************************/
    virtual void disableDriver ( void ) {
        if ( local_block == -1 ) return;
        SnoozeBlock *p = SnoozeBlock::root_block[local_block];
        reverseList( p, local_block );
    }
    
    /***********************************************************************************
     *  drivers override this function to clear isr flags, gets called in wakeup isr
     ***********************************************************************************/
    virtual void clearIsrFlags ( void ) {
    
    }
    
    /***********************************************************************************
     *  called before low speed operation (2 MHZ)
     *
     *  @param SNOOZE_BLOCK
     *
     *  @return true
     ***********************************************************************************/
    uint8_t set_runlp( SNOOZE_BLOCK ) __attribute__((always_inline, unused)) {
        SnoozeBlock *p = &configuration;
        p->mode = RUN_LP;
        p->enableDriver( );
        if ( F_CPU == TWO_MHZ ) return 1;
        start_lptmr_systick( );
        pee_blpi( );
        stop_lptmr_systick( 1999 );
        enter_vlpr( );
        return 1;
    }
    
    /***********************************************************************************
     *  called before high speed operation (F_CPU MHZ)
     *
     *  @param SNOOZE_BLOCK
     *
     *  @return false
     ***********************************************************************************/
    uint8_t set_run( SNOOZE_BLOCK ) __attribute__((always_inline, unused)) {
        SnoozeBlock *p = &configuration;
        p->mode = RUN;
        if ( F_CPU == TWO_MHZ ) return 0;
        exit_vlpr( );
        start_lptmr_systick( );
        blpi_pee( );
        stop_lptmr_systick( ( F_CPU / 1000 ) - 1 );
        p->disableDriver( );
        return 0;
    }
    DMAMEM static SLEEP_MODE mode;
    DMAMEM static int source;
    volatile bool isUsed;
    bool isDriver;
};
#endif /* defined(SnoozeBlock_h) */
