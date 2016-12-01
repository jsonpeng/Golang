/**
@file   zip_application.c - Z/IP host controller interface application layer implementation.

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
#include <stdlib.h>
#ifndef OS_MAC_X
#include <malloc.h>
#endif
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef OS_LINUX
#include <netdb.h>
#endif
#include "../include/zip_application.h"
#include "../include/zwave/ZW_transport_api.h"
#include "../include/zwave/ZW_classcmd.h"
#include "../include/zwave/ZW_controller_api.h"


/**
@defgroup Application Application layer APIs
To provide network management and controller functions.
@{
*/

#ifdef  DEBUG_ZIPAPI
/**
ip_addr_show - Display IPv4/IPv6 address and port
@param[in]	plt_ctx		Platform context
@param[in]	sock_addr	Socket address
@param[in]	sock_len	Socket address length
@return
*/
void ip_addr_show(plt_ctx_t *plt_ctx, struct sockaddr *sock_addr, socklen_t sock_len)
{
    char        hostBfr[100];
    char        servBfr[20];

    /*
    ** Display the address-specific information.
    */
    getnameinfo( sock_addr,
                 sock_len,
                 hostBfr,
                 sizeof( hostBfr ),
                 servBfr,
                 sizeof( servBfr ),
                 NI_NUMERICHOST | NI_NUMERICSERV );

    switch (sock_addr->sa_family)
    {
        case AF_INET:   /* IPv4 address. */
            {
                //struct sockaddr_in *p = (struct sockaddr_in*) sock_addr;
                debug_msg_show(plt_ctx, "addr:%s, port:%s\n", hostBfr, servBfr);
/*
                debug_msg_show(plt_ctx, \
                              "   sin_addr  = sin_family: %d\n"
                              "               sin_addr:   %s\n"
                              "               sin_port:   %s\n",
                              p->sin_family,
                              hostBfr,
                              servBfr );
*/
                break;
            }

        case AF_INET6:   /* IPv6 address. */
            {
                //struct sockaddr_in6 *p = (struct sockaddr_in6*) sock_addr;
                debug_msg_show(plt_ctx, "addr:%s, port:%s\n", hostBfr, servBfr);
/*
                debug_msg_show(plt_ctx,  \
                              "   sin6_addr = sin6_family:   %d\n"
                              "               sin6_addr:     %s\n"
                              "               sin6_port:     %s\n"
                              "               sin6_flowinfo: %d\n"
                              "               sin6_scope_id: %d\n",
                              p->sin6_family,
                              hostBfr,
                              servBfr,
                              p->sin6_flowinfo,
                              p->sin6_scope_id );
*/
                break;
            }
    }
}
#endif

/**
appl_updt_node_sts - Update node status
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node
@param[in]	sts		        Status (APPL_NODE_STS_XXX)
@return
*/
void    appl_updt_node_sts(appl_layer_ctx_t *appl_ctx, uint8_t node_id, uint8_t sts)
{
    int node_is_ctlr;   //Flag to indicate the node being updated is the controller

    //debug_msg_show(appl_ctx->plt_ctx, "*****node:%u, sts:%u\n", node_id, sts);
    if (!appl_ctx->cb_thrd_run)
    {
        return;
    }

    if (node_id == 0)
    {   //Controller's node id
        node_id = appl_ctx->ctl_node_id;
        node_is_ctlr = 1;
    }
    else
    {
        node_is_ctlr = 0;
    }

    if ((node_id > 0) && (node_id <= 232))
    {
        if (appl_ctx->node_sts[node_id] != sts)
        {   //Node status has changed

            //Update node status
            appl_ctx->node_sts[node_id] = sts;

            //Callback higher layer to report node status change
            if (appl_ctx->node_sts_cb)
            {
                appl_ctx->node_sts_cb(appl_ctx, node_id, sts);
            }
        }

        if (node_is_ctlr)
        {
            return;
        }

        if (sts == APPL_NODE_STS_UP)
        {
            if (appl_ctx->node_sts[appl_ctx->ctl_node_id] == APPL_NODE_STS_DOWN)
            {
                //If other node is UP, this implies controller is UP too
                //Update controller status
                appl_ctx->node_sts[appl_ctx->ctl_node_id] = APPL_NODE_STS_UP;

                //Callback higher layer to report controller node status change
                if (appl_ctx->node_sts_cb)
                {
                    appl_ctx->node_sts_cb(appl_ctx, appl_ctx->ctl_node_id, APPL_NODE_STS_UP);
                }
            }
        }
    }
}


/**
appl_get_node_sts - Get node status
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node id (ranging from 1 to 232)
@return node status (APPL_NODE_STS_XXX)
*/
uint8_t    appl_get_node_sts(appl_layer_ctx_t *appl_ctx, uint8_t node_id)
{
    if ((node_id > 0) && (node_id <= 232))
    {
        return appl_ctx->node_sts[node_id];
    }

    return APPL_NODE_STS_DOWN;

}


/**
appl_clr_node_sts - Clear all node status
@param[in]	appl_ctx		Context
@return
*/
static void    appl_clr_node_sts(appl_layer_ctx_t *appl_ctx)
{
    memset(appl_ctx->node_sts, 0, 233);
}


/**
appl_get_all_node_sts - Get all node status
@param[in]	appl_ctx		Context
@param[out]	node_sts_buf	Buffer (min. size of 233 bytes)to store all the node status. Individual node status
                            can be access using the node id as index to the buffer.
@return
*/
void    appl_get_all_node_sts(appl_layer_ctx_t *appl_ctx, uint8_t *node_sts_buf)
{
    memcpy(node_sts_buf, appl_ctx->node_sts, 233);
}


/**
appl_seq_num_gen - generate sequence number for the Z/IP frame
@param[in]	appl_ctx		Context
@param[in]	node_id		    Destination node id
@param[in]	wkup_poll		Flag to indicate this sequence number is assigned for wakeup notification polling
@return     The generated sequence number
*/
static uint8_t appl_seq_num_gen(appl_layer_ctx_t *appl_ctx, uint8_t node_id, uint8_t wkup_poll)
{
    uint8_t             seq_num;
    uint16_t            i;
    appl_zip_seq_num_t  *seq_num_db;

    plt_mtx_lck(appl_ctx->zip_seq_num_mtx);

    seq_num_db = appl_ctx->zip_seq_num_db;

    if (wkup_poll)
    {
        //Remove all polling sequence numbers that have been assigned to this node
        for (i=0; i<=0xFF; i++)
        {
            if (seq_num_db[i].seq_num_valid
                && seq_num_db[i].wkup_poll
                && (seq_num_db[i].node_id == node_id))
            {   //Remove it
                seq_num_db[i].seq_num_valid = 0;
                seq_num_db[i].node_id = 0xFF;
                seq_num_db[i].wkup_poll = 0;
            }
        }
    }

    //Assign next unused sequence number
    seq_num = appl_ctx->zip_next_seq_num;
    for (i=0; i<=0xFF; i++)
    {
        if (!seq_num_db[seq_num].seq_num_valid)
        {
            seq_num_db[seq_num].seq_num_valid = 1;
            seq_num_db[seq_num].node_id = node_id;
            seq_num_db[seq_num].wkup_poll = wkup_poll;

            appl_ctx->zip_next_seq_num = seq_num + 1;

            plt_mtx_ulck(appl_ctx->zip_seq_num_mtx);
            return seq_num;
        }
        seq_num++;
    }


    //No more unused sequence number
    //Force using the next used but non polling sequence number
    seq_num = appl_ctx->zip_next_seq_num;
    for (i=0; i<=0xFF; i++)
    {
        if (!seq_num_db[seq_num].wkup_poll)
        {
            seq_num_db[seq_num].seq_num_valid = 1;
            seq_num_db[seq_num].node_id = node_id;
            seq_num_db[seq_num].wkup_poll = wkup_poll;

            appl_ctx->zip_next_seq_num = seq_num + 1;

            plt_mtx_ulck(appl_ctx->zip_seq_num_mtx);
            return seq_num;
        }
        seq_num++;
    }

    plt_mtx_ulck(appl_ctx->zip_seq_num_mtx);
    return seq_num;
}


/**
appl_seq_num_remove - remove Z/IP frame sequence number
@param[in]	appl_ctx    Context
@param[in]	seq_num     Z/IP frame sequence number
@return
*/
static void appl_seq_num_remove(appl_layer_ctx_t *appl_ctx, uint8_t seq_num)
{
    appl_zip_seq_num_t  *seq_num_db;

    plt_mtx_lck(appl_ctx->zip_seq_num_mtx);

    seq_num_db = appl_ctx->zip_seq_num_db;

    if (seq_num_db[seq_num].seq_num_valid)
    {
        seq_num_db[seq_num].seq_num_valid = 0;
        seq_num_db[seq_num].node_id = 0xFF;
        seq_num_db[seq_num].wkup_poll = 0;
    }
    plt_mtx_ulck(appl_ctx->zip_seq_num_mtx);
}


/**
appl_seq_num_rm_by_nodeid - remove Z/IP frame sequence numbers by node id
@param[in]	appl_ctx    Context
@param[in]	node_id     Node id. If 0xFF, all sequence numbers will be removed
@return
*/
void appl_seq_num_rm_by_nodeid(appl_layer_ctx_t *appl_ctx, uint8_t node_id)
{
    uint16_t            i;
    appl_zip_seq_num_t  *seq_num_db;

    plt_mtx_lck(appl_ctx->zip_seq_num_mtx);

    seq_num_db = appl_ctx->zip_seq_num_db;

    if (node_id != 0xFF)
    {
        //Check if there any sequence number been assigned to this node
        for (i=0; i<=0xFF; i++)
        {
            if (seq_num_db[i].seq_num_valid
                && (seq_num_db[i].node_id == node_id))
            {
                seq_num_db[i].seq_num_valid = 0;
                seq_num_db[i].node_id = 0xFF;
                seq_num_db[i].wkup_poll = 0;
            }
        }
    }
    else
    {   //Remove all sequence numbers
        for (i=0; i<=0xFF; i++)
        {
            seq_num_db[i].seq_num_valid = 0;
            seq_num_db[i].node_id = 0xFF;
            seq_num_db[i].wkup_poll = 0;
        }
    }

    plt_mtx_ulck(appl_ctx->zip_seq_num_mtx);
}


/**
appl_seq_num_nodeid_get - Get node id from sequence number
@param[in]	appl_ctx    Context
@param[in]	seq_num     Z/IP frame sequence number
@return Node id if found; else return 0xFF
*/
static uint8_t appl_seq_num_nodeid_get(appl_layer_ctx_t *appl_ctx, uint8_t seq_num)
{
    uint8_t             node_id = 0xFF;
    appl_zip_seq_num_t  *seq_num_db;

    plt_mtx_lck(appl_ctx->zip_seq_num_mtx);

    seq_num_db = appl_ctx->zip_seq_num_db;

    if (seq_num_db[seq_num].seq_num_valid)
    {
        node_id = seq_num_db[seq_num].node_id;
    }
    plt_mtx_ulck(appl_ctx->zip_seq_num_mtx);

    return node_id;
}


/**
zip_send_cb - generic callback for sending the Z/IP network management request
@param[in]	appl_ctx		Context
@param[in]	tx_sts          Transmit completion status
@param[in]	user_prm        User parameter
@param[in]	node_id         Node id
@return
*/
static void zip_send_cb(appl_layer_ctx_t *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    if (tx_sts != 0)
    {
        debug_msg_show(appl_ctx->plt_ctx, "zip_send_cb error:%d, node id:%u", tx_sts, node_id);
    }
}


/**
appl_wait_to_snd - Wait for the condition until sending of command is allowed
@param[in]	appl_ctx		Context
@return  1 on success, 0 on failure
@post   If success, the caller is responsible to unlock the mutex "snd_mtx".
*/
static int  appl_wait_to_snd(appl_layer_ctx_t   *appl_ctx)
{
    int ret;

    plt_mtx_lck(appl_ctx->snd_mtx);
    ret = 0;
    while ((appl_ctx->wait_cmd_cb || appl_ctx->wait_nm_cb)
           && ret == 0)
        ret = plt_cond_timedwait(appl_ctx->snd_cv, appl_ctx->snd_mtx, APPL_WAIT_SEND_TIMEOUT);

    if (ret == 1)
    {   //Wait timeout
        debug_msg_show(appl_ctx->plt_ctx, "Cond wait timeout");
    }

    if ((!appl_ctx->wait_cmd_cb) && (!appl_ctx->wait_nm_cb)){
 //       debug_msg_show(appl_ctx->plt_ctx, "Cond wait success");
        return  1; //Wait success
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);

    return 0;
}


/**
appl_cb_tmout_cb - Timer callback when no callback received after sending a command.
@param[in] data     Pointer to the appl_layer_ctx_t
@return
*/
static void    appl_cb_tmout_cb(void *data)
{
    appl_layer_ctx_t    *appl_ctx = (appl_layer_ctx_t   *)data;
    appl_tmout_ctx_t    cb_ctx;

    debug_msg_show(appl_ctx->plt_ctx, "appl_cb_tmout_cb: timeout");

    plt_mtx_lck(appl_ctx->snd_mtx);

    //Release the waiting for callback flag
    appl_ctx->wait_cmd_cb = 0;

    //Stop the timer and release timer resource
    plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
    appl_ctx->cb_tmr_ctx = NULL;

    //Save the callback function and parameter before releasing the lock
    cb_ctx = appl_ctx->snd_tmout_ctx;
    appl_updt_node_sts(appl_ctx, cb_ctx.node_id, APPL_NODE_STS_DOWN);
    appl_ctx->snd_tmout_ctx.node_id = 0xFF;//invalidate

    //Wake up any wait thread
    plt_cond_signal(appl_ctx->snd_cv);

    plt_mtx_ulck(appl_ctx->snd_mtx);

    //Remove sequence number from the database
    appl_seq_num_remove(appl_ctx, cb_ctx.seq_num);

    //Call the callback function
    if (cb_ctx.send_data_cb)
    {
        cb_ctx.send_data_cb(appl_ctx, APPL_TX_STATUS_TIMEOUT, cb_ctx.snd_dat_cb_prm, cb_ctx.node_id);
    }
}


/**
appl_nm_sts_tmout_cb - Timer callback when no status received after sending a network management command.
@param[in] data     Pointer to the appl_layer_ctx_t
@return
*/
static void    appl_nm_sts_tmout_cb(void *data)
{
    appl_layer_ctx_t    *appl_ctx = (appl_layer_ctx_t   *)data;
    appl_tmout_ctx_t    cb_ctx;

    debug_msg_show(appl_ctx->plt_ctx, "appl_nm_sts_tmout_cb: timeout");

    plt_mtx_lck(appl_ctx->snd_mtx);

    //Release the waiting for callback flag
    appl_ctx->wait_nm_cb = 0;

    //Stop the timer and release timer resource
    plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
    appl_ctx->cb_tmr_ctx = NULL;

    //Save the callback function and parameter before releasing the lock
    cb_ctx = appl_ctx->snd_tmout_ctx;
    appl_updt_node_sts(appl_ctx, cb_ctx.node_id, APPL_NODE_STS_DOWN);
    appl_ctx->snd_tmout_ctx.node_id = 0xFF;//invalidate

    //Wake up any wait thread
    plt_cond_signal(appl_ctx->snd_cv);

    plt_mtx_ulck(appl_ctx->snd_mtx);

    //Remove sequence number from the database
    appl_seq_num_remove(appl_ctx, cb_ctx.seq_num);

    //Call the callback function
    if (cb_ctx.send_data_cb)
    {
        cb_ctx.send_data_cb(appl_ctx, APPL_TX_STATUS_TIMEOUT, cb_ctx.snd_dat_cb_prm, cb_ctx.node_id);
    }
}


/**
appl_cb_thrd - Thread to serve the send status callback requests
@param[in]	data		The session context
@return
*/
static void appl_cb_thrd(void   *data)
{
    appl_layer_ctx_t *appl_ctx = (appl_layer_ctx_t *)data;
    util_lst_t       *cb_req_lst;

    appl_ctx->cb_thrd_sts = 1;
    while (1)
    {
        //Wait for callback request
        plt_sem_wait(appl_ctx->cb_thrd_sem);

        //Check whether to exit the thread
        if (appl_ctx->cb_thrd_run == 0)
        {
            appl_ctx->cb_thrd_sts = 0;
            return;
        }

        cb_req_lst = util_list_get(appl_ctx->cb_thrd_mtx, &appl_ctx->cb_req_hd);

        //Callback
        if (cb_req_lst)
        {
            appl_cb_req_t    *cb_req;

            cb_req = (appl_cb_req_t *)cb_req_lst->wr_buf;

            if (appl_ctx->cb_thrd_run)
            {
                cb_req->cb(appl_ctx, cb_req->tx_sts, cb_req->user_prm, cb_req->node_id);
            }
            free(cb_req_lst);
        }
    }
}


/**
zw_send_old_sts_cb - old (not the current) send status callback
@param[in]	ssn_ctx		Session layer context
@param[in]	tx_sts		Transmit completion status
@param[in]	user_prm    User defined parameters for the callback
@param[in]	rsved       Reserved (Note: should be ignored)
@param[in]	seq_num     Z/IP frame sequence number
@param[in]  src_addr    Z/IP packet source address
@return
*/
static void zw_send_old_sts_cb(ssn_layer_ctx_t *ssn_ctx, int8_t tx_sts, void *user_prm, uint8_t rsved, uint8_t seq_num,
                               struct sockaddr *src_addr)
{
    appl_layer_ctx_t    *appl_ctx = (appl_layer_ctx_t *)user_prm;
    uint8_t             ipaddr[16];
    uint8_t             ipaddr_sz;

    //Remove sequence number from the database
    if (tx_sts != SESSION_ERROR_DEST_BUSY)
    {
        uint8_t node_id;

        node_id = appl_seq_num_nodeid_get(appl_ctx, seq_num);
        appl_seq_num_remove(appl_ctx, seq_num);

        if ((node_id == 0xFF) && src_addr)
        {   //No node is associated with the sequence number
            //Find node from source address
            if (appl_ctx->use_ipv4)
            {
                struct sockaddr_in  *sa4 = (struct sockaddr_in *)src_addr;

                memcpy(ipaddr, &sa4->sin_addr.s_addr, 4);
                ipaddr_sz = 4;

            }
            else
            {
                struct sockaddr_in6  *sa6 = (struct sockaddr_in6 *)src_addr;
                memcpy(ipaddr, sa6->sin6_addr.s6_addr, 16);
                ipaddr_sz = 16;
            }

            zip_translate_ipaddr(appl_ctx, ipaddr, ipaddr_sz, &node_id);

            if (node_id != 0xFF)
            {
                appl_seq_num_rm_by_nodeid(appl_ctx, node_id);
            }
        }

        if ((node_id != 0xFF) && appl_ctx->old_sts_cb)
        {
            appl_ctx->old_sts_cb(appl_ctx, tx_sts, appl_ctx->old_sts_prm, node_id);
        }
    }
}


/**
zip_old_status_cb_set - Set callback function to receive old send status
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to receive old send status
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return
*/
void  zip_old_status_cb_set(appl_layer_ctx_t *appl_ctx, tx_cmplt_cb_t cb, void *cb_prm)
{
    appl_ctx->old_sts_cb = cb;
    appl_ctx->old_sts_prm = cb_prm;
}


/**
zw_send_data_cb - call back high-level application layer to report transmit completion status
@param[in]	ssn_ctx		Session layer context
@param[in]	tx_sts		Transmit completion status
@param[in]	user_prm    User defined parameters for the callback
@param[in]	node_id     Destination node of the transmission
@param[in]	seq_num     Z/IP frame sequence number
@param[in]  src_addr    Z/IP packet source address
@return
*/
static void zw_send_data_cb(ssn_layer_ctx_t *ssn_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id, uint8_t seq_num,
                            struct sockaddr *src_addr)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    tx_cmplt_cb_t       cb;          //transmit completion status callback function

    //Remove sequence number from the database
#ifdef WKUP_BY_MAILBOX_ACK
    if (tx_sts != SESSION_ERROR_DEST_BUSY)
#endif
        appl_seq_num_remove(appl_ctx, seq_num);

    plt_mtx_lck(appl_ctx->snd_mtx);
    if (appl_ctx->wait_cmd_cb)
    {
        appl_ctx->wait_cmd_cb = 0;
        //Stop the timer and release timer resource
        plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
        appl_ctx->cb_tmr_ctx = NULL;

        //Wake up any wait thread
        plt_cond_signal(appl_ctx->snd_cv);
    }

    //Check for consistency
    if ((user_prm == appl_ctx->snd_tmout_ctx.snd_dat_cb_prm) && (node_id == appl_ctx->snd_tmout_ctx.node_id))
    {
        //Save the callback function before releasing the lock
        cb = appl_ctx->snd_tmout_ctx.send_data_cb;
    }
    else
    {
        cb = NULL;
    }

    //Clear the callback function
    appl_ctx->snd_tmout_ctx.send_data_cb = NULL;

    plt_mtx_ulck(appl_ctx->snd_mtx);

    if (cb && appl_ctx->cb_thrd_run)
    {
        int             ret_val;
        appl_cb_req_t   cb_req;

        cb_req.cb = cb;
        cb_req.tx_sts = tx_sts;
        cb_req.user_prm = user_prm;
        cb_req.node_id = node_id;

        ret_val = util_list_add(appl_ctx->cb_thrd_mtx, &appl_ctx->cb_req_hd,
                                 (uint8_t *)&cb_req, sizeof(appl_cb_req_t));
        if (ret_val == 0)
        {
            plt_sem_post(appl_ctx->cb_thrd_sem);
        }

        switch (tx_sts)
        {
            case ZWHCI_NO_ERROR:
                appl_updt_node_sts(appl_ctx, node_id, APPL_NODE_STS_UP);
                break;

            case SESSION_ERROR_SND_FRM_TMOUT:
            case SESSION_ERROR_UNREACHABLE:
#ifdef WKUP_BY_MAILBOX_ACK
            case SESSION_ERROR_DEST_BUSY:
#endif
                appl_updt_node_sts(appl_ctx, node_id, APPL_NODE_STS_DOWN);
                break;
        }
    }
}


/**
zw_nm_cmplt_cb - network management command completion callback
@param[in]	appl_ctx	Application layer context
@return
*/
static void  zw_nm_cmplt_cb(appl_layer_ctx_t *appl_ctx)
{
    plt_mtx_lck(appl_ctx->snd_mtx);
    if (appl_ctx->wait_nm_cb)
    {
        appl_ctx->wait_nm_cb = 0;
        //Stop the timer and release timer resource
        plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
        appl_ctx->cb_tmr_ctx = NULL;

        //Wake up any wait thread
        plt_cond_signal(appl_ctx->snd_cv);

    }
    plt_mtx_ulck(appl_ctx->snd_mtx);
}


/**
zw_send_data_ex - Send data to a node
@param[in]	appl_ctx		Context
@param[in]	prm             The parameters
@param[in]	cb              The callback function on transmit completion
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@param[in]	dest_addr       Destination IP address.  This parameter supersedes prm->node_id
@param[in]	nm_cmd          Flag to indicate whether the command sent is a network management command
@return  0 on success, negative error number on failure
*/
static int zw_send_data_ex(appl_layer_ctx_t *appl_ctx, appl_snd_data_t *prm, tx_cmplt_cb_t cb, void *cb_prm,
                           struct sockaddr *dest_addr, int nm_cmd)
{
    tpt_layer_ctx_t         *tpt_ctx = &appl_ctx->ssn_ctx.frm_ctx.tpt_ctx;
    int                     result;
    struct sockaddr_in6     sa6;
    struct sockaddr_in      sa4;
    ssn_cmd_snd_param_t     snd_prm;

    if (!appl_ctx->cb_thrd_run)
    {
        return ZWHCI_ERROR_SHUTDOWN;
    }

    //Check whether to wait for previous command to complete
    if (cb)
    {
        if (!appl_wait_to_snd(appl_ctx))
            return  APPL_ERROR_WAIT_CB;

        if (!appl_ctx->cb_thrd_run)
        {
            plt_mtx_ulck(appl_ctx->snd_mtx);
            return ZWHCI_ERROR_SHUTDOWN;
        }

        //Save the pointer to callback function and the callback parameter
        appl_ctx->snd_tmout_ctx.send_data_cb = cb;
        appl_ctx->snd_tmout_ctx.snd_dat_cb_prm = cb_prm;
        appl_ctx->snd_tmout_ctx.node_id = prm->node_id;

        //Set the callback parameter
        snd_prm.sts_cb = zw_send_data_cb;
        snd_prm.cb_prm = cb_prm;
        snd_prm.node_id = prm->node_id;
    }
    else
    {
        snd_prm.sts_cb = NULL;
        snd_prm.cb_prm = NULL;
    }

    if (dest_addr)
    {
        snd_prm.dst = dest_addr;
    }
    else if (tpt_ctx->use_ipv4)
    {
        //Convert destination node id to IPv4 address
        sa4.sin_family = AF_INET;
        sa4.sin_port = htons(ZWAVE_HOME_CTL_PORT);

        if (prm->node_id == 0)
        {   // Z/IP router
            memcpy(&sa4.sin_addr.s_addr, &tpt_ctx->zip_router[12], 4);
        }
        else
        {
            if (prm->node_id <= (MAX_ZWAVE_NODE + 1))
            {
                node_ipaddr_t *node_ipaddr_ent;

                node_ipaddr_ent = &appl_ctx->node_ip_tbl[prm->node_id-1];

                if (node_ipaddr_ent->valid)
                {
                    memcpy(&sa4.sin_addr.s_addr, node_ipaddr_ent->ipaddr, node_ipaddr_ent->ipaddr_sz);
                }
                else
                {
                    if (cb)
                    {
                        plt_mtx_ulck(appl_ctx->snd_mtx);
                    }
                    return APPL_ERROR_RSLV_NODE_ID;
                }
            }
            else
            {
                if (cb)
                {
                    plt_mtx_ulck(appl_ctx->snd_mtx);
                }
                return APPL_INVALID_NODE_ID;
            }
        }

        snd_prm.dst = (struct sockaddr *)&sa4;

    }
    else
    {
        //Convert destination node id to IPv6 address
        sa6.sin6_family = AF_INET6;
        sa6.sin6_port = htons(ZWAVE_HOME_CTL_PORT);
        sa6.sin6_flowinfo = 0;
        sa6.sin6_scope_id = 0;

        if (prm->node_id == 0)
        {   // Z/IP router
            memcpy(sa6.sin6_addr.s6_addr, tpt_ctx->zip_router, IPV6_ADDR_LEN);
        }
        else
        {
            if (prm->node_id <= (MAX_ZWAVE_NODE + 1))
            {
                node_ipaddr_t *node_ipaddr_ent;

                node_ipaddr_ent = &appl_ctx->node_ip_tbl[prm->node_id-1];

                if (node_ipaddr_ent->valid)
                {
                    memcpy(sa6.sin6_addr.s6_addr, node_ipaddr_ent->ipaddr, node_ipaddr_ent->ipaddr_sz);
                }
                else
                {
                    if (cb)
                    {
                        plt_mtx_ulck(appl_ctx->snd_mtx);
                    }
                    return APPL_ERROR_RSLV_NODE_ID;
                }
            }
            else
            {
                if (cb)
                {
                    plt_mtx_ulck(appl_ctx->snd_mtx);
                }
                return APPL_INVALID_NODE_ID;
            }
        }

        snd_prm.dst = (struct sockaddr *)&sa6;
    }
#ifdef WKUP_BY_MAILBOX_ACK
    snd_prm.seq_num = appl_seq_num_gen(appl_ctx, prm->node_id, prm->wkup_poll);
#else
    snd_prm.seq_num = appl_seq_num_gen(appl_ctx, prm->node_id, 0);
#endif
    if (cb)
    {
        appl_ctx->snd_tmout_ctx.seq_num = snd_prm.seq_num;
    }
    snd_prm.cmd_buf = prm->dat_buf;
    snd_prm.cmd_len = prm->dat_len;
    snd_prm.ep_addr = prm->ep_addr;
    snd_prm.flag = prm->flag;
    snd_prm.encap_fmt = prm->encap_fmt;
    snd_prm.wkup_poll = prm->wkup_poll;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &snd_prm);

    if (result == 0)
    {
        if (cb)
        {
            if (nm_cmd)
            {
                //Set flag to indicate waiting for callback to prevent the next command from sending
                appl_ctx->wait_nm_cb = 1;
                // Start the network management status callback timer
                appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_nm_sts_tmout_cb, appl_ctx);

            }
            else
            {
                //Set flag to indicate waiting for callback to prevent the next command from sending
                appl_ctx->wait_cmd_cb = 1;
                // Start the send status callback timer
                appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_cb_tmout_cb, appl_ctx);
            }

            if (!appl_ctx->cb_tmr_ctx)
            {   //timer not working
                debug_msg_show(appl_ctx->plt_ctx, "Error: appl callback timer not working");
                result = SESSION_ERROR_SYSTEM;
            }
        }
    }
    else
    {
        debug_msg_show(appl_ctx->plt_ctx, "ssn_cmd_snd with error:%d\n", result);
        appl_seq_num_remove(appl_ctx, snd_prm.seq_num);
    }

    if (cb)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
    }
    return result;

}


/**
zw_send_data - Send data to a node
@param[in]	appl_ctx		Context
@param[in]	prm             The parameters
@param[in]	cb              The callback function on transmit completion
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return  0 on success, negative error number on failure
*/
int    zw_send_data(appl_layer_ctx_t *appl_ctx, appl_snd_data_t *prm, tx_cmplt_cb_t cb, void *cb_prm)
{
    return zw_send_data_ex(appl_ctx, prm, cb, cb_prm, NULL, 0);
}


/**
appl_nmgmt_proxy - Handle network management proxy commands
@param[in]	appl_ctx	Application layer context
@param[in]	cmd_buf     Command buffer
@param[in]	cmd_len     Command length
@return
*/
static void appl_nmgmt_proxy(appl_layer_ctx_t *appl_ctx,
                             uint8_t *cmd_buf, uint16_t cmd_len)
{
    switch(cmd_buf[1])
    {
        case NODE_LIST_REPORT:
            if (cmd_len >= 34)
            {
                node_list_get_cb_t cb;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                if (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NODE_LIST_GET].seq_num)
                {   //Solicited node list
                    cb = appl_ctx->nm_cb_info[NM_OP_NODE_LIST_GET].cb;
                    appl_ctx->nm_cb_info[NM_OP_NODE_LIST_GET].cb = NULL;
                }
                else
                {
                    cb = NULL;
                }

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3], cmd_buf[4], cmd_buf + 5, 29);
                }
                else if (appl_ctx->nodelst_cb)
                {   //unsolicited node list
                    appl_ctx->nodelst_cb(appl_ctx, cmd_buf[3], cmd_buf[4], cmd_buf + 5, 29);
                }
            }
            break;

        case NODE_INFO_CACHED_REPORT:
            if (cmd_len >= 7)
            {
                int                     parse_ok = 0;
                node_info_chd_get_cb_t  cb;
                appl_node_info_t        ni;

                memset(&ni, 0, sizeof(appl_node_info_t));

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NODE_INFO_CACHED_GET].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_NODE_INFO_CACHED_GET].cb : NULL;

                //Get saved node id
                ni.node_id = appl_ctx->nm_cb_info[NM_OP_NODE_INFO_CACHED_GET].node_id;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    ni.status = cmd_buf[3] >> 4;
                    ni.age = cmd_buf[3] & 0x0F;

                    ni.listen = (cmd_buf[4] & 0x80)? 1 : 0;
                    ni.optional = (cmd_buf[5] & 0x80)? 1 : 0;
                    ni.sensor = (cmd_buf[5] & 0x60)? 1 : 0;

                    if (cmd_len >= 10)
                    {
                        ni.basic = cmd_buf[7];
                        ni.gen = cmd_buf[8];
                        ni.spec = cmd_buf[9];

                        if(util_cmd_cls_parse(cmd_buf + 10, cmd_len - 10, &ni.cmd_cls, &ni.cmd_cnt, &ni.cmd_cls_sec, &ni.cmd_cnt_sec))
                        {
                            parse_ok = 1;
                        }

                    }

                    cb(appl_ctx, &ni);
                    if (parse_ok)
                    {
                        free(ni.cmd_cls);
                        free(ni.cmd_cls_sec);
                    }
                }
            }
            break;
    }
}


/**
appl_nmgmt_basic - Handle network management basic commands
@param[in]	appl_ctx	Application layer context
@param[in]	cmd_buf     Command buffer
@param[in]	cmd_len     Command length
@return
*/
static void appl_nmgmt_basic(appl_layer_ctx_t *appl_ctx,
                              uint8_t *cmd_buf, uint16_t cmd_len)
{
    switch (cmd_buf[1])
    {
        case DEFAULT_SET_COMPLETE:
            if (cmd_len >= 4)
            {
                set_deflt_cb_t  cb;
                uint8_t         nodeid;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_DEFAULT_SET].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_DEFAULT_SET].cb : NULL;

                //Get saved node id
                nodeid = appl_ctx->nm_cb_info[NM_OP_DEFAULT_SET].node_id;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3], nodeid);
                }
            }
            break;

        case LEARN_MODE_SET_STATUS:
            if (cmd_len >= 6)
            {
                set_lrn_mod_cb_t    cb;
                appl_lrn_mod_sec2_t cb_prm = {0};

                cb_prm.sts = cmd_buf[3];
                cb_prm.node_id = cmd_buf[5];

                if (cmd_len >= 23)
                {
                    cb_prm.sec2_valid = 1;
                    cb_prm.grnt_keys = cmd_buf[6];
                    memcpy(cb_prm.dsk, cmd_buf + 7, MAX_DSK_KEY_LEN);
                }

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_LEARN_MODE_SET].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_LEARN_MODE_SET].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, &cb_prm);
                }

            }
            break;

        case NETWORK_UPDATE_REQUEST_STATUS:
            if (cmd_len >= 4)
            {
                req_nw_updt_cb_t    cb;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NETWORK_UPDATE_REQUEST].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_NETWORK_UPDATE_REQUEST].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3]);
                }

            }
            break;

        case DSK_REPORT_V2:
            if (cmd_len >= 19)
            {
                get_dsk_cb_t    cb;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
//              cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_DSK_GET].seq_num)?
//                    appl_ctx->nm_cb_info[NM_OP_DSK_GET].cb : NULL;

                cb = appl_ctx->nm_cb_info[NM_OP_DSK_GET].cb; //Work around for Z/IP gateway returns diff. seq. number

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf + 3, cmd_len - 3);
                }

            }
            break;

    }
}


/**
appl_nmgmt_inclusion - Handle network management inclusion commands
@param[in]	appl_ctx	Application layer context
@param[in]	cmd_buf     Command buffer
@param[in]	cmd_len     Command length
@return
*/
static void appl_nmgmt_inclusion(appl_layer_ctx_t *appl_ctx,
                                 uint8_t *cmd_buf, uint16_t cmd_len)
{
    switch (cmd_buf[1])
    {
        case NODE_ADD_STATUS:
            if (cmd_len >= 7)
            {
                int                     parse_ok = 0;
                add_node_nw_cb_t        cb;
                appl_node_add_cb_prm_t  cb_prm = {0};
                appl_node_info_sec2_t   *add_ni;

                cb_prm.rpt_type = NODE_ADD_STATUS;
                add_ni = &cb_prm.rpt_prm.node_info;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NODE_ADD].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_NODE_ADD].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                add_ni->node_info.status = cmd_buf[3];

                add_ni->node_info.node_id = cmd_buf[5];

                //Check if there is any node info, i.e. basic/generic/specific device class and command classes.
                if (cmd_buf[6] > 1)
                {
                    uint8_t ni_len;

                    ni_len = cmd_buf[6] - 1;

                    if ((ni_len > 0) && (cmd_len >= ni_len + 7))
                    {
                        add_ni->node_info.listen = (cmd_buf[7] & 0x80)? 1 : 0;
                        add_ni->node_info.optional = (cmd_buf[8] & 0x80)? 1 : 0;
                        add_ni->node_info.sensor = (cmd_buf[8] & 0x60)? 1 : 0;

                        if (ni_len >= 5)
                        {
                            add_ni->node_info.basic = cmd_buf[9];
                            add_ni->node_info.gen = cmd_buf[10];
                            add_ni->node_info.spec = cmd_buf[11];
                            if(util_cmd_cls_parse(cmd_buf + 12, ni_len - 5, &add_ni->node_info.cmd_cls,
                                                  &add_ni->node_info.cmd_cnt, &add_ni->node_info.cmd_cls_sec,
                                                  &add_ni->node_info.cmd_cnt_sec))
                            {
                                parse_ok = 1;
                            }
                        }
                    }

                    //Check for security 2
                    if (cmd_len >= ni_len + 9)
                    {
                        add_ni->sec2_valid = 1;
                        add_ni->grnt_keys = cmd_buf[7 + ni_len];
                        add_ni->key_xchg_fail_type = cmd_buf[8 + ni_len];
                    }
                }

                if (cb)
                {
                    cb(appl_ctx, &cb_prm);
                }

                if (parse_ok)
                {
                    free(add_ni->node_info.cmd_cls);
                    free(add_ni->node_info.cmd_cls_sec);
                }
            }
            break;

        case NODE_REMOVE_STATUS:
            if (cmd_len >= 5)
            {
                rm_node_nw_cb_t    cb;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NODE_REMOVE].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_NODE_REMOVE].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3], cmd_buf[4]);
                }

            }
            break;

        case FAILED_NODE_REMOVE_STATUS:
            if (cmd_len >= 5)
            {
                rm_failed_node_cb_t     cb;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_ID_REMOVE].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_ID_REMOVE].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3], cmd_buf[4]);
                }

            }
            break;

        case FAILED_NODE_REPLACE_STATUS:
            if (cmd_len >= 5)
            {
                add_node_nw_cb_t        cb;
                appl_node_add_cb_prm_t  cb_prm = {0};
                appl_node_rp_sec2_t     *node_rp_sts;

                cb_prm.rpt_type = FAILED_NODE_REPLACE_STATUS;
                node_rp_sts = &cb_prm.rpt_prm.node_rp_info;

                node_rp_sts->sts = cmd_buf[3];
                node_rp_sts->node_id = cmd_buf[4];

                if (cmd_len >= 7)
                {   //Security 2 supported
                    node_rp_sts->sec2_valid = 1;
                    node_rp_sts->grnt_keys = cmd_buf[5];
                    node_rp_sts->key_xchg_fail_type = cmd_buf[6];
                }

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, &cb_prm);
                }

            }
            break;

        case NODE_NEIGHBOR_UPDATE_STATUS:
            if (cmd_len >= 4)
            {
                req_node_nbr_updt_cb_t  cb;
                uint8_t                 node_id;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_REQ_NODE_NBR_UPDATE].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_REQ_NODE_NBR_UPDATE].cb : NULL;

                node_id = appl_ctx->nm_cb_info[NM_OP_REQ_NODE_NBR_UPDATE].node_id;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3], node_id);
                }

            }
            break;

        case RETURN_ROUTE_ASSIGN_COMPLETE:
        case RETURN_ROUTE_DELETE_COMPLETE:
            if (cmd_len >= 4)
            {
                ret_route_cb_t  cb;
                uint8_t         node_id;
                int             i;

                i = (cmd_buf[1] == RETURN_ROUTE_ASSIGN_COMPLETE)? NM_OP_ASSIGN_RET_ROUTE : NM_OP_DELETE_RET_ROUTE;
                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[i].seq_num)?
                      appl_ctx->nm_cb_info[i].cb : NULL;

                node_id = appl_ctx->nm_cb_info[i].node_id;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                zw_nm_cmplt_cb(appl_ctx);

                if (cb)
                {
                    cb(appl_ctx, cmd_buf[3], node_id);
                }

            }
            break;

        case NODE_ADD_KEYS_REPORT_V2:
            if (cmd_len >= 4)
            {
                add_node_nw_cb_t        cb;
                appl_node_add_cb_prm_t  cb_prm = {0};

                cb_prm.rpt_type = NODE_ADD_KEYS_REPORT_V2;
                cb_prm.rpt_prm.req_keys = cmd_buf[3];

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                //Try for add node callback
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NODE_ADD].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_NODE_ADD].cb : NULL;
                //Try for replace node callback
                if (!cb)
                {
                    cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].seq_num)?
                          appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].cb : NULL;
                }

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, &cb_prm);
                }
            }
            break;

        case NODE_ADD_DSK_REPORT_V2:
            if (cmd_len >= 4)
            {
                add_node_nw_cb_t        cb;
                appl_node_add_cb_prm_t  cb_prm = {0};
                uint8_t                 dsk_len;

                dsk_len = cmd_buf[3] & 0x1F;

                if (dsk_len > MAX_DSK_KEY_LEN)
                {
                    break;
                }

                if (cmd_len < (dsk_len + 4))
                {
                    break;
                }

                cb_prm.rpt_type = NODE_ADD_DSK_REPORT_V2;
                cb_prm.rpt_prm.dsk_rpt.key_len = dsk_len;
                memcpy(cb_prm.rpt_prm.dsk_rpt.dsk_keys, cmd_buf + 4, dsk_len);

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                //Try for add node callback
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_NODE_ADD].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_NODE_ADD].cb : NULL;
                //Try for replace node callback
                if (!cb)
                {
                    cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].seq_num)?
                          appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].cb : NULL;
                }

                plt_mtx_ulck(appl_ctx->zip_mtx);

                if (cb)
                {
                    cb(appl_ctx, &cb_prm);
                }
            }
            break;
    }
}


/**
appl_nmgmt_primary - Handle network management primary commands
@param[in]	appl_ctx	Application layer context
@param[in]	cmd_buf     Command buffer
@param[in]	cmd_len     Command length
@return
*/
static void appl_nmgmt_primary(appl_layer_ctx_t *appl_ctx,
                                 uint8_t *cmd_buf, uint16_t cmd_len)
{
    switch (cmd_buf[1])
    {
        case CONTROLLER_CHANGE_STATUS:
            if (cmd_len >= 6)
            {
                int                 parse_ok = 0;
                ctlr_chg_nw_cb_t    cb;
                appl_node_info_t    add_ni;

                plt_mtx_lck(appl_ctx->zip_mtx);
                //Check for sequence number
                cb = (cmd_buf[2] == appl_ctx->nm_cb_info[NM_OP_CONTROLLER_CHANGE].seq_num)?
                      appl_ctx->nm_cb_info[NM_OP_CONTROLLER_CHANGE].cb : NULL;

                plt_mtx_ulck(appl_ctx->zip_mtx);

                memset(&add_ni, 0, sizeof(appl_node_info_t));

                add_ni.status = cmd_buf[3];

                add_ni.node_id = cmd_buf[5];

                //Check if there is any node info, i.e. basic/generic/specific device class and command classes.
                if ((add_ni.status == ADD_NODE_STATUS_DONE) && (cmd_len >= 7) && (cmd_buf[6] > 1))
                {
                    uint8_t ni_len;

                    ni_len = cmd_buf[6] - 1;

                    if ((ni_len > 0) && (cmd_len >= ni_len + 7))
                    {
                        add_ni.listen = (cmd_buf[7] & 0x80)? 1 : 0;
                        add_ni.optional = (cmd_buf[8] & 0x80)? 1 : 0;
                        add_ni.sensor = (cmd_buf[8] & 0x60)? 1 : 0;

                        if (ni_len >= 5)
                        {
                            add_ni.basic = cmd_buf[9];
                            add_ni.gen = cmd_buf[10];
                            add_ni.spec = cmd_buf[11];
                            if(util_cmd_cls_parse(cmd_buf + 12, ni_len - 5, &add_ni.cmd_cls,
                                                  &add_ni.cmd_cnt, &add_ni.cmd_cls_sec, &add_ni.cmd_cnt_sec))
                            {
                                parse_ok = 1;
                            }

                        }
                    }
                }


                if (cb)
                {
                    cb(appl_ctx, &add_ni);
                }
                if (parse_ok)
                {
                    free(add_ni.cmd_cls);
                    free(add_ni.cmd_cls_sec);
                }
            }
            break;
    }
}


/**
appl_zip_hdlr - Handle zip commands
@param[in]	appl_ctx	Application layer context
@param[in]	cmd_buf     Command buffer
@param[in]	cmd_len     Command length
@return
*/
static void appl_zip_hdlr(appl_layer_ctx_t *appl_ctx,
                          uint8_t *cmd_buf, uint16_t cmd_len)
{
    switch (cmd_buf[1])
    {
        case ZIP_NODE_ADVERTISEMENT:
            if (cmd_len >= ZIP_FRM_ND_ADVT_SZ)
            {
                node_advt_cb_t      cb;
                nd_advt_rpt_t       rpt;
                uint8_t             *homeid;

                plt_mtx_lck(appl_ctx->zip_mtx);
                cb = appl_ctx->node_advt_cb;
                plt_mtx_ulck(appl_ctx->zip_mtx);

                rpt.addr_local = (cmd_buf[2] & 0x04)? 1 : 0;
                rpt.status = cmd_buf[2] & 0x03;
                rpt.node_id = cmd_buf[3];
                memcpy(&rpt.ipv6_addr, cmd_buf + 4, IPV6_ADDR_LEN);
                homeid = cmd_buf + 4 + IPV6_ADDR_LEN;
                rpt.homeid = ((uint32_t)homeid[0])<<24 | ((uint32_t)homeid[1])<<16
                             | ((uint32_t)homeid[2])<<8 | ((uint32_t)homeid[3]);


                if (cb)
                {
                    cb(appl_ctx, &rpt);
                }
            }
            break;
    }
}


/**
appl_nw_mgmt_hdlr - Handle network management command
@param[in]	appl_ctx	Application layer context
@param[in]	cmd_buf     Command buffer
@param[in]	cmd_len     Command length
@return     1=command is handled; 0=command is not network management command
*/
static int  appl_nw_mgmt_hdlr(appl_layer_ctx_t *appl_ctx,
                              uint8_t *cmd_buf, uint16_t cmd_len)
{
    switch(cmd_buf[0])
    {
        case COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY:
            appl_nmgmt_proxy(appl_ctx, cmd_buf, cmd_len);
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC:
            appl_nmgmt_basic(appl_ctx, cmd_buf, cmd_len);
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION:
            appl_nmgmt_inclusion(appl_ctx, cmd_buf, cmd_len);
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY:
            appl_nmgmt_primary(appl_ctx, cmd_buf, cmd_len);
            break;

        case COMMAND_CLASS_ZIP_ND: //Strictly speaking, this is not a network management function
            appl_zip_hdlr(appl_ctx, cmd_buf, cmd_len);
            break;

        default:
            //Not network management command
            return 0;

    }

    return 1;
}


/**
appl_unsolicited_cmd_cb - Despatch the unsolicited command received
@param[in]	ssn_ctx		Session layer context
@param[in]	src_addr	Source address
@param[in]	ep_addr	    Source and destination endpoint addresses
@param[in]	buf         Command buffer
@param[in]	len         Command length
@param[in] flag         Flag, see ZIP_FLAG_XXX
@param[in]	encap_fmt   Z-wave encapsulation format (valid if flag ZIP_FLAG_ENCAP_INFO is set).
                        This format should be passed back when responding
                        to unsolicited command with the flag ZIP_FLAG_ENCAP_INFO is set.
@return
*/
static void    appl_unsolicited_cmd_cb(struct _ssn_layer_ctx *ssn_ctx, struct sockaddr *src_addr,
                                       frm_ep_t *ep_addr, uint8_t *buf, uint16_t len, uint16_t flag,
                                       uint16_t encap_fmt)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;

#ifdef SHOW_PACKAT_INFO
    debug_msg_show(appl_ctx->plt_ctx, "Appl layer rcx command:");
    debug_bin_show(appl_ctx->plt_ctx, buf, len);
    debug_msg_show(appl_ctx->plt_ctx, "From:");
    #ifdef  DEBUG_ZIPAPI
    if (appl_ctx->use_ipv4)
    {
        ip_addr_show(appl_ctx->plt_ctx, src_addr, sizeof(struct sockaddr_in));
    }
    else
    {
        ip_addr_show(appl_ctx->plt_ctx, src_addr, sizeof(struct sockaddr_in6));
    }
    #endif
#endif

    //Handle command
    if (appl_ctx->cb_thrd_run && buf && (len >= 2))
    {
        if (appl_nw_mgmt_hdlr(appl_ctx, buf, len) == 0)
        {   //The command is not a network management command
            //Call back to user application command handler
            appl_ctx->application_command_handler_cb(appl_ctx, src_addr, ep_addr, buf, len, flag, encap_fmt);
        }
    }
}


/**
zip_node_list_get - Get node list from Z/IP router through callback function
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function for receiving node list
@return  0 on success, negative error number on failure
*/
int zip_node_list_get(appl_layer_ctx_t   *appl_ctx, node_list_get_cb_t cb)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY, NODE_LIST_GET, 0 };


    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Node list get sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_NODE_LIST_GET].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_NODE_LIST_GET].seq_num = cmd[2];
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zip_node_info_chd_get - Get cached node info from Z/IP router through callback function
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
int zip_node_info_chd_get(appl_layer_ctx_t   *appl_ctx, node_info_chd_get_cb_t cb, uint8_t nodeid, uint8_t max_age)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY, NODE_INFO_CACHED_GET, 0, 0, 0};

    //Max age
    if (max_age > 0x0F)
    {
        max_age = 0x0F;
    }
    cmd[3] = max_age;

    //Node id
    cmd[4] = nodeid;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Node info cached get sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_NODE_INFO_CACHED_GET].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_NODE_INFO_CACHED_GET].seq_num = cmd[2];
        appl_ctx->nm_cb_info[NM_OP_NODE_INFO_CACHED_GET].node_id = nodeid;
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zip_node_id_get - Resolve IPv6 address into node id through callback function
@param[in]	appl_ctx		Context
@param[in]  tgt_ip          Target IPv6 address of the Z-wave node
@param[in]	cb              The callback function for receiving node id
@param[in]	retry           Flag to indicate if this call is second attempt to get node id
@return  0 on success, negative error number on failure
*/
int zip_node_id_get(appl_layer_ctx_t *appl_ctx, node_advt_cb_t cb, uint8_t *tgt_ip, int retry)
{
    tpt_layer_ctx_t         *tpt_ctx = &appl_ctx->ssn_ctx.frm_ctx.tpt_ctx;
    int                     result;
    struct sockaddr         *sock_addr;
    struct sockaddr_in6     zip_router_addr6;
    struct sockaddr_in      zip_router_addr4;

    if (tpt_ctx->use_ipv4)
    {
        zip_router_addr4.sin_family = AF_INET;
        zip_router_addr4.sin_port = htons(ZWAVE_HOME_CTL_PORT);
        memcpy(&zip_router_addr4.sin_addr.s_addr, &tpt_ctx->zip_router[12], 4);
        sock_addr = (struct sockaddr *)&zip_router_addr4;
    }
    else
    {
        zip_router_addr6.sin6_family = AF_INET6;
        zip_router_addr6.sin6_port = htons(ZWAVE_HOME_CTL_PORT);
        zip_router_addr6.sin6_flowinfo = 0;
        zip_router_addr6.sin6_scope_id = 0;
        memcpy(zip_router_addr6.sin6_addr.s6_addr, tpt_ctx->zip_router, IPV6_ADDR_LEN);
        sock_addr = (struct sockaddr *)&zip_router_addr6;
    }

    plt_mtx_lck(appl_ctx->zip_mtx);
    result = frm_node_id_get(&appl_ctx->ssn_ctx.frm_ctx, sock_addr, tgt_ip, retry);
    if (result == 0)
    {   //Save the callback function
        appl_ctx->node_advt_cb = cb;
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zip_node_ipv6_get - Resolve a node id into IPv6 address through callback function
@param[in]	appl_ctx		Context
@param[in]  node_id         Z-wave node id
@param[in]	cb              The callback function for receiving node list
@return  0 on success, negative error number on failure
*/
int zip_node_ipv6_get(appl_layer_ctx_t *appl_ctx, node_advt_cb_t cb, uint8_t node_id)
{
    tpt_layer_ctx_t         *tpt_ctx = &appl_ctx->ssn_ctx.frm_ctx.tpt_ctx;
    int                     result;
    struct sockaddr         *sock_addr;
    struct sockaddr_in6     zip_router_addr6;
    struct sockaddr_in      zip_router_addr4;

    if (tpt_ctx->use_ipv4)
    {
        zip_router_addr4.sin_family = AF_INET;
        zip_router_addr4.sin_port = htons(ZWAVE_HOME_CTL_PORT);
        memcpy(&zip_router_addr4.sin_addr.s_addr, &tpt_ctx->zip_router[12], 4);
        sock_addr = (struct sockaddr *)&zip_router_addr4;
    }
    else
    {
        zip_router_addr6.sin6_family = AF_INET6;
        zip_router_addr6.sin6_port = htons(ZWAVE_HOME_CTL_PORT);
        zip_router_addr6.sin6_flowinfo = 0;
        //zip_router_addr6.sin6_scope_id = if_nametoindex(tpt_ctx->host_addr.intf);
        zip_router_addr6.sin6_scope_id = 0;
        memcpy(zip_router_addr6.sin6_addr.s6_addr, tpt_ctx->zip_router, IPV6_ADDR_LEN);
        sock_addr = (struct sockaddr *)&zip_router_addr6;
    }

    plt_mtx_lck(appl_ctx->zip_mtx);
    result = frm_node_ipv6_get(&appl_ctx->ssn_ctx.frm_ctx, sock_addr, node_id);
    if (result == 0)
    {   //Save the callback function
        appl_ctx->node_advt_cb = cb;
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_set_default - Set the Controller back to the factory default state
@param[in]	appl_ctx		Context
@param[in]	nodeid          Node id. If zero, the destination is Z/IP router
@param[in]	cb              The callback function to report completion.
@return  0 on success, negative error number on failure
*/
int    zw_set_default(appl_layer_ctx_t   *appl_ctx, set_deflt_cb_t cb, uint8_t nodeid)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC, DEFAULT_SET, 0 };

    snd_prm.node_id = nodeid;
    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Node list get sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_DEFAULT_SET].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_DEFAULT_SET].seq_num = cmd[2];
        appl_ctx->nm_cb_info[NM_OP_DEFAULT_SET].node_id = nodeid;
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    //Clear all node alive status
    appl_clr_node_sts(appl_ctx);

    return result;
}


/**
zw_set_learn_mode - Set learn mode, used to add or remove the controller to/from a Z-Wave network.
@param[in]	appl_ctx		Context
@param[in]	nodeid          Node id. If zero, the destination is Z/IP router
@param[in]	mode            Mode (ZW_SET_LEARN_MODE_XXX) could be to start the learn mode in classic or NWI or to stop the learn mode.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/
int    zw_set_learn_mode(appl_layer_ctx_t *appl_ctx, set_lrn_mod_cb_t cb, uint8_t nodeid, uint8_t  mode)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC, LEARN_MODE_SET, 0, 0, 0};

    //Set mode
    cmd[4] = mode;

    snd_prm.node_id = nodeid;
    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Set learn mode sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_LEARN_MODE_SET].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_LEARN_MODE_SET].seq_num = cmd[2];
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_sec2_dsk_get - Get Z/IP gateway DSK
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report Z/IP gateway DSK.
@return  0 on success, negative error number on failure
*/
int    zw_sec2_dsk_get(appl_layer_ctx_t *appl_ctx, get_dsk_cb_t cb)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC, DSK_GET_V2, 0};

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_DSK_GET].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_DSK_GET].seq_num = cmd[2];
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zip_node_info_send - Send node information from a node to another node or all nodes.
@param[in]	appl_ctx    Context
@param[in]	nif_prm     Send node information frame parameters
@return  0 on success, negative error number on failure
*/
int    zip_node_info_send(appl_layer_ctx_t   *appl_ctx, appl_snd_nif_t  *nif_prm)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC, NODE_INFORMATION_SEND, 0, 0, 0, 0};

    //Destination node
    cmd[4] = nif_prm->to_node_id;

    //Transmit options
    cmd[5] = nif_prm->tx_opt;

    snd_prm.node_id = nif_prm->fr_node_id;
    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Send node information sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, nif_prm->cb, nif_prm->cb_prm);

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_request_network_update - Request network topology updates from the SUC/SIS node.
                            Secondary controllers can only use this call when a SUC is present in the network.
                            All controllers can use this call in case a SUC ID Server (SIS) is available.
                            Routing Slaves can only use this call, when a SUC is present in the network.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function on completion.
@return  0 on success, negative error number on failure.
*/
int    zw_request_network_update(appl_layer_ctx_t   *appl_ctx, req_nw_updt_cb_t cb)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC, NETWORK_UPDATE_REQUEST, 0 };

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Request network topology update sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_NETWORK_UPDATE_REQUEST].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_NETWORK_UPDATE_REQUEST].seq_num = cmd[2];
    }
    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_sec2_key_set - Security 2 key set
@param[in]	appl_ctx		Context
@param[in]	granted_key     Granted key
@return  0 on success, negative error number on failure
*/
int    zw_sec2_key_set(appl_layer_ctx_t *appl_ctx, uint8_t granted_key)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, NODE_ADD_KEYS_SET_V2, 0, 0, 0};

    //Check whether to accept or reject the request
    if (granted_key)
    {
        cmd[3] = 1;
    }

    //Granted key
    cmd[4] = granted_key;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Add node sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_sec2_dsk_set - Security 2 dsk set
@param[in]	appl_ctx	Context
@param[in]	accept      Flag to indicate whether to accept the joining node DSK
@param[in]	dsk_len     Number of bytes (max. 16 bytes) of DSK to send.
@param[in]	dsk         DSK to send
@return  0 on success, negative error number on failure
*/
int    zw_sec2_dsk_set(appl_layer_ctx_t *appl_ctx, int accept, uint8_t dsk_len, uint8_t *dsk)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, NODE_ADD_DSK_SET_V2, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (dsk_len > MAX_DSK_KEY_LEN)
    {
        return -1;
    }

    //Check whether to accept or reject the request
    if (accept)
    {
        cmd[3] = (dsk_len | 0x80);
        memcpy(cmd + 4, dsk, dsk_len);
    }
    else
    {   //reject
        dsk_len = 0;
    }

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = dsk_len + 4;
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Add node sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_add_node_to_network - Add node to a network
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report status.
@param[in]	mode            Mode (ADD_NODE_XXX) to control the add node process.
@param[in]	tx_opt          Transmit options (TRANSMIT_OPTION_XXX) to control if transmissions must use special properties.
@return  0 on success, negative error number on failure
*/
int    zw_add_node_to_network(appl_layer_ctx_t   *appl_ctx, add_node_nw_cb_t cb, uint8_t  mode, uint8_t tx_opt)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, NODE_ADD, 0, 0, 0, 0};

    /*
    WARNING: The Node Add state should ALWAYS be disabled after use to avoid adding other nodes than
    expected. It is recommended to have a timer that disables the Node Add state after a while without any
    activity.
    */

    //Mode
    cmd[4] = mode;

    //Transmit options
    cmd[5] = tx_opt;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Add node sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_NODE_ADD].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_NODE_ADD].seq_num = cmd[2];
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_remove_node_from_network - Remove node from a network
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report status.
@param[in]	mode            Mode (REMOVE_NODE_XXX) to control the remove node process.
@return  0 on success, negative error number on failure
*/
int    zw_remove_node_from_network(appl_layer_ctx_t   *appl_ctx, rm_node_nw_cb_t cb, uint8_t  mode)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, NODE_REMOVE, 0, 0, 0};

    //Mode
    cmd[4] = mode;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Remove node sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_NODE_REMOVE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_NODE_REMOVE].seq_num = cmd[2];
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_remove_failed_node_id - Remove a non-responding node from the routing table in the requesting controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the failed node to be deleted.
@param[in]	cb              The callback function on remove process completion.
@return  0 on success, negative error number on failure.
@pre    The node must be on the failed node ID list and as an extra precaution also fail to respond before it is removed.
*/
int    zw_remove_failed_node_id(appl_layer_ctx_t   *appl_ctx, rm_failed_node_cb_t cb, uint8_t node_id)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, FAILED_NODE_REMOVE, 0, 0};

    //Node id to be removed
    cmd[3] = node_id;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Remove node sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_ID_REMOVE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_ID_REMOVE].seq_num = cmd[2];
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_replace_failed_node - Replace a non-responding node with a new one in the requesting controller.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function on replace process completion.
@param[in]	node_id         The Node ID (1...232) of the failed node to be deleted.
@param[in]	tx_opt          Transmit options: TRANSMIT_OPTION_LOW_POWER
@param[in]	mode            Mode to control start (START_FAILED_NODE_REPLACE)/stop (STOP_FAILED_NODE_REPLACE)
@return  0 on success, negative error number on failure. If APPL_OPER_NOT_STARTED is returned, check the resp_flg output.
*/
int  zw_replace_failed_node(appl_layer_ctx_t   *appl_ctx, add_node_nw_cb_t cb, uint8_t node_id,
                            uint8_t tx_opt, uint8_t mode)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, FAILED_NODE_REPLACE, 0, 0, 0, 0};

    //Node id to be replaced
    cmd[3] = node_id;

    //Transmit option
    cmd[4] = tx_opt;

    //Mode
    cmd[5] = mode;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Replace node sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_FAILED_NODE_REPLACE].seq_num = cmd[2];
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_request_node_neighbor_update - Get the neighbors from the specified node. Can only be called by a primary/inclusion controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the node that the controller wants to get new neighbors from.
@param[in]	cb              The callback function with the status of request.
@return  0 on success, negative error number on failure.
*/
int    zw_request_node_neighbor_update(appl_layer_ctx_t   *appl_ctx, req_node_nbr_updt_cb_t cb, uint8_t node_id)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, NODE_NEIGHBOR_UPDATE_REQUEST, 0, 0};

    //Node ID of the node that should perform Node Neighbor search.
    cmd[3] = node_id;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Request node neighbor update sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_REQ_NODE_NBR_UPDATE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_REQ_NODE_NBR_UPDATE].seq_num = cmd[2];
        //Save node id
        appl_ctx->nm_cb_info[NM_OP_REQ_NODE_NBR_UPDATE].node_id = node_id;
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_controller_change - Add a controller to the Z-Wave network and transfer the role as
                       primary controller to it. The controller invoking this function will become
                       secondary.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report status.
@param[in]	mode            Mode (CONTROLLER_CHANGE_XXX)to start/stop operation.
@param[in]	tx_opt          Transmit options (TRANSMIT_OPTION_EXPLORE  and/or TRANSMIT_OPTION_LOW_POWER)
@return  0 on success, negative error number on failure
*/
int    zw_controller_change(appl_layer_ctx_t   *appl_ctx, ctlr_chg_nw_cb_t cb, uint8_t  mode, uint8_t tx_opt)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY, CONTROLLER_CHANGE, 0, 0, 0, 0};

    //Mode
    cmd[4] = mode;

    //Transmit options
    cmd[5] = tx_opt;

    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    //Controller change sequence number
    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data(appl_ctx, &snd_prm, zip_send_cb, NULL);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_CONTROLLER_CHANGE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_CONTROLLER_CHANGE].seq_num = cmd[2];
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_assign_return_route - Assign static return routes (up to 4) to a Routing Slave node or Enhanced Slave node.
@param[in]	appl_ctx		Context
@param[in]	src_node_id     The Node ID (1...232) of the routing slave that should get the return routes.
@param[in]	dst_node_id     The Destination node ID (1...232).
@param[in]	cb              Completion callback function.
@return  0 on success, negative error number on failure
*/
int    zw_assign_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t src_node_id, uint8_t dst_node_id, ret_route_cb_t cb)
{

    /*
    Calculate the shortest routes from the Routing Slave node (src_node_id) to the destination node
    (dst_node_id) and transmits the return routes to the Routing Slave node (src_node_id). The destination
    node is part of the return routes assigned to the slave.
    */
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, RETURN_ROUTE_ASSIGN, 0, 0, 0};

    cmd[3] = src_node_id;
    cmd[4] = dst_node_id;

    snd_prm.node_id = 0;
    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    //memset(&snd_prm.ep_addr, 0, sizeof(frm_ep_t));
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data_ex(appl_ctx, &snd_prm, zip_send_cb, NULL, NULL, 1);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_ASSIGN_RET_ROUTE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_ASSIGN_RET_ROUTE].seq_num = cmd[2];
        //Save node id
        appl_ctx->nm_cb_info[NM_OP_ASSIGN_RET_ROUTE].node_id = src_node_id;
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zw_delete_return_route - Delete all static return routes from a Routing Slave or Enhanced Slave.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the routing slave node.
@param[in]	cb              The callback function.
@return  0 on success, negative error number on failure
*/
int    zw_delete_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, ret_route_cb_t cb)
{
    int             result;
    appl_snd_data_t snd_prm = {0};
    uint8_t         cmd[] = {COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION, RETURN_ROUTE_DELETE, 0, 0};

    cmd[3] = node_id;

    snd_prm.node_id = 0;
    snd_prm.dat_buf = cmd;
    snd_prm.dat_len = sizeof(cmd);
    memset(&snd_prm.ep_addr, 0, sizeof(frm_ep_t));
    snd_prm.flag = ZIP_FLAG_SECURE;

    plt_mtx_lck(appl_ctx->zip_mtx);

    cmd[2] = appl_ctx->zip_op_seq_num++;

    result = zw_send_data_ex(appl_ctx, &snd_prm, zip_send_cb, NULL, NULL, 1);

    if (result == 0)
    {   //Save the callback function and sequence number
        appl_ctx->nm_cb_info[NM_OP_DELETE_RET_ROUTE].cb = cb;
        appl_ctx->nm_cb_info[NM_OP_DELETE_RET_ROUTE].seq_num = cmd[2];
        //Save node id
        appl_ctx->nm_cb_info[NM_OP_DELETE_RET_ROUTE].node_id = node_id;
    }

    plt_mtx_ulck(appl_ctx->zip_mtx);

    return result;
}


/**
zip_node_ipaddr_set - Add a node-IP address entry into the translation table
@param[in]	appl_ctx		Context
@param[in]	ipaddr          IP address
@param[in]	ipaddr_sz       IP address size in bytes
@param[in]  node_id         Z-wave node id
@return  1 on success, zero on failure
*/
int  zip_node_ipaddr_set(appl_layer_ctx_t *appl_ctx, uint8_t *ipaddr, uint8_t ipaddr_sz, uint8_t node_id)
{
    node_ipaddr_t *node_ipaddr_ent;

    if ((node_id == 0) || (node_id > (MAX_ZWAVE_NODE + 1)) || (ipaddr_sz > IPV6_ADDR_LEN))
    {
        return 0;
    }
    node_ipaddr_ent = &appl_ctx->node_ip_tbl[node_id-1];

    memcpy(node_ipaddr_ent->ipaddr, ipaddr, ipaddr_sz);
    node_ipaddr_ent->ipaddr_sz = ipaddr_sz;
    node_ipaddr_ent->valid = 1;
    //debug_msg_show(appl_ctx->plt_ctx, "node_ip entry added:%u", node_id);

    return 1;
}


