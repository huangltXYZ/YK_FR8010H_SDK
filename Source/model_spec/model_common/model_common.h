
#ifndef MODEL_COMMON_H__
#define MODEL_COMMON_H__


/**
 * Access layer opcode type.
 *
 * The format of the opcodes is given in the table below:
 * @tagMeshSp table 3.43
 *
 * | Byte 0     | Byte 1     | Byte 2     | Description                                                                                   |
 * | ---------- | ---------- | ---------- | --------------------------------------------------------------------------------------------- |
 * | `0xxxxxxx` |            |            | 1-octet Bluetooth SIG Opcodes (excluding 01111111)                                            |
 * | `01111111` |            |            | Reserved for Future Use                                                                       |
 * | `10xxxxxx` | `xxxxxxxx` |            | 2-octet Bluetooth SIG Opcodes                                                                 |
 * | `11xxxxxx` | `zzzzzzzz` | `zzzzzzzz` | 3-octet Vendor Specific Opcodes. `z` denotes company identifier packed in little-endian order |
 *
 * To initialize an access_opcode_t, use the @ref ACCESS_OPCODE_SIG() or @ref ACCESS_OPCODE_VENDOR() macros.
 */
typedef struct
{
    /** 14-bit or 7-bit Bluetooth SIG defined opcode or 6-bit vendor specific opcode. */
    uint16_t opcode;
    /** Company ID. Set to @ref ACCESS_COMPANY_ID_NONE if it is a Bluetooth SIG defined opcode. */
    uint16_t company_id;
} access_opcode_t;


/**
 * Access layer opcode handler callback type.
 *
 * @param[in] handle    Access layer model handle.
 * @param[in] p_message Access RX message structure.
 * @param[in] p_args    Optional generic argument pointer.
 */
typedef void (*access_opcode_handler_cb_t)(const mesh_model_msg_ind_t * p_rx_msg);
		
/** Company ID value for Bluetooth SIG opcodes or models. */
#define ACCESS_COMPANY_ID_NONE   (0xFFFF)

/** Company ID value for Amazon. */
#define ACCESS_COMPANY_ID_AMAZON (0x0171)

#define ACCESS_OPCODE_SIG(opcode)             { (opcode), ACCESS_COMPANY_ID_NONE }		

#define ACCESS_OPCODE_VENDOR(opcode, company) { (opcode), (company) }
/**
 * Opcode handler type.
 *
 * Each specific model implementation is assumed to statically define an array of "opcode handlers",
 * one handler for each of the expected opcodes of the given model.
 */
typedef struct
{
    /** The model opcode. */
    access_opcode_t opcode;
    /** The opcode handler callback for the given opcode. */
    access_opcode_handler_cb_t handler;
} access_opcode_handler_t;



/*针对Vendor模型的消息处理*/
typedef void (*access_opcode_Vendor_handler_cb_t)(const mesh_model_msg_ind_t * p_rx_msg, void * p_args);
typedef struct
{
    /** The model opcode. */
    access_opcode_t opcode;
    /** The opcode handler callback for the given opcode. */
    access_opcode_Vendor_handler_cb_t handler;
} access_opcode_vendor_handler_t;


/** Timestamp type for all time-values */
typedef uint32_t timestamp_t;

/** Callback type for callbacks at finished timers */
typedef void(* timer_callback_t)(timestamp_t);

/**
 * Function type for the generic scheduler timeout callback.
 * @param[in] timestamp Timestamp when the timeout triggered.
 * @param[in] p_context Pointer to the data provided when starting the timer. */
typedef void (*timer_sch_callback_t)(timestamp_t timestamp, void * p_context);

/** State of a timer. */
typedef enum
{
    TIMER_EVENT_STATE_UNUSED,      /**< Not present in the scheduler */
    TIMER_EVENT_STATE_ADDED,       /**< Added for processing */
    TIMER_EVENT_STATE_IN_CALLBACK  /**< Currently being called */
} timer_event_state_t;

/**
 * Timer event structure for schedulable timers.
 */
typedef struct timer_event
{
    volatile timer_event_state_t state;     /**< Timer event state. */
    timestamp_t                  timestamp; /**< Timestamp at which to fire. Is updated by the scheduler if periodic.  */
    timer_sch_callback_t         cb;        /**< Callback function to call when the timer fires. Called asynchronously. */
    uint32_t                     interval;  /**< Interval in us between each fire for periodic timers, or 0 if single-shot. */
    void *                       p_context; /**< Pointer to data passed on to the callback. */
    struct timer_event*          p_next;    /**< Pointer to next event in linked list. Only for internal usage. */
} timer_event_t;

/** Structure for tracking TID expiry for the models */
typedef struct
{
    /** Source address */
    uint16_t src;
    /** Destination address */
    uint16_t dst;
    /** Previously received Opcode */
    uint32_t message_id;
    /** Previously received TID */
    uint8_t old_tid;
    /** New transaction indicator flag */
    bool new_transaction;
    /** Expiration timer instance */
    uint32_t ulTime;
} tid_tracker_t;

bool model_tid_validate(tid_tracker_t * p_tid_tracker, mesh_model_msg_ind_t const *p_rx_msg,
                        uint32_t message_id, uint8_t tid);

/** Transition time value to indicate unknown transition time */
#define MODEL_TRANSITION_TIME_UNKNOWN               (UINT32_MAX)


/** Transition time value to indicate the default transition time should be used */
#ifndef MODEL_ACKNOWLEDGED_TRANSACTION_TIMEOUT
#define MODEL_ACKNOWLEDGED_TRANSACTION_TIMEOUT      (SEC_TO_US(30))
#endif

/** Product-specific transition limitations to align transition data regarding product abilities. */
#ifndef TRANSITION_STEP_MIN_MS
#define TRANSITION_STEP_MIN_MS                  (45)
#endif
/** Maximum value of transition time (in ms) possible in steps of 100 ms */
#define TRANSITION_TIME_STEP_100MS_MAX          (6200ul)
/** Maximum value of transition time (in ms) possible in steps of 1 s */
#define TRANSITION_TIME_STEP_1S_MAX             (TmrSecondsToMilliseconds(62ul))
/** Maximum value of transition time (in ms) possible in steps of 10 s */
#define TRANSITION_TIME_STEP_10S_MAX            (TmrSecondsToMilliseconds(620ul))
/** Maximum value of transition time (in ms) possible in steps of 10 min */
#define TRANSITION_TIME_STEP_10M_MAX            (TmrMinutes(620ul))
/** Max value of encoded transition time step value */
#define TRANSITION_TIME_MAX                     (0x3E)
/** Unknown encoded transition time value */
#define TRANSITION_TIME_UNKNOWN                 (0x3F)
/** Maximum permissible transition time in milliseconds */
#define TRANSITION_TIME_MAX_MS                  (TRANSITION_TIME_STEP_10M_MAX)

/** Delay field step factor in milliseconds */
#define DELAY_TIME_STEP_FACTOR_MS               (5)
/** Maximum encoded value of the delay field */
#define DELAY_TIME_STEP_MAX                     (0xFF)
/** Maximum permisible delay time in milliseconds */
#define DELAY_TIME_MAX_MS                       (DELAY_TIME_STEP_MAX * DELAY_TIME_STEP_FACTOR_MS)




/** Generic Transition parameters for the model messages. */
typedef struct
{
    uint32_t transition_time_ms;                            /**< Transition time value in milliseconds */
    uint32_t delay_ms;                                         /**< Message execution delay in milliseconds */
} model_transition_t;



/** Default transition time get callback prototype
 *
 * @param[in]  element_index    Index of the model element that is requesting default transition
 *                              time value.
 *
 * @retval Returns value of default transition time in milliseconds, zero, if unavailable.
 */
 typedef uint32_t (*default_transition_time_value_get_cb_t)(uint16_t element_index);

/**
 * Gets the decoded value of the transition time in milliseconds.
 *
 * @param[in] enc_transition_time Encoded value of the transition time as specified in the
 *            @tagMeshMdlSp.
 *
 * @returns Transition time in milliseconds.
 */
uint32_t model_transition_time_decode(uint8_t enc_transition_time);

/**
 * Gets the encoded value of the transition time as specified in the @tagMeshMdlSp
 * Note that the provided value will be rounded down to the nearest possible representation.
 *
 * @param[in] transition_time Transition time in milliseconds.
 *
 * @returns Encoded value of the transition time as specified in the @tagMeshMdlSp.
 */
uint8_t model_transition_time_encode(uint32_t transition_time);

/**
 * Validates the given transition time value.
 *
 * @param[in]  enc_transition_time  Encoded transition time value
 *
 * @retval True                     If encoded transition time is a valid value for the models.
 * @retval False                    If encoded transition time is invalid for the models and cannot be set.
 *
 */
bool model_transition_time_is_valid(uint8_t enc_transition_time);

/**
 * Gets the decoded value of the delay time in milliseconds
 *
 * @param[in] enc_delay Encoded value of the delay time as specified in the @tagMeshMdlSp.
 *
 * @returns delay time in milliseconds.
 */
uint32_t model_delay_decode(uint8_t enc_delay);

/**
 * Gets the encoded value of the delay time as specified in the @tagMeshMdlSp.
 * Note that the provided value will be rounded down to the nearest possible representation.
 *
 * @param[in] delay Delay time in milliseconds.
 *
 * @returns Encoded value of the delay time as specified in the @tagMeshMdlSp.
 */
uint8_t model_delay_encode(uint32_t delay);




bool handle_set_message_validate(uint16_t short_message_bytes,
										uint16_t long_message_bytes,
										uint16_t message_bytes,
										uint8_t transition_time,
										uint8_t delay,
										model_transition_t ** pp_transition);

void Mesh_Tid_TimeProc(U32 ulPeriod);

/** @} end of MODEL_COMMON */

#endif /* MODEL_COMMON_H__ */

