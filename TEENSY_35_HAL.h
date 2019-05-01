#if defined(__MK64FX512__)
#define SNOOZE_BLOCK SnoozeBlock &configuration

uint32_t llwu_clear_flags( void );
int llwu_disable( void );

void initialize( void );
#endif
