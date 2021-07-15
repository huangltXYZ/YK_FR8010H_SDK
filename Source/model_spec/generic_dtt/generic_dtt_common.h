
#ifndef GENERIC_DTT_COMMON_H__
#define GENERIC_DTT_COMMON_H__


/**
 * @defgroup GENERIC_DTT_MODEL Generic Default Transition Time model
 * @ingroup MESH_API_GROUP_GENERIC_MODELS
 * Generic Default Transition Time (dtt) model implements the message based interface required to set the Default Transition Time value on the server. Server model
 * itself is a stateless model. The state information resides in the user application.
 * @{
 */

/** Model Company ID */
#define GENERIC_DTT_COMPANY_ID 0xFFFF

/**
 * Defines default value for Generic Default Transition Time (independent, root only) state.
 */
#ifndef GENERIC_DTT_DEFAULT_DTT
#define GENERIC_DTT_DEFAULT_DTT                    (0x0)
#endif


/**
 * Unpacked message structure typedefs are used for API interfaces and for implementing model code. This helps to minimize code
 * footprint.
 */

/** Parameters for the Default Transition Time Set message. */
typedef struct
{
    uint32_t transition_time_ms;                               /**< Transition time value in milliseconds */
} generic_dtt_set_params_t;

/** Parameters for the Default Transition Time Status message. */
typedef struct
{
    uint32_t transition_time_ms;                               /**< Transition time value in milliseconds */
} generic_dtt_status_params_t;

/**@} end of GENERIC_DTT_MODEL */
#endif /* GENERIC_DTT_COMMON_H__ */