/**
zip_node_ipaddr_rm - Remove a node-IP address entry from the translation table
@param[in]	appl_ctx		Context
@param[in]  node_id         Z-wave node id
@return  1 on success, zero on failure
*/
int  zip_node_ipaddr_rm(appl_layer_ctx_t *appl_ctx, uint8_t node_id)
{
    if ((node_id == 0) || (node_id > MAX_ZWAVE_NODE))
    {
        return 0;
    }
    //debug_msg_show(appl_ctx->plt_ctx, "node_ip entry removed:%u", node_id);

    appl_ctx->node_ip_tbl[node_id-1].valid = 0;

    //Invalidate the cache entry
    if (appl_ctx->node_ip_cache.valid == node_id)//Note: The "valid" member stores the node id
    {
        appl_ctx->node_ip_cache.valid = 0;
    }

    return 1;
}


/**
zip_translate_ipaddr - Resolve an IP address to the node id of a node
@param[in]	appl_ctx		Context
@param[in]	ipaddr          IP address
@param[in]	ipaddr_sz       IP address size in bytes
@param[out] node_id         Z-wave node id
@return  1 on success, zero on failure
*/
int  zip_translate_ipaddr(appl_layer_ctx_t *appl_ctx, uint8_t *ipaddr, uint8_t ipaddr_sz, uint8_t *node_id)
{
    unsigned        i;

    if (ipaddr_sz > IPV6_ADDR_LEN)
    {
        return 0;
    }

    //Search from the cache first
    if (memcmp(appl_ctx->node_ip_cache.ipaddr, ipaddr, appl_ctx->node_ip_cache.ipaddr_sz) == 0)
    {
        if (appl_ctx->node_ip_cache.valid > 0)
        {
            *node_id = appl_ctx->node_ip_cache.valid;//Note: The "valid" member stores the node id
            //debug_msg_show(appl_ctx->plt_ctx, "ip translate cache hit");
            return 1;
        }
    }

    //Start searching from the table
    for (i=0; i<MAX_ZWAVE_NODE; i++)
    {
        if (appl_ctx->node_ip_tbl[i].valid)
        {
            if (memcmp(appl_ctx->node_ip_tbl[i].ipaddr, ipaddr, appl_ctx->node_ip_tbl[i].ipaddr_sz) == 0)
            {   //Found
                *node_id = i + 1;
                //Cache for the next use
                if (appl_ctx->node_ip_cache.valid != (i + 1))
                {
                    appl_ctx->node_ip_cache.valid = (i + 1);
                    appl_ctx->node_ip_cache.ipaddr_sz = ipaddr_sz;
                    memcpy(appl_ctx->node_ip_cache.ipaddr, ipaddr, ipaddr_sz);
                    //debug_msg_show(appl_ctx->plt_ctx, "ip translate cache miss");
                    return 1;
                }
            }
        }
    }

    //debug_msg_show(appl_ctx->plt_ctx, "ip translate not found");
    return 0;

}


#ifdef TCP_PORTAL
/**
zwhci_init - Init the application layer.
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
int zwhci_init(appl_layer_ctx_t   *appl_ctx, int portal_fd, void *portal_ssl,
               tls_clnt_prof_t *portal_prof, uint8_t *zip_router,
               net_err_cb_t err_cb, void *user)
#else
/**
zwhci_init - Init the application layer.
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
int zwhci_init(appl_layer_ctx_t   *appl_ctx, uint8_t *zip_router,
               uint16_t host_port, int16_t use_ipv4,
               uint8_t *dtls_psk, uint8_t dtls_psk_len)
#endif
{
    int                 ret_val;
    tpt_layer_ctx_t     *tpt_ctx;

    //Zeroes the session layer context
    memset(&appl_ctx->ssn_ctx, 0, sizeof(ssn_layer_ctx_t));

    tpt_ctx = &appl_ctx->ssn_ctx.frm_ctx.tpt_ctx;

#ifdef TCP_PORTAL
    //Save TLS related info
    tpt_ctx->sock_fd = portal_fd;

    tpt_ctx->ssl = portal_ssl;

    tpt_ctx->zipr_prof = *portal_prof;

    //Set the host listening and sending port
    tpt_ctx->host_port = ZWAVE_HOME_CTL_PORT;

    //Set the transport layer IP version
    appl_ctx->use_ipv4 = tpt_ctx->use_ipv4 = 0;

    //Save the gateway address
    memcpy(tpt_ctx->zip_router, zip_router, IPV6_ADDR_LEN);

    //Save network error callback
    tpt_ctx->tpt_net_err_cb = err_cb;
    tpt_ctx->user = user;


#else
    //Set the host listening and sending port
    tpt_ctx->host_port = host_port;

    //Set the transport layer IP version
    appl_ctx->use_ipv4 = tpt_ctx->use_ipv4 = use_ipv4;

    //Save the gateway address
    memcpy(tpt_ctx->zip_router, zip_router, (use_ipv4)? IPV4_ADDR_LEN : IPV6_ADDR_LEN);

    //Save DTLS pre-shared key
    tpt_ctx->dtls_psk_len = dtls_psk_len;
    memcpy(tpt_ctx->dtls_psk, dtls_psk, dtls_psk_len);

#endif
    //Init session layer
    appl_ctx->ssn_ctx.unsolicited_cmd_cb = appl_unsolicited_cmd_cb;
    appl_ctx->ssn_ctx.appl_layer_ctx = appl_ctx;
    appl_ctx->ssn_ctx.plt_ctx = appl_ctx->plt_ctx;

    ret_val = ssn_init(&appl_ctx->ssn_ctx);

    if (ret_val != 0)
        return ret_val;

    //Init application layer
    appl_ctx->wait_cmd_cb = 0;
    appl_ctx->wait_nm_cb = 0;
    appl_ctx->zip_next_seq_num = (plt_rand_get() & 0x00FF);
    appl_ctx->zip_op_seq_num = (plt_rand_get() & 0x00FF);

    if (appl_ctx->cb_tmout_ms < APPL_CB_TMOUT_MIN)
        appl_ctx->cb_tmout_ms = APPL_CB_TMOUT_MIN;

    if (!plt_mtx_init(&appl_ctx->snd_mtx))
        goto l_APPL_INIT_ERROR1;

    if (!plt_mtx_init(&appl_ctx->zip_mtx))
        goto l_APPL_INIT_ERROR2;

    if (!plt_mtx_init(&appl_ctx->zip_seq_num_mtx))
        goto l_APPL_INIT_ERROR2A;

    if (plt_cond_init(&appl_ctx->snd_cv) == 0)
        goto l_APPL_INIT_ERROR3;

    if (!plt_mtx_init(&appl_ctx->cb_thrd_mtx))
        goto l_APPL_INIT_ERROR4;

    if (!plt_sem_init(&appl_ctx->cb_thrd_sem))
        goto l_APPL_INIT_ERROR5;

    appl_ctx->cb_thrd_run = 1;
    if (plt_thrd_create(appl_cb_thrd, appl_ctx) < 0)
        goto l_APPL_INIT_ERROR6;

    //Register old send status callback
    ssn_old_snd_status_cb_set(&appl_ctx->ssn_ctx, zw_send_old_sts_cb, appl_ctx);

    return 0;

l_APPL_INIT_ERROR6:
    plt_sem_destroy(appl_ctx->cb_thrd_sem);

l_APPL_INIT_ERROR5:
    plt_mtx_destroy(appl_ctx->cb_thrd_mtx);

l_APPL_INIT_ERROR4:
    plt_cond_destroy(appl_ctx->snd_cv);

l_APPL_INIT_ERROR3:
    plt_mtx_destroy(appl_ctx->zip_seq_num_mtx);

l_APPL_INIT_ERROR2A:
    plt_mtx_destroy(appl_ctx->zip_mtx);

l_APPL_INIT_ERROR2:
    plt_mtx_destroy(appl_ctx->snd_mtx);

l_APPL_INIT_ERROR1:
    ssn_exit(&appl_ctx->ssn_ctx);

    return INIT_ERROR_APPL;

}


/**
zwhci_exit - Clean up the application layer
@param[in,out]	appl_ctx		Context
@return
*/
void zwhci_exit(appl_layer_ctx_t   *appl_ctx)
{

    ssn_exit(&appl_ctx->ssn_ctx);

    //Stop the timer
    plt_mtx_lck(appl_ctx->snd_mtx);
    if (appl_ctx->cb_tmr_ctx)
    {
        plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
        appl_ctx->cb_tmr_ctx = 0;
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    //Stop thread
    appl_ctx->cb_thrd_run = 0;
    plt_sem_post(appl_ctx->cb_thrd_sem);

    while (appl_ctx->cb_thrd_sts)
    {
        plt_sleep(100);
    }

    //Extra time for the thread to fully exit
    plt_sleep(100);

    util_list_flush(appl_ctx->cb_thrd_mtx, &appl_ctx->cb_req_hd);

    plt_mtx_destroy(appl_ctx->snd_mtx);
    plt_mtx_destroy(appl_ctx->zip_mtx);
    plt_mtx_destroy(appl_ctx->zip_seq_num_mtx);
    plt_cond_destroy(appl_ctx->snd_cv);
    plt_sem_destroy(appl_ctx->cb_thrd_sem);
    plt_mtx_destroy(appl_ctx->cb_thrd_mtx);
}


/**
zwhci_shutdown - Shutdown the application layer, stop sending any data
@param[in]	appl_ctx    Context
@return
*/
void zwhci_shutdown(appl_layer_ctx_t *appl_ctx)
{

    appl_ctx->cb_thrd_run = 0;

    plt_mtx_lck(appl_ctx->snd_mtx);

    appl_ctx->wait_nm_cb = 0;
    appl_ctx->wait_cmd_cb = 0;

    //Stop the timer
    if (appl_ctx->cb_tmr_ctx)
    {
        plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
        appl_ctx->cb_tmr_ctx = NULL;
    }

    //Wake up all wait threads
    plt_cond_broadcast(appl_ctx->snd_cv);

    plt_mtx_ulck(appl_ctx->snd_mtx);

}


/**
@}
*/


