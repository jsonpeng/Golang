/**
@file   zip_application.h - Z/IP interface application layer implementation header file.

        To provide network management and controller functions.

@author David Chow

@version    1.0 7-6-11  Initial release

@copyright © 2014 SIGMA DESIGNS, INC. THIS IS AN UNPUBLISHED WORK PROTECTED BY SIGMA DESIGNS, INC.
AS A TRADE SECRET, AND IS NOT TO BE USED OR DISCLOSED EXCEPT AS PROVIDED Z-WAVE CONTROLLER DEVELOPMENT KIT
LIMITED LICENSE AGREEMENT. ALL RIGHTS RESERVED.

NOTICE: ALL INFORMATION CONTAINED HEREIN IS CONFIDENTIAL AND/OR PROPRIETARY TO SIGMA DESIGNS
AND MAY BE COVERED BY U.S. AND FOREIGN PATENTS, PATENTS IN PROCESS, AND ARE PROTECTED BY TRADE SECRET
OR COPYRIGHT LAW. DISSEMINATION OR REPRODUCTION OF THE SOURCE CODE CONTAINED HEREIN IS EXPRESSLY FORBIDDEN
TO ANYONE EXCEPT LICENSEES OF SIGMA DESIGNS  WHO HAVE EXECUTED A SIGMA DESIGNS' Z-WAVE CONTROLLER DEVELOPMENT KIT
LIMITED LICENSE AGREEMENT. THE COPYRIGHT NOTICE ABOVE IS NOT EVIDENCE OF ANY ACTUAL OR INTENDED PUBLICATION OF
THE SOURCE CODE. THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR
IMPLY ANY RIGHTS  TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL A PRODUCT
THAT IT  MAY DESCRIBE.


THE SIGMA PROGRAM AND ANY RELATED DOCUMENTATION OR TOOLS IS PROVIDED TO COMPANY "AS IS" AND "WITH ALL FAULTS",
WITHOUT WARRANTY OF ANY KIND FROM SIGMA. COMPANY ASSUMES ALL RISKS THAT LICENSED MATERIALS ARE SUITABLE OR ACCURATE
FOR COMPANY'S NEEDS AND COMPANY'S USE OF THE SIGMA PROGRAM IS AT COMPANY'S OWN DISCRETION AND RISK. SIGMA DOES NOT
GUARANTEE THAT THE USE OF THE SIGMA PROGRAM IN A THIRD PARTY SERVICE ENVIRONMENT OR CLOUD SERVICES ENVIRONMENT WILL BE:
(A) PERFORMED ERROR-FREE OR UNINTERRUPTED; (B) THAT SIGMA WILL CORRECT ANY THIRD PARTY SERVICE ENVIRONMENT OR
CLOUD SERVICE ENVIRONMENT ERRORS; (C) THE THIRD PARTY SERVICE ENVIRONMENT OR CLOUD SERVICE ENVIRONMENT WILL
OPERATE IN COMBINATION WITH COMPANY'S CONTENT OR COMPANY APPLICATIONS THAT UTILIZE THE SIGMA PROGRAM;
(D) OR WITH ANY OTHER HARDWARE, SOFTWARE, SYSTEMS, SERVICES OR DATA NOT PROVIDED BY SIGMA. COMPANY ACKNOWLEDGES
THAT SIGMA DOES NOT CONTROL THE TRANSFER OF DATA OVER COMMUNICATIONS FACILITIES, INCLUDING THE INTERNET, AND THAT
THE SERVICES MAY BE SUBJECT TO LIMITATIONS, DELAYS, AND OTHER PROBLEMS INHERENT IN THE USE OF SUCH COMMUNICATIONS
FACILITIES. SIGMA IS NOT RESPONSIBLE FOR ANY DELAYS, DELIVERY FAILURES, OR OTHER DAMAGE RESULTING FROM SUCH ISSUES.
SIGMA IS NOT RESPONSIBLE FOR ANY ISSUES RELATED TO THE PERFORMANCE, OPERATION OR SECURITY OF THE THIRD PARTY SERVICE
ENVIRONMENT OR CLOUD SERVICES ENVIRONMENT THAT ARISE FROM COMPANY CONTENT, COMPANY APPLICATIONS OR THIRD PARTY CONTENT.
SIGMA DOES NOT MAKE ANY REPRESENTATION OR WARRANTY REGARDING THE RELIABILITY, ACCURACY, COMPLETENESS, CORRECTNESS, OR
USEFULNESS OF THIRD PARTY CONTENT OR SERVICE OR THE SIGMA PROGRAM, AND DISCLAIMS ALL LIABILITIES ARISING FROM OR RELATED
TO THE SIGMA PROGRAM OR THIRD PARTY CONTENT OR SERVICES. TO THE EXTENT NOT PROHIBITED BY LAW, THESE WARRANTIES ARE EXCLUSIVE.
SIGMA OFFERS NO WARRANTY OF NON-INFRINGEMENT, TITLE, OR QUIET ENJOYMENT. NEITHER SIGMA NOR ITS SUPPLIERS OR LICENSORS
SHALL BE LIABLE FOR ANY INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OR LOSS (INCLUDING DAMAGES FOR LOSS OF
BUSINESS, LOSS OF PROFITS, OR THE LIKE), ARISING OUT OF THIS AGREEMENT WHETHER BASED ON BREACH OF CONTRACT,
INTELLECTUAL PROPERTY INFRINGEMENT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY, PRODUCT LIABILITY OR OTHERWISE,
EVEN IF SIGMA OR ITS REPRESENTATIVES HAVE BEEN ADVISED OF OR OTHERWISE SHOULD KNOW ABOUT THE POSSIBILITY OF SUCH DAMAGES.
THERE ARE NO OTHER EXPRESS OR IMPLIED WARRANTIES OR CONDITIONS INCLUDING FOR SOFTWARE, HARDWARE, SYSTEMS, NETWORKS OR
ENVIRONMENTS OR FOR MERCHANTABILITY, NONINFRINGEMENT, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE.

The Sigma Program  is not fault-tolerant and is not designed, manufactured or intended for use or resale as on-line control
equipment in hazardous environments requiring fail-safe performance, such as in the operation of nuclear facilities,
aircraft navigation or communication systems, air traffic control, direct life support machines, or weapons systems,
in which the failure of the Sigma Program, or Company Applications created using the Sigma Program, could lead directly
to death, personal injury, or severe physical or environmental damage ("High Risk Activities").  Sigma and its suppliers
specifically disclaim any express or implied warranty of fitness for High Risk Activities.Without limiting Sigma's obligation
of confidentiality as further described in the Z-Wave Controller Development Kit Limited License Agreement, Sigma has no
obligation to establish and maintain a data privacy and information security program with regard to Company's use of any
Third Party Service Environment or Cloud Service Environment. For the avoidance of doubt, Sigma shall not be responsible
for physical, technical, security, administrative, and/or organizational safeguards that are designed to ensure the
security and confidentiality of the Company Content or Company Application in any Third Party Service Environment or
Cloud Service Environment that Company chooses to utilize.
*/
#ifndef _ZIP_APPLICATION_DAVID_
#define _ZIP_APPLICATION_DAVID_

#include "zip_session.h"

#ifdef OS_LINUX
#if !defined(OS_MAC_X) && !defined(OS_ANDROID)
#define NI_NUMERICHOST	1	/**< Don't try to look up hostname.  */
#define NI_NUMERICSERV  2	/**< Don't convert port number to name.  */
#endif
#endif

/**
@defgroup Application Application layer APIs
To provide network management and controller functions.
@ingroup zwarecapi
@{
*/
///
/// Boolean definition
#ifndef TRUE
#define TRUE  1         ///< True
#endif
#ifndef FALSE
#define FALSE 0         ///< False
#endif


#define APPL_CB_TMOUT_MIN        (FRAME_SEND_TIMEOUT_MIN * (FRAME_MAX_RESEND + 1) + 100)  ///< Min. callback timeout from session layer
#define APPL_WAIT_SEND_TIMEOUT      7000    ///< The maximum wait time for sending a command in milliseconds
#define MAX_ZWAVE_NODE              232     ///< The maximum number of Z-wave nodes in a network
#define UNSOLICITED_NODE_ID         (MAX_ZWAVE_NODE + 1)  ///< The temporary node id assigned to unsolicited node

#define START_FAILED_NODE_REPLACE       0x01    ///< Start failed node replace
#define STOP_FAILED_NODE_REPLACE        0x05    ///< Stop failed node replace

/** @name Nw_mgmnt_op_id
* Network management operation id definition
@{
*/
#define NM_OP_NODE_LIST_GET             0   ///< Get node list
#define NM_OP_NODE_INFO_CACHED_GET      1   ///< Get cached node info
#define NM_OP_DEFAULT_SET               2   ///< Reset z-wave controller
#define NM_OP_LEARN_MODE_SET            3   ///< Start learn mode
#define NM_OP_NETWORK_UPDATE_REQUEST    4   ///< Start network update
#define NM_OP_NODE_ADD                  5   ///< Add node
#define NM_OP_NODE_REMOVE               6   ///< Remove node
#define NM_OP_FAILED_NODE_ID_REMOVE     7   ///< Remove failed node
#define NM_OP_FAILED_NODE_REPLACE       8   ///< Replace failed node
#define NM_OP_REQ_NODE_NBR_UPDATE       9   ///< Request node neighbor update
#define NM_OP_CONTROLLER_CHANGE         10  ///< Start controller change
#define NM_OP_ASSIGN_RET_ROUTE          11  ///< Assign return route
#define NM_OP_DELETE_RET_ROUTE          12  ///< Delete return route
#define NM_OP_DSK_GET                   13  ///< Get Z/IP gateway DSK

#define NM_OP_TOTAL                     14  ///< Total number of operations
///@}

/** @name Node_status
* Node status definition
@{
*/
#define APPL_NODE_STS_DOWN              1   ///< Node is either down or sleeping
#define APPL_NODE_STS_UP                0   ///< Node is alive
///@}

///
/// Node information
typedef struct
{
    uint8_t   node_id;       ///< node identifier
    uint8_t   listen;        ///< flag to indicate whether the node is always listening (awake)
    uint8_t   optional;      /**< flag to indciate whether the node supports more command classes
                                  in addition to the ones covered by the device classes listed in this message.*/
    uint8_t   sensor;        ///< flag to indicate whether the node is a sensor
    uint8_t   status;        ///< status indicating the progress
    uint8_t   age;           ///< age of the NodeInfo frame. Time is given in 2^n minutes
    uint8_t   basic;         ///< basic device class
    uint8_t   gen;           ///< generic device class
    uint8_t   spec;          ///< specific device class
    uint8_t   cmd_cnt;       ///< the number of command classes in the unsecure cmd_cls field
    uint8_t   cmd_cnt_sec;   ///< the number of command classes in the secure cmd_cls field
    uint16_t  *cmd_cls;      ///< unsecure command classes
    uint16_t  *cmd_cls_sec;  ///< secure command classes

} appl_node_info_t;

///
/// Send data parameters
typedef struct
{
    uint8_t   node_id;      ///< node identifier; zero for Z/IP router
    uint8_t   wkup_poll;    ///< flag to indicate this frame is sent for the purpose of wakeup notification polling
    uint16_t  flag;         ///< flag, see ZIP_FLAG_XXX
    uint16_t  encap_fmt;    ///< Z-wave encapsulation format (valid if flag ZIP_FLAG_ENCAP_INFO is set).
    frm_ep_t  ep_addr;      ///< source and destination endpoint addresses
    uint16_t  dat_len;      ///< length of the dat_buf field
    uint8_t   *dat_buf;     ///< data buffer

} appl_snd_data_t;

///
/// Network management callback information
typedef struct
{
    void      *cb;          ///< callback function pointer
    uint8_t   node_id;      ///< node identifier; zero for Z/IP router
    uint8_t   seq_num;      ///< sequence number of the request

} nm_cb_info_t;

///
/// Node-IP address translation table entry
typedef struct
{
    uint8_t   valid;                ///< Flag to indicate the IP address is valid
    uint8_t   ipaddr_sz;            ///< IP address size in bytes
    uint8_t   ipaddr[IPV6_ADDR_LEN];///< IP address
} node_ipaddr_t;

#define ZIP_ND_INFORMATION_OK           0   /**< valid information in both the IPv6 Address and Node ID fields*/
#define ZIP_ND_INFORMATION_OBSOLETE     1   /**< information in the IPv6 Address and Node ID fields is obsolete.
                                                 No node exists in the network with this address information.*/
#define ZIP_ND_INFORMATION_NOT_FOUND    2   /**< responding Z/IP Router could not locate valid information.*/

///
/// Z/IP ND node advertisement report
typedef struct
{
    uint8_t addr_local;     ///< flag to indicate that the requester asked for the site-local address (a.k.a. ULA).
    uint8_t status;         ///< status of the address/node id resolution query (ZIP_ND_INFORMATION_XXX)
    uint8_t node_id;        ///< node id
    uint8_t ipv6_addr[IPV6_ADDR_LEN];        ///< IPv6 address that corresponds to the node id
    uint32_t    homeid;     ///< Z-wave home id

} nd_advt_rpt_t;

///
/// Node information with security 2 support
typedef struct
{
    appl_node_info_t    node_info;  ///< node information
    int                 sec2_valid; ///< flag to indicate whether the following security 2 parameters are valid
    uint8_t             grnt_keys;  ///< security 2: granted keys
    uint8_t             key_xchg_fail_type;  ///< security 2: key exchange failed type. Zero means success.
} appl_node_info_sec2_t;

#define MAX_DSK_KEY_LEN     16  /**< Maximum DSK key length*/

///
/// Add node DSK report
typedef struct
{
    uint8_t             dsk_keys[MAX_DSK_KEY_LEN];  ///< DSK buffer
    uint8_t             key_len;                    ///< DSK key length
} appl_dsk_rpt_t;

///
/// Node replace with security 2 support
typedef struct
{
    uint8_t     sts;        ///< status
    uint8_t     node_id;    ///< node id of replaced node
    int         sec2_valid; ///< flag to indicate whether the following security 2 parameters are valid
    uint8_t     grnt_keys;  ///< security 2: granted keys
    uint8_t     key_xchg_fail_type;  ///< security 2: key exchange failed type. Zero means success.
} appl_node_rp_sec2_t;

///
/// Learnt mode callback parameters with security 2 support
typedef struct
{
    uint8_t     sts;        ///< status
    uint8_t     node_id;    ///< node id assigned by including controller
    int         sec2_valid; ///< flag to indicate whether the following security 2 parameters are valid
    uint8_t     grnt_keys;  ///< security 2: granted keys
    uint8_t     dsk[MAX_DSK_KEY_LEN];  ///< security 2: DSK bytes.
} appl_lrn_mod_sec2_t;

///
/// Node add status callback parameters
typedef struct
{
    uint8_t   rpt_type;     ///< report types : NODE_ADD_STATUS, NODE_ADD_KEYS_REPORT_V2, or NODE_ADD_DSK_REPORT_V2
    union
    {
        appl_node_info_sec2_t   node_info;      ///< For NODE_ADD_STATUS
        appl_node_rp_sec2_t     node_rp_info;   ///< For FAILED_NODE_REPLACE_STATUS
        uint8_t                 req_keys;       ///< For NODE_ADD_KEYS_REPORT_V2
        appl_dsk_rpt_t          dsk_rpt;        ///< For NODE_ADD_DSK_REPORT_V2
    } rpt_prm;

} appl_node_add_cb_prm_t;


//Forward declaration of application layer context
struct _appl_layer_ctx;

///
/// Transmit completion status callback function
typedef void (*tx_cmplt_cb_t)(struct _appl_layer_ctx *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id);

///
/// Node list get callback function
typedef void (*node_list_get_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t node_list_ctlr_id, uint8_t *node_list, uint8_t nl_len);

///
/// Node info cached get callback function
typedef void (*node_info_chd_get_cb_t)(struct _appl_layer_ctx *appl_ctx, appl_node_info_t *cached_node_info);

///
/// Add node to network callback function
typedef void (*add_node_nw_cb_t)(struct _appl_layer_ctx *appl_ctx, appl_node_add_cb_prm_t *cb_prm);

///
/// Controller change callback function
typedef void (*ctlr_chg_nw_cb_t)(struct _appl_layer_ctx *appl_ctx, appl_node_info_t *node_info);

///
/// Remove node callback function
typedef void (*rm_node_nw_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t org_node_id);

///
/// Set learn mode callback function
typedef void (*set_lrn_mod_cb_t)(struct _appl_layer_ctx *appl_ctx, appl_lrn_mod_sec2_t *cb_prm);

///
/// Get Z/IP gateway DSK callback function
typedef void (*get_dsk_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t *dsk, uint8_t dsk_len);

///
/// Remove failed node callback function
typedef void (*rm_failed_node_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t node_id);

///
/// Replace failed node callback function
//typedef void (*rplc_failed_node_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t node_id);

///
/// Request node neighbor update callback function
typedef void (*req_node_nbr_updt_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t node_id);

///
/// Assign/delete return route completion callback function
typedef void (*ret_route_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t src_node_id);

///
/// Request network topology update callback function
typedef void (*req_nw_updt_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts);

///
/// Set default callback function
typedef void (*set_deflt_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t nodeid);

///
/// Node advertisement callback function
typedef void (*node_advt_cb_t)(struct _appl_layer_ctx *appl_ctx, nd_advt_rpt_t *rpt);

#ifdef TCP_PORTAL
///
/// Network error callback function
typedef void (*net_err_cb_t)(void *user);

#endif

///
/// Firmware version report callback function
typedef void (*fw_ver_cb_t)(uint8_t major, uint8_t minor, void *user);

///
/// Firmware update status report callback function
typedef void (*fw_sts_cb_t)(uint8_t status, void *user);


typedef void (*node_sts_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t node_id, uint8_t sts);
/**<
Callback function to report node alive status change
@param[in]	appl_ctx    The application layer context
@param[in]	node_id	    Node
@param[in]	sts		    Status (APPL_NODE_STS_XXX)
@return
*/


typedef void (*appl_command_hdlr_fn)(struct _appl_layer_ctx *appl_ctx, struct sockaddr *src_addr,
                                     frm_ep_t *ep_addr, uint8_t *cmd_buf, uint16_t cmd_len,
                                     uint16_t flag, uint16_t encap_fmt);
/**<
application command handler callback function
@param[in]	appl_ctx    context
@param[in]	src_addr    source address of the received command
@param[in]	ep_addr     destination endpoint of the received command
@param[in]	cmd_buf	    buffer to stored the received command
@param[in]	cmd_len	    length of buf
@param[in]	flag	    flag, see ZIP_FLAG_XXX
@param[in]	encap_fmt   Z-wave encapsulation format (valid if flag ZIP_FLAG_ENCAP_INFO is set).
                        This format should be passed back when responding
                        to unsolicited command with the flag ZIP_FLAG_ENCAP_INFO is set.

@return
*/


///
/// Send node information frame parameters
typedef struct
{
    uint8_t         fr_node_id;   ///< Node that sends the node information frame; zero for Z/IP router
    uint8_t         to_node_id;   ///< Desination node to receive the node information frame. 0xFF = broadcast
    uint8_t         tx_opt;       ///< Transmit options TRANSMIT_OPTION_XXX
    tx_cmplt_cb_t   cb;           ///< The callback function on transmit completion
    void            *cb_prm;      ///< Callback function

} appl_snd_nif_t;

///
/// Status callback request
typedef struct
{
    tx_cmplt_cb_t   cb;             ///< The status callback function
    void            *user_prm;      ///< The parameters of cb
    int8_t          tx_sts;         ///< Transmit completion status
    uint8_t         node_id;        ///< Node id
} appl_cb_req_t;

/// Send data transmit completion timeout callback info
typedef struct
{
    tx_cmplt_cb_t   send_data_cb;     ///< callback function
    void            *snd_dat_cb_prm;  ///< callback user parameter
    uint8_t         node_id;          ///< destination node
    uint8_t         seq_num;          ///< sequence number of frame to the specific destination node
} appl_tmout_ctx_t;

/// Z/IP frame sequence number
typedef struct
{
    uint16_t  seq_num_valid;    ///< flag to indicate this sequence number for the Z/IP frame is valid or in use
    uint8_t   node_id;          ///< destination node
    uint8_t   wkup_poll;        ///< flag to indicate this sequence number is assigned for wakeup notification polling
} appl_zip_seq_num_t;

///
/// Z-wave HCI application layer context
typedef struct _appl_layer_ctx
{
    //Callback functions to high-level application layer
    appl_command_hdlr_fn    application_command_handler_cb;///< application command handler
    node_sts_cb_t       node_sts_cb;    ///< Node alive status change callback function
    node_list_get_cb_t  nodelst_cb;     ///< Unsolicited node list callback function
    tx_cmplt_cb_t       old_sts_cb;     ///< Callback function to receive old send status

    /** Callback function information for Z/IP network management operations */
    nm_cb_info_t    nm_cb_info[NM_OP_TOTAL];


    //Data
    void      *data;                ///< For high-level application layer to store data/context
    struct plt_mtx_t *snd_mtx;      ///< Mutex for sending command
    struct plt_mtx_t *zip_mtx;      ///< Mutex for zip network management operation
    struct plt_mtx_t *zip_seq_num_mtx;///< Mutex for zip sequence number
    void      *snd_cv;              ///< Condition variable for sending command
    void      *cb_tmr_ctx;          ///< Callback waiting timer context
    void      *old_sts_prm;         ///< Old send status callback user-defined parameter
    node_advt_cb_t node_advt_cb;    ///< Node advertisement callback function
    uint32_t  cb_tmout_ms;          ///< Callback timeout from session layer in milliseconds
    uint8_t   node_sts[232+1];      ///< Nodes status (APPL_NODE_STS_XXX)
    uint8_t   ctl_node_id;          ///< Controller's node id
    struct plt_mtx_t *cb_thrd_mtx;  ///< Mutex for status callback thread
    void      *cb_thrd_sem;         ///< Semaphore for waiting of status callback requests
    util_lst_t   *cb_req_hd;        ///< Head of linked list for status callback requests
    int       use_ipv4;             ///< Flag to indicate whether to use IPv4 as transport IP protocol
    volatile int cb_thrd_run;       ///< Control the callback thread whether to run. 1 = run, 0 = stop
    volatile int cb_thrd_sts;       ///< Callback thread status. 1 = run, 0 = thread exited
    appl_zip_seq_num_t zip_seq_num_db[256];///< Sequence number database for the Z/IP frame
    uint8_t   zip_next_seq_num;     ///< Next Z/IP sequence number
    uint8_t   zip_op_seq_num;       ///< Sequence number for the Z/IP network management operation
    volatile int  wait_cmd_cb;      ///< Flag to indicate whether to wait for command callback
                                    ///< in order for the next command to be sent.
    volatile int  wait_nm_cb;       ///< Flag to indicate whether to wait for network management command callback
                                    ///< in order for the next command to be sent.
    appl_tmout_ctx_t  snd_tmout_ctx;///< Send data transmit completion status timeout callback info (used by timer callback only)
    ssn_layer_ctx_t   ssn_ctx;      ///< Session layer context
    plt_ctx_t       *plt_ctx;       ///< Platform context
    node_ipaddr_t   node_ip_tbl[MAX_ZWAVE_NODE+1];    ///< Node-IP address translation table
    node_ipaddr_t   node_ip_cache;  ///< Cache the last query of Node-IP address translation

} appl_layer_ctx_t;

#ifdef TCP_PORTAL
int     zwhci_init(appl_layer_ctx_t *appl_ctx, int portal_fd, void *portal_ssl, tls_clnt_prof_t *portal_prof,
                   uint8_t *zip_router, net_err_cb_t err_cb, void *user);
/**<
Init the application layer.
Should be called once before calling the other application layer functions
@param[in,out]	appl_ctx		Context
@param[in]	portal_fd		    File descriptor to connect to Z/IP gateway/router using TLS
@param[in]	portal_ssl		    SSL object pointer to connect to Z/IP gateway/router using TLS
@param[in]	portal_prof		    Profile of the Z/IP gateway/router that is connected to the portal
@param[in]	zip_router		    Z/IP router (gateway) address
@param[in]	err_cb		        Callback when there is unrecoverable network error occurred
@param[in]	user		        User parameter that will passed when err_cb is invoked
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should call zwhci_shutdown(), followed by zwhci_exit() to properly clean up this layer.
*/
#else
int     zwhci_init(appl_layer_ctx_t *appl_ctx, uint8_t *zip_router, uint16_t host_port, int16_t use_ipv4,
                   uint8_t *dtls_psk, uint8_t dtls_psk_len);
/**<
Init the application layer.
Should be called once before calling the other application layer functions
@param[in,out]	appl_ctx		Context
@param[in]	    zip_router		Z/IP router (gateway) address
@param[in]	    host_port		Host listening and sending port
@param[in]      use_ipv4        Flag to indicate whether to use IPv4 as transport IP protocol
@param[in]	    dtls_psk		DTLS pre-shared key
@param[in]	    dtls_psk_len	DTLS pre-shared key length (in bytes)
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should call zwhci_shutdown(), followed by zwhci_exit() to properly clean up this layer.
*/
#endif

void zwhci_shutdown(appl_layer_ctx_t *appl_ctx);
/**<
Shutdown the application layer, stop sending any data
@param[in]	appl_ctx    Context
@return
*/

void    zwhci_exit(appl_layer_ctx_t *appl_ctx);
/**<
Clean up the application layer
@param[in,out]	appl_ctx		Context
@return
*/

int     zw_send_data(appl_layer_ctx_t *appl_ctx, appl_snd_data_t  *prm, tx_cmplt_cb_t cb, void *cb_prm);
/**<
Send data to a node
@param[in]	appl_ctx		Context
@param[in]	prm             The parameters
@param[in]	cb              The callback function on transmit completion
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return  0 on success, negative error number on failure
*/

int     zip_node_list_get(appl_layer_ctx_t *appl_ctx, node_list_get_cb_t cb);
/**<
Get node list from Z/IP router through callback function
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function for receiving node list
@return  0 on success, negative error number on failure
*/

int     zip_node_info_chd_get(appl_layer_ctx_t *appl_ctx, node_info_chd_get_cb_t cb, uint8_t nodeid, uint8_t max_age);
/**<
Get cached node info from Z/IP router through callback function
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function for receiving node info
@param[in]	nodeid          Node id
@param[in]	max_age         The maximum age of the NodeInfo frame, given in 2^n minutes.
                            If the cache entry does not exist or if it is older than the value given in this field,
                            the ZipRouter will attempt to get a Fresh NodeInfo frame before
                            responding to the Node Info Cached Get command. A value of 15 means infinite,
                            i.e. No Cache Refresh.
@return  0 on success, negative error number on failure
*/

int     zip_node_id_get(appl_layer_ctx_t *appl_ctx, node_advt_cb_t cb, uint8_t *tgt_ip, int retry);
/**<
Resolve IPv6 address into node id through callback function
@param[in]	appl_ctx		Context
@param[in]  tgt_ip          Target IPv6 address of the Z-wave node
@param[in]	cb              The callback function for receiving node id
@param[in]	retry           Flag to indicate if this call is second attempt to get node id
@return  0 on success, negative error number on failure
*/

int     zip_node_ipv6_get(appl_layer_ctx_t *appl_ctx, node_advt_cb_t cb, uint8_t node_id);
/**<
Resolve a node id into IPv6 address through callback function
@param[in]	appl_ctx		Context
@param[in]  node_id         Z-wave node id
@param[in]	cb              The callback function for receiving node list
@return  0 on success, negative error number on failure
*/

int     zw_set_default(appl_layer_ctx_t *appl_ctx, set_deflt_cb_t cb, uint8_t nodeid);
/**<
Set the Controller back to the factory default state
@param[in]	appl_ctx		Context
@param[in]	nodeid          Node id. If zero, the destination is Z/IP router
@param[in]	cb              The callback function to report completion.
@return  0 on success, negative error number on failure
*/

int     zw_set_learn_mode(appl_layer_ctx_t *appl_ctx, set_lrn_mod_cb_t cb, uint8_t nodeid, uint8_t  mode);
/**<
Set learn mode, used to add or remove the controller to/from a Z-Wave network.
@param[in]	appl_ctx		Context
@param[in]	nodeid          Node id. If zero, the destination is Z/IP router
@param[in]	mode            Mode (ZW_SET_LEARN_MODE_XXX) could be to start the learn mode in classic or NWI or to stop the learn mode.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/

int     zip_node_info_send(appl_layer_ctx_t *appl_ctx, appl_snd_nif_t  *nif_prm);
/**<
Send node information from a node to another node or all nodes.
@param[in]	appl_ctx    Context
@param[in]	nif_prm     Send node information frame parameters
@return  0 on success, negative error number on failure
*/

int     zw_request_network_update(appl_layer_ctx_t *appl_ctx, req_nw_updt_cb_t cb);
/**<
Request network topology updates from the SUC/SIS node.
Secondary controllers can only use this call when a SUC is present in the network.
All controllers can use this call in case a SUC ID Server (SIS) is available.
Routing Slaves can only use this call, when a SUC is present in the network.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function on completion.
@return  0 on success, negative error number on failure.
*/

int    zw_sec2_key_set(appl_layer_ctx_t *appl_ctx, uint8_t granted_key);
/**<
Security 2 key set
@param[in]	appl_ctx		Context
@param[in]	granted_key     Granted key
@return  0 on success, negative error number on failure
*/

int    zw_sec2_dsk_set(appl_layer_ctx_t *appl_ctx, int accept, uint8_t dsk_len, uint8_t *dsk);
/**<
Security 2 dsk set
@param[in]	appl_ctx	Context
@param[in]	accept      Flag to indicate whether to accept the joining node DSK
@param[in]	dsk_len     Number of bytes (max. 16 bytes) of DSK to send.
@param[in]	dsk         DSK to send
@return  0 on success, negative error number on failure
*/

int    zw_sec2_dsk_get(appl_layer_ctx_t *appl_ctx, get_dsk_cb_t cb);
/**<
Get Z/IP gateway DSK
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report Z/IP gateway DSK.
@return  0 on success, negative error number on failure
*/

int     zw_add_node_to_network(appl_layer_ctx_t *appl_ctx, add_node_nw_cb_t cb, uint8_t  mode, uint8_t tx_opt);
/**<
Add node to a network
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report status.
@param[in]	mode            Mode (ADD_NODE_XXX) to control the add node process.
@param[in]	tx_opt          Transmit options (TRANSMIT_OPTION_XXX) to control if transmissions must use special properties.
@return  0 on success, negative error number on failure
*/

int     zw_remove_node_from_network(appl_layer_ctx_t *appl_ctx, rm_node_nw_cb_t cb, uint8_t  mode);
/**<
Remove node from a network
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report status.
@param[in]	mode            Mode (REMOVE_NODE_XXX) to control the remove node process.
@return  0 on success, negative error number on failure
*/

int     zw_remove_failed_node_id(appl_layer_ctx_t *appl_ctx, rm_failed_node_cb_t cb, uint8_t node_id);
/**<
Remove a non-responding node from the routing table in the requesting controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the failed node to be deleted.
@param[in]	cb              The callback function on remove process completion.
@return  0 on success, negative error number on failure.
@pre    The node must be on the failed node ID list and as an extra precaution also fail to respond before it is removed.
*/

int     zw_replace_failed_node(appl_layer_ctx_t *appl_ctx, add_node_nw_cb_t cb, uint8_t node_id, uint8_t tx_opt, uint8_t mode);
/**<
Replace a non-responding node with a new one in the requesting controller.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function on replace process completion.
@param[in]	node_id         The Node ID (1...232) of the failed node to be deleted.
@param[in]	tx_opt          Transmit options: TRANSMIT_OPTION_LOW_POWER
@param[in]	mode            Mode to control start (START_FAILED_NODE_REPLACE)/stop (STOP_FAILED_NODE_REPLACE)
@return  0 on success, negative error number on failure. If APPL_OPER_NOT_STARTED is returned, check the resp_flg output.
*/

