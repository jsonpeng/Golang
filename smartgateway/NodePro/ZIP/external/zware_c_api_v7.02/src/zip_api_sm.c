/**
@file   zip_api_sm.c - Z/IP High Level API state-machines implementation.

@author David Chow

@version    1.0 28-11-14  Initial release

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
#ifdef TCP_PORTAL
#include <signal.h>
#endif
#include "../include/zip_api_pte.h"
#include "../include/zip_api_util.h"
#include "../include/zip_poll.h"
#ifdef ZIP_V2
#include "../include/zip_mdns.h"
#endif
#include "../include/zwave/ZW_controller_api.h"

// External function declarations
void    zwnet_notify(zwnet_p nw, uint8_t op, uint16_t sts);
int     zwnet_dev_rec_find(zwnet_p nw, uint16_t vid, uint16_t ptype, uint16_t pid, dev_rec_t *dev_rec);
void    zwnet_dev_usr_def_ver_updt(zwnode_p  node);
void	zwnet_add_user_def_intf(zwnode_p node);
uint16_t *zwnet_cmd_cls_find(const uint16_t *cc_lst, uint16_t cc, unsigned cc_cnt);
int     zwnet_ni_sec_updt(zwep_p ep, uint16_t *sec_cls, uint8_t len);
void    zwnet_cmd_cls_show(zwnet_p nw, uint16_t *cmd_cls_buf, uint8_t cnt);
int     zwnet_cmd_cls_add(uint16_t new_cc, const uint16_t *src_cc_lst, uint8_t src_cc_cnt,
                          uint16_t **dst_cc_lst, uint8_t *dst_cc_cnt);
uint8_t zwnet_cmd_cls_dedup(uint16_t *cmd_cls_lst, uint8_t cmd_cnt);
void    zwnet_zwver_rpt_cb(zwif_p intf, uint16_t proto_ver, uint16_t app_ver, uint8_t lib_type, ext_ver_t *ext_ver);
void    zwnet_node_cb(zwnet_p nw, uint8_t node_id, int mode);

// Internal function declarations
static void   zwnet_sm_1_ep_info_cb(zwnet_p nw, uint8_t first_node_id);
static void   zwnet_ep_tmout_cb(void *data);
static int    zwnet_ep_info_sm(zwnet_p nw, zwnet_ep_evt_t evt, uint8_t *data);
int           zwnet_node_info_sm(zwnet_p   nw, zwnet_ni_evt_t  evt, uint8_t *data);
static void   zwnet_1_ep_evt_raise(zwifd_p ifd, zwnet_1_ep_evt_t  evt);
int           zwnet_1_ep_info_sm(zwnet_p nw, zwnet_1_ep_evt_t evt, uint8_t *data);


/**
@defgroup SM State-machine APIs
Implementation of internal state-machines
@{
*/

/**
zwnet_1_ep_cb - Create a single endpoint info state-machine callback request to the callback thread
@param[in]	ifd     Interface descriptor
@param[in]  evt     The event for the state-machine
@return
*/
static void zwnet_1_ep_cb(zwifd_p ifd, zwnet_1_ep_evt_t  evt)
{
    zwnet_p         nw = ifd->net;
    zwnet_cb_req_t  cb_req = {0};

    //Send request to callback thread
    cb_req.type = CB_TYPE_1_EP_SM;
    cb_req.param.ep_cb.ifd = *ifd;
    cb_req.param.ep_cb.evt = evt;

    util_list_add(nw->cb_mtx, &nw->cb_req_hd,
                  (uint8_t *)&cb_req, sizeof(zwnet_cb_req_t));
    plt_sem_post(nw->cb_sem);
}


/**
zwnet_tmout_cb - State-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_node_info_sm(nw, EVT_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_ver_rpt_cb - Version report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The command class the version is referred to
@param[in]	ver	    The version of cls
@return
*/
static void zwnet_ver_rpt_cb(zwif_p intf, uint16_t cls, uint8_t ver)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = cls >> 8;
    data[1] = cls & 0x00FF;
    data[2] = ver;
    memcpy(data + 4, &intf, sizeof(zwif_p));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_VER_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_ver_get - Get version of each command class and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	first_cls   First command class to query
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_ver_get(zwnet_p nw, zwif_p intf, uint16_t first_cls)
{
    int     result;
    zwifd_t ifd;
    uint8_t param;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_ver_rpt_cb, VERSION_COMMAND_CLASS_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the version of first command class
    param = (uint8_t)first_cls;
    result = zwif_get_report(&ifd, &param, 1, VERSION_COMMAND_CLASS_GET, zwif_tx_sts_cb);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_ver_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Save the command class in version get
    nw->ni_sm_cls = first_cls;

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_GET_CMD_VER;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);

    return ZW_ERR_NONE;

}


/**
zwnet_info_rpt_cb - ZWave+ info report callback
@param[in]	intf	    The interface that received the report
@param[in]	info	    Z-Wave+ information
@return
*/
static void zwnet_info_rpt_cb(zwif_p intf, zwplus_info_t *info)
{
    zwnet_p     nw;
    uint8_t     data[32];

    memcpy(data, &intf, sizeof(zwif_p));
    memcpy(data + sizeof(zwif_p), &info, sizeof(zwplus_info_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_INFO_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_1_ep_tmout_cb - State-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_1_ep_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_1_ep_info_sm(nw, EVT_S_EP_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_zwplus_info_get - Get ZWave+ info and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		Zwave+ info interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_zwplus_info_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache)
    {
        return ZW_ERR_NOT_APPLICABLE;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_info_rpt_cb, ZWAVEPLUS_INFO_REPORT);
    if (result != 0)
    {
        return result;
    }

    result = zwif_get_report(&ifd, NULL, 0, ZWAVEPLUS_INFO_GET, zwif_tx_sts_cb);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_zwplus_info_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_ZWPLUS_INFO;
    return ZW_ERR_NONE;
}


/**
zwnet_max_group_rpt_cb - report callback for maximum number of supported groupings
@param[in]	ifd	      interface
@param[in]	max_grp   maximum number of groupings
@param[in]	valid     validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_max_group_rpt_cb(zwifd_p ifd,  uint8_t max_grp, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_GRP_CNT_REPORT);
}


/**
zwnet_sm_max_group_get - Get group count and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_max_group_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache)
    {
        return ZW_ERR_NOT_APPLICABLE;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_max_group_rpt_cb, ASSOCIATION_GROUPINGS_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             ASSOCIATION_GROUPINGS_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_max_group_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Change sub-state
    nw->ni_sm_sub_sta = GRP_SUBSTA_MAX_SUPP;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MAX_GROUP;

    return ZW_ERR_NONE;
}


/**
zwnet_max_usr_code_rpt_cb - report callback for maximum number of supported user codes
@param[in]	ifd	      interface
@param[in]	usr_num   number of supported user codes
@param[in]	valid     validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_max_usr_code_rpt_cb(zwifd_p ifd,  uint8_t usr_num, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_USR_CODE_CNT_REPORT);
}


/**
zwnet_sm_max_usr_code_get - Get maximum user codes count and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_max_usr_code_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_USR_CODE_CNT_REPORT);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_max_usr_code_rpt_cb, USERS_NUMBER_REPORT);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 USERS_NUMBER_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_max_usr_code_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MAX_USR_CODES;

    return ZW_ERR_NONE;
}


/**
zwnet_meter_sup_rpt_cb - report callback for meter capabilities
@param[in]	ifd	        interface
@param[in]	meter_cap	meter capabilities
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_meter_sup_rpt_cb(zwifd_p ifd, zwmeter_cap_p meter_cap, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_METER);
}


/**
zwnet_meter_rpt_cb - report callback for meter reading
@param[in]	ifd	        interface
@param[in]	meter_dat   current value and unit of the meter
@param[in]	ts          time stamp.  If this is zero, the callback has no data and hence other parameter
                        values should be ignored.
@return
*/
static void zwnet_meter_rpt_cb(zwifd_p ifd, zwmeter_dat_p meter_dat, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_METER_DAT);
}


/**
zwnet_sm_meter_rpt_get -  Get meter report
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more report to get; negative error number on failure
*/
static int zwnet_sm_meter_rpt_get(zwnet_p nw, zwif_p intf)
{
    int             result;
    int             i;
    zwmeter_cap_p   meter_cap;
    zwifd_t         ifd;
	uint8_t		    unit_sup[8];    /**< supported units*/
	uint8_t		    unit_cnt = 0;   /**< supported unit count*/

    meter_cap = (zwmeter_cap_p)intf->data;

    if (!meter_cap)
    {
        return ZW_ERR_INTF_NO_DATA;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_meter_rpt_cb, METER_REPORT);
    if (result != 0)
    {
        return result;
    }

    for (i=0; i<8; i++)
    {
        if ((1<<i) & meter_cap->unit_sup)
        {
            unit_sup[unit_cnt++] = i;
        }
    }

    if (nw->snsr_rd_idx < unit_cnt)
    {
        //Get the meter reading
        uint8_t param;

        if (intf->real_ver == 2)
        {
            param = (unit_sup[nw->snsr_rd_idx] & 0x03) << 3;
        }
        else
        {
            param = (unit_sup[nw->snsr_rd_idx] & 0x07) << 3;
        }

        result = zwif_get_report(&ifd, &param, 1, METER_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_meter_rpt_get: get report with error:%d", result);
            return result;
        }

        //Restart timer
        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
        return ZW_ERR_NONE;
    }

    //No more report to get
    return 1;
}


/**
zwnet_sm_meter_get - Get meter capabilities and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_meter_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    if (intf->real_ver < 2)
    {   //Get meter reading

        result = zwif_set_sm_report(&ifd, zwnet_meter_rpt_cb, METER_REPORT);
        if (result != 0)
        {
            return result;
        }

        result = zwif_get_report(&ifd, NULL, 0, METER_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_meter_get: get report with error:%d", result);
            return result;
        }

        //Change sub-state
        nw->ni_sm_sub_sta = METER_SUBSTA_DATA;

    }
    else
    {   //Version 2 and above, get supported meter units

        //Check whether to use cache
        if (nw->single_ep_job.use_cache && intf->data_cnt)
        {   //Have cache, insert event
            zwnet_1_ep_cb(&ifd, EVT_S_EP_METER);
        }
        else
        {
            //Setup report callback
            result = zwif_set_sm_report(&ifd, zwnet_meter_sup_rpt_cb, METER_SUPPORTED_REPORT_V2);

            if (result != 0)
            {
                return result;
            }

            //Request for report
            result = zwif_get_report(&ifd, NULL, 0,
                                     METER_SUPPORTED_GET_V2, zwif_tx_sts_cb);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_meter_get: get report with error:%d", result);
                return result;
            }
        }

        //Change sub-state
        nw->ni_sm_sub_sta = METER_SUBSTA_UNIT;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_METER;

    return ZW_ERR_NONE;
}


/**
zwnet_meter_desc_rpt_cb - report callback for meter descriptor
@param[in]	ifd	        interface
@param[in]	meter	    meter descriptor
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_meter_desc_rpt_cb(zwifd_p ifd, zwmeter_p meter, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_METER_TBL_DESC);
}


/**
zwnet_sm_meter_desc_get - Get meter descriptor and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_meter_desc_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, skip the rest as there is no AV readings (data) to be cached
        return ZW_ERR_NOT_APPLICABLE;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_meter_desc_rpt_cb, METER_TBL_TABLE_ID_REPORT);

    if (result != 0)
    {
        return result;
    }

    result = zwif_set_sm_report(&ifd, zwnet_meter_desc_rpt_cb, METER_TBL_TABLE_POINT_ADM_NO_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             METER_TBL_TABLE_ID_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_meter_desc_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_METER_DESC;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_cfg_param_send_cb - Send configuration parameter transmit status callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	node_id     Node id
@return
*/
static void zwnet_sm_cfg_param_send_cb(appl_layer_ctx_t *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    zwif_p  intf = (zwif_p)user_prm;
    zwnet_p nw = (zwnet_p)appl_ctx->data;
    uint8_t data[32];

    plt_mtx_lck(nw->mtx);

    if (intf)
    {
        data[0] = (uint8_t)tx_sts;
        memcpy(data + 2, &intf, sizeof(zwif_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_CFG_SET_STATUS, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_cfg_param_send - Send configuration parameters to the device
@param[in]	intf		        interface
@param[in]	cfg_param_rec_hd	configuration parameter records head
@param[in]	idx		            indicate which record to use. 0 means use the record head, 1 means the next and so on.
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_cfg_param_send(zwif_p intf, if_rec_config_t *cfg_param_rec_hd, uint16_t idx)
{
    uint16_t        i;
    zwifd_t         ifd;
    if_rec_config_t *cfg_param_rec = cfg_param_rec_hd;
    uint8_t         cmd[12];

    for (i=0; i<idx; i++)
    {
        cfg_param_rec = cfg_param_rec->next;
        if (!cfg_param_rec)
        {
            return ZW_ERR_FILE_EOF;//No more record
        }
    }

    switch (cfg_param_rec->param_size)
    {
        case 1:
            cmd[4] = cfg_param_rec->param_val & 0xFF;
            break;

        case 2:
            cmd[4] = (cfg_param_rec->param_val >> 8) & 0xFF;
            cmd[5] = cfg_param_rec->param_val & 0xFF;
            break;

        case 4:
            cmd[4] = (cfg_param_rec->param_val >> 24) & 0xFF;
            cmd[5] = (cfg_param_rec->param_val >> 16) & 0xFF;
            cmd[6] = (cfg_param_rec->param_val >> 8) & 0xFF;
            cmd[7] = cfg_param_rec->param_val & 0xFF;
            break;

        default:
            return ZW_ERR_VALUE;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_CONFIGURATION;
    cmd[1] = CONFIGURATION_SET;
    cmd[2] = cfg_param_rec->param_num;
    cmd[3] = cfg_param_rec->param_size;

    //Send the command
    zwif_get_desc(intf, &ifd);
    return zwif_exec_ex(&ifd, cmd, 4 + cfg_param_rec->param_size, zwnet_sm_cfg_param_send_cb, intf, 0, NULL);

}


/**
zwnet_sm_cfg_param_set - Set configuration parameters and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_cfg_param_set(zwnet_p nw, zwif_p intf)
{
    int             result;
    zwnode_p        node;
    if_rec_config_t *cfg_param_rec;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache)
    {
        return ZW_ERR_NOT_APPLICABLE;
    }

    if (!nw->sm_job.auto_cfg)
        return ZW_ERR_DISALLOWED;

	node = intf->ep->node;

    if (!node->dev_cfg_valid)
        return ZW_ERR_DEVCFG_NOT_FOUND;

    //Check whether device configuration record has parameters to configure
    cfg_param_rec =  (if_rec_config_t *)zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_CONFIG);

    if (!cfg_param_rec)
        return ZW_ERR_DEVCFG_NOT_FOUND;

    //Send the first configuration parameter
    result = zwnet_sm_cfg_param_send(intf, cfg_param_rec, 0);

    if (result >= 0)
    {
        nw->dev_cfg_rd_idx = 1;
        nw->dev_cfg_param = *cfg_param_rec;

        //Restart timer
        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
        nw->single_ep_sta = S_EP_STA_SET_CFG_PARAM;
    }

    return result;
}


/**
zwnet_thrmo_fan_md_sup_rpt_cb - Report callback for supported thermostat fan operating modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat fan operating modes (ZW_THRMO_FAN_MD_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_thrmo_fan_md_sup_rpt_cb(zwifd_p ifd, uint8_t off, uint8_t mode_len, uint8_t *mode, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_FAN_MD_SUP);
}


/**
zwnet_sm_thrmo_fan_mode_sup_get - Get supported thermostat fan modes and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_fan_mode_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_THRMO_FAN_MD_SUP);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_thrmo_fan_md_sup_rpt_cb, THERMOSTAT_FAN_MODE_SUPPORTED_REPORT);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 THERMOSTAT_FAN_MODE_SUPPORTED_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_fan_mode_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_FAN_MD;

    //Change sub-state
    nw->ni_sm_sub_sta = THRMO_FAN_MD_SUBSTA_SUPP;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_fan_mode_rpt_cb - Thermostat fan mode report callback
@param[in]	ifd	    The interface that received the report
@param[in]	off     fan off mode flag. Non-zero indicates that the fan is fully OFF,
                    “0” indicates that it is possible to change between Fan Modes.
@param[in]	mode    fan operating mode, ZW_THRMO_FAN_MD_XXX
@param[in]	ts      Time stamp
@return
*/
static void zwnet_thrmo_fan_mode_rpt_cb(zwifd_p ifd, uint8_t off, uint8_t mode, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_FAN_MD_DAT);
}


/**
zwnet_sm_thrmo_fan_mode_get - Get thermostat fan mode and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_fan_mode_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_thrmo_fan_mode_rpt_cb, THERMOSTAT_FAN_MODE_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_FAN_MODE_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_fan_mode_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_FAN_MD;

    //Change sub-state
    nw->ni_sm_sub_sta = THRMO_FAN_MD_SUBSTA_DATA;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_md_sup_rpt_cb - Report callback for supported thermostat modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat modes (ZW_THRMO_MD_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_thrmo_md_sup_rpt_cb(zwifd_p ifd, uint8_t mode_len, uint8_t *mode, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_MD_SUP);
}


/**
zwnet_sm_thrmo_mode_sup_get - Get supported thermostat modes and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_mode_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_THRMO_MD_SUP);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_thrmo_md_sup_rpt_cb, THERMOSTAT_MODE_SUPPORTED_REPORT);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 THERMOSTAT_MODE_SUPPORTED_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_mode_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_MD;

    //Change sub-state
    nw->ni_sm_sub_sta = THRMO_MD_SUBSTA_SUPP;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_mode_rpt_cb - Thermostat mode report callback
@param[in]	ifd	    The interface that received the report
@param[in]	mode    mode, ZW_THRMO_MD_XXX
@param[in]	ts      Time stamp
@return
*/
static void zwnet_thrmo_mode_rpt_cb(zwifd_p ifd, uint8_t mode, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_MD_DAT);
}


/**
zwnet_sm_thrmo_mode_get - Get thermostat mode and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_mode_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_thrmo_mode_rpt_cb, THERMOSTAT_MODE_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_MODE_GET, zwif_tx_sts_cb);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_mode_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_MD;

    //Change sub-state
    nw->ni_sm_sub_sta = THRMO_MD_SUBSTA_DATA;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_op_rpt_cb - Thermostat operating state report callback
@param[in]	ifd	    The interface that received the report
@param[in]	state   operating state, ZW_THRMO_OP_STA_XXX
@param[in]	ts      Time stamp
@return
*/
static void zwnet_thrmo_op_rpt_cb(zwifd_p ifd, uint8_t state, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_OP_REPORT);
}


/**
zwnet_sm_thrmo_op_get - Get thermostat operating state and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_op_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_thrmo_op_rpt_cb, THERMOSTAT_OPERATING_STATE_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_OPERATING_STATE_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_op_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_OP;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_fan_sta_rpt_cb - Thermostat fan state report callback
@param[in]	ifd	    The interface that received the report
@param[in]	state   fan state, ZW_THRMO_FAN_STA_XXX
@param[in]	ts      Time stamp
@return
*/
static void zwnet_thrmo_fan_sta_rpt_cb(zwifd_p ifd, uint8_t state, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_FAN_STA);
}


/**
zwnet_sm_thrmo_fan_sta_get - Get thermostat operating state and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_fan_sta_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_thrmo_fan_sta_rpt_cb, THERMOSTAT_FAN_STATE_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_FAN_STATE_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_fan_sta_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_FAN_STA;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_setp_sup_rpt_cb - Report callback for supported thermostat setpoints
@param[in]	ifd	        interface
@param[in]	type_len    size of type buffer
@param[in]	type        buffer to store supported thermostat setpoint types (ZW_THRMO_SETP_TYP_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_thrmo_setp_sup_rpt_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_THRMO_SETP_SUP);
}


/**
zwnet_sm_thrmo_setp_sup_get - Get supported thermostat setpoint types and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_setp_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_THRMO_SETP_SUP);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_thrmo_setp_sup_rpt_cb, THERMOSTAT_SETPOINT_SUPPORTED_REPORT);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 THERMOSTAT_SETPOINT_SUPPORTED_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_setp_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_SETP;

    //Change sub-state
    nw->ni_sm_sub_sta = THRMO_SETP_SUBSTA_SUPP;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_setp_rpt_cb - Thermostat setpoint report callback
@param[in]	ifd	    The interface that received the report
@param[in]	data    setpoint data
@param[in]	ts      Time stamp
@return
*/
static void zwnet_thrmo_setp_rpt_cb(zwifd_p ifd, zwsetp_p value, time_t ts)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[16 + 8];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        memcpy(data, &value, sizeof(zwsetp_p));
        memcpy(data + sizeof(zwsetp_p), &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_THRMO_SETP_DAT, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_thrmo_setp_get - Get thermostat setpoint value and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		Interface
@return  0 on success, 1 on no more thermostat setpoint value to get; negative error number on failure
*/
static int zwnet_sm_thrmo_setp_get(zwnet_p nw, zwif_p intf)
{
    zwifd_t                 ifd;
    int                     result;
    if_thrmo_setp_data_t    *setp_dat = (if_thrmo_setp_data_t *)intf->data;
    uint8_t                 setp_type;

    if (!setp_dat)
    {
        return ZW_ERR_FAILED;
    }

    if (nw->thrmo_rd_idx >= setp_dat->setp_len)
    {
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_thrmo_setp_rpt_cb, THERMOSTAT_SETPOINT_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    setp_type = setp_dat->setp[nw->thrmo_rd_idx];

    if (nw->thrmo_setp_intp)
    {   //Supported thermostat setpoint bit-mask using interpretation B
        setp_type -= 4;
    }
    result = zwif_get_report(&ifd, &setp_type, 1, THERMOSTAT_SETPOINT_GET, zwif_tx_sts_cb);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_setp_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);

    nw->single_ep_sta = S_EP_STA_GET_THRMO_SETP;

    //Change sub-state
    nw->ni_sm_sub_sta = THRMO_SETP_SUBSTA_DATA;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_lvl_sup_rpt_cb - multi level switch type callback
@param[in]	ifd	interface
@param[in]	pri_type    primary switch type, SW_TYPE_XX
@param[in]	sec_type    secondary switch type , SW_TYPE_XX.
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
@return
*/
static void zwnet_sm_lvl_sup_rpt_cb(zwifd_p ifd, uint8_t pri_type, uint8_t sec_type, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_MUL_SWITCH_SUP);
}


/**
zwnet_sm_lvl_sup_get - Get supported switch types and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_lvl_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_MUL_SWITCH_SUP);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_sm_lvl_sup_rpt_cb, SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 SWITCH_MULTILEVEL_SUPPORTED_GET_V3, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_lvl_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MUL_SWITCH;

    //Change sub-state
    nw->ni_sm_sub_sta = LEVEL_SUBSTA_SUPP;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_alrm_sup_rpt_cb - Report callback for supported alarm types
@param[in]	ifd	        interface
@param[in]	have_vtype  flag to indicate whether vendor specific alarm type supported. 1=supported; else 0=unsupported
@param[in]	ztype_len   size of ztype buffer
@param[in]	ztype       buffer to store supported Z-wave alarm types (ZW_ALRM_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_sm_alrm_sup_rpt_cb(zwifd_p ifd, uint8_t have_vtype, uint8_t ztype_len, uint8_t *ztype, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_ALARM_TYPE);
}


/**
zwnet_sm_alrm_sup_get - Get supported alarm types and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_alrm_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check version as this command is only valid for version 2 and above
    if (intf->real_ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_ALARM_TYPE);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_sm_alrm_sup_rpt_cb, ALARM_TYPE_SUPPORTED_REPORT_V2);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 ALARM_TYPE_SUPPORTED_GET_V2, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_alrm_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_ALARM;

    //Change sub-state
    nw->ni_sm_sub_sta = ALARM_SUBSTA_TYPE;

    return ZW_ERR_NONE;
}


/**
zwnet_alrm_evt_rpt_cb - alarm supported events report callback
@param[in]	ifd	        interface
@param[in]	ztype       Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	evt_len     size of evt buffer
@param[in]	evt_msk     buffer to store supported event bit-masks of the ztype
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
@return
*/
void zwnet_alrm_evt_rpt_cb(zwifd_p ifd, uint8_t ztype, uint8_t evt_len, uint8_t *evt_msk, int valid)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = ztype;
        data[1] = evt_len;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_ALARM_EVENT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_alrm_evt_get -  Get the supported alarm events
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more supported events to get; negative error number on failure
*/
static int zwnet_sm_alrm_evt_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    int                 need_get_event = 1;
    zwifd_t             ifd;
    if_alarm_data_t     *alarm_dat = (if_alarm_data_t *)intf->data;

    if (nw->alrm_rd_idx >= alarm_dat->type_evt_cnt)
    {   //No more supported events to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {
        while (nw->alrm_rd_idx < alarm_dat->type_evt_cnt)
        {
            if (alarm_dat->type_evt[nw->alrm_rd_idx].evt_len == 0)
            {
                need_get_event = 1;
                break;
            }
            else
            {
                need_get_event = 0;
                nw->alrm_rd_idx++;
            }
        }
    }

    if (!need_get_event)
    {
        //Have cache, no more supported events to get
        return 1;
    }

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_alrm_evt_rpt_cb, EVENT_SUPPORTED_REPORT_V3);
    if (result != 0)
    {
        return result;
    }

    //Get the supported events
    result = zwif_get_report(&ifd, &alarm_dat->type_evt[nw->alrm_rd_idx].ztype, 1,
                             EVENT_SUPPORTED_GET_V3, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_alrm_evt_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_prot_sup_rpt_cb - Report callback for supported protection states
@param[in]	ifd	        interface
@param[in]	sup_sta     supported Protection States
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_sm_prot_sup_rpt_cb(zwifd_p ifd, zwprot_sup_p sup_sta, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_PROT);
}


/**
zwnet_sm_prot_sup_get - Get supported protection states and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_prot_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check version as this command is only valid for version 2 and above
    if (intf->real_ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_PROT);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_sm_prot_sup_rpt_cb, PROTECTION_SUPPORTED_REPORT_V2);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0,
                                 PROTECTION_SUPPORTED_GET_V2, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_prot_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_PROT;

    return ZW_ERR_NONE;
}


/**
zwnet_bsensor_type_rpt_cb - binary sensor supported types report callback
@param[in]	ifd	        interface
@param[in]	type_len    size of sensor type buffer
@param[in]	type        buffer to store supported sensor types (ZW_BSENSOR_TYPE_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
@return
*/
static void zwnet_bsensor_type_rpt_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_BSENSOR_TYPE_REPORT);
}


