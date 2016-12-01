#include "../../include/zip_api.h"


#define  MAX_DTLS_PSK       64                 //Maximum DTLS pre-shared key hex string length

#define ADD_NODE_STS_UNKNOWN    0   ///<Add node status: unknown
#define ADD_NODE_STS_PROGRESS   1   ///<Add node status: in progress
#define ADD_NODE_STS_DONE       2   ///<Add node status: done

#define     SEC2_ENTER_KEY_REQ  1   ///< Bit-mask for allowing S2 key request callback
#define     SEC2_ENTER_DSK      2   ///< Bit-mask for allowing S2 DSK callback

/** user application context*/
typedef struct
{
    volatile int    init_status;   ///< Network initialization status. 0=unknown; 1=done
    volatile int    add_status;    ///< Add node status.  0=unknown; 1=in progress; 2=done
    volatile unsigned   sec2_cb_enter;///< Control security 2 callback entry bitmask, see SEC2_ENTER_XXX. bit set = allowed callback, 0 = not allowed
    volatile int    sec2_cb_exit;  ///< Security 2 callback status. 1 = exited callback, 0 = waiting or still in the callback
    uint8_t         sec2_add_node; ///< Flag to determine whether to use security 2 when adding node
    sec2_add_prm_t  sec2_add_prm;  ///< Add node with security 2 parameters

    int             use_ipv4;      ///< Flag to indicate whether to use IPv4 or IPv6. 1=IPv4; 0=IPv6
    zwnet_p         zwnet;         ///< Network handle
} hl_appl_ctx_t;

extern hl_appl_ctx_t   g_appl_ctx;
int AddNode();