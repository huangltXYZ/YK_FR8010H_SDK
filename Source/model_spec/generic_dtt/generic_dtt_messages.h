
#ifndef GENERIC_DTT_MESSAGES_H__
#define GENERIC_DTT_MESSAGES_H__

/**
 * @internal
 * @defgroup GENERIC_DTT_MESSAGES Internal header
 * @ingroup GENERIC_DTT_MODEL
 * This internal header contains packed structures required for message parsing.
 * @{
 */

/** Generic Default Transition Time model message opcodes. */
typedef enum
{
    GENERIC_DTT_OPCODE_GET = 0x820D,
    GENERIC_DTT_OPCODE_SET = 0x820E,
    GENERIC_DTT_OPCODE_SET_UNACKNOWLEDGED = 0x820F,
    GENERIC_DTT_OPCODE_STATUS = 0x8210
} generic_dtt_opcode_t;

/** Packed message structure typedefs are used for packing and unpacking byte stream. */

/** Message format for the Default Transition Time Set message. */
typedef struct __attribute((packed))
{
    uint8_t transition_time;                                /**< Encoded transition time value */
} generic_dtt_set_msg_pkt_t;

/** Message format for the Default Transition Time Status message. */
typedef struct __attribute((packed))
{
    uint8_t transition_time;                                /**< Encoded transition time value */
} generic_dtt_status_msg_pkt_t;

/**@} end of GENERIC_DTT_MODEL_INTENRAL */
#endif /* GENERIC_DTT_MESSAGES_H__ */