int     zw_request_node_neighbor_update(appl_layer_ctx_t *appl_ctx, req_node_nbr_updt_cb_t cb, uint8_t node_id);
/**<
Get the neighbors from the specified node. Can only be called by a primary/inclusion controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the node that the controller wants to get new neighbors from.
@param[in]	cb              The callback function with the status of request.
@return  0 on success, negative error number on failure.
*/

int     zw_controller_change(appl_layer_ctx_t *appl_ctx, ctlr_chg_nw_cb_t cb, uint8_t  mode, uint8_t tx_opt);
/**<
Add a controller to the Z-Wave network and transfer the role as primary controller to it.
The controller invoking this function will become secondary.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report status.
@param[in]	mode            Mode (CONTROLLER_CHANGE_XXX)to start/stop operation.
@param[in]	tx_opt          Transmit options (TRANSMIT_OPTION_EXPLORE  and/or TRANSMIT_OPTION_LOW_POWER)
@return  0 on success, negative error number on failure
*/

int     zw_assign_return_route(appl_layer_ctx_t *appl_ctx, uint8_t src_node_id, uint8_t dst_node_id, ret_route_cb_t cb);
/**<
Assign static return routes (up to 4) to a Routing Slave node or Enhanced Slave node.
@param[in]	appl_ctx		Context
@param[in]	src_node_id     The Node ID (1...232) of the routing slave that should get the return routes.
@param[in]	dst_node_id     The Destination node ID (1...232).
@param[in]	cb              The callback function.
@return  0 on success, negative error number on failure
*/

int     zw_delete_return_route(appl_layer_ctx_t *appl_ctx, uint8_t node_id, ret_route_cb_t cb);
/**<
Delete all static return routes from a Routing Slave or Enhanced Slave.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the routing slave node.
@param[in]	cb              The callback function.
@return  0 on success, negative error number on failure
*/

int     zip_node_ipaddr_set(appl_layer_ctx_t *appl_ctx, uint8_t *ipaddr, uint8_t ipaddr_sz, uint8_t node_id);
/**<
Add a node-IP address entry into the translation table
@param[in]	appl_ctx		Context
@param[in]	ipaddr          IP address
@param[in]	ipaddr_sz       IP address size in bytes
@param[in]  node_id         Z-wave node id
@return  1 on success, zero on failure
*/

int     zip_node_ipaddr_rm(appl_layer_ctx_t *appl_ctx, uint8_t node_id);
/**<
Remove a node-IP address entry from the translation table
@param[in]	appl_ctx		Context
@param[in]  node_id         Z-wave node id
@return  1 on success, zero on failure
*/

int     zip_translate_ipaddr(appl_layer_ctx_t *appl_ctx, uint8_t *ipaddr, uint8_t ipaddr_sz, uint8_t *node_id);
/**<
Resolve an IP address to the node id of a node
@param[in]	appl_ctx		Context
@param[in]	ipaddr          IP address
@param[in]	ipaddr_sz       IP address size in bytes
@param[out] node_id         Z-wave node id
@return  1 on success, zero on failure
*/

void    appl_updt_node_sts(appl_layer_ctx_t *appl_ctx, uint8_t node_id, uint8_t sts);
/**<
Update node status
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node
@param[in]	sts		        Status (APPL_NODE_STS_XXX)
@return
*/

uint8_t appl_get_node_sts(appl_layer_ctx_t *appl_ctx, uint8_t node_id);
/**<
Get node status
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node id (ranging from 1 to 232)
@return node status (APPL_NODE_STS_XXX)
*/

void    appl_get_all_node_sts(appl_layer_ctx_t *appl_ctx, uint8_t *node_sts_buf);
/**<
Get all node status
@param[in]	appl_ctx		Context
@param[out]	node_sts_buf	Buffer (min. size of 233 bytes)to store all the node status. Individual node status
                            can be access using the node id as index to the buffer.
@return
*/

void  zip_old_status_cb_set(appl_layer_ctx_t *appl_ctx, tx_cmplt_cb_t cb, void *cb_prm);
/**<
Set callback function to receive old send status
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to receive old send status
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return
*/

void appl_seq_num_rm_by_nodeid(appl_layer_ctx_t *appl_ctx, uint8_t node_id);
/**<
Remove Z/IP frame sequence numbers by node id
@param[in]	appl_ctx    Context
@param[in]	node_id     Node id. If 0xFF, all sequence numbers will be removed
@return
*/


/**
@}
*/

#endif /* _ZIP_APPLICATION_DAVID_ */