/**
zwnet_bsensor_rpt_cb - binary sensor reading report callback
@param[in]	ifd	    interface
@param[in]	state	0=idle, else event detected
@param[in]	type	sensor type, ZW_BSENSOR_TYPE_XXX. If type equals to zero, sensor type is unknown.
@param[in]	ts      time stamp.  If this is zero, the callback has no data and hence other parameter
                    values should be ignored.
@return
*/
static void zwnet_bsensor_rpt_cb(zwifd_p ifd, uint8_t state, uint8_t type, time_t ts)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[16 + 8];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = type;
        data[1] = state;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_BSENSOR_INFO_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_bsensor_rpt_get -  Get binary sensor report
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more report to get; negative error number on failure
*/
static int zwnet_sm_bsensor_rpt_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
	uint8_t		        *sensor_type;       /**< supported sensor type */
	uint8_t		        sensor_cnt;         /**< supported sensor type count*/

    sensor_type = (uint8_t *)intf->data;
    sensor_cnt = intf->data_cnt;

    if (!sensor_type)
    {
        return ZW_ERR_INTF_NO_DATA;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_bsensor_rpt_cb, SENSOR_BINARY_REPORT);
    if (result != 0)
    {
        return result;
    }

    if (nw->snsr_rd_idx < sensor_cnt)
    {
        //Get the sensor reading
        result = zwif_get_report(&ifd, &sensor_type[nw->snsr_rd_idx], 1, SENSOR_BINARY_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_bsensor_rpt_get: get report with error:%d", result);
            return result;
        }

        //Restart timer
        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
        return ZW_ERR_NONE;
    }

    //No more report to get
    return 1;
}


/**
zwnet_sm_bsensor_get - Get the supported binary sensors and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_bsensor_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    if (intf->real_ver < 2)
    {   //Get binary sensor reading

        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_bsensor_rpt_cb, SENSOR_BINARY_REPORT);
        if (result != 0)
        {
            return result;
        }

        //Get the sensor reading
        result = zwif_get_report(&ifd, NULL, 0, SENSOR_BINARY_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_bsensor_get: get report with error:%d", result);
            return result;
        }

        //Change sub-state
        nw->ni_sm_sub_sta = BSENSOR_SUBSTA_DATA;

    }
    else
    {   //Version 2. Get supported binary sensor types

        //Check whether to use cache
        if (nw->single_ep_job.use_cache && intf->data_cnt)
        {   //Have cache, insert event
            zwnet_1_ep_cb(&ifd, EVT_S_EP_BSENSOR_TYPE_REPORT);
        }
        else
        {
            //Setup report callback
            result = zwif_set_sm_report(&ifd, zwnet_bsensor_type_rpt_cb, SENSOR_BINARY_SUPPORTED_SENSOR_REPORT_V2);
            if (result != 0)
            {
                return result;
            }

            //Get supported binary sensor types report
            result = zwif_get_report(&ifd, NULL, 0, SENSOR_BINARY_SUPPORTED_GET_SENSOR_V2, zwif_tx_sts_cb);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_bsensor_get: get report with error:%d", result);
                return result;
            }
        }

        //Change sub-state
        nw->ni_sm_sub_sta = BSENSOR_SUBSTA_TYPE;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_BIN_SENSOR;

    return ZW_ERR_NONE;
}


/**
zwnet_csc_sup_rpt_cb - report callback for maximum supported scenes and key attributes
@param[in]	ifd	      interface
@param[in]	scene_cnt  maximum number of supported scenes.
@param[in]	sameKA	   it indicates if all scenes are supporting the same Key Attributes. 1 - same, 0 - different
@param[in]	KA_array_len    length/width of key attribute array. For v1 COMMAND_CLASS_CENTRAL_SCENE, KA_array_len will be 0.
@param[in]	KA_array   key attribute array. It contains all the supported key attributes for each scene. \n
						When KA_array_len is 0, KA_array should be ignored.\n
						If sameKA is 1, KA_array is a one-dimensional array with length indicated by 'KA_array_len'.\n
						All the scenes support the same set of Key Attribute.\n
						For each row of the array, the first element is the number of valid key attributes given in this row.
						If a particular scene does not support any key attribute, the first element for that row will be 0.\n
						If sameKA is 0, KA_array is a two-dimensional array with size KA_array_len x scene_cnt.\n
						For each row of the array, the first element is the number of valid key attributes given in this row.
						If a particular scene does not support any key attribute, the first element for that row will be 0.\n
@param[in]	valid      validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
*/
static void zwnet_csc_sup_rpt_cb(zwifd_p ifd, uint8_t scene_cnt, uint8_t sameKA, uint8_t KA_array_len, uint8_t *KA_array, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_CENTRAL_SCENE_SUP);
}


/**
zwnet_sm_csc_sup_get - Get Central scene supported scenes and key attributes and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_csc_sup_get(zwnet_p nw, zwif_p intf)
{
	int     result;
	zwifd_t ifd;

	zwif_get_desc(intf, &ifd);

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, insert event
        zwnet_1_ep_cb(&ifd, EVT_S_EP_CENTRAL_SCENE_SUP);
    }
    else
    {
        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_csc_sup_rpt_cb, CENTRAL_SCENE_SUPPORTED_REPORT);

        if (result != 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&ifd, NULL, 0, CENTRAL_SCENE_SUPPORTED_GET, zwif_tx_sts_cb);

        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_csc_sup_get: zwif_get_report with error:%d", result);
            return result;
        }
    }

	//Restart timer
	plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

	nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
	nw->single_ep_sta = S_EP_STA_GET_CENTRAL_SCENE;

	return ZW_ERR_NONE;
}


/**
zwnet_alrm_snsr_type_rpt_cb - alarm sensor supported types report callback
@param[in]	ifd	        interface
@param[in]	type_len    size of sensor type buffer
@param[in]	type        buffer to store supported sensor types (ZW_ALRM_SNSR_TYPE_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
@return
*/
static void zwnet_alrm_snsr_type_rpt_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type, int valid)
{
	zwnet_1_ep_evt_raise(ifd, EVT_S_EP_ALRM_SNSR_TYPE_REPORT);
}


/**
zwnet_alrm_snsr_rpt_cb - alarm sensor reading report callback
@param[in]	ifd	    interface
@param[in]	state	0=idle, else event detected
@param[in]	type	sensor type, ZW_ALRM_SNSR_TYPE_XXX. If type equals to zero, sensor type is unknown.
@param[in]	ts      time stamp.  If this is zero, the callback has no data and hence other parameter
values should be ignored.
@return
*/
static void zwnet_alrm_snsr_rpt_cb(zwifd_p ifd, zw_alrm_snsr_p value, time_t ts)
{
	zwnet_p     nw = ifd->net;
	zwif_p      intf;
	uint8_t     data[16 + 8];

	plt_mtx_lck(nw->mtx);
	intf = zwif_get_if(ifd);
	if (intf)
	{
		//data[0] = type;
		//data[1] = state;
		memcpy(data, &value, sizeof(zw_alrm_snsr_p));
		memcpy(data + sizeof(zw_alrm_snsr_p), &intf, sizeof(zwif_p));
		zwnet_1_ep_info_sm(nw, EVT_S_EP_ALRM_SNSR_INFO_REPORT, data);
	}
	plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_alrm_snsr_rpt_get -  Get alarm sensor report
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more report to get; negative error number on failure
*/
static int zwnet_sm_alrm_snsr_rpt_get(zwnet_p nw, zwif_p intf)
{
	int                 result;
	zwifd_t             ifd;
	uint8_t		        *sensor_type;       /**< supported sensor type */
	uint8_t		        sensor_cnt;         /**< supported sensor type count*/

	sensor_type = (uint8_t *)intf->data;
	sensor_cnt = intf->data_cnt;

	if (!sensor_type)
	{
		return ZW_ERR_INTF_NO_DATA;
	}

	zwif_get_desc(intf, &ifd);

	//Setup report callback
	result = zwif_set_sm_report(&ifd, zwnet_alrm_snsr_rpt_cb, SENSOR_ALARM_REPORT);
	if (result != 0)
	{
		return result;
	}

	if (nw->snsr_rd_idx < sensor_cnt)
	{
		//Get the sensor reading
		result = zwif_get_report(&ifd, &sensor_type[nw->snsr_rd_idx], 1, SENSOR_ALARM_GET, zwif_tx_sts_cb);
		if (result < 0)
		{
			debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_alrm_snsr_rpt_get: get report with error:%d", result);
			return result;
		}

		//Restart timer
		plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
		nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
		return ZW_ERR_NONE;
	}

	//No more report to get
	return 1;
}


/**
zwnet_sm_alrm_snsr_get - Get the supported alarm sensors and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_alrm_snsr_get(zwnet_p nw, zwif_p intf)
{
	int     result;
	zwifd_t ifd;

	zwif_get_desc(intf, &ifd);

	// Get supported alarm sensor types

	//Check whether to use cache
	if (nw->single_ep_job.use_cache && intf->data_cnt)
	{   //Have cache, insert event
		zwnet_1_ep_cb(&ifd, EVT_S_EP_ALRM_SNSR_TYPE_REPORT);
	}
	else
	{
		//Setup report callback
		result = zwif_set_sm_report(&ifd, zwnet_alrm_snsr_type_rpt_cb, SENSOR_ALARM_SUPPORTED_REPORT);
		if (result != 0)
		{
			return result;
		}

		//Get supported alarm sensor types report
		result = zwif_get_report(&ifd, NULL, 0, SENSOR_ALARM_SUPPORTED_GET, zwif_tx_sts_cb);
		if (result < 0)
		{
			debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_alrm_snsr_get: get report with error:%d", result);
			return result;
		}
	}

	//Change sub-state
	nw->ni_sm_sub_sta = ALRM_SNSR_SUBSTA_TYPE;


	//Restart timer
	plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
	nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
	nw->single_ep_sta = S_EP_STA_GET_ALRM_SENSOR;

	return ZW_ERR_NONE;
}


/**
zwnet_1_ep_evt_raise - Raise an event to the single endpoint state-machine
@param[in]	ifd	    The interface that received the report
@param[in]	evt     Event
@return
*/
static void zwnet_1_ep_evt_raise(zwifd_p ifd, zwnet_1_ep_evt_t  evt)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        zwnet_1_ep_info_sm(nw, evt, (uint8_t *)intf);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_dlck_rpt_cb - Door lock operation status report callback
@param[in]	ifd	    The interface that received the report
@param[in]	op_sts  Operation status
@param[in]	ts      Time stamp
@return
*/
static void zwnet_dlck_rpt_cb(zwifd_p ifd, zwdlck_op_p  op_sts, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_DOORLOCK_OP_REPORT);
}


/**
zwnet_sm_dlck_op_get - Get door lock operating state and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_dlck_op_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_dlck_rpt_cb, DOOR_LOCK_OPERATION_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             DOOR_LOCK_OPERATION_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_dlck_op_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_DOORLOCK;
    //Change sub-state
    nw->ni_sm_sub_sta = DOORLCK_SUBSTA_OP;

    return ZW_ERR_NONE;
}


/**
zwnet_dlck_cfg_rpt_cb - Door lock configuration report callback
@param[in]	ifd	    The interface that received the report
@param[in]	cfg     Configuration
@param[in]	ts      Time stamp
@return
*/
static void zwnet_dlck_cfg_rpt_cb(zwifd_p ifd, zwdlck_cfg_p  dlck_cfg, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_DOORLOCK_CFG_REPORT);
}


/**
zwnet_sm_dlck_cfg_get - Get door lock configuration and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_dlck_cfg_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_dlck_cfg_rpt_cb, DOOR_LOCK_CONFIGURATION_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             DOOR_LOCK_CONFIGURATION_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_dlck_cfg_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_DOORLOCK;
    //Change sub-state
    nw->ni_sm_sub_sta = DOORLCK_SUBSTA_CFG;

    return ZW_ERR_NONE;
}

/**
zwnet_switch_rpt_cb - Binary switch state report callback
@param[in]	ifd	    The interface that received the report
@param[in]	on		0=off, else on
@param[in]	ts      Time stamp
@return
*/
static void zwnet_switch_rpt_cb(zwifd_p ifd, uint8_t on, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_SWITCH_REPORT);
}


/**
zwnet_sm_switch_get - Get binary switch state and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_switch_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_switch_rpt_cb, SWITCH_BINARY_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             SWITCH_BINARY_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_switch_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_SWITCH;

    return ZW_ERR_NONE;
}


/**
zwnet_batt_rpt_cb - Battery level report callback
@param[in]	ifd	    The interface that received the report
@param[in]	lvl		Level
@param[in]	ts      Time stamp
@return
*/
static void zwnet_batt_rpt_cb(zwifd_p ifd, uint8_t lvl, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_BATTERY_REPORT);
}


/**
zwnet_sm_batt_get - Get battery level and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		Interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_batt_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_batt_rpt_cb, BATTERY_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             BATTERY_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_batt_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_BATTERY;

    return ZW_ERR_NONE;
}


/**
zwnet_basic_rpt_cb - Basic value report callback
@param[in]	ifd	    The interface that received the report
@param[in]	lvl		Level
@param[in]	ts      Time stamp
@return
*/
static void zwnet_basic_rpt_cb(zwifd_p ifd, uint8_t lvl, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_BASIC_REPORT);
}


/**
zwnet_sm_basic_get - Get basic value and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		Interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_basic_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache)
    {
        return ZW_ERR_NOT_APPLICABLE;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_basic_rpt_cb, BASIC_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             BASIC_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_basic_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    //Use shorter timer for BASIC GET since it is rarely useful and some devices may not support
    //it in endpoint (e.g. Wintop iSensor).  This is to avoid long timeout that causes the device goes into
    //sleep mode.
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_BASIC_GET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_BASIC;

    return ZW_ERR_NONE;
}


/**
zwnet_lvl_rpt_cb - Multilevel switch level report callback
@param[in]	ifd	    The interface that received the report
@param[in]	on		0=off, else on
@param[in]	ts      Time stamp
@return
*/
static void zwnet_lvl_rpt_cb(zwifd_p ifd, uint8_t on, time_t ts)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_MUL_SWITCH_DAT);
}


/**
zwnet_sm_lvl_get - Get multilevel switch level and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_lvl_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_lvl_rpt_cb, SWITCH_MULTILEVEL_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get report
    result = zwif_get_report(&ifd, NULL, 0,
                             SWITCH_MULTILEVEL_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_lvl_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MUL_SWITCH;

    //Change sub-state
    nw->ni_sm_sub_sta = LEVEL_SUBSTA_DATA;

    return ZW_ERR_NONE;
}


/**
zwnet_add_ctlr_grp_cb - Add controller node/endpoint to group callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	node_id     Node id
@return
*/
static void zwnet_add_ctlr_grp_cb(appl_layer_ctx_t *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    zwif_p  intf = (zwif_p)user_prm;
    zwnet_p nw = (zwnet_p)appl_ctx->data;
    uint8_t data[32];

    plt_mtx_lck(nw->mtx);

    if (intf)
    {
        data[0] = (uint8_t)tx_sts;
        memcpy(data + 2, &intf, sizeof(zwif_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_SET_STATUS, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_add_ctlr_grp - Add controller to a group
@param[in]	nw		    Network
@param[in]	first_intf	First interface of an endpoint
@param[in]	alt_intf	First interface of an alternate endpoint (optional; if not used, it should be NULL)
@param[in]	grp_id		Group id
@param[in]	cb		    Callback function for transmit status
@return  0 on success, negative error number on failure
*/
static int zwnet_add_ctlr_grp(zwnet_p nw, zwif_p first_intf, zwif_p alt_intf, uint8_t grp_id, tx_cmplt_cb_t cb)
{
    int         mul_ch_assoc_present = 0;   //Flag to indicate whether Multi Channel Association CC presents.
    int         res;
    zwif_p      intf;
    uint8_t     cmd[8];
    zwifd_t     ifd;

    //Find multi-channel association interface first
    intf = zwif_find_cls(first_intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2);

    if (intf)
    {
        mul_ch_assoc_present = 1;
    }
    else if ((intf = zwif_find_cls(first_intf, COMMAND_CLASS_ASSOCIATION)) != NULL)
    {
        //Found association command class
    }
    //Try alternate interface
    else if ((intf = zwif_find_cls(alt_intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2)) != NULL)
    {
        mul_ch_assoc_present = 1;
    }
    else if ((intf = zwif_find_cls(alt_intf, COMMAND_CLASS_ASSOCIATION)) != NULL)
    {
    }
    else
    {
        //Not found
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    if (mul_ch_assoc_present)
    {
        //
        //All Controller Role types (CSC, SSC, PC, RPC) MUST use the Multi Channel Association CC with
        //Endpoint ID = 1 asserted when performing the association sets. This MUST be done regardless of
        //whether the Controller is a Multi Channel device or not.
        //

        //Prepare the command
        cmd[0] = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2;
        cmd[1] = MULTI_CHANNEL_ASSOCIATION_SET_V2;
        cmd[2] = grp_id;
        cmd[3] = MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2;
        cmd[4] = nw->ctl.nodeid;
        cmd[5] = 1; //endpoint

    }
    else
    {
        //Prepare the command
        cmd[0] = COMMAND_CLASS_ASSOCIATION;
        cmd[1] = ASSOCIATION_SET;
        cmd[2] = grp_id;
        cmd[3] = nw->ctl.nodeid;
    }

    //Send the command
    zwif_get_desc(intf, &ifd);
    res = zwif_exec_ex(&ifd, cmd, (mul_ch_assoc_present)? 6 : 4, (cb)? cb : zwif_tx_sts_cb, intf, 0, NULL);

    return res;
}


/**
zwnet_grp_rpt_cb - Group members info report callback
@param[in]	ifd	        Interface
@param[in]	group	    Grouping identifier
@param[in]	max_cnt	    Maximum number of members the grouping identifier above supports
@param[in]	cnt	        The number of members in the grouping in this report
@param[in]	grp_member	An array of cnt group members in the grouping. Note that the group members may contain
                        non-existence node/endpoint.
@return
*/
void zwnet_grp_rpt_cb(zwifd_p ifd, uint8_t group, uint8_t max_cnt, uint8_t cnt, grp_member_t *grp_member)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[64];

    plt_mtx_lck(nw->mtx);

    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = group;
        data[1] = cnt;

        memcpy(data + 2, &grp_member, sizeof(grp_member_t *));
        memcpy(data + 2 + sizeof(grp_member_t *), &intf, sizeof(zwif_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_MEMBER_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_lifeline_cfg - Configure lifeline
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
/*
static int zwnet_sm_lifeline_cfg(zwnet_p nw, zwif_p intf)
{
    int     result;

    result = zwnet_add_ctlr_grp(nw, intf->ep->intf, NULL, 1, NULL);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_lifeline_cfg: set assoc with error:%d", result);
        return result;
    }

    return ZW_ERR_NONE;
}
*/


/**
zwnet_grp_cmd_lst_rpt_cb - group command list report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_id	        Grouping identifier
@param[in]	cmd_ent_cnt	    Number of entries in the command list
@param[in]	cmd_lst	        Command list
@return
*/
static void zwnet_grp_cmd_lst_rpt_cb(zwif_p intf, uint8_t grp_id, uint8_t cmd_ent_cnt, grp_cmd_ent_t *cmd_lst)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = grp_id;
    data[1] = cmd_ent_cnt;
    memcpy(data + 2, &intf, sizeof(zwif_p));
    memcpy(data + 2 + sizeof(zwif_p), &cmd_lst, sizeof(grp_cmd_ent_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_CMD_LST_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_grp_cmd_lst_get -  Get the command lists in the group
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more command list to get; negative error number on failure
*/
static int zwnet_sm_grp_cmd_lst_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;
    uint8_t             param[2];


    if (nw->grp_rd_idx >= grp_data->group_cnt)
    {   //No more command list to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_grp_cmd_lst_rpt_cb, ASSOCIATION_GROUP_COMMAND_LIST_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the command list
    param[0] = 0x80;
    param[1] = ++nw->grp_rd_idx;

    result = zwif_get_report(&ifd, param, 2,
                             ASSOCIATION_GROUP_COMMAND_LIST_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_cmd_lst_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_grp_info_start -  Start get command lists in the first group
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, otherwise negative error number on failure
*/
static int zwnet_sm_grp_info_start(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwif_p              assoc_intf;
    if_grp_info_dat_t   *grp_data;
    uint8_t             grp_cnt;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache)
    {
        return ZW_ERR_NOT_APPLICABLE;
    }

    //Get maximum supported groupings
    assoc_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_ASSOCIATION);

    if (!assoc_intf)
    {
        assoc_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2);
    }

    if (!assoc_intf)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_start: can't find grouping interface");
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    if (assoc_intf->data_cnt == 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_start: maximum supporting groups is 0");
        return ZW_ERR_VALUE;
    }

    //Get group information interface
    grp_cnt = *((uint8_t *)assoc_intf->data);

    if (grp_cnt == 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_start: can't get maximum supporting groups");
        return ZW_ERR_INTF_NO_DATA;
    }

    grp_data = (if_grp_info_dat_t *)calloc(1, sizeof(if_grp_info_dat_t) + (sizeof(zw_grp_info_p) * grp_cnt));

    if (grp_data)
    {
        if (intf->tmp_data)
        {
            //Free interface specific data
            zwif_dat_rm(intf);
        }

        intf->tmp_data = grp_data;

        grp_data->group_cnt = grp_cnt;

        //Get command list for group #1
        nw->grp_rd_idx = 0;
        result = zwnet_sm_grp_cmd_lst_get(nw, intf);
        if (result == 0)
        {
            //Change state
            nw->single_ep_sta = S_EP_STA_GET_GROUP_INFO;

            //Change sub-state
            nw->ni_sm_sub_sta = GRP_INFO_SUBSTA_CMD_LST;
        }
        return result;
    }

    return ZW_ERR_MEMORY;
}


/**
zwnet_grp_name_rpt_cb - group name report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_id	        Grouping identifier
@param[in]	name	        Group name
@param[in]	name_len        String length of group name
@return
*/
static void zwnet_grp_name_rpt_cb(zwif_p intf, uint8_t grp_id, uint8_t name_len, uint8_t *name)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = grp_id;
    data[1] = name_len;
    memcpy(data + 2, &intf, sizeof(zwif_p));
    memcpy(data + 2 + sizeof(zwif_p), &name, sizeof(uint8_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_NAME_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_grp_name_get -  Get the group name
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more group name to get; negative error number on failure
*/
static int zwnet_sm_grp_name_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;
    uint8_t             param;


    if (nw->grp_rd_idx >= grp_data->group_cnt)
    {   //No more group name to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_grp_name_rpt_cb, ASSOCIATION_GROUP_NAME_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the group name
    param = ++nw->grp_rd_idx;

    result = zwif_get_report(&ifd, &param, 1,
                             ASSOCIATION_GROUP_NAME_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_name_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_grp_info_rpt_cb - group info report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_cnt	        Group count
@param[in]	dynamic	        Flag to indicate dynamic info. 1=dynamic; 0=static
@param[in]	grp_info        Group information
@return
*/
static void zwnet_grp_info_rpt_cb(zwif_p intf, uint8_t grp_cnt, uint8_t dynamic, zw_grp_info_ent_t *grp_info)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = grp_cnt;
    data[1] = dynamic;
    memcpy(data + 2, &intf, sizeof(zwif_p));
    memcpy(data + 2 + sizeof(zwif_p), &grp_info, sizeof(zw_grp_info_ent_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_INFO_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_grp_info_get -  Get the group information
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success; negative error number on failure
*/
static int zwnet_sm_grp_info_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    uint8_t             param[2];


    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_grp_info_rpt_cb, ASSOCIATION_GROUP_INFO_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the group information
    param[0] = 0x40;    //enable list mode
    param[1] = 1;       //group id (ignored when list mode is enabled)

    result = zwif_get_report(&ifd, param, 2,
                             ASSOCIATION_GROUP_INFO_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_grp_get - get information on specified group of interface through report callback
@param[in]		ifd	interface
@param[in]		group	    group id
@param[in]		cb	        report callback function
@return  0 on success, else ZW_ERR_XXX
*/
static int zwnet_sm_grp_get(zwifd_p ifd, uint8_t group, zwrep_group_fn cb)
{
    int  result;

    if ((ifd->cls != COMMAND_CLASS_ASSOCIATION) &&
        (ifd->cls != COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2))
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (group == 0)
    {
        return ZW_ERR_VALUE;
    }

    //Setup report callback
    result = zwif_set_sm_report(ifd, cb, ASSOCIATION_REPORT);

    if (result == 0)
    {
        //Request for report
        result = zwif_get_report(ifd, &group, 1, ASSOCIATION_GET, zwif_tx_sts_cb);
    }
    return result;

}


/**
zwnet_sm_grp_del - remove all members from specified group
@param[in]	ifd	        interface
@param[in]	group	    group id
@return  0 on success, else ZW_ERR_XXX
*/
static int zwnet_sm_grp_del(zwifd_p ifd, uint8_t group)
{
    uint8_t     cmd[4];

    //Check whether the interface belongs to the right command class
    if ((ifd->cls != COMMAND_CLASS_ASSOCIATION) &&
        (ifd->cls != COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2))
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Prepare the command
    cmd[0] = (uint8_t)ifd->cls;
    cmd[1] = ASSOCIATION_REMOVE;
    cmd[2] = group;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_tx_sts_cb);
}


/**
zwnet_name_rpt_cb - Node naming report callback
@param[in]	intf	    The interface that received the report
@param[in]	name	    Node name
@param[in]	len	        Length of node name
@param[in]	char_pres   Char presentation
@return
*/
static void zwnet_name_rpt_cb(zwif_p intf, char *name, uint8_t len, uint8_t char_pres)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[16 + ZW_LOC_STR_MAX];

    data[0] = char_pres;
    data[1] = len;
    memcpy(data + 2, name, len);
    memcpy(data + 2 + len, &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_NAME_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_name_get - Get the name of the node and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_name_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check whether to use cache
    if (nw->single_ep_job.use_cache)
    {
        return ZW_ERR_NOT_APPLICABLE;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_name_rpt_cb, NODE_NAMING_NODE_NAME_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the node name
    result = zwif_get_report(&ifd, NULL, 0, NODE_NAMING_NODE_NAME_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_name_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_NODE_NAMELOC;

    return ZW_ERR_NONE;

}


/**
zwnet_manf_rpt_cb - manufacturer and product id report callback
@param[in]	intf	        The interface that received the report
@param[in]	manf_pdt_id	    Array that stores manufacturer, product type and product id
@return
*/
static void zwnet_manf_rpt_cb(zwif_p intf, uint16_t *manf_pdt_id)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[16 + 6];

    memcpy(data, manf_pdt_id, 6);
    memcpy(data + 6, &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(intf->ep->node->net, EVT_MANF_PDT_ID_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_manf_get - Get the manufacturer and product id of the node and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_manf_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_manf_rpt_cb, MANUFACTURER_SPECIFIC_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the manufacturer id, product type id and product id
    result = zwif_get_report(&ifd, NULL, 0, MANUFACTURER_SPECIFIC_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_manf_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_GET_MANF_PDT_ID;

    //Change sub-state
    nw->ni_sm_sub_sta = MS_SUBSTA_PDT_ID;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_dev_id_rpt_cb - device id report callback
@param[in]	intf	    The interface that received the report
@param[in]	dev_id	    Device ID
@return
*/
static void zwnet_dev_id_rpt_cb(zwif_p intf, dev_id_t *dev_id)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[32];

    memcpy(data, &dev_id, sizeof(dev_id_t *));
    memcpy(data + sizeof(dev_id_t *), &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_DEVICE_ID_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_dev_id_get - Get device id
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	dev_id_type Device id type: DEV_ID_TYPE_XXX
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_dev_id_get(zwnet_p nw, zwif_p intf, uint8_t dev_id_type)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_dev_id_rpt_cb, DEVICE_SPECIFIC_REPORT_V2);
    if (result != 0)
    {
        return result;
    }

    //Get the device id
    result = zwif_get_report(&ifd, &dev_id_type, 1, DEVICE_SPECIFIC_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_dev_id_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);

    //Change sub-state
    nw->ni_sm_sub_sta = MS_SUBSTA_DEV_ID_OEM;

    return ZW_ERR_NONE;

}


/**
zwnet_sensor_rpt_cb - sensor reading report callback
@param[in]	ifd	    interface
@param[in]	value	The current value and unit of the sensor.
@param[in]	ts      time stamp.  If this is zero, the callback has no data and hence other parameter
                    values should be ignored.
@return
*/
static void zwnet_sensor_rpt_cb(zwifd_p ifd, zwsensor_t *value, time_t ts)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[16 + 8];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        memcpy(data, &value, sizeof(zwsensor_t *));
        memcpy(data + sizeof(zwsensor_t *), &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_SENSOR_INFO_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sensor_type_rpt_cb - sensor supported types report callback
@param[in]	ifd	        interface
@param[in]	type_len    size of sensor type buffer
@param[in]	type        buffer to store supported sensor types (ZW_SENSOR_TYPE_XXX)
@param[in]	valid       validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
@return
*/
static void zwnet_sensor_type_rpt_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type, int valid)
{
    zwnet_1_ep_evt_raise(ifd, EVT_S_EP_SENSOR_TYPE_REPORT);
}


/**
zwnet_sm_sensor_get - Get the supported sensors and units and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_sensor_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check version 5 supported
    if (intf->real_ver < 5)
    {
        //Use sensor multi-level get command to obtain sensor type and unit info

        //Setup report callback
        result = zwif_set_sm_report(&ifd, zwnet_sensor_rpt_cb, SENSOR_MULTILEVEL_REPORT);
        if (result != 0)
        {
            return result;
        }

        //Get the sensor reading
        result = zwif_get_report(&ifd, NULL, 0, SENSOR_MULTILEVEL_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_get: get report with error:%d", result);
            return result;
        }

        //Change sub-state
        nw->ni_sm_sub_sta = SENSOR_SUBSTA_DATA_TYPE;
    }
    else
    {   //Get supported sensor types

        //Check whether to use cache
        if (nw->single_ep_job.use_cache && intf->data_cnt)
        {   //Have cache, insert event
            zwnet_1_ep_cb(&ifd, EVT_S_EP_SENSOR_TYPE_REPORT);
        }
        else
        {
            //Setup report callback
            result = zwif_set_sm_report(&ifd, zwnet_sensor_type_rpt_cb, SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5);
            if (result != 0)
            {
                return result;
            }

            //Get supported sensor types report
            result = zwif_get_report(&ifd, NULL, 0, SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V5, zwif_tx_sts_cb);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_get: get report with error:%d", result);
                return result;
            }
        }

        //Change sub-state
        nw->ni_sm_sub_sta = SENSOR_SUBSTA_TYPE;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_SENSOR;

    return ZW_ERR_NONE;
}


/**
zwnet_sensor_unit_rpt_cb - sensor supported units report callback
@param[in]	ifd	            interface
@param[in]	sensor_type     sensor type, ZW_SENSOR_TYPE_XXX
@param[in]	sensor_unit_msk bitmask of units supported for the sensor_type, ZW_SENSOR_UNIT_XXX
@param[in]	valid           validity of the report. If 1 the report is valid; else this report contains no data and should be ignored.
@return
*/
static void zwnet_sensor_unit_rpt_cb(zwifd_p ifd, uint8_t sensor_type, uint8_t sensor_unit_msk, int valid)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = sensor_type;
        //data[1] = sensor_unit_msk;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_SENSOR_UNIT_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_sensor_unit_get -  Get the supported sensor units
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more supported units to get; negative error number on failure
*/
static int zwnet_sm_sensor_unit_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    int                 need_get_unit = 1;
    zwifd_t             ifd;
    if_sensor_data_t    *sensor_dat = (if_sensor_data_t *)intf->data;

    if (nw->snsr_rd_idx >= intf->data_cnt)
    {   //No more supported units to get
        return 1;
    }

    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {
        while (nw->snsr_rd_idx < intf->data_cnt)
        {
            if (sensor_dat[nw->snsr_rd_idx].sensor_unit == 0)
            {
                need_get_unit = 1;
                break;
            }
            else
            {
                need_get_unit = 0;
                nw->snsr_rd_idx++;
            }
        }
    }

    if (!need_get_unit)
    {
        //Have cache, no more supported units to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_sensor_unit_rpt_cb, SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5);
    if (result != 0)
    {
        return result;
    }

    //Get the supported units
    result = zwif_get_report(&ifd, &sensor_dat[nw->snsr_rd_idx].sensor_type, 1,
                             SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V5, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_unit_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_sensor_rpt_get -  Get sensor report
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more report to get; negative error number on failure
*/
static int zwnet_sm_sensor_rpt_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    int                 i;
    int                 found;
    zwifd_t             ifd;
    if_sensor_data_t    *sensor_dat = (if_sensor_data_t *)intf->data;
    uint8_t             param[2];
	uint8_t		        sensor_type;        /**< supported sensor type */
	uint8_t		        sensor_unit_mask;   /**< supported sensor units (bit-mask)*/
    uint8_t		        sensor_unit;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_sensor_rpt_cb, SENSOR_MULTILEVEL_REPORT);
    if (result != 0)
    {
        return result;
    }

    while (nw->snsr_rd_idx < intf->data_cnt)
    {
        sensor_type = sensor_dat[nw->snsr_rd_idx].sensor_type;
        sensor_unit_mask = sensor_dat[nw->snsr_rd_idx].sensor_unit;

        found = 0;
        for (i=nw->snsr_unit_idx; i<4; i++)
        {
            if (sensor_unit_mask & (1 << i))
            {   //Found supported unit
                sensor_unit = i;
                found = 1;
                break;
            }
        }

        if (!found)
        {   //No more supported unit
            //Use next supported sensor type
            nw->snsr_rd_idx++;
            nw->snsr_unit_idx = 0;
            continue;
        }

        param[0] = sensor_type;
        param[1] = (sensor_unit & 0x03) << 3;

        //Get the sensor reading
        result = zwif_get_report(&ifd, param, 2, SENSOR_MULTILEVEL_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_get: get report with error:%d", result);
            return result;
        }

        //Save the unit
        nw->snsr_unit_idx = sensor_unit;

        //Restart timer
        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
        return ZW_ERR_NONE;
    }

    //No more report to get
    return 1;
}


/**
zwnet_av_raw_rpt_cb - simple AV raw report callback
@param[in]	intf	        The interface that received the report
@param[in]	cmd_buf	        Command buffer
@param[in]	cmd_len         Length of command buffer
@return
*/
void zwnet_av_raw_rpt_cb(zwif_p intf, uint16_t cmd_len, uint8_t *cmd_buf)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[48];

    plt_mtx_lck(nw->mtx);
    if (cmd_buf[1] == SIMPLE_AV_CONTROL_REPORT)
    {
        data[0] = cmd_buf[2];//total number of reports
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_AV_REPORT_CNT, data);
    }
    else if (cmd_buf[1] == SIMPLE_AV_CONTROL_SUPPORTED_REPORT)
    {
        data[0] = cmd_buf[2];//report number
        cmd_len -= 3;
        data[1] = cmd_len >> 8;//bit mask buffer length (MSB)
        data[2] = cmd_len & 0xFF;//bit mask buffer length (LSB)
        memcpy(data + 4, &intf, sizeof(zwif_p));
        cmd_buf += 3;
        memcpy(data + 4 + sizeof(zwif_p), &cmd_buf, sizeof(uint8_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_AV_CTL_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_av_get - Get the supported simple AV control and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_av_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;


    //Check whether to use cache
    if (nw->single_ep_job.use_cache && intf->data_cnt)
    {   //Have cache, skip the rest as there is no AV readings (data) to be cached
        return ZW_ERR_NOT_APPLICABLE;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_av_raw_rpt_cb, SIMPLE_AV_CONTROL_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get supported simple AV control report count
    result = zwif_get_report(&ifd, NULL, 0, SIMPLE_AV_CONTROL_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_av_get: get report with error:%d", result);
        return result;
    }

    //Change sub-state
    nw->ni_sm_sub_sta = AV_SUBSTA_RPT_CNT;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_AV;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_av_ctl_get -  Get the supported simple AV control report
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	av_cap		AV capabilities
@return  0 on success, 1 on no more report to get; negative error number on failure
*/
static int zwnet_sm_av_ctl_get(zwnet_p nw, zwif_p intf, zwif_av_cap_t *av_cap)
{
    int     result;
    zwifd_t ifd;
    uint8_t rpt_num;

    if (av_cap->rpt_num == av_cap->total_rpt)
    {   //No more report to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_sm_report(&ifd, zwnet_av_raw_rpt_cb, SIMPLE_AV_CONTROL_SUPPORTED_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the next report
    rpt_num = av_cap->rpt_num + 1;
    result = zwif_get_report(&ifd, &rpt_num, 1,
                             SIMPLE_AV_CONTROL_SUPPORTED_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_av_ctl_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_mul_inst_rpt_cb - Multi instance report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The command class the report is referred to
@param[in]	inst	The number of instances of a given command class
@return
*/
static void zwnet_mul_inst_rpt_cb(zwif_p intf, uint8_t cls, uint8_t inst)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = cls;
    data[1] = inst;
    memcpy(data + 2, &intf, sizeof(zwif_p));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_MULTI_INSTANCE_REP, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_inst_get - Get the number of instances of a given command class
                    and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	cmd_cls		The command class to query for number of supported instances
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_inst_get(zwnet_p nw, zwif_p intf, uint16_t cmd_cls)
{
    int     result;
    zwifd_t ifd;
    uint8_t param;

    //Check for extended command class (2-byte command class)
    if (cmd_cls & 0xFF00)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_inst_get: extended command class UNSUPPORTED");
        return ZW_ERR_FAILED;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_mul_inst_rpt_cb, MULTI_INSTANCE_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the number of instances of first command class
    param = (uint8_t)cmd_cls;
    result = zwif_get_report(&ifd, &param, 1, MULTI_INSTANCE_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_inst_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_MULTI_INSTANCE;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_mul_ch_ep_rpt_cb - Multi channel end point report callback
@param[in]	intf	        The interface that received the report
@param[in]	num_of_ep	    Number of end points embedded in the node. The maximum number of end points is 127.
@param[in]	flag	        Bit-7: 1 if the device has a dynamic number of end points.
                            Bit-6: 1 if all the end points in the node has the same generic and specific command class.
@return
*/
static void zwnet_mul_ch_ep_rpt_cb(zwif_p intf, uint8_t num_of_ep, uint8_t flag)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = flag;
    data[1] = num_of_ep;
    memcpy(data + 2, &intf, sizeof(zwif_p));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_EP_NUM_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_ch_get - Get the number of end points in a node
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_ch_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_mul_ch_ep_rpt_cb, MULTI_CHANNEL_END_POINT_REPORT_V2);
    if (result != 0)
    {
        return result;
    }

    //Get the number of end points in a node
    result = zwif_get_report(&ifd, NULL, 0, MULTI_CHANNEL_END_POINT_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_ch_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_MULTI_CHANNEL;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_cap_rpt_cb - Multi channel capabilitiy report callback
@param[in]	intf	    The interface that received the report
@param[in]	ep_cap	    The end point capability
@return
*/
static void zwnet_cap_rpt_cb(zwif_p intf, ep_cap_t *ep_cap)
{
    zwnet_p     nw;
    uint8_t     data[32];

    memcpy(data, &intf, sizeof(zwif_p));
    memcpy(data + sizeof(zwif_p), &ep_cap, sizeof(ep_cap_t *));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_EP_CAP_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_cap_get - Get the capability of an end point
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	ep_id		end point id of the interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_cap_get(zwnet_p nw, zwif_p intf, uint8_t ep_id)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_cap_rpt_cb, MULTI_CHANNEL_CAPABILITY_REPORT_V2);
    if (result != 0)
    {
        return result;
    }

    //Get the number of end points in a node
    result = zwif_get_report(&ifd, &ep_id, 1, MULTI_CHANNEL_CAPABILITY_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_cap_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_sm_ep_info_cb - Get endpoint info callback
@param[in]	nw		        Network
@param[in]	ep		        Endpoint
@param[in]	first_node_id   The first node id
@return  0 on success, negative error number on failure
*/
static void zwnet_sm_ep_info_cb(zwnet_p nw, uint8_t first_node_id)
{
    zwnet_node_info_sm(nw, EVT_EP_INFO_DONE, NULL);
}


/**
zwnet_sm_ep_info_get - Get endpoint info by invoking the endpoint info state-machine
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_ep_info_get(zwnet_p nw, zwep_p ep)
{
    int         state;
    ep_sm_job_t sm_job = {0};

    if (!ep)
    {
        return ZW_ERR_EP_NOT_FOUND;
    }

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;

    //Initialize endpoint info state-machine
    nw->ep_sm_sta = EP_STA_IDLE;

    sm_job.cb = zwnet_sm_ep_info_cb;
    sm_job.first_ep = ep;
    sm_job.use_cache = 0;

    state = zwnet_ep_info_sm(nw, EVT_EP_SM_START, (uint8_t *)&sm_job);

    if (state == EP_STA_IDLE)
    {
        return ZW_ERR_FAILED;
    }

    //Change state
    nw->ni_sm_sta = ZWNET_STA_GET_EP_INFO;

    return 0;
}


/**
zwnet_wkup_rpt_cb - wake up capabilities report callback
@param[in]	ifd	    interface
@param[in]	cap		capabilities report, null for notification
@return	    Only apply to notification: 0=no command pending to send; 1=commands pending to send.
*/
static int zwnet_wkup_rpt_cb(zwifd_p ifd, zwif_wakeup_p cap)
{
    zwnet_p     nw;
    zwif_p      intf;
    uint8_t     data[32];   //Note: for MAC OS X 64-bit, pointer is 8-byte long.

    nw = ifd->net;

    plt_mtx_lck(nw->mtx);

    intf = zwif_get_if(ifd);

    if (!intf)
    {
        plt_mtx_ulck(nw->mtx);
        return 0;
    }

    memcpy(data, &cap, sizeof(zwif_wakeup_p));
    memcpy(data + sizeof(zwif_wakeup_p), &intf, sizeof(zwif_p));

    zwnet_node_info_sm(nw, EVT_WKUP_CAP_REP, data);
    plt_mtx_ulck(nw->mtx);
    return 0;
}


/**
zwnet_sm_wkup_cfg - Get device's wakeup interval and notification node
@param[in]	nw		    Network
@param[in]	wkup_intf	Wake up interface
@return  0 on successfully sending WAKE_UP_INTERVAL_GET;
         negative error number on failure
*/
static int zwnet_sm_wkup_cfg(zwnet_p nw, zwif_p wkup_intf)
{
    int         result;
    zwifd_t     ifd;

    zwif_get_desc(wkup_intf, &ifd);

    result = zwif_set_report(&ifd, zwnet_wkup_rpt_cb, WAKE_UP_INTERVAL_REPORT);
    if (result != 0)
    {
        return result;
    }

    if (wkup_intf->ver >= 2)
    {
        result = zwif_set_report(&ifd, zwnet_wkup_rpt_cb, WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2);
        if (result != 0)
        {
            return result;
        }
    }

    //Get wake up interval
    zwif_get_report(&ifd, NULL, 0, WAKE_UP_INTERVAL_GET, zwif_tx_sts_cb);

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_WKUP_CFG;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return 0;

}


/**
zwnet_rr_sts_cb - Return route completion callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwnet_rr_sts_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, uint8_t node_id)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    zwnode_p    node;
    zwep_p      ep;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, node_id);

    if (!node)
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "zwnet_rr_sts_cb:invalid node id:%u", node_id);
        plt_mtx_ulck(nw->mtx);
        return;
    }

    ep = &node->ep;

    data[0] = tx_sts;
    memcpy(data + 2, &ep, sizeof(zwep_p));
    zwnet_node_info_sm(nw, EVT_RR_TX_STS, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_rr - Assign controller node return route and update the state-machine's state
@param[in]	nw		    Network
@param[in]	node		Node
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_rr(zwnet_p nw, zwnode_p node)
{
    int     result;

    result = zw_assign_return_route(&nw->appl_ctx,
                                    node->nodeid,
                                    nw->ctl.nodeid,
                                    zwnet_rr_sts_cb);
    if (result != 0)
    {
        return result;
    }

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_ASSIGN_RR;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);

    return ZW_ERR_NONE;

}


/**
zwnet_sm_others_cb - Get other command classes info callback
@param[in]	nw		        Network
@param[in]	first_node_id   The first node id
@return
*/
static void zwnet_sm_others_cb(zwnet_p nw, uint8_t first_node_id)
{
    zwnet_node_info_sm(nw, EVT_GET_OTHERS_DONE, NULL);
}


/**
zwnet_sm_updt_nameloc - Update name location of an endpoint
@param[in]	data	Character stream with byte 0 = char. presentation, byte 1 = string length
@param[in]	nameloc	Name location character buffer to be updated with UTF-8 string
@return
*/
static void zwnet_sm_updt_nameloc(uint8_t *data, char *nameloc)
{
    int     result;
    uint8_t str_len;    //string length

    str_len = data[1];

    //Check whether to convert into utf8
    if (data[0] == CHAR_PRES_UTF16)
    {
        uint8_t *utf16_buf;
#ifdef WIN32
        char    utf8_str[ZW_LOC_STR_MAX + 1];
#else
        uint8_t utf8_str[ZW_LOC_STR_MAX + 1];
#endif

        utf16_buf = (uint8_t *)calloc(1, str_len + 2);//additional 2 bytes for null characters
        if (utf16_buf)
        {
            memcpy(utf16_buf, data + 2, str_len);

            //convert into utf8
#ifdef WIN32
            result = plt_utf16_to_8((const char *)utf16_buf, utf8_str, ZW_LOC_STR_MAX + 1, 1);
#else
            result = plt_utf16_to_8((const uint16_t *)utf16_buf, utf8_str, ZW_LOC_STR_MAX + 1, 1);
#endif
            free(utf16_buf);
            if (result == 0)
            {
                size_t utf8_len;

                utf8_len = strlen((const char *)utf8_str);

                if (utf8_len > ZW_LOC_STR_MAX)
                {
                    utf8_len = ZW_LOC_STR_MAX;
                }

                //Database values to have priority over the values stored in the device
                if (*nameloc == '\0')
                {
                    //Copy node name/loc to the endpoint
                    memcpy(nameloc, utf8_str, utf8_len);
                    nameloc[utf8_len] = '\0';
                }
            }
        }
    }
    else
    {   //UTF-8 format
        //Database values to have priority over the values stored in the device
        if (*nameloc == '\0')
        {
            //Check for valid UTF-8 string
            str_len = plt_utf8_chk(data + 2, str_len);

            //Copy node name/loc to the endpoint
            memcpy(nameloc, data + 2, str_len);
            nameloc[str_len] = '\0';
        }
    }
}


/**
zwnet_sm_ver_rpt_hdlr - Handle command class version report
@param[in]	nw		    Network
@param[in]	ver_intf	Version	interface
@param[in]	ver_rpt_cb	Version	report callback
@param[in]	cmd_cls     Reported command class
@param[in]	cmd_cls_ver Reported command class version
@return  0 on successfully sending next command class query; 1 = no more command class to query; negative on error
*/
int zwnet_sm_ver_rpt_hdlr(zwnet_p nw, zwif_p *ver_intf, void *ver_rpt_cb, uint16_t cmd_cls, uint8_t cmd_cls_ver)
{
    zwif_p intf;
    zwep_p ep;

    ep = (*ver_intf)->ep;

    intf = zwif_find_cls(ep->intf, cmd_cls);

    if (intf)
    {
        zwif_p  nxt_intf;
        zwif_p  new_intf;
        zwifd_t ver_ifd;
        uint8_t usr_def_ver = 0;

        //Save the next interface
        nxt_intf = (zwif_p)intf->obj.next;

        //Get user-defined version
        if (intf->ep->node->dev_cfg_valid)
        {
            usr_def_ver = zwdev_if_ver_get(intf->ep->node->dev_cfg_rec.ep_rec, intf->ep->epid, cmd_cls);
        }

        //User-defined version must be greater than the real version
        if (usr_def_ver < cmd_cls_ver)
        {
            usr_def_ver = cmd_cls_ver;
        }


        //Update command class version
        if (usr_def_ver > intf->ver)
        {
            //Replace the old interface with a new interface
            new_intf = zwif_create(cmd_cls, usr_def_ver, intf->propty);

            if (new_intf)
            {
                //Update real version
                if (cmd_cls_ver)
                {
                    new_intf->real_ver = cmd_cls_ver;
                }

                //Save back link to end point
                new_intf->ep = ep;

                //Free interface specific data
                zwif_dat_rm(intf);

                //Replace the old interface in the end point
                zwobj_rplc((zwobj_p *)(&ep->intf), &intf->obj, &new_intf->obj);

                //Check whether the replaced interface is command class version
                if (cmd_cls == COMMAND_CLASS_VERSION)
                {   //Update the version interface
                    *ver_intf = new_intf;
                }
            }
        }

        //Get next command class version

        //Find the next non-basic/non-extended command class
        while (nxt_intf)
        {
            if ((nxt_intf->cls != COMMAND_CLASS_BASIC) &&
                ((nxt_intf->cls & 0xFF00) == 0))
            {
                break;
            }
            nxt_intf =  (zwif_p)nxt_intf->obj.next;
        }

        zwif_get_desc(*ver_intf, &ver_ifd);

        if (nxt_intf)
        {
            uint8_t param;
            int     result;

            //Setup report callback
            zwif_set_report(&ver_ifd, ver_rpt_cb, VERSION_COMMAND_CLASS_REPORT);

            param = (uint8_t)nxt_intf->cls;
            result = zwif_get_report(&ver_ifd, &param, 1,
                                     VERSION_COMMAND_CLASS_GET, zwif_tx_sts_cb);

            if (result >= 0)
            {
                //Save the version get command class
                nw->ni_sm_cls = nxt_intf->cls;

                return 0;
            }

            debug_zwapi_msg(&nw->plt_ctx, "zwif_get_report version with error:%d", result);
            return result;
        }
        //No more command class version to query
        return 1;
    }

    return ZW_ERR_INTF_NOT_FOUND;
}


/**
zwnet_ep0_dedup - Hide duplicate functional interfaces at endpoint 0
@param[in]	node  Node
@return
*/
static void zwnet_ep0_dedup(zwnode_p node)
{
    static const uint16_t func_cmd_cls[] = {COMMAND_CLASS_SWITCH_MULTILEVEL, COMMAND_CLASS_SWITCH_BINARY,
        COMMAND_CLASS_SENSOR_MULTILEVEL, COMMAND_CLASS_SENSOR_BINARY, COMMAND_CLASS_DOOR_LOCK, COMMAND_CLASS_USER_CODE,
        COMMAND_CLASS_ALARM, COMMAND_CLASS_METER, COMMAND_CLASS_METER_PULSE, COMMAND_CLASS_SIMPLE_AV_CONTROL,
        COMMAND_CLASS_METER_TBL_MONITOR, COMMAND_CLASS_BATTERY, COMMAND_CLASS_THERMOSTAT_FAN_MODE, COMMAND_CLASS_THERMOSTAT_FAN_STATE,
        COMMAND_CLASS_THERMOSTAT_MODE, COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, COMMAND_CLASS_THERMOSTAT_SETBACK,
        COMMAND_CLASS_THERMOSTAT_SETPOINT, COMMAND_CLASS_CLOCK, COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE,
        COMMAND_CLASS_PROTECTION, COMMAND_CLASS_INDICATOR, COMMAND_CLASS_CENTRAL_SCENE, COMMAND_CLASS_SENSOR_ALARM
                                           };
    zwep_p ep0;
    zwep_p ep1;
    zwif_p intf0;       //Pointer to interface of endpoint 0
    zwif_p intf1;       //Pointer to interface of endpoint 1

    ep0 = &node->ep;

    ep1 = (zwep_p)ep0->obj.next;

    if (ep1)
    {
        intf0 = ep0->intf;
        while (intf0)
        {
            if (zwnet_cmd_cls_find(func_cmd_cls, intf0->cls, sizeof(func_cmd_cls)/sizeof(uint16_t)))
            {   //The interface is in the list of functional command classes
                intf1 = ep1->intf;
                while (intf1)
                {
                    if (intf0->cls == intf1->cls)
                    {   //Duplicated, mark it as hidden
                        intf0->propty |= IF_PROPTY_HIDDEN;
                        break;
                    }
                    intf1 = (zwif_p)intf1->obj.next;
                }
            }

            //Point to next interface
            intf0 = (zwif_p)intf0->obj.next;
        }
    }
}


#ifdef  HIDE_FW_UPD_IF_FOREIGN_NODE
/**
zwnet_hide_fw_upd - Hide COMMAND_CLASS_FIRMWARE_UPDATE_MD in all endpoints of the node
@param[in]	node  Node
@return
*/
static void zwnet_hide_fw_upd(zwnode_p node)
{
    zwep_p ep;
    zwif_p intf;

	for (ep = &node->ep; ep != NULL; ep = (zwep_p)ep->obj.next)
	{
		for(intf = ep->intf; intf != NULL; intf = (zwif_p)intf->obj.next)
		{
			if(intf->cls == COMMAND_CLASS_FIRMWARE_UPDATE_MD)
			{
				intf->propty |= IF_PROPTY_HIDDEN;
                break;	//Not supposed to have 2 firmware update MD cc in the same endpoint, move to next endpoint
			}
		}
	}
}
#endif


/**
zwnet_loc_rpt_cb - Location report callback
@param[in]	intf	    The interface that received the report
@param[in]	loc	        Location
@param[in]	len	        Length of location
@param[in]	char_pres   Char presentation
@return
*/
static void zwnet_loc_rpt_cb(zwif_p intf, char *loc, uint8_t len, uint8_t char_pres)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[16 + ZW_LOC_STR_MAX];

    data[0] = char_pres;
    data[1] = len;
    memcpy(data + 2, loc, len);
    memcpy(data + 2 + len, &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_LOC_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sec_rpt_cb - Supported security commands report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The buffer that store the command classes
@param[in]	cnt	    Command class count
@return
*/
static void zwnet_sec_rpt_cb(zwif_p intf, uint16_t *cls, uint8_t cnt)
{
    zwnet_p     nw;
    uint8_t     data[32];   //Note: for MAC OS X 64-bit, pointer is 8-byte long.

    data[0] = cnt;
    memcpy(data + 2, &cls, sizeof(uint16_t *));
    memcpy(data + 2 + sizeof(uint16_t *), &intf, sizeof(zwif_p));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_ep_info_sm(nw, EVT_EP_SEC_SUP_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_ep_tmout_cb - State-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_ep_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_ep_info_sm(nw, EVT_EP_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_ep_ver_rpt_cb - Version report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The command class the version is referred to
@param[in]	ver	    The version of cls
@return
*/
static void zwnet_ep_ver_rpt_cb(zwif_p intf, uint16_t cls, uint8_t ver)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = cls >> 8;
    data[1] = cls & 0x00FF;
    data[2] = ver;

    memcpy(data + 4, &intf, sizeof(zwif_p));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_ep_info_sm(nw, EVT_EP_VER_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_ep_sm_ver_get - Get version of each command class of the endpoint and update the state-machine's state
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static int zwnet_ep_sm_ver_get(zwnet_p nw, zwep_p ep)
{
    int     result;
    zwifd_t ifd;
    zwif_p  ver_intf;

    //Find version interface
    if (ep)
    {
        ver_intf = zwif_find_cls(ep->intf, COMMAND_CLASS_VERSION);
        if (ver_intf)
        {
            zwif_p    temp_intf;

            zwif_get_desc(ver_intf, &ifd);

            //Setup report callback
            result = zwif_set_report(&ifd, zwnet_ep_ver_rpt_cb, VERSION_COMMAND_CLASS_REPORT);
            if (result != 0)
            {
                return result;
            }

            //Get the version of first command class

            //Find the first non-basic/non-extended command class
            temp_intf = ep->intf;
            while (temp_intf)
            {
                if ((temp_intf->cls != COMMAND_CLASS_BASIC)
                    && ((temp_intf->cls & 0xFF00) == 0))
                {
                    break;
                }
                temp_intf =  (zwif_p)temp_intf->obj.next;
            }

            if (temp_intf)
            {
                uint8_t param;

                param = (uint8_t)temp_intf->cls;
                result = zwif_get_report(&ifd, &param, 1, VERSION_COMMAND_CLASS_GET, zwif_tx_sts_cb);

                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_ep_sm_ver_get: zwif_get_report with error:%d", result);
                    return result;
                }
                //Update state-machine's state
                nw->ep_sm_sta = EP_STA_GET_CMD_VER;

                //Save the command class in version get
                nw->ni_sm_cls = temp_intf->cls;

                //Restart timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_ep_tmout_cb, nw);

                return ZW_ERR_NONE;
            }
        }
    }

    return ZW_ERR_EP_NOT_FOUND;

}


/**
zwnet_ep_sm_sec_get - Get security supported command classes of the endpoint and update the state-machine's state
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static int zwnet_ep_sm_sec_get(zwnet_p nw, zwep_p ep)
{
    int         result;
    zwifd_t     ifd;
    zwif_p      intf;
    uint8_t     cmd_buf[2];

    //Check whether the node is secure
    if (!ep->node->sec_incl)
        return ZW_ERR_UNSUPPORTED;

    //Check whether the endpoint has security command class
    intf = zwif_find_cls(ep->intf, COMMAND_CLASS_SECURITY);

    if (intf)
    {
        //This node supports security command class
        //Setup report callback

        zwif_get_desc(intf, &ifd);

        result = zwif_set_report(&ifd, zwnet_sec_rpt_cb, SECURITY_COMMANDS_SUPPORTED_REPORT);
        if (result == 0)
        {
            //Get the supported secure command classes

            //Initialize report setting
            if (nw->sec_cmd_cls.cmd_cls_buf)
            {
                free(nw->sec_cmd_cls.cmd_cls_buf);
                nw->sec_cmd_cls.cmd_cls_buf = NULL;
            }

            cmd_buf[0] = COMMAND_CLASS_SECURITY;
            cmd_buf[1] = SECURITY_COMMANDS_SUPPORTED_GET;

            //Make sure SECURITY_COMMANDS_SUPPORTED_GET is always sent securely
            ifd.propty |= IF_PROPTY_SECURE;
            result = zwif_exec(&ifd, cmd_buf, 2, NULL);

            if (result >= 0)
            {
                //Update state-machine's state
                nw->ep_sm_sta = EP_STA_GET_SEC_CMD_SUP;

                //Restart timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_ep_tmout_cb, nw);

                return ZW_ERR_NONE;
            }
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_ep_sm_sec_get: zwsec_cmd_sup_get with error:%d", result);
            return result;
        }
        else
        {
            return result;
        }
    }
    return ZW_ERR_UNSUPPORTED;
}


/**
zwnet_ep_sm_next - Start processing next endpoint
@param[in]	nw		Network
@return 0 on success processing of next endpoint, negative error number on failure
*/
static int zwnet_ep_sm_next(zwnet_p nw)
{
    int         result;
    zwep_p      ep;
    ep_sm_job_t sm_job = {0};


    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;    //make sure timer context is null, else restart timer will crash

    //Next endpoint becomes the first endpoint
    ep = (zwep_p)nw->ep_sm_job.first_ep->obj.next;
    nw->ep_sm_job.first_ep = ep;
    while (ep)
    {
        //Check whether the endpoint is a secure endpoint
        //and if so, get the security supported command classes
        result = zwnet_ep_sm_sec_get(nw, ep);
        if (result == 0)
        {
            return 0;
        }
        //Start processing version command class
        result = zwnet_ep_sm_ver_get(nw, ep);
        if (result == 0)
        {
            return 0;
        }
        //Process other command classes
        sm_job.cb = zwnet_sm_1_ep_info_cb;
        sm_job.first_ep = ep;
        sm_job.use_cache = nw->ep_sm_job.use_cache;

        if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
        {
            //Change state
            nw->ep_sm_sta = EP_STA_GET_EP_INFO;
            return 0;
        }
        else
        {
            if (nw->single_ep_sta != S_EP_STA_IDLE)
            {
                debug_zwapi_msg(&nw->plt_ctx,
                                "zwnet_ep_sm_next: single endpoint state-machine is in wrong state: %d",
                                nw->single_ep_sta);
            }
        }

        //Point to next endpoint
        ep = (zwep_p)ep->obj.next;
        nw->ep_sm_job.first_ep = ep;
    }

    return ZW_ERR_FAILED;
}


/**
zwnet_1_ep_job_cmplt - Reset to idle state and callback to notify job completion
@param[in]	nw		    Network
@return
*/
static void zwnet_1_ep_job_cmplt(zwnet_p nw)
{
    sm_fn   cb;
    //Completed job
    nw->single_ep_sta = S_EP_STA_IDLE;
    cb = nw->single_ep_job.cb;
    memset(&nw->single_ep_job, 0, sizeof(ep_sm_job_t));

    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;

    //Invoke user callback
    if (cb)
    {
        cb(nw, 0);
    }
}


/**
zwnet_1_ep_sm_cls_hdlr - Handle command classes in an endpoint
@param[in]	nw		    Network
@param[in]	ep		    end point
@param[in]	act		    starting action
@return 0 on success handling of next command class, negative error number on failure
*/
static int zwnet_1_ep_sm_cls_hdlr(zwnet_p nw, zwep_p ep, zwnet_1_ep_act_t act)
{
	//TODO: for now, alarm get is not part of the state-machine yet.
	//This is because if it is a push device, the alarm will always be sent through
	//unsolicited report. Alarm get call will always yield "no notification", possibily
	//without any valid type and event information (Diehl actuator). In this case,
	//the report will not be cached.
	//Until we have an actual pull mode device, depends on the expectation of the UI,
	//this behavior may need to changed.
    static const zw_sm_hdlr_dat_t hdlr_dat[] = {
        //IMPORTANT: The order of the entries must follow the order in enum zwnet_1_ep_act_t
        {zwnet_sm_zwplus_info_get, COMMAND_CLASS_ZWAVEPLUS_INFO, 0},    //0
        {zwnet_sm_sensor_get, COMMAND_CLASS_SENSOR_MULTILEVEL, 0},
        {zwnet_sm_max_group_get, COMMAND_CLASS_ASSOCIATION, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2},
        {zwnet_sm_grp_info_start, COMMAND_CLASS_ASSOCIATION_GRP_INFO, 0},
        {zwnet_sm_thrmo_fan_mode_sup_get, COMMAND_CLASS_THERMOSTAT_FAN_MODE, 0},
        {zwnet_sm_thrmo_mode_get, COMMAND_CLASS_THERMOSTAT_MODE, 0},    //5
        {zwnet_sm_thrmo_setp_sup_get, COMMAND_CLASS_THERMOSTAT_SETPOINT, 0},
        {zwnet_sm_lvl_get, COMMAND_CLASS_SWITCH_MULTILEVEL, 0},
        {zwnet_sm_av_get, COMMAND_CLASS_SIMPLE_AV_CONTROL, 0},
        {zwnet_sm_alrm_sup_get, COMMAND_CLASS_ALARM, 0},
        {zwnet_sm_prot_sup_get, COMMAND_CLASS_PROTECTION, 0},    //10
        {zwnet_sm_max_usr_code_get, COMMAND_CLASS_USER_CODE, 0},
        {zwnet_sm_meter_get, COMMAND_CLASS_METER, 0},
        {zwnet_sm_meter_desc_get, COMMAND_CLASS_METER_TBL_MONITOR, 0},
        {zwnet_sm_cfg_param_set, COMMAND_CLASS_CONFIGURATION, 0},
        {zwnet_sm_bsensor_get, COMMAND_CLASS_SENSOR_BINARY, 0}, //15
        {zwnet_sm_dlck_op_get, COMMAND_CLASS_DOOR_LOCK, 0},
		{zwnet_sm_csc_sup_get, COMMAND_CLASS_CENTRAL_SCENE, 0 },
		{zwnet_sm_alrm_snsr_get, COMMAND_CLASS_SENSOR_ALARM, 0},
        {zwnet_sm_switch_get, COMMAND_CLASS_SWITCH_BINARY, 0},
        {zwnet_sm_thrmo_op_get, COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, 0}, //20
        {zwnet_sm_batt_get, COMMAND_CLASS_BATTERY, 0},
        {zwnet_sm_thrmo_fan_sta_get, COMMAND_CLASS_THERMOSTAT_FAN_STATE, 0},
        {zwnet_sm_name_get, COMMAND_CLASS_NODE_NAMING, 0},
        {zwnet_sm_basic_get, COMMAND_CLASS_BASIC, 0}    //MUST be the last one
    };

    int    i;
    int    size;
	int    result;
    zwif_p intf;

    result = ZW_ERR_FAILED;

    size = sizeof(hdlr_dat)/sizeof(zw_sm_hdlr_dat_t);

    for (i=act; i<size; i++)
    {
        intf = zwif_find_cls(ep->intf, hdlr_dat[i].cls);
        if (!intf && hdlr_dat[i].alt_cls)
        {
            intf = zwif_find_cls(ep->intf, hdlr_dat[i].alt_cls);
        }
        //Skip hidden interface
        if (intf && !(intf->propty & IF_PROPTY_HIDDEN))
        {
            result = hdlr_dat[i].get_fn(nw, intf);
            if (result == 0)
            {
                break;
            }
        }
    }

    return result;
}


/**
zwnet_1_ep_complt_tmr_cb - single endpoint state-machine completion timer callback
@param[in] data     Pointer to network
@return
*/
static void zwnet_1_ep_complt_tmr_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call job completion function
    zwnet_1_ep_job_cmplt(nw);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_1_ep_info_sm - Single endpoint info state-machine
@param[in] nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return   Non-zero when the state-machine is started from idle state; otherwise return zero
*/
int zwnet_1_ep_info_sm(zwnet_p nw, zwnet_1_ep_evt_t evt, uint8_t *data)
{
    int     result;
    zwif_p  intf = NULL;

    switch (nw->single_ep_sta)
    {
        //----------------------------------------------------------------
        case S_EP_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_S_EP_SM_START)
            {
                ep_sm_job_t  *ep_sm_job = (ep_sm_job_t *)data;

                nw->single_ep_job = *ep_sm_job;
                nw->single_ep_job.sts = 1;

                //Start processing command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, ep_sm_job->first_ep, S_EP_ACT_ZWPLUS_INFO) != 0)
                {   //Failed, callback using zwnet_1_ep_job_cmplt()

                    //debug_zwapi_msg(&nw->plt_ctx, "zwnet_1_ep_info_sm: nothing to process for this endpoint");

                    //Stop timer
                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

                    //Start the delay timer
                    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_1_ep_complt_tmr_cb, nw);
                }

                return 1;
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_ZWPLUS_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_INFO_REPORT)
                {
                    zwplus_info_t   *info;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&info, data + sizeof(zwif_p), sizeof(zwplus_info_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Save the Z-wave+ information into endpoint
                    intf->ep->zwplus_info = *info;
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_SNSR) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_SENSOR:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_SENSOR_INFO_REPORT)
                {
                    if_sensor_data_t    *sensor_dat;
                    zwsensor_t          *snsr_report;

                    memcpy(&snsr_report, data, sizeof(zwsensor_t *));
                    memcpy(&intf, data + sizeof(zwsensor_t *), sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (nw->ni_sm_sub_sta == SENSOR_SUBSTA_DATA_TYPE)
                    {   //Sensor reading and type (For version 1 to 4)

                        //Save sensor type and unit
                        sensor_dat = (if_sensor_data_t *)malloc(sizeof(if_sensor_data_t));

                        if (sensor_dat)
                        {
                            sensor_dat->sensor_type = snsr_report->type;
                            sensor_dat->sensor_unit = (0x01 << snsr_report->unit);

                            if (intf->data_cnt > 0)
                            {
                                free(intf->data);
                            }

                            //Assign to interface
                            intf->data_cnt = 1;
                            intf->data_item_sz = sizeof(if_sensor_data_t);
                            intf->data = sensor_dat;
                        }
                    }
                    else if (nw->ni_sm_sub_sta == SENSOR_SUBSTA_DATA)
                    {   //Sensor readings for each type and unit (For version 5 and greater)

                        if_sensor_data_t    *sensor_dat = (if_sensor_data_t *)intf->data;

                        if (sensor_dat && (nw->snsr_rd_idx < intf->data_cnt))
                        {
                            if ((snsr_report->type == sensor_dat[nw->snsr_rd_idx].sensor_type)
                                && (snsr_report->unit == nw->snsr_unit_idx))
                            {   //The report is correct.  Get the next supported unit of the same sensor type reading
                                nw->snsr_unit_idx++;
                                result = zwnet_sm_sensor_rpt_get(nw, intf);
                                if (result == 0)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_SENSOR_TYPE_REPORT)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != SENSOR_SUBSTA_TYPE)
                    {
                        break;
                    }

                    if (intf->data_cnt > 0)
                    {
                        //Start getting supported sensor units
                        nw->snsr_rd_idx = 0;
                        result = zwnet_sm_sensor_unit_get(nw, intf);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = SENSOR_SUBSTA_UNIT;
                            break;
                        }
                        else if (result == 1)
                        {   //Get readings for each sensor type and unit

                            nw->snsr_unit_idx = nw->snsr_rd_idx = 0;

                            result = zwnet_sm_sensor_rpt_get(nw, intf);
                            if (result == 0)
                            {
                                //Change sub-state
                                nw->ni_sm_sub_sta = SENSOR_SUBSTA_DATA;
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_SENSOR_UNIT_REPORT)
                {
                    if_sensor_data_t    *sensor_dat;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != SENSOR_SUBSTA_UNIT)
                    {
                        break;
                    }

                    sensor_dat = (if_sensor_data_t *)intf->data;

                    //Check whether the sensor type is correct
                    if ((nw->snsr_rd_idx < intf->data_cnt)
                        && (sensor_dat[nw->snsr_rd_idx].sensor_type == data[0]))
                    {

                        //Get next supported sensor units
                        nw->snsr_rd_idx++;
                        result = zwnet_sm_sensor_unit_get(nw, intf);
                        if (result == 0)
                        {
                            break;
                        }
                        else if (result == 1)
                        {   //Get readings for each sensor type and unit

                            nw->snsr_unit_idx = nw->snsr_rd_idx = 0;

                            result = zwnet_sm_sensor_rpt_get(nw, intf);
                            if (result == 0)
                            {
                                //Change sub-state
                                nw->ni_sm_sub_sta = SENSOR_SUBSTA_DATA;
                                break;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    if ((nw->ni_sm_sub_sta != SENSOR_SUBSTA_TYPE) && (nw->ni_sm_sub_sta != SENSOR_SUBSTA_UNIT))
                    {
                        zwnet_1_ep_job_cmplt(nw);
                        break;
                    }
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_MAX_GROUP) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_MAX_GROUP:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_GRP_CNT_REPORT)
                {
                    uint8_t *max_grp;

                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_SUBSTA_MAX_SUPP)
                    {
                        break;
                    }

                    max_grp = (uint8_t *)intf->data;

                    if (max_grp)
                    {
                        //Set controller node id into group 1
                        if (nw->sm_job.auto_cfg)
                        {
                            zwnode_p        node = intf->ep->node;
                            if_rec_grp_t    *grp_rec;
                            int             i;
                            int             j;

                            //Check whether device configuration record has other groups to configure
                            nw->dev_cfg_rd_idx = 0;
                            if (node->dev_cfg_valid)
                            {
                                grp_rec = (if_rec_grp_t *)zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_GROUP);
                                if (grp_rec)
                                {    //Copy groups except group 1
                                    for (i=j=0; j<grp_rec->grp_cnt; j++)
                                    {
                                        if (grp_rec->grp_id[j] != 1)
                                        {
                                            nw->dev_cfg_grp.grp_id[i++] = grp_rec->grp_id[j];
                                        }
                                    }
                                    nw->dev_cfg_grp.grp_cnt = i;
                                }
                            }
                            else
                            {
                                nw->dev_cfg_grp.grp_cnt = 0;
                            }

                            result = zwnet_add_ctlr_grp(nw, intf->ep->intf, NULL, 1, zwnet_add_ctlr_grp_cb);

                            if (result >= 0)
                            {
                                //Change sub-state
                                nw->ni_sm_sub_sta = GRP_SUBSTA_SET;

                                //Restart timer
                                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_GRP_SET_STATUS)
                {
                    zwifd_t grp_ifd;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_SUBSTA_SET)
                    {
                        break;
                    }

                    if (nw->dev_cfg_grp.grp_cnt)
                    {
                        if (nw->dev_cfg_rd_idx < nw->dev_cfg_grp.grp_cnt)
                        {
                            result = zwnet_add_ctlr_grp(nw, intf->ep->intf, NULL, nw->dev_cfg_grp.grp_id[nw->dev_cfg_rd_idx], zwnet_add_ctlr_grp_cb);
                            if (result >= 0)
                            {
                                nw->dev_cfg_rd_idx++;

                                //Restart timer
                                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
                                break;
                            }
                        }
                    }

                    //Check whether controller id was set successfully into group 1
                    zwif_get_desc(intf, &grp_ifd);

                    result = zwnet_sm_grp_get(&grp_ifd, 1, zwnet_grp_rpt_cb);
                    if (result >= 0)
                    {
                        //Change sub-state
                        nw->ni_sm_sub_sta = GRP_SUBSTA_GET;

                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
                        break;
                    }
                }
                else if (evt == EVT_S_EP_GRP_MEMBER_REPORT)
                {
                    int          i;
                    int          ctlr_id_found = 0;
                    grp_member_t *grp_member;
                    uint8_t      grp_cnt;

                    grp_cnt = data[1];
                    memcpy(&grp_member, data + 2, sizeof(grp_member_t *));
                    memcpy(&intf, data + 2 + sizeof(grp_member_t *), sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_SUBSTA_GET)
                    {
                        break;
                    }

                    //Check for lifeline group
                    if (data[0] != 1)
                    {
                        break;
                    }

                    //Check for controller id in the group
                    for (i=0; i<grp_cnt; i++)
                    {
                        if (grp_member[i].node_id == nw->ctl.nodeid)
                        {
                            if (grp_member[i].ep_id <= 1)
                            {
                                ctlr_id_found = 1;
                                break;
                            }
                        }
                    }

                    if (!ctlr_id_found)
                    {
                        zwifd_t grp_ifd;

                        zwif_get_desc(intf, &grp_ifd);

                        //Clear all member in the group
                        result = zwnet_sm_grp_del(&grp_ifd, 1);
                        if (result >= 0)
                        {
                            //Wait for 0.5s before add controller id again.
                            //This is to prevent some devices that can't accept two commands together to fail.
                            plt_sleep(500);

                            zwnet_add_ctlr_grp(nw, intf->ep->intf, NULL, 1, NULL);
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_AGI) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_GROUP_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_GRP_CMD_LST_REPORT)
                {
                    if_grp_info_dat_t   *grp_data;
                    grp_cmd_ent_t       *cmd_lst;
                    uint8_t             cmd_ent_cnt;
                    uint8_t             grp_id;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_INFO_SUBSTA_CMD_LST)
                    {
                        break;
                    }

                    grp_id = data[0];
                    cmd_ent_cnt = data[1];
                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&cmd_lst, data + 2 + sizeof(zwif_p), sizeof(grp_cmd_ent_t *));

                    grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    //Check whether the report is what we requested.
                    if (grp_id != nw->grp_rd_idx)
                    {
                        break;
                    }

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (grp_data->grp_info[grp_id-1] == NULL)
                    {
                        zw_grp_info_p   grp_info;
                        grp_info = (zw_grp_info_p)calloc(1, sizeof(zw_grp_info_t) + (cmd_ent_cnt * sizeof(grp_cmd_ent_t)));

                        if (grp_info)
                        {
                            grp_data->grp_info[grp_id-1] = grp_info;
                            grp_data->valid_grp_cnt++;

                            grp_info->grp_num = grp_id;
                            grp_info->cmd_ent_cnt = cmd_ent_cnt;
                            memcpy(grp_info->cmd_lst, cmd_lst, cmd_ent_cnt * sizeof(grp_cmd_ent_t));

                            //Get command list for next group
                            result = zwnet_sm_grp_cmd_lst_get(nw, intf);
                            if (result == 0)
                            {
                                break;
                            }
                            else if (result == 1)
                            {   //No more command list to get
                                //Get group name
                                nw->grp_rd_idx = 0;
                                result = zwnet_sm_grp_name_get(nw, intf);
                                if (result == 0)
                                {
                                    //Change sub-state
                                    nw->ni_sm_sub_sta = GRP_INFO_SUBSTA_NAME;
                                    break;
                                }
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_GRP_NAME_REPORT)
                {
                    if_grp_info_dat_t   *grp_data;
                    zw_grp_info_p       grp_info;
                    uint8_t             *name;
                    uint8_t             grp_id;
                    uint8_t             str_len;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_INFO_SUBSTA_NAME)
                    {
                        break;
                    }

                    grp_id = data[0];
                    str_len = data[1];
                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&name, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    //Check whether the report is what we requested.
                    if (grp_id != nw->grp_rd_idx)
                    {
                        break;
                    }

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    grp_info = grp_data->grp_info[grp_id-1];

                    if (grp_info)
                    {
                        //Check for valid UTF-8 string
                        str_len = plt_utf8_chk(name, str_len);

                        memcpy(grp_info->name, name, str_len);
                        grp_info->name[str_len] = '\0';
                    }

                    //Get group name for next group
                    result = zwnet_sm_grp_name_get(nw, intf);
                    if (result == 0)
                    {
                        break;
                    }
                    else if (result == 1)
                    {   //No more group name to get
                        //Get group info
                        nw->grp_rd_idx = 0;
                        result = zwnet_sm_grp_info_get(nw, intf);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = GRP_INFO_SUBSTA_INFO;
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_GRP_INFO_REPORT)
                {
                    if_grp_info_dat_t   *grp_data;
                    int                 i;
                    zw_grp_info_p       grp_info;
                    zw_grp_info_ent_t   *grp_info_entries;
                    uint8_t             grp_cnt;
                    uint8_t             grp_id;
                    uint8_t             dynamic;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_INFO_SUBSTA_INFO)
                    {
                        break;
                    }

                    grp_cnt = data[0];
                    dynamic = data[1];
                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&grp_info_entries, data + 2 + sizeof(zwif_p), sizeof(zw_grp_info_ent_t *));

                    grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    grp_data->dynamic = dynamic;

                    grp_id = 0;

                    for (i=0; i<grp_cnt; i++)
                    {
                        grp_id = grp_info_entries[i].grp_num;

                        if (grp_id <= grp_data->group_cnt)
                        {
                            grp_info = grp_data->grp_info[grp_id-1];

                            if (grp_info)
                            {
                                grp_info->profile = grp_info_entries[i].profile;
                                grp_info->evt_code = grp_info_entries[i].evt_code;
                            }
                        }
                    }

                    if ((grp_cnt == grp_data->group_cnt)
                        || (grp_id == grp_data->group_cnt))
                    {   //Done.
                        nw->ni_sm_sub_sta = 0;
                    }
                    else
                    {
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_THRMO_FAN_MD) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_THRMO_FAN_MD:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_THRMO_FAN_MD_SUP)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != THRMO_FAN_MD_SUBSTA_SUPP))
                    {
                        break;
                    }

                    //Get thermostat fan mode
                    if (zwnet_sm_thrmo_fan_mode_get(nw, intf) == 0)
                    {
                        break;
                    }

                }
                else if (evt == EVT_S_EP_THRMO_FAN_MD_DAT)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != THRMO_FAN_MD_SUBSTA_DATA))
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_THRMO_MD) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_THRMO_MD:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_THRMO_MD_DAT)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != THRMO_MD_SUBSTA_DATA))
                    {
                        break;
                    }

                    //Get supported thermostat modes
                    if (zwnet_sm_thrmo_mode_sup_get(nw, intf) == 0)
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_THRMO_MD_SUP)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != THRMO_MD_SUBSTA_SUPP))
                    {
                        break;
                    }

                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_THRMO_SETP) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_THRMO_SETP:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_THRMO_SETP_SUP)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != THRMO_SETP_SUBSTA_SUPP))
                    {
                        break;
                    }

                    //Get setpoint value for each setpoint type
                    if (intf->data_cnt > 0)
                    {
                        nw->thrmo_rd_idx = nw->thrmo_setp_intp = 0;
                        if (zwnet_sm_thrmo_setp_get(nw, intf) == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_THRMO_SETP_DAT)
                {
                    if_thrmo_setp_data_t    *setp_dat;
                    zwsetp_p                setp_report;

                    memcpy(&setp_report, data, sizeof(zwsetp_p));
                    memcpy(&intf, data + sizeof(zwsetp_p), sizeof(zwif_p));

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != THRMO_SETP_SUBSTA_DATA))
                    {
                        break;
                    }

                    setp_dat = (if_thrmo_setp_data_t *)intf->data;

                    if (setp_dat && (nw->thrmo_rd_idx < setp_dat->setp_len))
                    {
                        if (nw->thrmo_setp_intp == 0)
                        {   //Interpretation A

                            if (setp_report->type == setp_dat->setp[nw->thrmo_rd_idx])
                            {   //The report is correct.  Get setpoint value for the next setpoint type
                                nw->thrmo_rd_idx++;
                                if (zwnet_sm_thrmo_setp_get(nw, intf) == 0)
                                {
                                    break;
                                }
                            }
                            else if (setp_report->type == 0)
                            {
                                //The requested setpoint type is unsupported. The device is probably using interpretation B
                                //in its supported setpoint type bit-mask.
                                if ((setp_dat->setp[nw->thrmo_rd_idx] - 4)  > 6)
                                {
                                    nw->thrmo_setp_intp = 1;//change to interpretation B
                                    if (zwnet_sm_thrmo_setp_get(nw, intf) == 0)
                                    {
                                        break;
                                    }
                                }
                            }
                            else
                            {   //Unmatched report, ignore it.
                                break;
                            }
                        }
                        else
                        {    //Interpretation B
                            if (setp_report->type == (setp_dat->setp[nw->thrmo_rd_idx] - 4))
                            {   //The report is correct. Update supported setpoint type list
                                setp_dat->setp[nw->thrmo_rd_idx] -= 4;

                                //Get setpoint value for the next setpoint type
                                nw->thrmo_rd_idx++;
                                if (zwnet_sm_thrmo_setp_get(nw, intf) == 0)
                                {
                                    break;
                                }
                            }
                            else
                            {   //Unmatched report, ignore it.
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    if ((nw->ni_sm_sub_sta == THRMO_SETP_SUBSTA_DATA) && (nw->thrmo_setp_intp == 0))
                    {   //Get report using interpretation A has timed out
                        intf = zwif_find_cls(nw->single_ep_job.first_ep->intf, COMMAND_CLASS_THERMOSTAT_SETPOINT);
                        if (intf)
                        {
                            //The requested setpoint type is unsupported. The device is probably using interpretation B
                            //in its supported setpoint type bit-mask.
                            nw->thrmo_setp_intp = 1;//change to interpretation B
                            if (zwnet_sm_thrmo_setp_get(nw, intf) == 0)
                            {
                                break;
                            }
                        }
                    }
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_MUL_SWITCH) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_MUL_SWITCH:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_MUL_SWITCH_DAT)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != LEVEL_SUBSTA_DATA))
                    {
                        break;
                    }

                    //Get supported switch
                    if (zwnet_sm_lvl_sup_get(nw, intf) == 0)
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_MUL_SWITCH_SUP)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != LEVEL_SUBSTA_SUPP))
                    {
                        break;
                    }

                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_AV) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_AV:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_AV_REPORT_CNT)
                {
                    zwif_av_cap_t   *av_cap;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != AV_SUBSTA_RPT_CNT)
                    {
                        break;
                    }

                    if (intf->tmp_data)
                    {
                        av_cap = (zwif_av_cap_t *)intf->tmp_data;

                        //Clear data
                        if (av_cap->bit_mask_buf)
                        {
                            free(av_cap->bit_mask_buf);
                        }
                        memset(av_cap, 0, sizeof(zwif_av_cap_t));
                    }
                    else
                    {
                        intf->tmp_data = calloc(1, sizeof(zwif_av_cap_t));

                        av_cap = (zwif_av_cap_t *)intf->tmp_data;
                    }

                    if (av_cap && data[0])
                    {
                        av_cap->total_rpt = data[0];
                        //Start getting supported simple AV controls
                        result = zwnet_sm_av_ctl_get(nw, intf, av_cap);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = AV_SUBSTA_CTL_RPT;
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_AV_CTL_REPORT)
                {

                    memcpy(&intf, data + 4, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != AV_SUBSTA_CTL_RPT)
                    {
                        break;
                    }

                    if (intf->tmp_data)
                    {
                        uint8_t         rpt_num;        //Report number of this report
                        uint16_t        bit_mask_len;   //Bit mask length of this report
                        uint8_t         *tmp_buf;
                        uint8_t         *data_buf;
                        zwif_av_cap_t   *av_cap;

                        av_cap = (zwif_av_cap_t *)intf->tmp_data;

                        rpt_num = data[0];

                        bit_mask_len = data[1];
                        bit_mask_len = (bit_mask_len << 8) | data[2];

                        memcpy(&data_buf, data + 4 + sizeof(zwif_p), sizeof(uint8_t *));

                        //Check report number to avoid duplicate
                        if (av_cap->rpt_num >= rpt_num)
                        {
                            break;
                        }

                        //Store the bit-mask
                        if (av_cap->bit_mask_buf)
                        {
                            //Re-allocate memory
                            tmp_buf = realloc(av_cap->bit_mask_buf,
                                              av_cap->bit_mask_len + bit_mask_len);
                        }
                        else
                        {
                            //Allocate memory
                            tmp_buf = malloc(bit_mask_len);
                        }

                        if (tmp_buf)
                        {
                            av_cap->bit_mask_buf = tmp_buf;
                            memcpy(av_cap->bit_mask_buf + av_cap->bit_mask_len,
                                   data_buf, bit_mask_len);

                            av_cap->rpt_num = rpt_num;
                            av_cap->bit_mask_len += bit_mask_len;

                            //Get next report
                            result = zwnet_sm_av_ctl_get(nw, intf, av_cap);
                            if (result == 0)
                            {
                                break;
                            }

                            if (result == 1)
                            {   //No more report to get
                                //Save the supported AV control bit-mask

                                //Free any existing data
                                if (intf->data_cnt > 0)
                                {
                                    free(intf->data);
                                }

                                if (av_cap->bit_mask_len)
                                {
                                    intf->data = malloc(av_cap->bit_mask_len);
                                    if (intf->data)
                                    {
                                        memcpy(intf->data, av_cap->bit_mask_buf, av_cap->bit_mask_len);
                                        intf->data_cnt = 1;
                                        intf->data_item_sz = (uint8_t)av_cap->bit_mask_len;
                                    }
                                }
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_ALARM) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_ALARM:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_ALARM_TYPE)
                {

                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != ALARM_SUBSTA_TYPE)
                    {
                        break;
                    }

                    if (intf->data_cnt > 0)
                    {
                        //Start getting supported events of each alarm type
                        if (intf->real_ver > 2)
                        {
                            nw->alrm_rd_idx = 0;
                            result = zwnet_sm_alrm_evt_get(nw, intf);
                            if (result == 0)
                            {
                                //Change sub-state
                                nw->ni_sm_sub_sta = ALARM_SUBSTA_EVENT;
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_ALARM_EVENT)
                {
                    if_alarm_data_t     *alarm_dat;
                    uint8_t             idx;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != ALARM_SUBSTA_EVENT)
                    {
                        break;
                    }

                    alarm_dat = (if_alarm_data_t *)intf->data;
                    idx = nw->alrm_rd_idx;

                    //Check whether the alarm type is correct
                    if ((idx < alarm_dat->type_evt_cnt)
                        && (alarm_dat->type_evt[idx].ztype == data[0]))
                    {
                        //Get next supported events
                        nw->alrm_rd_idx++;
                        result = zwnet_sm_alrm_evt_get(nw, intf);
                        if (result == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_PROT) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_PROT:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_PROT)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_MAX_USR_CODES) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_MAX_USR_CODES:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_USR_CODE_CNT_REPORT)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_METER) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_METER:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_METER_DAT)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != METER_SUBSTA_DATA)
                    {
                        break;
                    }

                    if ((intf->real_ver > 1) && (nw->snsr_rd_idx < 7))
                    {   //version 2 and above

                        //Get next meter reading
                        nw->snsr_rd_idx++;
                        result = zwnet_sm_meter_rpt_get(nw, intf);
                        if (result == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_METER)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != METER_SUBSTA_UNIT)
                    {
                        break;
                    }

                    if (intf->data_cnt > 0)
                    {
                        //Get meter reading
                        nw->snsr_rd_idx = 0;
                        result = zwnet_sm_meter_rpt_get(nw, intf);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = METER_SUBSTA_DATA;
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_METER_DESC) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_METER_DESC:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_METER_TBL_DESC)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_CFG_PARAM) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_SET_CFG_PARAM:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_CFG_SET_STATUS)
                {
                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Send the next configuration parameter
                    result = zwnet_sm_cfg_param_send(intf, &nw->dev_cfg_param, nw->dev_cfg_rd_idx);

                    if (result >= 0)
                    {
                        nw->dev_cfg_rd_idx++;

                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_BIN_SNSR) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_BIN_SENSOR:
        //----------------------------------------------------------------
            {

                if (evt == EVT_S_EP_BSENSOR_INFO_REPORT)
                {
                    uint8_t *sensor_type;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != BSENSOR_SUBSTA_DATA)
                    {
                        break;
                    }

                    sensor_type = (uint8_t *)intf->data;

                    if ((data[0] > 0) && sensor_type)
                    {   //version 2

                        //Check whether the sensor type is correct
                        if ((nw->snsr_rd_idx < intf->data_cnt)
                            && (sensor_type[nw->snsr_rd_idx] == data[0]))
                        {
                            //Get next sensor type reading
                            nw->snsr_rd_idx++;
                            result = zwnet_sm_bsensor_rpt_get(nw, intf);
                            if (result == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_BSENSOR_TYPE_REPORT)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != BSENSOR_SUBSTA_TYPE)
                    {
                        break;
                    }

                    if (intf->data_cnt > 0)
                    {
                        //Get sensor reading
                        nw->snsr_rd_idx = 0;
                        result = zwnet_sm_bsensor_rpt_get(nw, intf);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = BSENSOR_SUBSTA_DATA;
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_DOORLOCK) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_DOORLOCK:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_DOORLOCK_OP_REPORT)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != DOORLCK_SUBSTA_OP))
                    {
                        break;
                    }

                    //Get door lock configuration
                    if (zwnet_sm_dlck_cfg_get(nw, intf) == 0)
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_DOORLOCK_CFG_REPORT)
                {
                    intf = (zwif_p)data;

                    //Validate endpoint and sub-state
                    if ((intf->ep != nw->single_ep_job.first_ep)
                        || (nw->ni_sm_sub_sta != DOORLCK_SUBSTA_CFG))
                    {
                        break;
                    }

                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_CSC) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

		//----------------------------------------------------------------
		case S_EP_STA_GET_CENTRAL_SCENE:
		//----------------------------------------------------------------
		{
			if (evt == EVT_S_EP_CENTRAL_SCENE_SUP)
			{
                intf = (zwif_p)data;

				//Check whether the endpoint matches the request
				if ((intf->ep != nw->single_ep_job.first_ep))
				{
					break;
				}
			}
			else if (evt == EVT_S_EP_NW_TMOUT)
			{
				//Skip the rest of command classes
				zwnet_1_ep_job_cmplt(nw);
				break;
			}
			else
			{   //Unknown event for this state
				break;
			}

			//Start processing other command classes
			if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_ALRM_SNSR) == 0)
			{
				break;
			}

			//Completed job
			zwnet_1_ep_job_cmplt(nw);
		}
		break;

		//----------------------------------------------------------------
		case S_EP_STA_GET_ALRM_SENSOR:
		//----------------------------------------------------------------
		{
			if (evt == EVT_S_EP_ALRM_SNSR_TYPE_REPORT)
			{
				intf = (zwif_p)data;

				//Check whether the endpoint matches the request
				if (intf->ep != nw->single_ep_job.first_ep)
				{
					break;
				}

				//Check whether sub-state is o.k.
				if (nw->ni_sm_sub_sta != ALRM_SNSR_SUBSTA_TYPE)
				{
					break;
				}

				if (intf->data_cnt > 0)
				{
					//Get sensor reading
					nw->snsr_rd_idx = 0;
					result = zwnet_sm_alrm_snsr_rpt_get(nw, intf);
					if (result == 0)
					{
						//Change sub-state
						nw->ni_sm_sub_sta = ALRM_SNSR_SUBSTA_DATA;
						break;
					}
				}
			}
			else if (evt == EVT_S_EP_ALRM_SNSR_INFO_REPORT)
			{
				zw_alrm_snsr_p         snsr_rpt;
				uint8_t					*sensor_type;

				memcpy(&snsr_rpt, data, sizeof(zw_alrm_snsr_p));
				memcpy(&intf, data + sizeof(zw_alrm_snsr_p), sizeof(zwif_p));

				//Check whether the endpoint matches the request
				if (intf->ep != nw->single_ep_job.first_ep)
				{
					break;
				}

				//Check whether sub-state is o.k.
				if (nw->ni_sm_sub_sta != ALRM_SNSR_SUBSTA_DATA)
				{
					break;
				}

				sensor_type = (uint8_t *)intf->data;

				if (snsr_rpt->type != 0xFF)
				{

					//Check whether the sensor type is correct
					if ((nw->snsr_rd_idx < intf->data_cnt)
						&& (sensor_type[nw->snsr_rd_idx] == snsr_rpt->type))
					{
						//Get next sensor type reading
						nw->snsr_rd_idx++;
						result = zwnet_sm_alrm_snsr_rpt_get(nw, intf);
						if (result == 0)
						{
							break;
						}
					}
				}
			}
			else if (evt == EVT_S_EP_NW_TMOUT)
			{
				//Skip the rest of command classes
				zwnet_1_ep_job_cmplt(nw);
				break;
			}
			else
			{   //Unknown event for this state
				break;
			}

			//Start processing other command classes
			if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_SWITCH) == 0)
			{
				break;
			}

			//Completed job
			zwnet_1_ep_job_cmplt(nw);
		}
		break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_SWITCH:
        case S_EP_STA_GET_THRMO_OP:
        case S_EP_STA_GET_BATTERY:
        case S_EP_STA_GET_BASIC:
        case S_EP_STA_GET_THRMO_FAN_STA:
        //----------------------------------------------------------------
            {   //Fetching readings of device for data caching
                static const zwnet_1_ep_tab_t s_ep_table[] = {
                    {S_EP_STA_GET_SWITCH, EVT_S_EP_SWITCH_REPORT, S_EP_ACT_THRMO_OP}
                    ,{S_EP_STA_GET_THRMO_OP, EVT_S_EP_THRMO_OP_REPORT, S_EP_ACT_BATTERY}
                    ,{S_EP_STA_GET_BATTERY, EVT_S_EP_BATTERY_REPORT, S_EP_ACT_THRMO_FAN_STA}
                    ,{S_EP_STA_GET_THRMO_FAN_STA, EVT_S_EP_THRMO_FAN_STA, S_EP_ACT_NAME_LOC}
                    ,{S_EP_STA_GET_BASIC, EVT_S_EP_BASIC_REPORT, S_EP_ACT_END}
                                                             };
                int i;
                int found = 0;

                //Search for matching state
                for (i=0; i< sizeof(s_ep_table)/sizeof(zwnet_1_ep_tab_t); i++)
                {
                    if (nw->single_ep_sta == s_ep_table[i].state)
                    {
                        found = 1;
                        break;
                    }
                }

                if (found && evt == s_ep_table[i].evt)
                {
                    intf = (zwif_p)data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Non critical, proceed to next command class
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if ((s_ep_table[i].act != S_EP_ACT_END)
                    && (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, s_ep_table[i].act) == 0))
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_NODE_NAMELOC:
        //----------------------------------------------------------------
            {
                zwep_p      ep;

                if (evt == EVT_S_EP_NAME_REPORT)
                {
                    zwifd_t ifd;
                    uint8_t str_len;    //string length

                    str_len = data[1];
                    memcpy(&intf, data + 2 + str_len, sizeof(zwif_p));
                    ep = intf->ep;
                    //Check whether the endpoint matches the request
                    if (ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    zwnet_sm_updt_nameloc(data, ep->name);

                    //Setup location report callback
                    zwif_get_desc(intf, &ifd);

                    result = zwif_set_sm_report(&ifd, zwnet_loc_rpt_cb, NODE_NAMING_NODE_LOCATION_REPORT);
                    if (result == 0)
                    {
                        //Get the location
                        result = zwif_get_report(&ifd, NULL, 0, NODE_NAMING_NODE_LOCATION_GET, zwif_tx_sts_cb);
                        if (result >= 0)
                        {
                            //Restart timer
                            plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                            nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
                            break;
                        }
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_1_ep_info_sm: zwif_get_report for loc with error:%d", result);
                    }

                }
                else if (evt == EVT_S_EP_LOC_REPORT)
                {
                    uint8_t     str_len;    //string length

                    str_len = data[1];
                    memcpy(&intf, data + 2 + str_len, sizeof(zwif_p));
                    ep = intf->ep;
                    //Check whether the endpoint matches the request
                    if (ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    zwnet_sm_updt_nameloc(data, ep->loc);
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    zwnet_1_ep_job_cmplt(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                if (zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_BASIC) == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;
    }

    return 0;
}


/**
zwnet_sm_1_ep_info_cb - Get single endpoint info callback
@param[in]	nw		        Network
@param[in]	first_node_id   The first node id
@return
*/
static void zwnet_sm_1_ep_info_cb(zwnet_p nw, uint8_t first_node_id)
{
    zwnet_ep_info_sm(nw, EVT_EP_S_EP_INFO_DONE, NULL);
}


/**
zwnet_ep_info_sm - Endpoint info state-machine
@param[in] nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return             The current state of the state-machine.
*/
static int zwnet_ep_info_sm(zwnet_p nw, zwnet_ep_evt_t evt, uint8_t *data)
{
    int curr_sta;
    int result;

    switch (nw->ep_sm_sta)
    {
        //----------------------------------------------------------------
        case EP_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_EP_SM_START)
            {
                ep_sm_job_t  sm_job;
                ep_sm_job_t  *ep_sm_job = (ep_sm_job_t *)data;

                if (ep_sm_job)
                {
                    //Save the job
                    nw->ep_sm_job = *ep_sm_job;
                }
                else
                {
                    break;
                }

                //Check whether the endpoint is a secure endpoint
                //and if so, get the security supported command classes
                result = zwnet_ep_sm_sec_get(nw, ep_sm_job->first_ep);
                if (result == 0)
                {
                    break;
                }

                //Start processing version command class
                result = zwnet_ep_sm_ver_get(nw, ep_sm_job->first_ep);
                if (result == 0)
                {
                    break;
                }

                //Process other command classes
                sm_job.cb = zwnet_sm_1_ep_info_cb;
                sm_job.first_ep = ep_sm_job->first_ep;
                sm_job.use_cache = ep_sm_job->use_cache;

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
                {
                    //Change state
                    nw->ep_sm_sta = EP_STA_GET_EP_INFO;
                    break;
                }
                else
                {
                    if (nw->single_ep_sta != S_EP_STA_IDLE)
                    {
                        debug_zwapi_msg(&nw->plt_ctx,
                                        "zwnet_ep_info_sm: single endpoint state-machine is in wrong state: %d",
                                        nw->single_ep_sta);
                    }
                }

                //Start processing next endpoint
                zwnet_ep_sm_next(nw);
            }
            break;

        //----------------------------------------------------------------
        case EP_STA_GET_SEC_CMD_SUP:
        //----------------------------------------------------------------
            {
                ep_sm_job_t sm_job = {0};
                zwep_p      ep;

                if (evt == EVT_EP_SEC_SUP_REPORT)
                {
                    zwif_p  intf;
                    uint16_t *sec_cls;

                    memcpy(&sec_cls, data + 2, sizeof(uint16_t *));
                    memcpy(&intf, data + 2 + sizeof(uint16_t *), sizeof(zwif_p));

                    ep = intf->ep;

                    //Check whether the endpoint matches the request
                    if (ep != nw->ep_sm_job.first_ep)
                    {
                        break;
                    }

                    //Add the secure command classes to interfaces
                    zwnet_ni_sec_updt(ep, sec_cls, data[0]);

                }
                else if (evt == EVT_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing version command class
                result = zwnet_ep_sm_ver_get(nw, nw->ep_sm_job.first_ep);
                if (result == 0)
                {
                    break;
                }

                //Process other command classes
                sm_job.cb = zwnet_sm_1_ep_info_cb;
                sm_job.first_ep = nw->ep_sm_job.first_ep;
                sm_job.use_cache = nw->ep_sm_job.use_cache;

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
                {
                    //Change state
                    nw->ep_sm_sta = EP_STA_GET_EP_INFO;
                    break;
                }
                else
                {
                    if (nw->single_ep_sta != S_EP_STA_IDLE)
                    {
                        debug_zwapi_msg(&nw->plt_ctx,
                                        "zwnet_ep_info_sm: single endpoint state-machine is in wrong state: %d",
                                        nw->single_ep_sta);
                    }
                }

                //Start processing next endpoint
                result = zwnet_ep_sm_next(nw);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                nw->ep_sm_sta = EP_STA_IDLE;

                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Invoke user callback
                if (nw->ep_sm_job.cb)
                {
                    nw->ep_sm_job.cb(nw, 0);
                }

            }
            break;

        //----------------------------------------------------------------
        case EP_STA_GET_CMD_VER:
        //----------------------------------------------------------------
            {
                ep_sm_job_t sm_job = {0};
                zwep_p      ep;

                if (evt == EVT_EP_VER_REPORT)
                {
                    zwif_p      ver_intf;
                    uint16_t    cmd_cls;
                    uint8_t     cmd_cls_ver;

                    cmd_cls = data[0];

                    cmd_cls = (cmd_cls << 8) | data[1];

                    cmd_cls_ver = data[2];

                    //Check whether the reported command class matches the request
                    if (cmd_cls != nw->ni_sm_cls)
                    {
                        break;
                    }

                    memcpy(&ver_intf, data + 4, sizeof(zwif_p));
                    ep = ver_intf->ep;
                    //Check whether the endpoint matches the request
                    if (ep != nw->ep_sm_job.first_ep)
                    {
                        break;
                    }

                    result = zwnet_sm_ver_rpt_hdlr(nw, &ver_intf, zwnet_ep_ver_rpt_cb, cmd_cls, cmd_cls_ver);

                    if (result == 0)
                    {   //Successfully send command to query next command class version
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_ep_tmout_cb, nw);
                        break;
                    }
                }
                else if (evt == EVT_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //
                //Start processing other command classes by using a single
                //endpoint info state-machine
                //

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Initialize endpoint info state-machine
                //nw->ep_sm_sta = EP_STA_IDLE;

                sm_job.cb = zwnet_sm_1_ep_info_cb;
                sm_job.first_ep = nw->ep_sm_job.first_ep;
                sm_job.use_cache = nw->ep_sm_job.use_cache;

                if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
                {
                    //Change state
                    nw->ep_sm_sta = EP_STA_GET_EP_INFO;
                    break;
                }
                else
                {
                    if (nw->single_ep_sta != S_EP_STA_IDLE)
                    {
                        debug_zwapi_msg(&nw->plt_ctx,
                                        "zwnet_ep_info_sm: single endpoint state-machine is in wrong state: %d",
                                        nw->single_ep_sta);
                    }
                }

                //Start processing next endpoint
                result = zwnet_ep_sm_next(nw);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                nw->ep_sm_sta = EP_STA_IDLE;

                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Invoke user callback
                if (nw->ep_sm_job.cb)
                {
                    nw->ep_sm_job.cb(nw, 0);
                }

            }
            break;

        //----------------------------------------------------------------
        case EP_STA_GET_EP_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_EP_S_EP_INFO_DONE)
                {
                    //Start processing next endpoint
                    result = zwnet_ep_sm_next(nw);
                    if (result == 0)
                    {
                        break;
                    }

                    //Completed job
                    nw->ep_sm_sta = EP_STA_IDLE;

                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                    nw->sm_tmr_ctx = NULL;

                    //Invoke user callback
                    if (nw->ep_sm_job.cb)
                    {
                        nw->ep_sm_job.cb(nw, 0);
                    }
                }
            }
            break;

    }
    curr_sta = nw->ep_sm_sta;

    return curr_sta;
}


/**
zwnet_ni_chd_get_cb - Get node info callback
@param[in]	appl_ctx    The application layer context
@param[in]	cached_ni   Cached node information
@return
*/
static void zwnet_ni_chd_get_cb(appl_layer_ctx_t *appl_ctx, appl_node_info_t *cached_ni)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_ni_chd_get_cb for node id:%u", cached_ni->node_id);
    debug_zwapi_msg(&nw->plt_ctx, "listening=%u, optional functions=%u, sensor=%u", cached_ni->listen, cached_ni->optional, cached_ni->sensor);
    debug_zwapi_msg(&nw->plt_ctx, "status=%u, age=%u minutes old", cached_ni->status, (unsigned)(1 << cached_ni->age));
    debug_zwapi_msg(&nw->plt_ctx, "Device type: basic=%02Xh, generic=%02Xh, specific=%02X", cached_ni->basic, cached_ni->gen, cached_ni->spec);
    if (cached_ni->cmd_cnt > 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "unsecure command classes:");
        zwnet_cmd_cls_show(nw, cached_ni->cmd_cls, cached_ni->cmd_cnt);

    }
    if (cached_ni->cmd_cnt_sec > 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "secure command classes:");
        zwnet_cmd_cls_show(nw, cached_ni->cmd_cls_sec, cached_ni->cmd_cnt_sec);

    }
#ifdef MAP_IP_ASSOC_TO_ASSOC
    zwnet_ip_assoc_map(cached_ni);
#endif
    //Update the network data structure
    plt_mtx_lck(nw->mtx);
    zwnet_node_info_update(nw, cached_ni, NULL);

    zwnet_node_info_sm(nw, EVT_NODE_INFO, (uint8_t *)cached_ni);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_ep_updt - Update or create end points with the given command class
@param[in]	ep	    The first end point
@param[in]	cmd_cls The command class
@param[in]	inst	The number of instances of a given command class
@param[in]	propty	Multi-instance command class properties
@return     0 on success, negative error number on failure
@pre        Caller must lock the nw->mtx before calling this function.
*/
static int zwnet_ep_updt(zwep_p  ep, uint8_t cmd_cls, uint8_t inst, uint8_t propty)
{
    zwep_p      new_ep;
    zwep_p      tmp_ep;
    zwif_p      intf;
    uint8_t     ver;
    uint8_t     i;

    if (inst == 0)
    {
        return 0;
    }

    tmp_ep = ep;

    //Get the command class version from the virtual end point
    intf = zwif_find_cls(ep->intf, cmd_cls);
    ver = (intf)? intf->ver : 1;

    //Create or update end points
    for (i=0; i < inst; i++)
    {
        if (!tmp_ep->obj.next)
        {   //Create an end point
            new_ep = (zwep_p)calloc(1, sizeof(zwep_t));
            if (!new_ep)
            {
                return ZW_ERR_MEMORY;
            }
            new_ep->generic = ep->generic;
            new_ep->specific = ep->specific;
            new_ep->node = ep->node;
            new_ep->epid = tmp_ep->epid + 1;//increment the end point id from previous end point
            //Add to the end point list
            tmp_ep->obj.next = &new_ep->obj;
            //
            //Create first interface of COMMAND_CLASS_BASIC
            //
            intf = zwif_create(COMMAND_CLASS_BASIC, 1, propty);

            if (!intf)
            {
                return ZW_ERR_MEMORY;
            }
            //Save back link to end point
            intf->ep = new_ep;

            //Add interface to the end point
            new_ep->intf = intf;
        }
        //Work on valid end point
        tmp_ep = (zwep_p)tmp_ep->obj.next;

        //Check whether the interface for the command class has already been created
        if (zwif_find_cls(tmp_ep->intf, cmd_cls) == NULL)
        {
            intf = zwif_create(cmd_cls, ver, propty);

            if (!intf)
            {
                return ZW_ERR_MEMORY;
            }
            //Save back link to end point
            intf->ep = tmp_ep;

            //Add interface to the end point
            if (tmp_ep->intf)
            {   //Second interface onwards
                zwobj_add(&tmp_ep->intf->obj.next, &intf->obj);
            }
            else
            {   //First interface
                tmp_ep->intf = intf;
            }

        }
    }

    return ZW_ERR_NONE;
}


/**
zwnet_ep_rm - Remove an end point
@param[in]	ep	        The end point
@return     0 on success, negative error number on failure
@pre        Caller must lock the nw->mtx before calling this function.
*/
static void zwnet_ep_rm(zwep_p  ep)
{
    zwep_p first_ep;    //The first end point in the list

    first_ep = &ep->node->ep;

    zwep_intf_rm_all(ep);
    if (ep != first_ep)
    {
        //Free the end point object
        zwobj_del(&first_ep->obj.next, &ep->obj);
    }
}


/**
zwnet_ep_add - Add an end point with the given capability
@param[in]	ep	        The first end point in the list
@param[in]	ep_cap	    The end point capability
@param[in]	propty      Multi-channel Command class properties
@return     0 on success, negative error number on failure
@pre        Caller must lock the nw->mtx before calling this function.
*/
static int zwnet_ep_add(zwep_p  ep, ep_cap_t *ep_cap, uint8_t propty)
{
    zwep_p      new_ep;
    zwep_p      old_ep;
    zwif_p      intf;
    int         result;
    int         node_has_changed;
    uint16_t    *new_cmd_cls;
    uint8_t     i;
    uint8_t     cls_ver;    //version of the command class
    uint8_t     new_cmd_cnt;


    //Check whether the end point has already been created
    old_ep = zwep_find(ep, ep_cap->ep);
    if (old_ep)
    {
        if ((old_ep->specific == ep_cap->spec)
            && (old_ep->generic == ep_cap->gen))
        {   //End point matches the capabilities

            //Check for command classes
            node_has_changed = 0;
            for (i = 0; i < ep_cap->cmd_cnt; i++)
            {
                if (zwif_find_cls(old_ep->intf, ep_cap->cmd_cls[i]) == NULL)
                {
                    node_has_changed = 1;
                    break;
                }
            }

            if (!node_has_changed)
            {
                return ZW_ERR_NONE;
            }

        }

        //Delete the obsolete end point
        zwnet_ep_rm(old_ep);

    }

    new_ep = (zwep_p)calloc(1, sizeof(zwep_t));
    if (!new_ep)
    {
        return ZW_ERR_MEMORY;
    }
    new_ep->generic = ep_cap->gen;
    new_ep->specific = ep_cap->spec;
    new_ep->node = ep->node;
    new_ep->epid = ep_cap->ep;

    //Add to the end point list
    zwobj_add(&ep->obj.next, &new_ep->obj);

    //Add COMMAND_CLASS_BASIC to the command class list if there is none in the list
    result = zwnet_cmd_cls_add(COMMAND_CLASS_BASIC, ep_cap->cmd_cls, ep_cap->cmd_cnt,
                               &new_cmd_cls, &new_cmd_cnt);

    if (result < 0)
    {
        return result;
    }

    //Remove any duplicate command classes
    new_cmd_cnt = zwnet_cmd_cls_dedup(new_cmd_cls, new_cmd_cnt);

    //Add command classes and interfaces
    for (i = 0; i < new_cmd_cnt; i++)
    {
        //The version number of the command class should be
        //inherited from the virtual end point
        intf = zwif_find_cls(ep->intf, new_cmd_cls[i]);
        cls_ver = (intf)? intf->ver : 1;

        intf = zwif_create(new_cmd_cls[i], cls_ver, propty);

        if (!intf)
        {
            if (new_cmd_cls != ep_cap->cmd_cls)
            {
                free(new_cmd_cls);
            }
            return ZW_ERR_MEMORY;
        }
        //Save back link to end point
        intf->ep = new_ep;

        //Add interface to the end point
        if (new_ep->intf)
        {   //Second interface onwards
            zwobj_add(&new_ep->intf->obj.next, &intf->obj);
        }
        else
        {   //First interface
            new_ep->intf = intf;
        }
    }

    if (new_cmd_cls != ep_cap->cmd_cls)
    {
        free(new_cmd_cls);
    }
    return ZW_ERR_NONE;
}


/**
zwnet_sm_next - Start querying information about the next node or
                call the notify callback.
@param[in]	nw	Network
@return
@pre        Caller must lock the nw->mtx before calling this function.
*/
static void    zwnet_sm_next(zwnet_p nw)
{
    int     result;
    uint8_t node_id;

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;    //make sure timer context is null, else restart timer will crash

#ifdef ENABLE_SEND_WKUP_NO_MORE_INFO
    //Send wake up no more info to sleeping node
    if (nw->sm_job.auto_cfg && (nw->sm_job.rd_idx < nw->sm_job.num_node))
    {
        zwnode_p node;

        node = zwnode_find(&nw->ctl, nw->sm_job.node_id[nw->sm_job.rd_idx]);
        if (node)
        {   //Check for sleeping node
            if (node->sleep_cap)
            {
                zwifd_t     ifd;
                zwif_p      intf;
                uint8_t     resp[] = { COMMAND_CLASS_WAKE_UP, WAKE_UP_NO_MORE_INFORMATION };

                intf = zwif_find_cls(node->ep.intf, COMMAND_CLASS_WAKE_UP);

                if (intf)
                {
                    //Send the response
                    zwif_get_desc(intf, &ifd);
                    zwif_exec_ex(&ifd, resp, 2, zwif_tx_sts_cb, NULL,
                                 ZWIF_OPT_SKIP_MUL_Q, NULL);
                }
            }
        }
    }
#endif

    //Point to next node
    nw->sm_job.rd_idx++;

    //Invoke notify callback to report the progress of every node
    if (nw->sm_job.op != ZWNET_OP_SLEEP_NODE_UPT)
    {
        zwnet_notify(nw, nw->sm_job.op,
                     (((uint16_t)nw->sm_job.num_node)<< 8) | (uint16_t)nw->sm_job.rd_idx);
    }

    //Process the next node
    if (nw->sm_job.rd_idx < nw->sm_job.num_node)
    {
        int retry_cnt = 11;
        node_id = nw->sm_job.node_id[nw->sm_job.rd_idx];

        while (retry_cnt-- > 0)
        {
            result = zip_node_info_chd_get(&nw->appl_ctx, zwnet_ni_chd_get_cb,
                                           node_id, ZWNET_NI_CACHE_GET_AGE);

            if (result == 0)
            {
                zwnode_p node;
                //Save the end point for time out event
                node = zwnode_find(&nw->ctl, node_id);
                if (node)
                {
                    zwep_get_desc(&node->ep, &nw->ni_sm_ep);
                }
                else
                {
                    nw->ni_sm_ep.epid = 0;
                    nw->ni_sm_ep.nodeid = node_id;
                    nw->ni_sm_ep.net = nw;
                }

                nw->ni_sm_sta = ZWNET_STA_GET_NODE_INFO;

                //Start timer
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_NODE_INFO_TMOUT, zwnet_tmout_cb, nw);
                return;
            }
            else if ((result == SESSION_ERROR_PREVIOUS_COMMAND_UNCOMPLETED) || (result == APPL_ERROR_WAIT_CB))
            {
                debug_zwapi_ts_msg(&nw->plt_ctx, "zwnet_sm_next: tx busy with error:%d", result);
                plt_sleep(1000);
            }
            else
            {
                debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_next: zw_request_node_info with error:%d", result);
                break;
            }
        }
    }

    //Completed job
    nw->ni_sm_sta = ZWNET_STA_IDLE;
    node_id = nw->sm_job.node_id[0];

    if (nw->sm_job.op == ZWNET_OP_SLEEP_NODE_UPT)
    {
        zwnet_node_cb(nw, node_id, ZWNET_NODE_UPDATED);
    }

    if (nw->sm_job.node_id)
    {
        free(nw->sm_job.node_id);
    }

    //Reset operation to "no operation"
    nw->curr_op = ZWNET_OP_NONE;

    //Invoke user callback
    if (nw->sm_job.cb)
    {
        nw->sm_job.cb(nw, node_id);
    }

    //Invoke notify callback
    zwnet_notify(nw, nw->sm_job.op, OP_DONE);

    //Clear the job
    memset(&nw->sm_job, 0, sizeof(sm_job_t));

}


/**
zwnet_sm_node_propty_updt - Update node properties based on interfaces at the root level
@param[in]	node        node
@return
*/
void zwnet_sm_node_propty_updt(zwnode_p node)
{
    zwif_p intf;
    zwif_p first_intf;

    first_intf = node->ep.intf;

    if (!first_intf)
    {
        return;
    }

    //Check whether the node has Wake Up command class
    intf = zwif_find_cls(first_intf, COMMAND_CLASS_WAKE_UP);
    if (intf)
    {   //Enable command queuing
        node->enable_cmd_q = 1;
        node->sleep_cap = 1;
        node->cmd_id = 0;
        node->cmd_num = 0;
    }
    else
    {   //Disable command queuing
        node->enable_cmd_q = 0;
        node->sleep_cap = 0;

        if (node->mul_cmd_q_cnt > 0)
        {
            //Remove command queue
            zwnode_cmd_q_rm(node);
        }
    }

    //Check whether the node has Multi Command Encapsulated command class
    intf = zwif_find_cls(first_intf, COMMAND_CLASS_MULTI_CMD);
    if (intf)
    {
        node->mul_cmd_cap = 1;
        node->mul_cmd_ctl = 1;    //turn it on  by default
    }
    else
    {
        node->mul_cmd_cap = 0;
        node->mul_cmd_ctl = 0;
    }

#ifdef CRC16_ENCAP
    //Check whether the node has CRC-16 Encapsulation command class
    intf = zwif_find_cls(first_intf, COMMAND_CLASS_CRC_16_ENCAP);
    node->crc_cap = (intf)? 1 : 0;
#endif

}


/**
zwnet_sm_cls_hdlr - Handle command classes in a node
@param[in]	nw		    Network
@param[in]	ep		    end point
@param[in]	act		    the starting point to execute actions
@return
*/
static void zwnet_sm_cls_hdlr(zwnet_p nw, zwep_p ep, zwnet_info_sm_act_t act)
{
    int    result;
    zwif_p intf;

    switch (act)
    {
        //------------------------------------------------------
        case ACT_START:
        //------------------------------------------------------
            //Update node properties based on interfaces at the root level
            zwnet_sm_node_propty_updt(ep->node);

            //Assign return route to controller
            if (nw->sm_job.assign_rr && (nw->ctl_role & ZWNET_CTLR_ROLE_INCL))
            {
                result = zwnet_sm_rr(nw, ep->node);
                if (result == 0)
                {
                    break;
                }
            }

        //------------------------------------------------------
        case ACT_VERSION:
        //------------------------------------------------------
            //Check whether the node has version command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_VERSION);
            if (intf)
            {
                zwif_p    temp_intf;

                //Find the first non-basic/non-extended command class
                temp_intf = ep->intf;
                while (temp_intf)
                {
                    if ((temp_intf->cls != COMMAND_CLASS_BASIC)
                        && ((temp_intf->cls & 0xFF00) == 0))
                    {
                        break;
                    }
                    temp_intf =  (zwif_p)temp_intf->obj.next;
                }

                if (temp_intf)
                {
                    result = zwnet_sm_ver_get(nw, intf, temp_intf->cls);
                    if (result == 0)
                    {
                        break;
                    }
                }
            }

        //------------------------------------------------------
        case ACT_MANF_SPEC:
        //------------------------------------------------------
            //Check whether the node has manufacturer specific command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_MANUFACTURER_SPECIFIC);
            if (intf)
            {
                result = zwnet_sm_manf_get(nw, intf);
                if (result == 0)
                {
                    break;
                }
            }

        //------------------------------------------------------
        case ACT_DEVICE_CFG:
        //------------------------------------------------------
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_WAKE_UP);
            if (intf)
            {
                if (zwnet_sm_wkup_cfg(nw, intf) == 0)
                    break;
            }

        //------------------------------------------------------
        case ACT_END_POINT:
        //------------------------------------------------------
            //Check whether the node has multi-instance/channel command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_MULTI_INSTANCE);
            if (intf)
            {
                if (intf->ver >= 2)
                {   //multi-channel command class
                    result = zwnet_sm_ch_get(nw, intf);
                    if (result == 0)
                    {
                        break;
                    }
                }
                else
                {   //multi-instance command class
                    result = zwnet_sm_inst_get(nw, intf, ep->intf->cls);
                    if (result == 0)
                    {
                        break;
                    }
                }
            }

        //------------------------------------------------------
        case ACT_OTHERS:
        //------------------------------------------------------
            {
                ep_sm_job_t sm_job = {0};
                //
                //Start processing other command classes in node level (i.e. ep0) by using a single
                //endpoint info state-machine
                //

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Initialize endpoint info state-machine
                //nw->ep_sm_sta = EP_STA_IDLE;

                sm_job.cb = zwnet_sm_others_cb;
                sm_job.first_ep = ep;
                sm_job.use_cache = 0;

                if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
                {
                    //Update state-machine's state
                    nw->ni_sm_sta = ZWNET_STA_GET_OTHERS;
                    break;
                }
                else
                {
                    if (nw->single_ep_sta != S_EP_STA_IDLE)
                    {
                        debug_zwapi_msg(&nw->plt_ctx,
                                        "zwnet_sm_cls_hdlr: single endpoint state-machine is in wrong state: %d",
                                        nw->single_ep_sta);
                    }
                }

            }


        //------------------------------------------------------
        // Process next node
        //------------------------------------------------------
            zwnet_sm_next(nw);

    }
}


/**
zwnet_node_info_cb - Get node info delay timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_node_info_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_node_info_sm(nw, EVT_GET_NODE_INFO, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_node_info_sm - Node info state-machine
@param[in] nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return             Non-zero if state-machine is started from initial state; else return zero.
*/
int zwnet_node_info_sm(zwnet_p nw, zwnet_ni_evt_t evt, uint8_t *data)
{
    int result;

    switch (nw->ni_sm_sta)
    {
        //----------------------------------------------------------------
        case ZWNET_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_GET_NODE_INFO)
            {
                sm_job_t  *sm_job = (sm_job_t *)data;

                if (sm_job)
                {
                    //Copy the job
                    nw->sm_job = *sm_job;
                    nw->sm_job.node_id = malloc(sm_job->num_node);
                    if (!nw->sm_job.node_id)
                    {
                        break;
                    }
                    memcpy(nw->sm_job.node_id, sm_job->node_id, sm_job->num_node);
                    nw->sm_job.rd_idx = 0;
                }

                if (nw->sm_job.node_id && (nw->sm_job.num_node > 0))
                {
                    zwnode_p node;

                    node = zwnode_find(&nw->ctl, nw->sm_job.node_id[0]);

                    //Save the end point for time out event
                    if (node)
                    {
                        zwep_get_desc(&node->ep, &nw->ni_sm_ep);
                    }
                    else
                    {
                        nw->ni_sm_ep.epid = 0;
                        nw->ni_sm_ep.nodeid = nw->sm_job.node_id[0];
                        nw->ni_sm_ep.net = nw;
                    }

                    if (nw->sm_job.skip_nif)
                    {   //To skip node info cached get, the node MUST exist
                        if (!node)
                        {
                            return 0;
                        }
#ifdef  HIDE_FW_UPD_IF_FOREIGN_NODE
                        //Remove firmware update interface at node level
                        zwnet_hide_fw_upd(node);
#endif
                        //Start processing other command classes
                        zwnet_sm_cls_hdlr(nw, &node->ep, ACT_START);

                        return 1;
                    }

                    result = zip_node_info_chd_get(&nw->appl_ctx, zwnet_ni_chd_get_cb,
                                                   nw->sm_job.node_id[0], ZWNET_NI_CACHE_GET_AGE);

                    if (result == 0)
                    {
                        nw->ni_sm_sta = ZWNET_STA_GET_NODE_INFO;
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_NODE_INFO_TMOUT, zwnet_tmout_cb, nw);

                        return 1;
                    }
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zw_request_node_info with error:%d", result);

                    //Error: free node_id
                    free(nw->sm_job.node_id);
                    nw->sm_job.node_id = NULL;
                    nw->sm_job.num_node = 0;

                    //Clean up and execute callback function if this is a delayed get node info operation
                    if (sm_job == NULL)
                    {
                        zwnet_sm_next(nw);
                    }
                }
            }
            else if (evt == EVT_DELAYED_GET_NODE_INFO)
            {   //Delayed get node info operation
                sm_job_t  *sm_job = (sm_job_t *)data;

                //Copy the job
                nw->sm_job = *sm_job;
                nw->sm_job.node_id = malloc(sm_job->num_node);
                if (!nw->sm_job.node_id)
                {
                    break;
                }
                memcpy(nw->sm_job.node_id, sm_job->node_id, sm_job->num_node);
                nw->sm_job.rd_idx = 0;

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

                //Start the delay timer
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, sm_job->delay, zwnet_node_info_cb, nw);

                return 1;
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_NODE_INFO:
        //----------------------------------------------------------------
            {
                zwep_p  ep = NULL;

                if (evt == EVT_NODE_INFO)
                {
                    appl_node_info_t    *node_info;
                    zwnode_p            node;

                    //Stop timer
                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                    nw->sm_tmr_ctx = NULL;

                    node_info = (appl_node_info_t *)data;

                    node = zwnode_find(&nw->ctl, node_info->node_id);

                    ep = &node->ep;

                    if (node)
                    {
#ifdef  HIDE_FW_UPD_IF_FOREIGN_NODE
                        //Remove firmware update interface at node level
                        zwnet_hide_fw_upd(node);
#endif
                        if (nw->sm_job.delay_slp_node && node->sleep_cap)
                        {   //Delay sleeping node detailed info query
                            node->nif_pending = 1;
                            zwnet_sm_next(nw);
                            break;
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_START);

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_ASSIGN_RR:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_RR_TX_STS)
                {
                    memcpy(&ep, data + 2, sizeof(zwep_p));
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_VERSION);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_CMD_VER:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_VER_REPORT)
                {
                    zwif_p      ver_intf;
                    uint16_t    cmd_cls;
                    uint8_t     cmd_cls_ver;

                    cmd_cls = data[0];

                    cmd_cls = (cmd_cls << 8) | data[1];

                    cmd_cls_ver = data[2];

                    //Check whether the reported command class matches the request
                    if (cmd_cls != nw->ni_sm_cls)
                    {
                        break;
                    }

                    memcpy(&ver_intf, data + 4, sizeof(zwif_p));

                    result = zwnet_sm_ver_rpt_hdlr(nw, &ver_intf, zwnet_ver_rpt_cb, cmd_cls, cmd_cls_ver);

                    if (result == 0)
                    {   //Successfully sent get version command
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                        break;
                    }
                    else if (result == 1)
                    {
                        zwifd_t ver_ifd;
                        //No more command class version to query
                        //Get library, protocol and application versions
                        zwif_get_desc(ver_intf, &ver_ifd);
                        result = zwif_set_report(&ver_ifd, zwnet_zwver_rpt_cb, VERSION_REPORT);
                        if (result == 0)
                        {
                            result = zwif_get_report(&ver_ifd, NULL, 0,
                                                     VERSION_GET, zwif_tx_sts_cb);

                            if (result >= 0)
                            {
                                //Restart timer
                                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                                break;
                            }

                            debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zwif_get_report detailed version with error:%d", result);
                        }
                    }

                    ep = ver_intf->ep;

                }
                else if (evt == EVT_ZWVER_REPORT)
                {
                    zwif_p      ver_intf;
                    zwnode_p    node;
                    ext_ver_t   *ext_ver;

                    memcpy(&ver_intf, data + 6, sizeof(zwif_p));
                    ep = ver_intf->ep;
                    node = ep->node;

                    memcpy(&ext_ver, data + 6 + sizeof(zwif_p), sizeof(ext_ver_t *));

                    node->proto_ver = ((uint16_t)data[0]) << 8 | data[1];
                    node->app_ver = ((uint16_t)data[2]) << 8 | data[3];
                    node->lib_type = data[4];

                    if (ext_ver)
                    {
                        if (node->ext_ver)
                        {
                            free(node->ext_ver);
                        }

                        node->ext_ver = (ext_ver_t *)malloc(sizeof(ext_ver_t) + (ext_ver->fw_cnt*2));
                        if (node->ext_ver)
                        {
                            node->ext_ver->hw_ver = ext_ver->hw_ver;
                            node->ext_ver->fw_cnt = ext_ver->fw_cnt;
                            memcpy(node->ext_ver->fw_ver, ext_ver->fw_ver, (ext_ver->fw_cnt*2));
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);

                    if (ep && ep->node->sleep_cap && (nw->sm_job.op == ZWNET_OP_NODE_UPDATE))
                    {   //Retry querying of sleeping node detailed info on next wakeup
                        ep->node->nif_pending = 1;
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: retry querying of sleeping node detailed info on next wakeup");
                    }

                    //Skip the rest of command classes
                    //Get saved end point
                    //ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_MANF_SPEC);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_MANF_PDT_ID:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_MANF_PDT_ID_REPORT)
                {
                    zwif_p      intf;
                    zwnode_p    node;
                    uint16_t    *manf_pdt_id;

                    memcpy(&intf, data + 6, sizeof(zwif_p));
                    ep = intf->ep;
                    node = ep->node;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != MS_SUBSTA_PDT_ID)
                    {
                        break;
                    }

                    //Copy manufacturer id, product type id and product id to the node
                    manf_pdt_id = (uint16_t *)data;
                    node->vid = manf_pdt_id[0];
                    node->vtype = manf_pdt_id[1];
                    node->pid = manf_pdt_id[2];

                    //Get the device category
                    if (zwnet_dev_rec_find(nw, node->vid, node->vtype, node->pid, &node->dev_cfg_rec))
                    {
                        node->dev_cfg_valid = 1;
                        node->category = node->dev_cfg_rec.category;
                        zwnet_dev_usr_def_ver_updt(node);
						//Check Device DB for any interface to "create"
						zwnet_add_user_def_intf(node);
                    }
                    else
                    {
                        node->dev_cfg_valid = 0;
                    }

                    //Check if version 2 is supported
                    if (intf->ver >= 2)
                    {
                        //Get device serial number
                        if (zwnet_sm_dev_id_get(nw, intf, DEV_ID_TYPE_OEM) == 0)
                            break;
                    }
                }
                else if (evt == EVT_DEVICE_ID_REPORT)
                {
                    zwif_p      intf;
                    zwnode_p    node;
                    dev_id_t    *dev_id;

                    memcpy(&dev_id, data, sizeof(dev_id_t *));
                    memcpy(&intf, data + sizeof(dev_id_t *), sizeof(zwif_p));
                    ep = intf->ep;
                    node = ep->node;

                    if (nw->ni_sm_sub_sta == MS_SUBSTA_DEV_ID_OEM)
                    {
                        if (dev_id->type != DEV_ID_TYPE_OEM)
                        {
                            //Save the device id
                            node->dev_id = *dev_id;
                        }
/*
                        if (dev_id->type == DEV_ID_TYPE_OEM)
                        {
                            //Get the OEM factory default Device ID Type
                            if ((dev_id->len > 0) && (dev_id->dev_id[0] != DEV_ID_TYPE_OEM))
                            {   //Get device id
                                if (zwnet_sm_dev_id_get(nw, intf, dev_id->dev_id[0]) == 0)
                                    break;
                            }
                            else
                            {   //Workaround for device that return no device id
                                //e.g. greenwave NS210-F binary switch
                                if (zwnet_sm_dev_id_get(nw, intf, DEV_ID_TYPE_SN) == 0)
                                    break;
                            }
                        }
*/
                    }
/*
                    else if (nw->ni_sm_sub_sta == MS_SUBSTA_DEV_ID)
                    {
                        if (dev_id->type != DEV_ID_TYPE_OEM)
                        {
                            //Save the device id
                            node->dev_id = *dev_id;
                        }
                    }
*/
                    else
                    {   //Unknown sub-state
                        break;
                    }

                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Skip the rest of command classes if timeout to get manufacturer specific info
                    if (nw->ni_sm_sub_sta != MS_SUBSTA_PDT_ID)
                    {
                        //Get saved end point
                        ep = zwep_get_ep(&nw->ni_sm_ep);
                    }
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_DEVICE_CFG);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_WKUP_CFG:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_WKUP_CAP_REP)
                {
                    zwnode_p        node;
                    zwif_p          intf;
                    zwif_wakeup_p   wkup_cap;
                    zwifd_t         ifd;
                    uint8_t         cmd[6];

                    memcpy(&wkup_cap, data, sizeof(zwif_wakeup_p));
                    memcpy(&intf, data + sizeof(zwif_wakeup_p), sizeof(zwif_p));

                    ep = intf->ep;
                    node = ep->node;

                    //Save wake up interval
                    node->wkup_intv = wkup_cap->cur;

                    if (nw->sm_job.auto_cfg)
                    {
                        //Set wakeup interval and notification receiving node
                        cmd[0] = COMMAND_CLASS_WAKE_UP;
                        cmd[1] = WAKE_UP_INTERVAL_SET;
                        cmd[2] = (node->wkup_intv >> 16) & 0xFF;
                        cmd[3] = (node->wkup_intv >> 8) & 0xFF;
                        cmd[4] = node->wkup_intv & 0xFF;
                        cmd[5] = node->net->ctl.nodeid;

                        //Send the command
                        zwif_get_desc(intf, &ifd);
                        zwif_exec(&ifd, cmd, 6, zwif_tx_sts_cb);

                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    //Get saved end point
                    //ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_END_POINT);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_MULTI_INSTANCE:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_MULTI_INSTANCE_REP)
                {
                    zwif_p  intf;
                    zwif_p  mc_intf;

                    memcpy(&mc_intf, data + 2, sizeof(zwif_p));
                    ep = mc_intf->ep;

                    //Update multi-channel version
                    ep->node->mul_ch_ver = 1;

                    intf = zwif_find_cls(ep->intf, data[0]);
                    if (intf)
                    {
                        //Create or update end points for the command class
                        zwnet_ep_updt(ep, data[0], data[1], mc_intf->propty);

                        //Get number of end points for next command class
                        intf = (zwif_p)intf->obj.next;
                        if (intf)
                        {
                            //Check for extended command class (2-byte command class)
                            if (intf->cls & 0xFF00)
                            {
                                debug_zwapi_msg(&nw->plt_ctx, "multi instance get: extended command class UNSUPPORTED");
                            }
                            else
                            {
                                zwifd_t ifd;
                                uint8_t param;

                                zwif_get_desc(mc_intf, &ifd);
                                param = (uint8_t)intf->cls;
                                result = zwif_get_report(&ifd, &param, 1,
                                                         MULTI_INSTANCE_GET, zwif_tx_sts_cb);

                                if (result >= 0)
                                {
                                    //Restart timer
                                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                                    break;
                                }

                                debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zwif_get_report mul inst with error:%d", result);
                            }
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    //Get saved end point
                    //ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start get version of endpoint 1 command classes
                result = zwnet_sm_ep_info_get(nw, (zwep_p)ep->obj.next);
                if (result < 0)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                }

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_MULTI_CHANNEL:
        //----------------------------------------------------------------
            {
                zwif_p      intf;
                zwnode_p    node;
                zwep_p      ep;

                ep = NULL;

                if (evt == EVT_EP_NUM_REPORT)
                {
                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    ep = intf->ep;
                    node = ep->node;

                    node->num_of_ep = data[1];
                    node->dynamic_ep = data[0] & 0x80;
                    node->ident_ep = data[0] & 0x40;

                    //Update multi-channel version
                    node->mul_ch_ver = 2;

                    //Check for changes in the number of endpoints
                    if (node->num_of_ep != zwep_count(node))
                    {
                        zwep_p temp_ep;

                        //Remove the second endpoint onwards (i.e. all real endpoints)
                        while ((temp_ep = (zwep_p)node->ep.obj.next) != NULL)
                        {
                            zwnet_ep_rm(temp_ep);
                        }
                    }

                    //Get capability of first end point
                    if (node->num_of_ep > 1)
                    {
                        result = zwnet_sm_cap_get(nw, intf, 1);
                        if (result == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_EP_CAP_REPORT)
                {
                    ep_cap_t    *epcap;
                    uint8_t     i;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&epcap, data + sizeof(zwif_p), sizeof(ep_cap_t *));

                    ep = intf->ep;
                    node = ep->node;
                    zwnet_ep_add(ep, epcap, intf->propty);

                    //Get next end point capability
                    if (epcap->ep < node->num_of_ep)
                    {
                        //Increment end point id
                        epcap->ep++;

                        if (node->ident_ep)
                        {   //All end points are identical
                            for (i = epcap->ep; i <= node->num_of_ep; i++)
                            {
                                epcap->ep = i;
                                zwnet_ep_add(ep, epcap, intf->propty);
                            }
                        }
                        else
                        {   //End points are different
                            result = zwnet_sm_cap_get(nw, intf, epcap->ep);
                            if (result == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Skip the rest of command classes
                    //Get saved end point
                    //ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start endpoint info state-machine
                result = zwnet_sm_ep_info_get(nw, (zwep_p)ep->obj.next);
                if (result < 0)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                }

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_EP_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_EP_INFO_DONE)
                {
                    zwnode_p node;

                    //Hide duplicate functional interfaces at endpoint 0
                    node = zwnode_find(&nw->ctl, nw->sm_job.node_id[nw->sm_job.rd_idx]);
#ifdef  HIDE_EP0_FUNC_IF
                    if (node)
                    {
                        zwnet_ep0_dedup(node);
                    }
#endif

#ifdef  HIDE_FW_UPD_IF_FOREIGN_NODE
					//Remove firmware update interface at endpoint level
                    if (node)
                    {
                        zwnet_hide_fw_upd(node);
                    }
#endif
                    //Start processing other command classes at root level
                    zwnet_sm_cls_hdlr(nw, &node->ep, ACT_OTHERS);

                }
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_OTHERS:
        //----------------------------------------------------------------
            if (evt == EVT_GET_OTHERS_DONE)
            {
                //Process next node
                zwnet_sm_next(nw);
            }
            break;
    }

    return 0;
}


/**
@}
*/






