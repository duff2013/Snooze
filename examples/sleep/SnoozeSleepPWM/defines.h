#if defined(KINETISK)
    #define F_TIMER F_BUS
#elif defined(KINETISL)
    #if F_CPU > 16000000
        #define F_TIMER (F_PLL/2)
    #else
        #define F_TIMER (F_PLL)
    #endif//Low Power
#endif


#if defined(KINETISK)
    #define DEFAULT_FTM_02_MOD (4096 - 1)
#elif defined(KINETISL)
    #define DEFAULT_FTM_02_MOD (4096 - 1)
#endif

#define DEFAULT_FTM_02_PRESCALE 0

#if F_TIMER == 120000000
    #define DEFAULT_FTM_MOD (61440 - 1)
    #define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 108000000
    #define DEFAULT_FTM_MOD (55296 - 1)
    #define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 96000000
    #define DEFAULT_FTM_MOD (49152 - 1)
    #define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 90000000
    #define DEFAULT_FTM_MOD (46080 - 1)
    #define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 80000000
    #define DEFAULT_FTM_MOD (40960 - 1)
    #define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 72000000
    #define DEFAULT_FTM_MOD (36864 - 1)
    #define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 64000000
    #define DEFAULT_FTM_MOD (65536 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 60000000
    #define DEFAULT_FTM_MOD (61440 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 56000000
    #define DEFAULT_FTM_MOD (57344 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 54000000
    #define DEFAULT_FTM_MOD (55296 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 48000000
    #define DEFAULT_FTM_MOD (49152 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 40000000
    #define DEFAULT_FTM_MOD (40960 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 36000000
    #define DEFAULT_FTM_MOD (36864 - 1)
    #define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 24000000
    #define DEFAULT_FTM_MOD (49152 - 1)
    #define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 16000000
    #define DEFAULT_FTM_MOD (32768 - 1)
    #define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 8000000
    #define DEFAULT_FTM_MOD (16384 - 1)
    #define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 4000000
    #define DEFAULT_FTM_MOD (8192 - 1)
    #define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 2000000
    #define DEFAULT_FTM_MOD (4096 - 1)
    #define DEFAULT_FTM_PRESCALE 0
#endif

#if defined(__MK20DX128__)
    #define FTM0_CH0_PIN 22
    #define FTM0_CH1_PIN 23
    #define FTM0_CH2_PIN  9
    #define FTM0_CH3_PIN 10
    #define FTM0_CH4_PIN  6
    #define FTM0_CH5_PIN 20
    #define FTM0_CH6_PIN 21
    #define FTM0_CH7_PIN  5
    #define FTM1_CH0_PIN  3
    #define FTM1_CH1_PIN  4
#elif defined(__MK20DX256__)
    #define FTM0_CH0_PIN 22
    #define FTM0_CH1_PIN 23
    #define FTM0_CH2_PIN  9
    #define FTM0_CH3_PIN 10
    #define FTM0_CH4_PIN  6
    #define FTM0_CH5_PIN 20
    #define FTM0_CH6_PIN 21
    #define FTM0_CH7_PIN  5
    #define FTM1_CH0_PIN  3
    #define FTM1_CH1_PIN  4
    #define FTM2_CH0_PIN 32
    #define FTM2_CH1_PIN 25
#elif defined(__MKL26Z64__)
    #define FTM0_CH0_PIN 22
    #define FTM0_CH1_PIN 23
    #define FTM0_CH2_PIN  9
    #define FTM0_CH3_PIN 10
    #define FTM0_CH4_PIN  6
    #define FTM0_CH5_PIN 20
    #define FTM1_CH0_PIN 16
    #define FTM1_CH1_PIN 17
    #define FTM2_CH0_PIN  3
    #define FTM2_CH1_PIN  4
#elif defined(__MK64FX512__)
    #define FTM0_CH0_PIN 22
    #define FTM0_CH1_PIN 23
    #define FTM0_CH2_PIN  9
    #define FTM0_CH3_PIN 10
    #define FTM0_CH4_PIN  6
    #define FTM0_CH5_PIN 20
    #define FTM0_CH6_PIN 21
    #define FTM0_CH7_PIN  5
    #define FTM1_CH0_PIN  3
    #define FTM1_CH1_PIN  4
    #define FTM2_CH0_PIN 29
    #define FTM2_CH1_PIN 30
    #define FTM3_CH0_PIN  2
    #define FTM3_CH1_PIN 14
    #define FTM3_CH2_PIN  7
    #define FTM3_CH3_PIN  8
    #define FTM3_CH4_PIN 35
    #define FTM3_CH5_PIN 36
    #define FTM3_CH6_PIN 37
    #define FTM3_CH7_PIN 38
#elif defined(__MK66FX1M0__)
    #define FTM0_CH0_PIN 22
    #define FTM0_CH1_PIN 23
    #define FTM0_CH2_PIN  9
    #define FTM0_CH3_PIN 10
    #define FTM0_CH4_PIN  6
    #define FTM0_CH5_PIN 20
    #define FTM0_CH6_PIN 21
    #define FTM0_CH7_PIN  5
    #define FTM1_CH0_PIN  3
    #define FTM1_CH1_PIN  4
    #define FTM2_CH0_PIN 29
    #define FTM2_CH1_PIN 30
    #define FTM3_CH0_PIN  2
    #define FTM3_CH1_PIN 14
    #define FTM3_CH2_PIN  7
    #define FTM3_CH3_PIN  8
    #define FTM3_CH4_PIN 35
    #define FTM3_CH5_PIN 36
    #define FTM3_CH6_PIN 37
    #define FTM3_CH7_PIN 38
    #define TPM1_CH0_PIN 16
    #define TPM1_CH1_PIN 17
#endif
