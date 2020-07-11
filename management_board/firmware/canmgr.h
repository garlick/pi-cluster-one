#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void canmgr_setup (uint8_t mod, uint8_t node);
EXTERNC void canmgr_finalize (void);
EXTERNC void canmgr_update (void);

EXTERNC int canmgr_identify_send (uint8_t node, uint8_t state);
EXTERNC int canmgr_power_send (uint8_t node, uint8_t state);


/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */

