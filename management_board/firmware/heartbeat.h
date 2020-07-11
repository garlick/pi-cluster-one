#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void heartbeat_setup (void);
EXTERNC void heartbeat_finalize (void);

EXTERNC void heartbeat_update (void);

/* Get/set heartbeat (byte) value for a given node.
 * Returns 0 on success, -1 if node is out of range.
 */
EXTERNC int heartbeat_node_set (uint8_t node, uint8_t val);
EXTERNC int heartbeat_node_get (uint8_t node, uint8_t *val, uint8_t *active);

EXTERNC uint8_t heartbeat_slots (void);

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */

