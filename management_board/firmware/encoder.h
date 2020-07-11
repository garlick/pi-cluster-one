#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void encoder_setup (void);
EXTERNC void encoder_finalize (void);

EXTERNC void encoder_update (void);


/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */

