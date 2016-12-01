/**
@file   zip_api_base.c - Z/IP High Level API base object, node, endpoint, interface, device implementation.


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
#include <ctype.h>
#include "../include/zip_api_pte.h"
#include "../include/zip_api_util.h"
#include "../include/zip_poll.h"
#include "../include/zip_set_poll.h"
#include "../include/zip_dev_poll.h"



extern const uint8_t data_storage_sz[3];
int         zwnet_node_info_sm(zwnet_p nw, zwnet_ni_evt_t evt, uint8_t *data);
static void zwif_get_desc_dat(zwif_p intf, zwifd_p desc);

/**
@defgroup Base Base object APIs
To be embedded by other structure as object
@{
*/

static const cmd_get_resp_t cmd_get_resp_tbl[] =
{   //Format: command class, get command, report command
    {COMMAND_CLASS_SENSOR_BINARY, SENSOR_BINARY_GET, SENSOR_BINARY_REPORT},
    {COMMAND_CLASS_SENSOR_BINARY, SENSOR_BINARY_SUPPORTED_GET_SENSOR_V2, SENSOR_BINARY_SUPPORTED_SENSOR_REPORT_V2},
    {COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_GET, SENSOR_MULTILEVEL_REPORT},
    {COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V5, SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5},
    {COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V5, SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5},
    {COMMAND_CLASS_METER, METER_GET, METER_REPORT},
    {COMMAND_CLASS_METER, METER_SUPPORTED_GET_V2, METER_SUPPORTED_REPORT_V2},
    {COMMAND_CLASS_METER_TBL_MONITOR, METER_TBL_TABLE_ID_GET, METER_TBL_TABLE_ID_REPORT},
    {COMMAND_CLASS_METER_PULSE, METER_PULSE_GET, METER_PULSE_REPORT},
    {COMMAND_CLASS_SIMPLE_AV_CONTROL, SIMPLE_AV_CONTROL_GET, SIMPLE_AV_CONTROL_REPORT},
    {COMMAND_CLASS_CONFIGURATION, CONFIGURATION_GET, CONFIGURATION_REPORT},
    {COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_GET, SWITCH_MULTILEVEL_REPORT},
    {COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_SUPPORTED_GET_V3, SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3},
    {COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_GET, SWITCH_BINARY_REPORT},
    {COMMAND_CLASS_ASSOCIATION, ASSOCIATION_GET, ASSOCIATION_REPORT},
    {COMMAND_CLASS_ASSOCIATION, ASSOCIATION_SPECIFIC_GROUP_GET_V2, ASSOCIATION_SPECIFIC_GROUP_REPORT_V2},
    {COMMAND_CLASS_ASSOCIATION, ASSOCIATION_GROUPINGS_GET, ASSOCIATION_GROUPINGS_REPORT},
    {COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2, ASSOCIATION_GET, ASSOCIATION_REPORT},
    {COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2, ASSOCIATION_GROUPINGS_GET, ASSOCIATION_GROUPINGS_REPORT},
    {COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION, COMMAND_RECORDS_SUPPORTED_GET, COMMAND_RECORDS_SUPPORTED_REPORT},
    {COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION, COMMAND_CONFIGURATION_GET, COMMAND_CONFIGURATION_REPORT},
    {COMMAND_CLASS_WAKE_UP, WAKE_UP_INTERVAL_GET, WAKE_UP_INTERVAL_REPORT},
    {COMMAND_CLASS_BASIC, BASIC_GET, BASIC_REPORT},
    {COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_OPERATION_GET, DOOR_LOCK_OPERATION_REPORT},
    {COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CONFIGURATION_GET, DOOR_LOCK_CONFIGURATION_REPORT},
    {COMMAND_CLASS_USER_CODE, USER_CODE_GET, USER_CODE_REPORT},
    {COMMAND_CLASS_USER_CODE, USERS_NUMBER_GET, USERS_NUMBER_REPORT},
    {COMMAND_CLASS_ALARM, ALARM_GET, ALARM_REPORT},
    {COMMAND_CLASS_ALARM, ALARM_TYPE_SUPPORTED_GET_V2, ALARM_TYPE_SUPPORTED_REPORT_V2},
    {COMMAND_CLASS_NOTIFICATION_V3, EVENT_SUPPORTED_GET_V3, EVENT_SUPPORTED_REPORT_V3},
    {COMMAND_CLASS_BATTERY, BATTERY_GET, BATTERY_REPORT},
    {COMMAND_CLASS_THERMOSTAT_FAN_MODE, THERMOSTAT_FAN_MODE_GET, THERMOSTAT_FAN_MODE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_FAN_MODE, THERMOSTAT_FAN_MODE_SUPPORTED_GET, THERMOSTAT_FAN_MODE_SUPPORTED_REPORT},
    {COMMAND_CLASS_THERMOSTAT_FAN_STATE, THERMOSTAT_FAN_STATE_GET, THERMOSTAT_FAN_STATE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_GET, THERMOSTAT_MODE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_SUPPORTED_GET, THERMOSTAT_MODE_SUPPORTED_REPORT},
    {COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, THERMOSTAT_OPERATING_STATE_GET, THERMOSTAT_OPERATING_STATE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_GET, THERMOSTAT_SETBACK_REPORT},
    {COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_GET, THERMOSTAT_SETPOINT_REPORT},
    {COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_SUPPORTED_GET, THERMOSTAT_SETPOINT_SUPPORTED_REPORT},
    {COMMAND_CLASS_CLOCK, CLOCK_GET, CLOCK_REPORT},
    {COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE, SCHEDULE_GET, SCHEDULE_REPORT},
    {COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE, SCHEDULE_CHANGED_GET, SCHEDULE_CHANGED_REPORT},
    {COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE, SCHEDULE_OVERRIDE_GET, SCHEDULE_OVERRIDE_REPORT},
    {COMMAND_CLASS_PROTECTION, PROTECTION_GET, PROTECTION_REPORT},
    {COMMAND_CLASS_PROTECTION, PROTECTION_SUPPORTED_GET_V2, PROTECTION_SUPPORTED_REPORT_V2},
    {COMMAND_CLASS_PROTECTION, PROTECTION_EC_GET_V2, PROTECTION_EC_REPORT_V2},
    {COMMAND_CLASS_PROTECTION, PROTECTION_TIMEOUT_GET_V2, PROTECTION_TIMEOUT_REPORT_V2},
#ifdef  TEST_EXT_CMD_CLASS
    {COMMAND_CLASS_EXT_TEST, BASIC_GET, BASIC_REPORT},
#endif
    {COMMAND_CLASS_INDICATOR, INDICATOR_GET, INDICATOR_REPORT},
    {COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_MD_GET, FIRMWARE_MD_REPORT},
    {COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_UPDATE_MD_REQUEST_GET, FIRMWARE_UPDATE_MD_REQUEST_REPORT},
    //{COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_UPDATE_MD_GET, FIRMWARE_UPDATE_MD_REPORT}//For controller, it won't send FIRMWARE_UPDATE_MD_GET
	{COMMAND_CLASS_POWERLEVEL, POWERLEVEL_GET, POWERLEVEL_REPORT},
    {COMMAND_CLASS_POWERLEVEL, POWERLEVEL_TEST_NODE_GET, POWERLEVEL_TEST_NODE_REPORT},
    {COMMAND_CLASS_IP_ASSOCIATION, IP_ASSOCIATION_GET, IP_ASSOCIATION_REPORT},
	{COMMAND_CLASS_CENTRAL_SCENE, CENTRAL_SCENE_SUPPORTED_GET, CENTRAL_SCENE_SUPPORTED_REPORT},
	{COMMAND_CLASS_SENSOR_ALARM, SENSOR_ALARM_GET, SENSOR_ALARM_REPORT},
	{COMMAND_CLASS_SENSOR_ALARM, SENSOR_ALARM_SUPPORTED_GET, SENSOR_ALARM_SUPPORTED_REPORT}
};

/**
zwobj_add - Add object to list
@param[in,out]	head	list head
@param[in]		obj		object
*/
void zwobj_add(zwobj_p *head, zwobj_p obj)
{
    zwobj_p temp;

    if (*head == NULL)
    {
        obj->next = NULL;
        *head = obj;
        return;
    }

    temp = *head;
    while (temp->next)
    {
        temp = temp->next;
    }

    temp->next = obj;
    obj->next = NULL;

}

/**
zwobj_del - Remove object from list
@param[in,out]	head	list head
@param[in]      obj     object
@return		Non-zero on success; otherwise zero if object not found in the list.
*/
int32_t zwobj_del(zwobj_p *head, zwobj_p obj)
{
    zwobj_p temp;

    if (*head == obj)
    {
        *head = (*head)->next;
        free(obj);
        return 1;
    }

    temp = *head;
    while (temp->next)
    {
        if (temp->next == obj)
        {
            temp->next = temp->next->next;
            free(obj);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}


/**
zwobj_rplc - Replace an object with a new one
@param[in,out]	head	list head
@param[in]      obj     object to be replaced
@param[in]      new_obj new object
@return		Non-zero on success; otherwise zero if object not found in the list.
@post       The obj will be invalid on success, don't use it hereafter.
*/
int32_t zwobj_rplc(zwobj_p *head, zwobj_p obj, zwobj_p new_obj)
{
    zwobj_p temp;

    if (*head == obj)
    {
        new_obj->ctx = obj->ctx;
        new_obj->next = obj->next;

        *head = new_obj;
        free(obj);
        return 1;
    }

    temp = *head;
    while (temp->next)
    {
        if (temp->next == obj)
        {
            new_obj->ctx = obj->ctx;
            new_obj->next = obj->next;

            temp->next = new_obj;
            free(obj);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/**
@}
@addtogroup Node Node APIs
@{
*/


/**
zwnode_get_net - Get associated network
@param[in]	noded	node
@return		network
*/
zwnet_p zwnode_get_net(zwnoded_p noded)
{
    return noded->net;
}


/**
zwnode_get_next - get next node in network
@param[in]	noded	    The initial node for searching the next node
@param[out]	nxt_noded	The next node if found
@return		ZW_ERR_XXX
*/
int zwnode_get_next(zwnoded_p noded, zwnoded_p nxt_noded)
{
    int         result;
    zwnode_p    node;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node && node->obj.next)
    {   //Found next node
        node = (zwnode_p)node->obj.next;
        result = zwnode_get_desc(node, nxt_noded);
        plt_mtx_ulck(noded->net->mtx);
        return result;
    }
    plt_mtx_ulck(noded->net->mtx);
    return  ZW_ERR_NODE_NOT_FOUND;
}

/**
zwnode_find - find a node based on node id
@param[in]	first_node	first node in the network
@param[in]	nodeid	    node id
@return		node if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwnode_p zwnode_find(zwnode_p first_node, uint8_t nodeid)
{
    zwnode_p    temp_node;

    temp_node = first_node;
    while (temp_node)
    {
        if (temp_node->nodeid == nodeid)
        {
            return temp_node;
        }
        temp_node = (zwnode_p)temp_node->obj.next;
    }

    return  NULL;
}


/**
zwnode_probe - send a "no operation" command to a node to test if it's reachable
@param[in]	node        node
@param[in]	cb		    callback function for transmit status
@param[in]	user	    user parameter of callback function
@return	ZW_ERR_xxx
*/
int zwnode_probe(zwnode_p node, tx_cmplt_cb_t cb, void *user)
{
    int                 result;
    appl_snd_data_t     prm = {0};
    uint8_t             cmd = COMMAND_CLASS_NO_OPERATION;

    prm.dat_len = 1;
    prm.dat_buf = &cmd;
    prm.node_id = node->nodeid;

    result = zw_send_data(&node->net->appl_ctx, &prm, cb, user);

    if (result < 0)
    {
        if ((result == SESSION_ERROR_PREVIOUS_COMMAND_UNCOMPLETED) || (result == APPL_ERROR_WAIT_CB))
        {
            result = ZW_ERR_BUSY;
        }
        else
        {
            result = ZW_ERR_OP_FAILED;
        }
    }

    return result;
}


/**
zwnode_set_user - set user specific information
@param[in]	noded	Node
@param[in]	user	user specific information
*/
void zwnode_set_user(zwnoded_p noded, void *user)
{
    zwnode_p node;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {   //Found the node
        node->obj.ctx = user;
    }
    plt_mtx_ulck(noded->net->mtx);
}


/**
zwnode_get_user - get user specific information
@param[in]	noded	Node
@return	user specific information if found; else return NULL
*/
void *zwnode_get_user(zwnoded_p noded)
{
    zwnode_p node;
    void *ret_val;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    ret_val = (node)? node->obj.ctx : NULL;

    plt_mtx_ulck(noded->net->mtx);

    return ret_val;
}


/**
zwnode_get_ext_ver - get extended version information
@param[in]	noded	Node
@return	extended version information if the node supports it; else return NULL
@post   Caller has to free the returned extended version information
*/
ext_ver_t *zwnode_get_ext_ver(zwnoded_p noded)
{
    zwnode_p    node;
    ext_ver_t   *ext_ver = NULL;

    if (!noded)
    {
        return NULL;
    }

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node && node->ext_ver)
    {
        ext_ver = (ext_ver_t *)malloc(sizeof(ext_ver_t) + (node->ext_ver->fw_cnt*2));
        if (ext_ver)
        {
            ext_ver->hw_ver = node->ext_ver->hw_ver;
            ext_ver->fw_cnt = node->ext_ver->fw_cnt;
            memcpy(ext_ver->fw_ver, node->ext_ver->fw_ver, (ext_ver->fw_cnt*2));
        }
    }

    plt_mtx_ulck(noded->net->mtx);

    return ext_ver;
}


/**
zwnode_get_desc - get device descriptor
@param[in]	node	node
@param[out]	desc	descriptor
@return		ZW_ERR_XXX
@pre        Caller must lock the nw->mtx before calling this function.
*/
int zwnode_get_desc(zwnode_p node, zwnoded_p desc)
{
    if (!node->net)
    {
        return ZW_ERR_NET_NOT_FOUND;
    }

    if (node->nodeid == 0)
    {
        return ZW_ERR_NODE_NOT_FOUND;
    }

    desc->net = node->net;
    desc->nodeid = node->nodeid;
    desc->pid = node->pid;
    desc->type = node->vtype;
    desc->vid = node->vid;
    desc->proto_ver = node->proto_ver;
    desc->app_ver = node->app_ver;
    desc->lib_type = node->lib_type;
    desc->dev_id = node->dev_id;
    desc->category = node->category;
    desc->sleep_cap = node->sleep_cap;
    desc->sensor = node->sensor;
    desc->wkup_intv = node->wkup_intv;
    desc->s2_keys_valid = node->s2_keys_valid;
    desc->s2_grnt_keys = node->s2_grnt_keys;

    return ZW_ERR_NONE;
}


/**
zwnode_get_ep - get first/default endpoint in device
@param[in]	noded	    The node
@param[out]	epd	        The default endpoint
@return		ZW_ERR_XXX
*/
int zwnode_get_ep(zwnoded_p noded, zwepd_p epd)
{
    zwnode_p node;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {   //Found node
        zwep_get_desc(&node->ep, epd);
        plt_mtx_ulck(noded->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(noded->net->mtx);
    return  ZW_ERR_EP_NOT_FOUND;
}


/**
zwnode_rm - Remove a node from the network.
@param[in]	nw	        The network
@param[in]	node_id     The node id to be removed
*/
void zwnode_rm(zwnet_p nw, uint8_t node_id)
{
    zwnode_p    node;

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, node_id);
    if (node)
    {
        zwnode_ep_rm_all(node);

        //Remove command queues
        zwnode_cmd_q_rm(node);
        zwnode_wait_cmd_q_rm(node);

        //Remove polling requests
        zwpoll_node_rm(nw, node_id);

        //Stop timers
        plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
        node->mul_cmd_tmr_ctx = NULL;
        plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
        node->wait_tmr_ctx = NULL;

        //Free extended version information
        if (node->ext_ver)
        {
            free(node->ext_ver);
            node->ext_ver = NULL;
        }

        if (nw->ctl.nodeid == node_id)
        {   //Cannot delete the first node, just clear the content
            zwnode_ctl_clr(nw);
        }
        else
            zwobj_del(&nw->ctl.obj.next, &node->obj);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnode_ep_rm_all - Remove all endpoints of a node
@param[in]	node	The node
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwnode_ep_rm_all(zwnode_p node)
{
    zwep_p ep;

    //Remove the second endpoint onwards
    while ((ep = (zwep_p)node->ep.obj.next) != NULL)
    {
        zwep_intf_rm_all(ep);
        zwobj_del(&node->ep.obj.next, &ep->obj);
    }
    //Remove the first ep
    zwep_intf_rm_all(&node->ep);
}


/**
zwif_dat_rm - Free command class specific data attached to the interface
@param[in]	intf	Interface
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwif_dat_rm(zwif_p intf)
{
    //Free command class specific data
    if (intf->data_cnt > 0)
    {
        free(intf->data);
    }

    //Free command class specific cached data
    if (intf->cch_dat_cnt > 0)
    {
		if(intf->b2lvlcch == 1)
		{
			int i = 0;
			cch_1lvl_t *p1lvl = (cch_1lvl_t *)intf->cch_data;

			for (i = 0; i < intf->cch_dat_cnt; i++, p1lvl++)
			{
				if(p1lvl && p1lvl->pcch_ele)
				{
					free(p1lvl->pcch_ele);
					p1lvl->pcch_ele = NULL;
				}
			}
		}

        free(intf->cch_data);
    }

    //Free temporary data
    if (intf->tmp_data)
    {
        if (intf->cls == COMMAND_CLASS_FIRMWARE_UPDATE_MD)
        {
            if_fw_tmp_dat_t *fw_data;

            fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;

            if (fw_data->fw_file)
            {
                fclose(fw_data->fw_file);
            }

            if (fw_data->restrt_tmr_ctx)
            {
                plt_tmr_stop(&fw_data->nw->plt_ctx, fw_data->restrt_tmr_ctx);
            }
        }
        else if (intf->cls == COMMAND_CLASS_ASSOCIATION_GRP_INFO)
        {
            int                 i;
            if_grp_info_dat_t   *grp_data;

            grp_data = (if_grp_info_dat_t *)intf->tmp_data;

            for (i=0; i<grp_data->valid_grp_cnt; i++)
            {
                free(grp_data->grp_info[i]);
            }
        }
        else if (intf->cls == COMMAND_CLASS_SIMPLE_AV_CONTROL)
        {
            zwif_av_cap_t   *av_cap;

            av_cap = (zwif_av_cap_t *)intf->tmp_data;

            if (av_cap->bit_mask_buf)
            {
                free(av_cap->bit_mask_buf);
            }
        }
        free(intf->tmp_data);
    }
}


/**
zwif_dat_mv - Move command class specific data attached to the interface
@param[in]	src_intf	Source interface
@param[in]	dst_intf	Destination interface
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwif_dat_mv(zwif_p src_intf, zwif_p dst_intf)
{
    //Move command class specific data
    dst_intf->data_cnt = src_intf->data_cnt;
    dst_intf->data_item_sz = src_intf->data_item_sz;
    dst_intf->data = src_intf->data;

    //Move command class specific cached data
    dst_intf->cch_dat_cnt = src_intf->cch_dat_cnt;
    dst_intf->cch_dat_item_sz = src_intf->cch_dat_item_sz;
    dst_intf->cch_data = src_intf->cch_data;
    dst_intf->b2lvlcch = src_intf->b2lvlcch;

    //Move temporary data
    dst_intf->tmp_data = src_intf->tmp_data;

}


/**
zwif_cmd_q_extra_handle - handle additional function calls for the queued command
@param[in]	nw          Network
@param[in]	extra       Pointer to the extra handle parameter
@param[in]	cmd_id      Command id
@param[in]	nodeid      The destination node id
@return
*/
static void zwif_cmd_q_extra_handle(zwnet_p nw, void *extra, uint16_t cmd_id, uint8_t nodeid)
{
    if (cmd_id == ZW_CID_GRP_ADD)
    {
        ret_route_t     *ret_route = (ret_route_t *)extra;
        zwnode_p        node;
        int             result;

        if ((nw->ctl_role & ZWNET_CTLR_ROLE_INCL) == 0)
        {   //Assign and delete return route not supported
            return;
        }

        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, nodeid);

        if (!node)
        {
            return;
        }

        node->add_grp_rr = *ret_route;
        plt_mtx_ulck(nw->mtx);
        //Delete all return routes first. Assignment of return routes will be done in callback
        result = zw_delete_return_route(&nw->appl_ctx, nodeid, zwif_grp_cb);

        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zw_delete_return_route with error:%d", result);

        }

    }
}


/**
zwif_cmd_q_xtra_rm - remove xtra command queue
@param[in]	nw		    Network
@param[in]	xtra_lst_hd List head
@return
*/
void zwif_cmd_q_xtra_rm(zwnet_p nw, util_lst_t **xtra_lst_hd)
{
    util_lst_t      *xtra_lst_ent;
    cmd_q_xtra_t    *xtra;

    while ((xtra_lst_ent = util_list_get(nw->mtx, xtra_lst_hd)) != NULL)
    {
        xtra = (cmd_q_xtra_t *)xtra_lst_ent->wr_buf;
        free(xtra->extra);
        free(xtra_lst_ent);
    }
}


/**
zwif_cmd_q_cb - send command from command queue callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	node_id     Node id
@return
*/
void zwif_cmd_q_cb(appl_layer_ctx_t  *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    zwnet_p         nw = (zwnet_p)appl_ctx->data;
    util_lst_t      *xtra_lst_hd = (util_lst_t  *)user_prm;
    util_lst_t      *xtra_lst_ent;
    cmd_q_xtra_t    *xtra;

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        while ((xtra_lst_ent = util_list_get(nw->mtx, &xtra_lst_hd)) != NULL)
        {
            xtra = (cmd_q_xtra_t *)xtra_lst_ent->wr_buf;
            zwif_cmd_q_extra_handle(nw, xtra->extra, xtra->cmd_id, xtra->node_id);
            free(xtra->extra);
            free(xtra_lst_ent);
        }

    }
    else
    {
        zwif_cmd_q_xtra_rm(nw, &xtra_lst_hd);
    }
}


/**
zwnode_cmd_q_rm - Remove all commands in command queue
@param[in]	node	The node
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwnode_cmd_q_rm(zwnode_p node)
{

    zwnode_mul_cmd_rm_q(node->net, &node->mul_cmd_q_hd);

    node->mul_cmd_q_cnt = 0;
}


/**
zwif_wait_cmd_q_cb - send command from command queue callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	node_id     Node id
@return
*/
static void zwif_wait_cmd_q_cb(appl_layer_ctx_t  *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    util_lst_t      *xtra_lst_hd = (util_lst_t  *)user_prm;
    cmd_q_xtra_t    *xtra;

    //Call back zwnode_wait_tx_cb
    xtra = (cmd_q_xtra_t *)xtra_lst_hd->wr_buf;
	zwnode_wait_tx_cb(appl_ctx, tx_sts, (void *)((uintptr_t)xtra->node_id), xtra->node_id);

    //Call back zwif_cmd_q_cb
    zwif_cmd_q_cb(appl_ctx, tx_sts, user_prm, node_id);
}


/**
zwnode_wait_cmd_q_rm - Remove all commands in command queue
@param[in]	node	The node
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwnode_wait_cmd_q_rm(zwnode_p node)
{

    zwnode_mul_cmd_rm_q(node->net, &node->wait_cmd_q_hd);

    node->wait_cmd_q_cnt = 0;
}

/**
zwnode_cmd_q_get - Get command queue associated with the specified node
@param[in]	noded	The node
@param[out]	cmd_q	The command queue
@return     The number of entries in cmd_q
@post       Caller must free the cmd_q after this call if return value > 0
*/
uint8_t zwnode_cmd_q_get(zwnoded_p noded, uint16_t **cmd_q)
{
    mul_cmd_q_ent_t  *cmd;
    util_lst_t  *cmd_lst_ent;   //command list entry
    zwnode_p    node;
    uint16_t    *temp_cmd_q;
    uint8_t     mul_cmd_q_cnt;
    uint8_t     ret_value = 0;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        if (node->mul_cmd_q_cnt == 0)
        {
            plt_mtx_ulck(noded->net->mtx);
            return 0;
        }
        //Allocate memory for the command queue
        temp_cmd_q = (uint16_t *)calloc(node->mul_cmd_q_cnt, sizeof(uint16_t));

        if (!temp_cmd_q)
        {
            plt_mtx_ulck(noded->net->mtx);
            return 0;
        }
        *cmd_q = temp_cmd_q;
        mul_cmd_q_cnt = node->mul_cmd_q_cnt;

        cmd_lst_ent = node->mul_cmd_q_hd;

        while ((mul_cmd_q_cnt-- > 0) && cmd_lst_ent)
        {
            cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;
            //Only copy the command id if this is the first command class command
            if (cmd->cmd_num == 1)
            {
                *temp_cmd_q++ = cmd->cmd_id;
                ret_value++;
            }
            cmd_lst_ent = cmd_lst_ent->next;
        }
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwnode_cmd_q_cancel - Cancel all pending commands in command queue
@param[in]	noded	The node
@return		ZW_ERR_XXX
*/
int zwnode_cmd_q_cancel(zwnoded_p noded)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        zwnode_cmd_q_rm(node);
        ret_value = ZW_ERR_NONE;
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwnode_cmd_q_ena_set - Control the command queue
@param[in]	noded	    node
@param[in]	enable	   1=enable command queuing; 0=disable command queuing and
                       delete the commands in the queue.
return      ZW_ERR_XXX
*/
int zwnode_cmd_q_ena_set(zwnoded_p noded, uint8_t enable)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        if (node->sleep_cap)
        {
            node->enable_cmd_q = enable;
            if (enable == 0)
            {
                zwnode_cmd_q_rm(node);
            }
            ret_value = ZW_ERR_NONE;
        }
        else
        {   //Node without sleeping capability is not
            //allowed to modify command queuing control
            ret_value = ZW_ERR_UNSUPPORTED;
        }
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;

}


/**
zwnode_cmd_q_ena_get - Get the setting of command queue control
@param[in]	noded	    node
@param[out]	enable	    Current setting of command queue control;
                        1=enable command queuing; 0=disable command queuing
return      ZW_ERR_XXX
*/
int zwnode_cmd_q_ena_get(zwnoded_p noded, uint8_t *enable)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        *enable = (node->enable_cmd_q)? 1 : 0;
        ret_value = ZW_ERR_NONE;
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwnode_mul_cmd_rm_q - Remove and free resources in the multi-command queue
@param[in]	net	        Network
@param[in]	cmd_q_hd	Command queue list head
@return
*/
void zwnode_mul_cmd_rm_q(zwnet_p net, util_lst_t  **cmd_q_hd)
{
    util_lst_t  *cmd_lst_ent;
    mul_cmd_q_ent_t *cmd_ent;

    while ((cmd_lst_ent = util_list_get(net->mtx, cmd_q_hd)) != NULL)
    {
        cmd_ent = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;
        if (cmd_ent->extra)
        {
            free(cmd_ent->extra);
        }
        free(cmd_lst_ent);
    }
}


/**
zwnode_mul_cmd_snd_frm - Send multi command frame
@param[in]	node	    Node
@param[in]	epid	    Endpoint id
@param[in]	cmd_q_hd	Command queue list head
@param[in]	cmd_buf		Buffer that stores multi command encapsulation
@param[in]	buf_len		Length of cmd_buf in bytes
@param[in]	secure_msg	Flag to indicate whether there is a secure message in the cmd_buf
@param[in]	wait_rpt	Flag to indicate whether to wait for report
@param[in]	cb		    callback function for transmit status
@param[in]	user	    user parameter of callback function
@return		ZW_ERR_xxx.  If return value is ZW_ERR_UNEXPECTED, the command queue
            (cmd_q_hd) has been flushed, and shouldn't be used thereafter.
*/
static int zwnode_mul_cmd_snd_frm(zwnoded_p noded, uint8_t epid, util_lst_t  **cmd_q_hd, uint8_t *cmd_buf, int buf_len,
                                  int secure_msg, int wait_rpt, tx_cmplt_cb_t cb, void *user)
{
    zwifd_t     multi_cmd_ifd;
    zwif_p      intf;
    zwnode_p    node;
    zwep_p      ep;
    zwnet_p     net = noded->net;

    plt_mtx_lck(net->mtx);

    node = zwnode_find(&net->ctl, noded->nodeid);

    if (!node)
    {
        goto l_MUL_CMD_SEND_ERR;
    }

    ep = zwep_find(&node->ep, epid);

    if (!ep)
    {
        goto l_MUL_CMD_SEND_ERR;
    }

    intf = zwif_find_cls(ep->intf, COMMAND_CLASS_MULTI_CMD);

    if (!intf)
    {
        goto l_MUL_CMD_SEND_ERR;
    }

    zwif_get_desc(intf, &multi_cmd_ifd);

    plt_mtx_ulck(net->mtx);

    if (secure_msg)
    {
        //Modify the multi-cmd interface descriptor to secure type
        multi_cmd_ifd.propty |= IF_PROPTY_SECURE;
    }

    return zwif_exec_ex(&multi_cmd_ifd, cmd_buf, buf_len, cb, user,
                        ZWIF_OPT_SKIP_ALL_IMD | ((wait_rpt)? ZWIF_OPT_WAIT_RPT_SET : 0), NULL);

l_MUL_CMD_SEND_ERR:
    plt_mtx_ulck(net->mtx);
    zwnode_mul_cmd_rm_q(net, cmd_q_hd);
    return ZW_ERR_UNEXPECTED;

}


/**
zwnode_single_cmd_send - Send single command without multi-command encapsulation
@param[in]	node	    Node
@param[in]	cmd	        command entry
@return		ZW_ERR_xxx
*/
static int zwnode_single_cmd_send(zwnoded_p noded, mul_cmd_q_ent_t *cmd)
{
    int             ret;
    int             opt;
    tx_cmplt_cb_t   exec_cb;
    void            *exec_cb_prm;

    if (cmd->wait_rpt)
    {   //Highest priority
        exec_cb = zwnode_wait_tx_cb;
		exec_cb_prm = (void *)((uintptr_t)noded->nodeid);
        opt = ZWIF_OPT_WAIT_RPT_SET;
        //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: 1 wait rpt");
    }
    else if (cmd->extra)
    {
        exec_cb = zwif_cmd_q_cb;
        exec_cb_prm = cmd->extra;
        opt = 0;
        //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: 1 xtra");
    }
    else
    {
        exec_cb = exec_cb_prm = NULL;
        opt = 0;
        //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: 1 normal");
    }

    ret = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len, exec_cb, exec_cb_prm,
                       ZWIF_OPT_SKIP_ALL_IMD | opt, NULL);

    if (ret < 0)
    {
        if (cmd->extra)
            zwif_cmd_q_xtra_rm(noded->net, &cmd->extra);
    }

    return ret;
}


/**
zwnode_mul_cmd_send - Send commands in the multi command buffer
@param[in]	node	    Node
@param[in]	cmd_q_hd	Command queue list head
@param[in]	max_sz	    Maximun frame size of the multi-command encapsulation
@return		ZW_ERR_xxx
*/
static int zwnode_mul_cmd_send(zwnoded_p noded, util_lst_t  **cmd_q_hd, uint8_t max_sz)
{
    zwnet_p     net;
    util_lst_t  *cmd_lst_ent;   //command list entry
    int         curr_ep_sup_mul_cmd = 0;		//current endpoint supports multi-command encapsulation
    uint8_t     curr_ep;		//current endpoint ID
    uint8_t     curr_node;	    //current node ID

    net = noded->net;

    //Initialize current ep and node id
    //Since multi-command encapsulation must be within the multi-channel encapsulation, only commands with the
    //same node & endpoint id be grouped together and be sent using multi-command encapsulation.
    curr_ep = curr_node = 0;

    //Adjust maximum frame size if it is greater than MAX_ZWAVE_PKT_SIZE
    if (max_sz > MAX_ZWAVE_PKT_SIZE)
    {
        max_sz = MAX_ZWAVE_PKT_SIZE;
    }

    cmd_lst_ent = util_list_get(net->mtx, cmd_q_hd);

    if (cmd_lst_ent)
    {
        int             ret;
        tx_cmplt_cb_t   exec_cb;
        void            *exec_cb_prm;
        mul_cmd_q_ent_t *cmd;
        util_lst_t      *xtra_lst_ent;
        cmd_q_xtra_t    *xtra;
        util_lst_t      *xtra_lst_hd = NULL;

        cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

        if (*cmd_q_hd == NULL)
        {   //Only 1 command in the command buffer, send the command without encapsulation
            ret = zwnode_single_cmd_send(noded, cmd);
            free(cmd_lst_ent);
        }
        else
        {   //Multiple commands in the queue, start encapsulate all the commands into multi command encapsulation
            uint8_t     mul_cmd_buf[MAX_ZWAVE_PKT_SIZE];// Multi Command Encapsulation buffer
            uint8_t     mul_cmd_wr_idx;                 // Index to the buffer for writing
            uint8_t     secure_msg = 0;                 // Flag to indicate one of the messages is secure
            uint8_t     wait_rpt = 0;                   // Flag to indicate to wait for report

            //Initialize the multi command buffer
            mul_cmd_buf[0] = COMMAND_CLASS_MULTI_CMD;
            mul_cmd_buf[1] = MULTI_CMD_ENCAP;
            mul_cmd_buf[2] = 0;
            mul_cmd_wr_idx = 3;

            while (cmd_lst_ent)
            {
                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

                //Check for multi-command support of the specific endpoint
                if (!curr_ep_sup_mul_cmd)
                {
                    zwif_p  cmd_intf;

                    plt_mtx_lck(net->mtx);

                    cmd_intf = zwif_get_if(&cmd->ifd);
                    if (zwif_find_cls(cmd_intf->ep->intf, COMMAND_CLASS_MULTI_CMD))
                    {
                        curr_ep_sup_mul_cmd = 1;
                    }

                    plt_mtx_ulck(net->mtx);
                }


                if (!curr_ep_sup_mul_cmd)
                {   //No multi-command support, send as per normal
                    zwnode_single_cmd_send(noded, cmd);
                }
                //Check whether there is enough buffer space
                else if (((!curr_node && !curr_ep) || (curr_node == cmd->ifd.nodeid && curr_ep == cmd->ifd.epid))
                         && ((mul_cmd_wr_idx + cmd->dat_len + 1) <= max_sz))//additional 1 byte for command length field
                {   //Save current node and endpoint id
                    curr_node = cmd->ifd.nodeid;
                    curr_ep = cmd->ifd.epid;

                    //Enough buffer
                    //Check whether the message is secure
                    if (cmd->ifd.propty & IF_PROPTY_SECURE)
                    {
                        secure_msg = 1;
                    }

                    //Check whether to wait for report
                    if (cmd->wait_rpt)
                    {
                        wait_rpt = 1;
                    }

                    //Queue the command
                    mul_cmd_buf[2]++; //increment number of commands
                    mul_cmd_buf[mul_cmd_wr_idx++] = cmd->dat_len;
                    memcpy(mul_cmd_buf + mul_cmd_wr_idx, cmd->dat_buf, cmd->dat_len);

                    //Adjustment
                    mul_cmd_wr_idx += cmd->dat_len;

                    //Handle extra function needed for this command
                    if (cmd->extra)
                    {
                        if ((xtra_lst_ent = util_list_get(net->mtx, &cmd->extra)) != NULL)
                        {
                            xtra = (cmd_q_xtra_t *)xtra_lst_ent->wr_buf;
                            ret = util_list_add(net->mtx, &xtra_lst_hd, (uint8_t *)xtra, sizeof(cmd_q_xtra_t));
                            if (ret < 0)
                            {
                                free(xtra->extra);
                            }
                            free(xtra_lst_ent);
                        }
                    }
                }
                else
                {   //Command queue is full, send the encapsulated message

                    ret = zwnode_mul_cmd_snd_frm(noded, curr_ep, cmd_q_hd, mul_cmd_buf, mul_cmd_wr_idx, secure_msg, 0, NULL, NULL);
                    //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: send when q full");

                    if (ret == ZW_ERR_UNEXPECTED)
                    {
                        zwif_cmd_q_xtra_rm(net, &xtra_lst_hd);
                        return ret;
                    }

                    //Reset the multi command buffer
                    mul_cmd_buf[2] = 0;
                    mul_cmd_wr_idx = 3;

                    //Reset current node and endpoint id
                    curr_ep = curr_node = 0;

                    //Reset flag
                    curr_ep_sup_mul_cmd = 0;

                    //Save the current command into the multi command encapsulation buffer
                    continue;
                }

                free(cmd_lst_ent);
                //Get next command
                cmd_lst_ent = util_list_get(net->mtx, cmd_q_hd);
            }

            //Check if anything left to send
            if (mul_cmd_wr_idx == 3)
            {   //Nothing to send
                return 0;
            }

            //Send the last multi-command frame
            if (xtra_lst_hd && wait_rpt)
            {
                exec_cb = zwif_wait_cmd_q_cb;
                exec_cb_prm = xtra_lst_hd;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: xtra and wait rpt");
            }
            else if (wait_rpt)
            {
                exec_cb = zwnode_wait_tx_cb;
				exec_cb_prm = (void *)((uintptr_t)noded->nodeid);
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: wait rpt");
            }
            else if (xtra_lst_hd)
            {
                exec_cb = zwif_cmd_q_cb;
                exec_cb_prm = xtra_lst_hd;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: xtra");
            }
            else
            {
                exec_cb = exec_cb_prm = NULL;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: normal");
            }

            ret = zwnode_mul_cmd_snd_frm(noded, curr_ep, cmd_q_hd, mul_cmd_buf, mul_cmd_wr_idx, secure_msg, wait_rpt,
                                         exec_cb, exec_cb_prm);

            if (ret < 0)
            {
                if (xtra_lst_hd)
                    zwif_cmd_q_xtra_rm(net, &xtra_lst_hd);
            }
        }
        return ret;
    }

    return ZW_ERR_NONE;
}


/**
zwnode_mul_cmd_ctl_set - Turn on/off multi command encapsulation
@param[in]	noded	Node
@param[in]	on	    1=turn on multi command encapsulation;  0=turn off and flush
                    the commands in the buffer.
@return		ZW_ERR_xxx
*/
int zwnode_mul_cmd_ctl_set(zwnoded_p noded, uint8_t on)
{
    int         result;
    zwnet_p     net;
    zwnode_p    node;

    net = noded->net;

    plt_mtx_lck(net->mtx);

    node = zwnode_find(&net->ctl, noded->nodeid);

    if (node)
    {
        if (node->mul_cmd_cap)
        {
            util_lst_t  *cmd_lst_hd;
            uint8_t     max_sz;

            node->mul_cmd_ctl = on;
            cmd_lst_hd = node->mul_cmd_q_hd;
            max_sz = (node->crc_cap)? (MAX_ZWAVE_PKT_SIZE - CRC16_OVERHEAD) : MAX_ZWAVE_PKT_SIZE;

            //Reset the command queue for the node
            node->mul_cmd_q_hd = NULL;
            node->mul_cmd_q_cnt = 0;

            if (on == 0)
            {   //Stop timer
                plt_tmr_stop(&net->plt_ctx, node->mul_cmd_tmr_ctx);
                node->mul_cmd_tmr_ctx = NULL;
            }

            plt_mtx_ulck(net->mtx);

            if (on == 0)
            {   //Stop and flush the command buffer
                zwnode_mul_cmd_send(noded, &cmd_lst_hd, max_sz);
            }
            return ZW_ERR_NONE;
        }
        else
        {
            result = ZW_ERR_UNSUPPORTED;
        }
    }
    else
    {
        result = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(net->mtx);
    return result;
}


/**
zwnode_mul_cmd_ctl_get - Get the current state of multi command encapsulation
@param[in]	noded	    node
@param[out]	on	        state: 1=on; 0=off
return      ZW_ERR_XXX
*/
int zwnode_mul_cmd_ctl_get(zwnoded_p noded, uint8_t *on)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        *on = (node->mul_cmd_ctl)? 1 : 0;
        ret_value = ZW_ERR_NONE;
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwep_nameloc_set - Set endpoint name and location string for device regardless whether the endpoint
                   has Node Naming And Location interface.  If it does, the strings will be sent to
                   the interface physically.
@param[in]	    epd	        The endpoint
@param[in,out]	nameloc	    Input:null terminated name & location string
                            Output:null terminated name & location string with proper UTF-8 delineation
@return		ZW_ERR_XXX
*/
int zwep_nameloc_set(zwepd_p epd, struct _zw_nameloc *nameloc)
{
    zwep_p      ep;
    uint8_t     str_len;    //Character string length

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);

    if (ep)
    {   //Found endpoint
        zwif_p  name_loc_if;
        //Store name
        //Check for valid UTF-8 string
        nameloc->name[ZW_LOC_STR_MAX] = '\0';
        str_len = strlen(nameloc->name);
        str_len = plt_utf8_chk((const uint8_t *)nameloc->name, str_len);

        memcpy(ep->name, nameloc->name, str_len);
        ep->name[str_len] = '\0';
        nameloc->name[str_len] = '\0';

        //Store location
        //Check for valid UTF-8 string
        nameloc->loc[ZW_LOC_STR_MAX] = '\0';
        str_len = strlen(nameloc->loc);
        str_len = plt_utf8_chk((const uint8_t *)nameloc->loc, str_len);

        memcpy(ep->loc, nameloc->loc, str_len);
        ep->loc[str_len] = '\0';
        nameloc->loc[str_len] = '\0';

        //Find COMMAND_CLASS_NODE_NAMING interface to send
        //the updated name and location UTF-8 strings
        name_loc_if = zwif_find_cls(ep->intf, COMMAND_CLASS_NODE_NAMING);
        if (name_loc_if)
        {
            zwifd_t ifd;

            zwif_get_desc(name_loc_if, &ifd);

            zwif_nameloc_set(&ifd, nameloc);
        }

        plt_mtx_ulck(epd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(epd->net->mtx);
    return  ZW_ERR_EP_NOT_FOUND;
}



/**
@}
@addtogroup EP Endpoint APIs
@{
*/


/**
zwep_find - find an endpoint based on endpoint id
@param[in]	first_ep	first endpoint in the node
@param[in]	epid	    endpoint id
@return		endpoint if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwep_p zwep_find(zwep_p first_ep, uint8_t epid)
{
    zwep_p    temp_ep;

    temp_ep = first_ep;
    while (temp_ep)
    {
        if (temp_ep->epid == epid)
        {
            return temp_ep;
        }
        temp_ep = (zwep_p)temp_ep->obj.next;
    }

    return  NULL;
}


/**
zwep_get_node - get associated device
@param[in]	epd	    Endpoint
@param[out]	noded	The node descriptor if found
@return		ZW_ERR_XXX
*/
int zwep_get_node(zwepd_p epd, zwnoded_p noded)
{
    zwnode_p node;

    plt_mtx_lck(epd->net->mtx);

    node = zwnode_find(&epd->net->ctl, epd->nodeid);

    if (node)
    {   //Found node
        zwnode_get_desc(node, noded);
        plt_mtx_ulck(epd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(epd->net->mtx);
    return  ZW_ERR_NODE_NOT_FOUND;
}

/**
zwep_get_ep - Get the endpoint of the specified endpoint descriptor
@param[in]	epd	    Endpoint descriptor
@return		The endpoint if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwep_p zwep_get_ep(zwepd_p epd)
{
    zwnode_p node;

    node = zwnode_find(&epd->net->ctl, epd->nodeid);

    if (node)
    {   //Found node
        return zwep_find(&node->ep, epd->epid);
    }
    return  NULL;
}


/**
zwep_get_if - get first interface in endpoint
@param[in]	epd	    endpoint
@param[out]	ifd	interface
@return		ZW_ERR_xxx
@post   Caller must free ifd->data if ifd->data_cnt > 0
*/
int zwep_get_if(zwepd_p epd, zwifd_p ifd)
{
    zwep_p  ep;
    int     result;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);

    result = ZW_ERR_INTF_NOT_FOUND;

    if (ep && ep->intf)
    {
        zwif_p intf = ep->intf;
        while (intf)
        {
            if (!(intf->propty & IF_PROPTY_HIDDEN))
            {   //Not hidden
                zwif_get_desc_dat(intf, ifd);
                result = ZW_ERR_NONE;
                break;
            }
            //Next
            intf = (zwif_p)intf->obj.next;
        }
    }
    plt_mtx_ulck(epd->net->mtx);
    return  result;
}


/**
zwep_get_next - get next endpoint in node
@param[in]	epd	    The initial endpoint for searching the next endpoint
@param[out]	nxt_epd	The next endpoint if found
@return		ZW_ERR_xxx
*/
int zwep_get_next(zwepd_p epd, zwepd_p nxt_epd)
{
    int     result;
    zwep_p  ep;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);
    if (ep && ep->obj.next)
    {   //Found endpoint
        ep = (zwep_p)ep->obj.next;
        result = zwep_get_desc(ep, nxt_epd);
        plt_mtx_ulck(epd->net->mtx);
        return result;
    }

    plt_mtx_ulck(epd->net->mtx);
    return  ZW_ERR_EP_NOT_FOUND;

}


/**
zwep_set_user - set user specific information
@param[in]	epd	    Endpoint
@param[in]	user	User specific information
*/
void zwep_set_user(zwepd_p epd, void *user)
{
    zwep_p  ep;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);
    if (ep)
    {   //Found endpoint
        ep->obj.ctx = user;
    }

    plt_mtx_ulck(epd->net->mtx);

}

/**
zwep_get_user - get user specific information
@param[in]	epd	    Endpoint
@return	user specific information if found; else return NULL
*/
void *zwep_get_user(zwepd_p epd)
{
    zwep_p  ep;
    void    *ret_val;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);

    ret_val = (ep)? ep->obj.ctx : NULL;

    plt_mtx_ulck(epd->net->mtx);

    return ret_val;
}


/**
zwep_get_desc - get endpoint descriptor
@param[in]	ep		endpoint
@param[out]	desc	descriptor
@return		ZW_ERR_XXX
*/
int zwep_get_desc(zwep_p ep, zwepd_p desc)
{
    desc->generic = ep->generic;
    desc->specific = ep->specific;
    desc->epid = ep->epid;
    desc->net = ep->node->net;
    desc->nodeid = ep->node->nodeid;
#ifdef USE_SAFE_VERSION
    strcpy_s(desc->name, ZW_LOC_STR_MAX + 1, ep->name);
    strcpy_s(desc->loc, ZW_LOC_STR_MAX + 1, ep->loc);
#else
    strcpy(desc->name, ep->name);
    strcpy(desc->loc, ep->loc);
#endif
    desc->zwplus_info = ep->zwplus_info;

    return ZW_ERR_NONE;
}


/**
zwep_intf_rm_all - Remove all interfaces of an endpoint
@param[in]	ep	The endpoint
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwep_intf_rm_all(zwep_p ep)
{
    zwif_p  intf;
    zwif_p  tmp_intf;

    intf = ep->intf;
    ep->intf = NULL;

    while (intf)
    {
        tmp_intf = intf;//save the interface to be freed
        zwif_dat_rm(intf);
        intf = (zwif_p)intf->obj.next;//point to next interface
        free(tmp_intf);
    }
}


/**
zwep_count - count the number of real endpoints in a node
@param[in]	node	Node
@return		The number of real endpoints in a node
@pre        Caller must lock the nw->mtx before calling this function.
*/
uint8_t zwep_count(zwnode_p node)
{
    zwep_p      temp_ep;
    uint8_t     ep_cnt;

    if (!node)
    {
        return  0;
    }

    ep_cnt = 0;
    temp_ep = &node->ep;
    while (temp_ep)
    {
        if (temp_ep->epid != 0)
        {
            ep_cnt++;
        }
        temp_ep =  (zwep_p)temp_ep->obj.next;
    }

    return  ep_cnt;
}


/**
@}
@addtogroup If Interface APIs
@{
*/

/**
zwif_alloc - Allocate memory for the interface and initialize it
@param[in]	cls	        Command class
@param[in]	ver	        Command class version
@param[in]	propty      Command class properties
@param[in]	intf_buf	Buffer that contains the reports command of the
                        specified command class
@param[in]	intf_buf_sz	The size of intf_buf
@return		interface; else return NULL
*/
static zwif_p zwif_alloc(uint16_t cls, uint8_t ver, uint8_t propty,
                         const uint8_t *intf_buf, uint8_t intf_buf_sz)

{
    int32_t i;
    zwif_p  intf;

    intf = (zwif_p)calloc(1, sizeof(zwif_t)
                          + (sizeof(if_rpt_t) * intf_buf_sz));
    if (!intf)
    {
        return NULL;
    }

    intf->cls = cls;
    intf->real_ver = intf->ver = ver;
    intf->propty = propty;
    intf->rpt_num = intf_buf_sz;

    for (i=0; i < intf_buf_sz; i++)
    {
        intf->rpt[i].rpt_cmd = intf_buf[i];
    }

    return intf;
}


/**
zwif_create - Create interface based on command class
@param[in]	cls	        class
@param[in]	ver	        class version
@param[in]	propty      command class properties
@return		interface; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwif_p zwif_create(uint16_t cls, uint8_t ver, uint8_t propty)
{
    switch (cls)
    {
        case COMMAND_CLASS_BASIC:
            {
                static const uint8_t intf_settings[] =
                {
                    BASIC_REPORT
                };
				return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_SWITCH_MULTILEVEL:
            {
                static const uint8_t intf_settings[] =
                {
                    SWITCH_MULTILEVEL_REPORT,
                    SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3   //version 3
                };
                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 3)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SWITCH_BINARY:
            {
                static const uint8_t intf_settings[] =
                {
                    SWITCH_BINARY_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SWITCH_ALL:
            {
                static const uint8_t intf_settings[] =
                {
                    SWITCH_ALL_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
            {
                static const uint8_t intf_settings[] =
                {
                    MANUFACTURER_SPECIFIC_REPORT,
                    DEVICE_SPECIFIC_REPORT_V2   //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 2)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_VERSION:
            {
                static const uint8_t intf_settings[] =
                {
                    VERSION_REPORT,
                    VERSION_COMMAND_CLASS_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_POWERLEVEL:
            {
                static const uint8_t intf_settings[] =
                {
                    POWERLEVEL_REPORT,
                    POWERLEVEL_TEST_NODE_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_CONTROLLER_REPLICATION:
            {
                return zwif_alloc(cls, ver, propty, NULL, 0);
            }
            break;

        case COMMAND_CLASS_NODE_NAMING:
            {
                static const uint8_t intf_settings[] =
                {
                    NODE_NAMING_NODE_NAME_REPORT,
                    NODE_NAMING_NODE_LOCATION_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SENSOR_BINARY:
            {
                static const uint8_t intf_settings[] =
                {
                    SENSOR_BINARY_REPORT,
                    SENSOR_BINARY_SUPPORTED_SENSOR_REPORT_V2    //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 2)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SENSOR_MULTILEVEL:
            {
                static const uint8_t intf_settings[] =
                {
                    SENSOR_MULTILEVEL_REPORT,
                    SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5, //version 5
                    SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 5)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ASSOCIATION:
            {
                static const uint8_t intf_settings[] =
                {
                    ASSOCIATION_REPORT,
                    ASSOCIATION_GROUPINGS_REPORT,
                    ASSOCIATION_SPECIFIC_GROUP_REPORT_V2  //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 2 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_IP_ASSOCIATION:
            {
                static const uint8_t intf_settings[] =
                {
                    IP_ASSOCIATION_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_MAILBOX:
            {
                static const uint8_t intf_settings[] =
                {
                    MAILBOX_CONFIGURATION_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION:
            {
                static const uint8_t intf_settings[] =
                {
                    COMMAND_RECORDS_SUPPORTED_REPORT,
                    COMMAND_CONFIGURATION_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2:
            {
                static const uint8_t intf_settings[] =
                {
                    MULTI_CHANNEL_ASSOCIATION_REPORT_V2,
                    MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;


        case COMMAND_CLASS_MULTI_CHANNEL_V2:
            {
                if (ver == 1)
                {   //multi-instance command class
                    static const uint8_t intf_settings[] =
                    {
                        MULTI_INSTANCE_REPORT
                    };
                    return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));

                }

                if (ver >= 2)
                {   //multi-channel command class
                    static const uint8_t intf_settings[] =
                    {
                        MULTI_INSTANCE_REPORT,
                        MULTI_CHANNEL_END_POINT_REPORT_V2,
                        MULTI_CHANNEL_CAPABILITY_REPORT_V2,
                        MULTI_CHANNEL_END_POINT_FIND_REPORT_V2
                    };

                    return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
                }
            }
            break;

        case COMMAND_CLASS_WAKE_UP:
            {
                static const uint8_t intf_settings[] =
                {
                    WAKE_UP_INTERVAL_REPORT,
                    WAKE_UP_NOTIFICATION,
                    WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2  //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 2 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_METER:
            {
                static const uint8_t intf_settings[] =
                {
                    METER_REPORT,
                    METER_SUPPORTED_REPORT_V2  //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_METER_PULSE:
            {
                static const uint8_t intf_settings[] =
                {
                    METER_PULSE_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_CONFIGURATION:
            {
                static const uint8_t intf_settings[] =
                {
                    CONFIGURATION_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_METER_TBL_MONITOR:
            {
                static const uint8_t intf_settings[] =
                {
                    METER_TBL_TABLE_POINT_ADM_NO_REPORT,
                    METER_TBL_TABLE_ID_REPORT,
                    METER_TBL_REPORT,
                    METER_TBL_STATUS_SUPPORTED_REPORT,
                    METER_TBL_STATUS_REPORT,
                    METER_TBL_CURRENT_DATA_REPORT,
                    METER_TBL_HISTORICAL_DATA_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SIMPLE_AV_CONTROL:
            {
                static const uint8_t intf_settings[] =
                {
                    SIMPLE_AV_CONTROL_REPORT,
                    SIMPLE_AV_CONTROL_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK:
            {
                static const uint8_t intf_settings[] =
                {
                    DOOR_LOCK_OPERATION_REPORT,
                    DOOR_LOCK_CONFIGURATION_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_USER_CODE:
            {
                static const uint8_t intf_settings[] =
                {
                    USER_CODE_REPORT,
                    USERS_NUMBER_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ALARM:
            {
                static const uint8_t intf_settings[] =
                {
                    ALARM_REPORT,
                    ALARM_TYPE_SUPPORTED_REPORT_V2,
                    EVENT_SUPPORTED_REPORT_V3

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 3)? ver : 3);
            }
            break;

        case COMMAND_CLASS_BATTERY:
            {
                static const uint8_t intf_settings[] =
                {
                    BATTERY_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_MODE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_FAN_MODE_REPORT,
                    THERMOSTAT_FAN_MODE_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_STATE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_FAN_STATE_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_MODE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_MODE_REPORT,
                    THERMOSTAT_MODE_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_OPERATING_STATE_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETBACK:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_SETBACK_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETPOINT:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_SETPOINT_REPORT,
                    THERMOSTAT_SETPOINT_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_CLOCK:
            {
                static const uint8_t intf_settings[] =
                {
                    CLOCK_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
            {
                static const uint8_t intf_settings[] =
                {
                    SCHEDULE_REPORT,
                    SCHEDULE_CHANGED_REPORT,
                    SCHEDULE_OVERRIDE_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_PROTECTION:
            {
                static const uint8_t intf_settings[] =
                {
                    PROTECTION_REPORT,
                    PROTECTION_SUPPORTED_REPORT_V2, //version 2
                    PROTECTION_EC_REPORT_V2,        //version 2
                    PROTECTION_TIMEOUT_REPORT_V2    //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_APPLICATION_STATUS:
            {
                static const uint8_t intf_settings[] =
                {
                    APPLICATION_BUSY,
                    APPLICATION_REJECTED_REQUEST
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_INDICATOR:
            {
                static const uint8_t intf_settings[] =
                {
                    INDICATOR_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
            {
                static const uint8_t intf_settings[] =
                {
                    FIRMWARE_MD_REPORT,
                    FIRMWARE_UPDATE_MD_REQUEST_REPORT,
                    FIRMWARE_UPDATE_MD_STATUS_REPORT,
                    //FIRMWARE_UPDATE_MD_REPORT,
                    FIRMWARE_UPDATE_MD_GET  //Handle firmware data get
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ZIP_GATEWAY:
            {
                static const uint8_t intf_settings[] =
                {
                    GATEWAY_MODE_REPORT,
                    GATEWAY_PEER_REPORT,
                    UNSOLICITED_DESTINATION_REPORT,
                    GATEWAY_WIFI_CONFIG_REPORT,
                    GATEWAY_WIFI_ENCRYPT_SUPPORTED_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ZIP_PORTAL:
            {
                static const uint8_t intf_settings[] =
                {
                    GATEWAY_CONFIGURATION_STATUS,
                    GATEWAY_CONFIGURATION_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ZWAVEPLUS_INFO:
            {
                static const uint8_t intf_settings[] =
                {
                    ZWAVEPLUS_INFO_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_DEVICE_RESET_LOCALLY:
            {
                static const uint8_t intf_settings[] =
                {
                    DEVICE_RESET_LOCALLY_NOTIFICATION
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_GRP_INFO:
            {
                static const uint8_t intf_settings[] =
                {
                    ASSOCIATION_GROUP_NAME_REPORT,
                    ASSOCIATION_GROUP_INFO_REPORT,
                    ASSOCIATION_GROUP_COMMAND_LIST_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SECURITY:
            {
                static const uint8_t intf_settings[] =
                {
                    SECURITY_COMMANDS_SUPPORTED_REPORT

                };
                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

		case COMMAND_CLASS_CENTRAL_SCENE:
            {
                static const uint8_t intf_settings[] =
                {
                    CENTRAL_SCENE_SUPPORTED_REPORT,
                    CENTRAL_SCENE_NOTIFICATION
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

		case COMMAND_CLASS_SENSOR_ALARM:
			{
				static const uint8_t intf_settings[] =
				{
					SENSOR_ALARM_REPORT,
					SENSOR_ALARM_SUPPORTED_REPORT

				};

				return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
			}
			break;

#ifdef  TEST_EXT_CMD_CLASS
        case COMMAND_CLASS_EXT_TEST:
            {
                static const uint8_t intf_settings[] =
                {
                    BASIC_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;
#endif
        default:
            return zwif_alloc(cls, ver, propty, NULL, 0);


    }
    return  NULL;
}


/**
zwif_find_cls - find an interface based on its command class
@param[in]	first_intf	first interface in an endpoint
@param[in]	cls	        the command class to search
@return		interface if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwif_p zwif_find_cls(zwif_p first_intf, uint16_t cls)
{
    zwif_p    temp_intf;

    if (!first_intf)
    {
        return  NULL;
    }

    temp_intf = first_intf;
    while (temp_intf)
    {
        if (temp_intf->cls == cls)
        {
            return temp_intf;
        }
        temp_intf =  (zwif_p)temp_intf->obj.next;
    }

    return  NULL;
}


/**
zwif_count - count the number of unsecure interfaces in an endpoint
@param[in]	first_intf	first interface in an endpoint
@return		The number of interfaces in an endpoint
@pre        Caller must lock the nw->mtx before calling this function.
*/
uint8_t zwif_count(zwif_p first_intf)
{
    zwif_p      temp_intf;
    uint8_t     unsecure_cnt;

    if (!first_intf)
    {
        return  0;
    }

    unsecure_cnt = 0;
    temp_intf = first_intf;
    while (temp_intf)
    {
        if (temp_intf->propty & IF_PROPTY_UNSECURE)
        {
            unsecure_cnt++;
        }
        temp_intf =  (zwif_p)temp_intf->obj.next;
    }

    return  unsecure_cnt;
}



/**
zwif_get_if - Get the interface of the specified interface descriptor
@param[in]	ifd	    Interface descriptor
@return		The interface if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwif_p zwif_get_if(zwifd_p ifd)
{
    zwnode_p node;
    zwep_p   ep;

    node = zwnode_find(&ifd->net->ctl, ifd->nodeid);

    if (node)
    {   //Found node
        ep = zwep_find(&node->ep, ifd->epid);
        if (ep)
        {   //Found endpoint
            return zwif_find_cls(ep->intf, ifd->cls);
        }
    }
    return  NULL;
}


/**
zwif_get_desc - get interface descriptor without the data portion
@param[in]	intf	interface
@param[out]	desc	descriptor
*/
void zwif_get_desc(zwif_p intf, zwifd_p desc)
{
    desc->cls = intf->cls;
    desc->ver = intf->ver;
	desc->real_ver = intf->real_ver;
    desc->propty = intf->propty;
    desc->epid = intf->ep->epid;
    desc->nodeid = intf->ep->node->nodeid;
    desc->net = intf->ep->node->net;

    //Reset the data portion of the descriptor
    desc->data_cnt = 0;
    desc->data = NULL;
}


/**
zwif_get_desc_dat - get interface descriptor with the data portion
@param[in]	intf	interface
@param[out]	desc	descriptor
@post   Caller must free desc->data if desc->data_cnt > 0
*/
static void zwif_get_desc_dat(zwif_p intf, zwifd_p desc)
{
    zwif_get_desc(intf, desc);
    //Copy command class specific data portion
    if ((intf->data_cnt > 0) && (intf->data_item_sz > 0))
    {
        desc->data = calloc(intf->data_cnt, intf->data_item_sz);
        if (desc->data)
        {
            desc->data_cnt = intf->data_cnt;
            memcpy(desc->data, intf->data, (intf->data_cnt * intf->data_item_sz));
        }
    }
}


/**
zwif_ifd_get - get the interface descriptor of the specified command class
@param[in]	ifd	        interface which has the same endpoint as the cmd_cls_ifd
@param[in]	cmd_cls	    specified command class
@param[out]	cmd_cls_ifd	interface descriptor of the specified command class
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_ifd_get(zwifd_p ifd, uint16_t cmd_cls, zwifd_p cmd_cls_ifd)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    int         result;

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        intf = zwif_find_cls(intf->ep->intf, cmd_cls);

        if (intf)
        {
            zwif_get_desc(intf, cmd_cls_ifd);
            result = ZW_ERR_NONE;
        }
        else
        {
            result = ZW_ERR_CLASS_NOT_FOUND;
        }
    }
    else
    {
        result = ZW_ERR_INTF_NOT_FOUND;
    }

    plt_mtx_ulck(nw->mtx);
    return result;
}


/**
zwif_get_next - get next interface in containing endpoint
@param[in]	ifd	    The initial interface for searching the next interface
@param[out]	nxt_ifd	The next interface if found
@return		ZW_ERR_xxx
@post   Caller must free nxt_ifd->data if nxt_ifd->data_cnt > 0
*/
int zwif_get_next(zwifd_p ifd, zwifd_p nxt_ifd)
{
    zwif_p  intf;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);
    if (intf && intf->obj.next)
    {   //Found next interface
        intf = (zwif_p)intf->obj.next;
        while (intf)
        {
            if (!(intf->propty & IF_PROPTY_HIDDEN))
            {   //Not hidden
                zwif_get_desc_dat(intf, nxt_ifd);
                plt_mtx_ulck(ifd->net->mtx);
                return ZW_ERR_NONE;
            }
            //Next
            intf = (zwif_p)intf->obj.next;
        }
    }
    plt_mtx_ulck(ifd->net->mtx);
    return  ZW_ERR_INTF_NOT_FOUND;

}


/**
zwif_get_ep - get endpoint containing this interface
@param[in]	ifd		interface
@param[out]	epd		endpoint
@return		ZW_ERR_xxx
*/
int zwif_get_ep(zwifd_p ifd, zwepd_p epd)
{
    zwif_p  intf;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);
    if (intf)
    {   //Found interface
        zwep_get_desc(intf->ep, epd);
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return  ZW_ERR_INTF_NOT_FOUND;
}


/**
zwif_set_user - Set user specific information
@param[in]	ifd	    Interface
@param[in]	user	User specific information
*/
void zwif_set_user(zwifd_p ifd, void *user)
{
    zwif_p  intf;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);
    if (intf)
    {   //Found interface
        intf->obj.ctx = user;
    }

    plt_mtx_ulck(ifd->net->mtx);
}


/**
zwif_get_user - get user specific information
@param[in]	ifd	interface
@return	user specific information if found; else return NULL
*/
void *zwif_get_user(zwifd_p ifd)
{
    zwif_p  intf;
    void *ret_val;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);

    ret_val = (intf)? intf->obj.ctx : NULL;

    plt_mtx_ulck(ifd->net->mtx);

    return ret_val;
}


/**
zwif_set_report - Setup a generic report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
@param[in]	rpt 	    Report command
@return      ZW_ERR_XXX
*/
int zwif_set_report(zwifd_p ifd, void *rpt_cb, uint8_t rpt)
{
    zwif_p  intf;
    int     i;

    plt_mtx_lck(ifd->net->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {   //Find the report
        for (i=0; i < intf->rpt_num; i++)
        {
            if (intf->rpt[i].rpt_cmd == rpt)
            {
                //Setup report callback
                intf->rpt[i].rpt_cb = rpt_cb;
                plt_mtx_ulck(ifd->net->mtx);
                return ZW_ERR_NONE;
            }
        }
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_RPT_NOT_FOUND;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return ZW_ERR_INTF_NOT_FOUND;
}


/**
zwif_set_sm_report - Setup a report callback function to state-machine
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
@param[in]	rpt 	    Report command
@return      ZW_ERR_XXX
@note   This function is reserved for use by state-machine only
*/
int zwif_set_sm_report(zwifd_p ifd, void *rpt_cb, uint8_t rpt)
{
    zwif_p  intf;
    int     i;

    plt_mtx_lck(ifd->net->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {   //Find the report
        for (i=0; i < intf->rpt_num; i++)
        {
            if (intf->rpt[i].rpt_cmd == rpt)
            {
                //Setup report callback
                intf->rpt[i].sm_rpt_cb = rpt_cb;
                plt_mtx_ulck(ifd->net->mtx);
                return ZW_ERR_NONE;
            }
        }
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_RPT_NOT_FOUND;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return ZW_ERR_INTF_NOT_FOUND;
}


/**
zwif_cmd_q_sec_cb - send command in the command queue using encryption transmission callback
@param[in]	nw		network
@param[in]  nodeid	destination node id of the transmission
@param[in]  status	0= transmission ok; else negative error number
@param[in]	user	user context
*/
//void zwif_cmd_q_sec_cb(zwnet_p nw, uint8_t nodeid, int status, void *user)
//{
//    //Callback
//    zwif_tx_sts_cb(&nw->appl_ctx,
//                   (status == 0)? TRANSMIT_COMPLETE_OK : TRANSMIT_COMPLETE_FAIL,
//                   user);
//
//    if (status != 0)
//    {
//        debug_zwapi_msg(&nw->plt_ctx, "zwif_cmd_q_sec_cb: node id:%u, status:%d",
//                        nodeid, status);
//    }
//
//}


/**
zwif_cls_cmd_get - parse a command buffer to get its class and command
@param[in]	cmd_buf		Command buffer
@param[in]  len	        The length of command buffer in bytes
@param[out] cmd_cls	    The command class
@param[out]	cmd	        The command
@return     1 if the command buffer is successfully parsed; otherwise return zero
*/
int zwif_cls_cmd_get(uint8_t* cmd_buf, uint16_t len, uint16_t *cmd_cls, uint8_t *cmd)
{
    uint16_t    cls;
    uint8_t     command;

    if (len < 2)
    {
        return 0;
    }

    cls = cmd_buf[0];

    if (cmd_buf[0] >= 0xF1)
    {   //Extended command class
        if (len < 3)
        {
            return 0;
        }

        cls = (cls << 8) | cmd_buf[1];
        command = cmd_buf[2];

    }
    else
    {
        command = cmd_buf[1];
    }

    *cmd_cls = cls;
    *cmd = command;

    return 1;

}


/**
zwnode_get_rpt - Check the input command to find the corresponding REPORT
@param[in]	cmd		    Command
@param[in]  len	        The length of command in bytes
@param[out] cmd_cls	    The command class of the report
@param[out]	rpt	        The report
@return     1 if the corresponding report is found; otherwise return 0
*/
int zwnode_get_rpt(uint8_t* cmd, uint8_t len, uint16_t *cmd_cls, uint8_t *rpt)
{
    int         i;
    int         total_ent;  //total entries in the table
    uint16_t    cls;
    uint8_t     get_cmd;


    if (!zwif_cls_cmd_get(cmd, len, &cls, &get_cmd))
    {
        return 0;
    }

    total_ent = sizeof(cmd_get_resp_tbl)/ sizeof(cmd_get_resp_t);

    for (i=0; i<total_ent; i++)
    {
        if ((cmd_get_resp_tbl[i].cls == cls) && (cmd_get_resp_tbl[i].get_cmd == get_cmd))
        {   //Found
            *cmd_cls = cls;
            *rpt = cmd_get_resp_tbl[i].rpt;
            return 1;
        }
    }
    return 0;
}


/**
zwnode_wait_rpt_chk - Check whether the report is the one which the node is waiting for
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@param[in]	cmd_len	The length of cmd_buf
@return
*/
void zwnode_wait_rpt_chk(zwif_p intf, uint8_t *cmd_buf, uint16_t cmd_len)
{
    zwnode_p    node = intf->ep->node;
    zwnet_p     nw = node->net;
    uint16_t    cls;
    uint8_t     rpt;

    //Get command class and the report command
    if (!zwif_cls_cmd_get(cmd_buf, cmd_len, &cls, &rpt))
    {
        return;
    }

    plt_mtx_lck(nw->mtx);

    if (node->wait_rpt_flg)
    {
        if ((node->wait_cmd == cls)
            && (node->wait_rpt == rpt))
        {   //The waited report has arrived

            //Stop timer
            plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
            node->wait_tmr_ctx = NULL;

            //Clear the wait report flag
            node->wait_rpt_flg = 0;

            //Submit request to wait thread for execution of queued commands
            util_list_add(nw->mtx, &nw->wait_q_req_hd,
                          &node->nodeid, sizeof(uint8_t));

            //Send the command in command queue
            plt_sem_post(nw->wait_q_sem);

        }
    }

    plt_mtx_ulck(nw->mtx);

}


/**
zwnode_wait_rpt_tmout_cb - Wait report timeout callback
@param[in] data     Pointer to node
@return
*/
static void    zwnode_wait_rpt_tmout_cb(void *data)
{
    zwnode_p    node = (zwnode_p)data;
    zwnet_p     nw = node->net;

    //debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_rpt_tmout_cb");
    plt_mtx_lck(nw->mtx);

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
    node->wait_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Clear the wait report flag
    node->wait_rpt_flg = 0;

    //Submit request to wait thread for execution of queued commands
    util_list_add(nw->mtx, &nw->wait_q_req_hd,
                  &node->nodeid, sizeof(uint8_t));
    plt_mtx_ulck(nw->mtx);
    plt_sem_post(nw->wait_q_sem);

}


/**
zwnode_wait_tx_cb - send command callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	nodeid      Node id
@return
*/
void zwnode_wait_tx_cb(appl_layer_ctx_t  *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t nodeid)
{
	unsigned    node_id = (uintptr_t)user_prm;
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    zwnode_p    node;

    plt_mtx_lck(nw->mtx);

    node = zwnode_find(&nw->ctl, node_id);

    if (!node)
    {
        plt_mtx_ulck(nw->mtx);
        return;
    }

    //debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_tx_cb: tx_sts:%d", tx_sts);

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        //Restart wait report timer
        plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
        node->wait_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNODE_WAIT_RPT_TMOUT, zwnode_wait_rpt_tmout_cb, node);

    }
    else
    {
        zwnode_wait_rpt_tmout_cb(node);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnode_wait_q_thrd - Thread to serve requests to execute queued commands for nodes that require wakeup beam (FLIRS)
@param[in]	data		Network
@return
*/
void zwnode_wait_q_thrd(void   *data)
{
    int             result;
    int             wait_rpt;
    int             retry_cnt;
    int             opt;
    zwnet_p         nw = (zwnet_p)data;
    zwnode_p        node;
    util_lst_t      *req_lst;
    util_lst_t      *cmd_lst_ent;   //command list entry
    mul_cmd_q_ent_t *cmd;
    tx_cmplt_cb_t   exec_cb;
    void            *exec_cb_prm;
    uint16_t        cmd_cls;
    uint8_t         rpt;
    uint8_t         node_id;

    nw->wait_q_thrd_sts = 1;
    while (1)
    {
        //Wait for a request
        plt_sem_wait(nw->wait_q_sem);

        //Check whether to exit the thread
        if (nw->wait_q_thrd_run == 0)
        {
            nw->wait_q_thrd_sts = 0;
            return;
        }

        while ((req_lst = util_list_get(nw->mtx, &nw->wait_q_req_hd)) != NULL)
        {
            //Get the node id of the request
            node_id = req_lst->wr_buf[0];
            free(req_lst);

            while (1)
            {   //Send commands for the node
                plt_mtx_lck(nw->mtx);

                node = zwnode_find(&nw->ctl, node_id);

                if (!node || node->wait_rpt_flg)
                {
                    plt_mtx_ulck(nw->mtx);
                    break;
                }

                //No report pending, send commands
                cmd_lst_ent = util_list_get(nw->mtx, &node->wait_cmd_q_hd);

                if (!cmd_lst_ent)
                {
                    plt_mtx_ulck(nw->mtx);
                    break;
                }

                node->wait_cmd_q_cnt--;

                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

                //Unlock network mutex before sending the command to prevent deadlock
                plt_mtx_ulck(nw->mtx);

                //Check whether the command is GET command with REPORT as response
                wait_rpt = zwnode_get_rpt(cmd->dat_buf, cmd->dat_len, &cmd_cls, &rpt);

                retry_cnt = 2;

                while (retry_cnt-- > 0)
                {
                    //Check whether to exit the thread
                    if (nw->wait_q_thrd_run == 0)
                    {
                        if (cmd->extra)
                        {
                            zwif_cmd_q_xtra_rm(nw, &cmd->extra);
                        }
                        free(cmd_lst_ent);
                        plt_sleep(2);//Act as memory barrier
                        nw->wait_q_thrd_sts = 0;

                        return;
                    }

                    if (cmd->extra)
                    {   //Highest priority
                        exec_cb = zwif_cmd_q_cb;
                        exec_cb_prm = cmd->extra;
                        opt = 0;
                        //debug_zwapi_msg(&nw->plt_ctx, "wait q thd: extra");

                    }
                    else if (wait_rpt)
                    {   //Don't need to fill up exec_cb and exec_cb_prm as they will be overwritten
                        exec_cb = exec_cb_prm = NULL;

                        opt = ZWIF_OPT_WAIT_REPORT;
                        //debug_zwapi_msg(&nw->plt_ctx, "wait q thd: wait rpt");
                    }
                    else
                    {
                        exec_cb = exec_cb_prm = NULL;
                        opt = 0;
                        //debug_zwapi_msg(&nw->plt_ctx, "wait q thd: normal");
                    }

                    result = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len,
                                          exec_cb, exec_cb_prm,
                                          ZWIF_OPT_SKIP_WK_BEAM | ZWIF_OPT_CMD_ENTRY | opt, cmd);

                    if (result >= 0)
                    {
                        break;
                    }

                    //Retry if error code is ZW_ERR_BUSY
                    if (result != ZW_ERR_BUSY)
                    {
                        break;
                    }
                }

                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_q_thrd: send data with error:%d", result);
                    if (cmd->extra)
                    {
                        zwif_cmd_q_xtra_rm(nw, &cmd->extra);
                    }
                    free(cmd_lst_ent);

                    //Continue sending the next command for this node
                    continue;
                }

                free(cmd_lst_ent);//From this point onwards, the cmd variable is invalid

                if (wait_rpt)
                {
                    //Stop sending command for this node
                    break;
                }
            }
        }
    }
}


/**
zwnode_post_wkup_notif_handler - Post wakeup notification handler
@param[in]	node		The sleeping node
@return
*/
static void zwnode_post_wkup_notif_handler(zwnode_p node)
{
    zwnet_p  nw = node->net;

    if (node->nif_pending)
    {   //Query of detailed node info is pending
        int      updt_node;
        sm_job_t sm_job = {0};

        plt_mtx_lck(nw->mtx);
        if (nw->curr_op == ZWNET_OP_NONE)
        {
            updt_node = 1;
        }
        else if ((nw->curr_op == ZWNET_OP_NODE_CACHE_UPT)
                 && (nw->dpoll_ctx->cur_device_type != DEV_POLL_DEV_SLEEPING))
        {   //Cancel lower priority polling
            zwnet_abort(nw);
            updt_node = 1;
            debug_zwapi_msg(&nw->plt_ctx, "Wake up, cancelled lower priority polling!");
        }
        else
        {   //Wait for next wake up
            updt_node = 0;
        }

        if (updt_node)
        {
            nw->curr_op = ZWNET_OP_SLEEP_NODE_UPT;

            sm_job.op = ZWNET_OP_SLEEP_NODE_UPT;
            sm_job.num_node = 1;
            sm_job.node_id = &node->nodeid;
            sm_job.cb = NULL;
            sm_job.auto_cfg = 0;
            sm_job.skip_nif = 1;

            if (zwnet_node_info_sm(node->net, EVT_GET_NODE_INFO, (uint8_t *)&sm_job))
            {   //o.k.
                node->nif_pending = 0;
            }
            else
            {
                nw->curr_op = ZWNET_OP_NONE;
            }
        }
        plt_mtx_ulck(nw->mtx);
    }
    else
    {   //Get uncached supported get & refresh data cache
        zwnoded_t  node_desc;

        if (zwnode_get_desc(node, &node_desc) == 0)
        {
            zwdpoll_on_wkup(nw->dpoll_ctx, &node_desc);
        }
    }
}


/**
zwnode_cmd_q_thrd - Thread to serve requests to execute queued commands for sleeping nodes
@param[in]	data		Network
@return
*/
void zwnode_cmd_q_thrd(void *data)
{
    zwnet_p         nw = (zwnet_p)data;
    zwnode_p        node;
    util_lst_t      *req_lst;
    int             result;
    zwnoded_t       noded;

    nw->cmd_q_thrd_sts = 1;

    while (1)
    {
        //Wait for a request
        plt_sem_wait(nw->cmd_q_sem);

        //Check whether to exit the thread
        if (nw->cmd_q_thrd_run == 0)
        {
            nw->cmd_q_thrd_sts = 0;
            return;
        }

        req_lst = util_list_get(nw->cmd_q_mtx, &nw->cmd_q_req_hd);

        if (req_lst)
        {
            util_lst_t      *cmd_lst_ent;   //command list entry
            cmd_q_req_t     *cmd_q_req;
            mul_cmd_q_ent_t *cmd;

            cmd_q_req = (cmd_q_req_t *)req_lst->wr_buf;

            plt_mtx_lck(nw->mtx);

            node = zwnode_find(&nw->ctl, cmd_q_req->node_id);

            if (!node)
            {   //Node no longer exists, remove its command queue
                plt_mtx_ulck(nw->mtx);
                zwnode_mul_cmd_rm_q(nw, &cmd_q_req->req_hd);
                free(req_lst);
                continue;
            }

            if (zwnode_get_desc(node, &noded) < 0)
            {
                plt_mtx_ulck(nw->mtx);
                zwnode_mul_cmd_rm_q(nw, &cmd_q_req->req_hd);
                free(req_lst);
                continue;
            }

//          intf = zwif_find_cls(node->ep.intf, COMMAND_CLASS_WAKE_UP);
//
//          if (intf)
//          {
//              cmd = (mul_cmd_q_ent_t *)calloc(1, sizeof(mul_cmd_q_ent_t) + 2);
//              if (cmd)
//              {   //Append "wakeup no more info" at the end of command list
//                  zwif_get_desc(intf, &cmd->ifd);
//                  cmd->cmd_id = ZW_CID_WKUP_NO_INFO;
//                  cmd->cmd_num = 1;
//                  memcpy(cmd->dat_buf, wakeup_resp, 2);
//                  cmd->dat_len = 2;
//                  util_list_add(nw->mtx, &cmd_q_req->req_hd,
//                                (uint8_t *)cmd, sizeof(mul_cmd_q_ent_t) + 2);
//                  free(cmd);
//              }
//          }

            //Check whether the node supports multi-command encapsulation
            if (node->mul_cmd_ctl)
            {
                uint8_t     max_sz;

                max_sz = (node->crc_cap)? (MAX_ZWAVE_PKT_SIZE - CRC16_OVERHEAD) : MAX_ZWAVE_PKT_SIZE;
                plt_mtx_ulck(nw->mtx);
                zwnode_mul_cmd_send(&noded, &cmd_q_req->req_hd, max_sz);
                free(req_lst);
                //Get uncached supported get & refresh data cache
                plt_sleep(500);//give the device some time to respond the commands in command queue
                plt_mtx_lck(nw->mtx);
                node = zwnode_find(&nw->ctl, noded.nodeid);
                if (node)
                {
                    zwnode_post_wkup_notif_handler(node);
                }
                plt_mtx_ulck(nw->mtx);
                continue;
            }

            plt_mtx_ulck(nw->mtx);

            while ((cmd_lst_ent = util_list_get(nw->mtx, &cmd_q_req->req_hd)) != NULL)
            {

                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

                if (cmd->extra)
                {
                    result = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len, zwif_cmd_q_cb, cmd->extra,
                                          ZWIF_OPT_SKIP_ALL_IMD, NULL);
                    if (result < 0)
                    {
                        zwif_cmd_q_xtra_rm(nw, &cmd->extra);
                    }
                }
                else
                {
                    result = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len, NULL, NULL,
                                          ZWIF_OPT_SKIP_ALL_IMD, NULL);

                }
                free(cmd_lst_ent);

                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnode_cmd_q_thrd: send data with error:%d", result);
                }

                //Check whether to exit the thread
                if (nw->cmd_q_thrd_run == 0)
                {
                    zwnode_mul_cmd_rm_q(nw, &cmd_q_req->req_hd);
                    break;
                }
            }
            free(req_lst);

            //Get uncached supported get & refresh data cache
            plt_sleep(500);//give the device some time to respond the commands in command queue

            plt_mtx_lck(nw->mtx);
            node = zwnode_find(&nw->ctl, noded.nodeid);
            if (node)
            {
                zwnode_post_wkup_notif_handler(node);
            }
            plt_mtx_ulck(nw->mtx);
        }
    }
}


/**
zwif_cmd_dequeue - Dequeue the first command in the command list for sleeping node
@param[in]	intf	Interface
@param[in]	rx_sts  Receive status
@return             0 if no command in the command list; else return 1
*/
static int zwif_cmd_dequeue(zwif_p intf)
{
    zwnode_p    node;
    int         result;

    node = intf->ep->node;
    plt_mtx_lck(node->net->mtx);

    if (node->mul_cmd_q_hd)
    {
        cmd_q_req_t     cmd_q_req;
        cmd_q_req.node_id = node->nodeid;
        cmd_q_req.req_hd = node->mul_cmd_q_hd;
        //Submit request to thread for execution of queued commands
        result = util_list_add(node->net->cmd_q_mtx, &node->net->cmd_q_req_hd,
                               (uint8_t *)&cmd_q_req, sizeof(cmd_q_req_t));
        if (result != 0)
        {   //Error, try to send on next wake up notification
            plt_mtx_ulck(node->net->mtx);
            return 0;
        }
        plt_sem_post(node->net->cmd_q_sem);
        //Remove the queue from the node
        node->mul_cmd_q_hd = NULL;
        node->mul_cmd_q_cnt = 0;
        plt_mtx_ulck(node->net->mtx);
        return 1;
    }
    plt_mtx_ulck(node->net->mtx);
    return 0;

}


/**
zwif_fw_restrt_tout_cb - Firmware update target restart timer callback
@param[in] data     Firmware update interface data
@return
*/
static void zwif_fw_restrt_tout_cb(void *data)
{
    if_fw_tmp_dat_t     *fw_data = (if_fw_tmp_dat_t *)data;
    zwnet_p             nw = fw_data->nw;
    zwnet_exec_req_t    req = {0};

    //Stop timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, fw_data->restrt_tmr_ctx);
    fw_data->restrt_tmr_ctx = NULL;

    req.node_id = fw_data->node.nodeid;
    req.action = EXEC_ACT_PROBE_RESTART_NODE;
    plt_mtx_ulck(nw->mtx);

    util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                  (uint8_t *)&req, sizeof(zwnet_exec_req_t));
    plt_sem_post(nw->nw_exec_sem);

}


/**
zwif_ip_assoc_1st_rpt_hdlr - Handle IP association first report
@param[in]	nw	    Network
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@return     Non-zero if o.k. ; else return zero
*/
static int zwif_ip_assoc_1st_rpt_hdlr(zwnet_p nw, zwif_p intf, uint8_t *cmd_buf)
{
    int                 valid_node_id = 0;
    uint8_t             node_id;
    uint8_t             actual_nodes;
    if_ip_grp_dat_t     *ip_grp_dat;

    //Create temp storage for subsequent reports
    actual_nodes = cmd_buf[4];

    ip_grp_dat = (if_ip_grp_dat_t *)calloc(1, sizeof(if_ip_grp_dat_t) + (actual_nodes * sizeof(grp_member_t)));

    if (!ip_grp_dat)
    {
        return 0;
    }

    ip_grp_dat->group = cmd_buf[2];
    ip_grp_dat->index = 1;
    ip_grp_dat->tot_entries = actual_nodes;

    if (actual_nodes > 0)
    {   //Resolve IPv6 address to node id
        if (nw->appl_ctx.use_ipv4)
        {
            if (util_is_ipv4_mapped_ipv6(cmd_buf + 5))
            {
                if (memcmp(&nw->zip_router_addr[12], cmd_buf + 5 + 12, 4) == 0)
                {
                    node_id = nw->ctl.nodeid;
                    valid_node_id = 1;
                }
                else if (zip_translate_ipaddr(&nw->appl_ctx, cmd_buf + 5 + 12, 4, &node_id))
                {
                    valid_node_id = 1;
                }
            }
        }
        else
        {
            if (memcmp(nw->zip_router_addr, cmd_buf + 5, IPV6_ADDR_LEN) == 0)
            {
                node_id = nw->ctl.nodeid;
                valid_node_id = 1;
            }
            else if (zip_translate_ipaddr(&nw->appl_ctx, cmd_buf + 5, IPV6_ADDR_LEN, &node_id))
            {
                valid_node_id = 1;
            }
        }
    }

    if (valid_node_id)
    {
        ip_grp_dat->grp_mbr[ip_grp_dat->valid_entries].node_id = node_id;
        ip_grp_dat->grp_mbr[ip_grp_dat->valid_entries++].ep_id = cmd_buf[21];
    }

    //Assign the storage to interface
    if (intf->tmp_data)
    {
        free(intf->tmp_data);
    }

    intf->tmp_data = ip_grp_dat;

    return 1;
}


/**
zwif_ip_assoc_other_rpt_hdlr - Handle IP association subsequent report
@param[in]	nw	    Network
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@return     Non-zero if o.k. ; else return zero
*/
static int zwif_ip_assoc_other_rpt_hdlr(zwnet_p nw, zwif_p intf, uint8_t *cmd_buf)
{
    int                 valid_node_id = 0;
    uint8_t             node_id;
    uint8_t             rpt_idx;
    if_ip_grp_dat_t     *ip_grp_dat = (if_ip_grp_dat_t *)intf->tmp_data;

    //Check the group id is same with the first report
    if (!ip_grp_dat || cmd_buf[2] != ip_grp_dat->group)
    {
        return 0;
    }

    //Check for incremental report index number
    rpt_idx = cmd_buf[3];
    if (rpt_idx <= ip_grp_dat->index)
    {
        return 0;
    }

    ip_grp_dat->index = rpt_idx;

    //Resolve IPv6 address to node id
    if (nw->appl_ctx.use_ipv4)
    {
        if (util_is_ipv4_mapped_ipv6(cmd_buf + 5))
        {
            if (memcmp(&nw->zip_router_addr[12], cmd_buf + 5 + 12, 4) == 0)
            {
                node_id = nw->ctl.nodeid;
                valid_node_id = 1;
            }
            else if (zip_translate_ipaddr(&nw->appl_ctx, cmd_buf + 5 + 12, 4, &node_id))
            {
                valid_node_id = 1;
            }
        }
    }
    else
    {
        if (memcmp(nw->zip_router_addr, cmd_buf + 5, IPV6_ADDR_LEN) == 0)
        {
            node_id = nw->ctl.nodeid;
            valid_node_id = 1;
        }
        else if (zip_translate_ipaddr(&nw->appl_ctx, cmd_buf + 5, IPV6_ADDR_LEN, &node_id))
        {
            valid_node_id = 1;
        }
    }

    if (valid_node_id)
    {
        ip_grp_dat->grp_mbr[ip_grp_dat->valid_entries].node_id = node_id;
        ip_grp_dat->grp_mbr[ip_grp_dat->valid_entries++].ep_id = cmd_buf[21];
    }

    return 1;
}


/**
zwif_dat_cache_alloc - Allocate memory for storage of data cache and time stamp
@param[in] intf        Interface
@param[in] dat_sz      Data size
@param[in] b2lvldata   Whether the cached data should be created as 2 level pointer-to-data. The main
					   purpose of this is for storing an array of variable-length data, or an array of fix-length data.
@return
*/
static void zwif_dat_cache_alloc(zwif_p intf, int dat_sz, uint8_t b2lvldata)
{
	if(b2lvldata == FALSE)
	{
		intf->cch_data = calloc(1, sizeof(time_t) + dat_sz);
		if (intf->cch_data)
		{
			intf->cch_dat_item_sz = sizeof(time_t) + dat_sz;
			intf->cch_dat_cnt = 1;
		}
	}
	else
	{
		intf->cch_data = calloc(1, sizeof(cch_1lvl_t));
		if (intf->cch_data)
		{
			((cch_1lvl_t *)intf->cch_data)->pcch_ele = calloc(1, sizeof(time_t) + dat_sz);
			if(((cch_1lvl_t *)intf->cch_data)->pcch_ele)
			{
				intf->b2lvlcch = 1;
				intf->cch_dat_item_sz = sizeof(cch_1lvl_t);
				intf->cch_dat_cnt = 1;

				((cch_1lvl_t *)intf->cch_data)->cch_ele_sz = sizeof(time_t) + dat_sz;
			}
			else
			{
				free(intf->cch_data);
				intf->cch_data = NULL;
			}
		}
	}
}


/**
zwif_snsr_cache_updt - Update sensor value cache
@param[in] intf        Interface
@param[in] snsr_value  Sensor value
@param[in] time_stamp  Time stamp
@return  Non-zero if cached value is different from the live report value; else return zero
*/
static int zwif_snsr_cache_updt(zwif_p intf, zwsensor_t *snsr_value, time_t *time_stamp)
{
    int     i;
    int     ent_sz;     //entry size
    int     found = 0;
    int     cache_chg = 0;
    void    *tmp_buf;
    time_t  *tmstamp;
    uint8_t *cch_data;
    zwsensor_t *cch_snsr_value;

    ent_sz = sizeof(time_t) + sizeof(zwsensor_t);

    if ((intf->cch_dat_cnt == 0) || (intf->cch_dat_item_sz != ent_sz))
    {
        return 1;
    }

    cch_data = intf->cch_data;

    //Find matching type and unit
    for (i=0; i<intf->cch_dat_cnt; i++)
    {
        tmstamp = (time_t *)(cch_data + (i * ent_sz));
        cch_snsr_value = (zwsensor_t *)(tmstamp + 1);
        if ((cch_snsr_value->type == snsr_value->type)
            && (cch_snsr_value->unit == snsr_value->unit))
        {
            found = 1;
            break;
        }
    }

    if (found)
    {   //Check whether cache value is stale
        if ((cch_snsr_value->precision != snsr_value->precision)
            || (cch_snsr_value->size != snsr_value->size))
        {
            cache_chg = 1;
        }
        else
        {
            if(memcmp(cch_snsr_value->data, snsr_value->data, snsr_value->size) != 0)
            {
                cache_chg = 1;
            }
        }
    }
    else
    {   //Allocate memory for new entry
        tmp_buf = realloc(intf->cch_data, ent_sz * (intf->cch_dat_cnt + 1));
        if (!tmp_buf)
        {
            return 1;
        }
        cch_data = tmp_buf;
        tmstamp = (time_t *)(cch_data + (intf->cch_dat_cnt * ent_sz));
        cch_snsr_value = (zwsensor_t *)(tmstamp + 1);

        //Save the newly allocated buffer
        intf->cch_data = tmp_buf;
        intf->cch_dat_cnt++;

        cache_chg = 1;
    }
    //Update cache
    *tmstamp = *time_stamp;
    *cch_snsr_value = *snsr_value;

    return cache_chg;
}


/**
zwif_thrmo_setp_cache_updt - Update thermostat setpoint value cache
@param[in] intf        Interface
@param[in] thrmo_value Thermostat setpoint value
@param[in] time_stamp  Time stamp
@return  Non-zero if cached value is different from the live report value; else return zero
*/
static int zwif_thrmo_setp_cache_updt(zwif_p intf, zwsetp_t *thrmo_value, time_t *time_stamp)
{
    int     i;
    int     ent_sz;     //entry size
    int     found = 0;
    int     cache_chg = 0;
    void    *tmp_buf;
    time_t  *tmstamp;
    uint8_t *cch_data;
    zwsetp_t *cch_thrmo_value;

    ent_sz = sizeof(time_t) + sizeof(zwsetp_t);

    if ((intf->cch_dat_cnt == 0) || (intf->cch_dat_item_sz != ent_sz))
    {
        return 1;
    }

    cch_data = intf->cch_data;

    //Find matching type and unit
    for (i=0; i<intf->cch_dat_cnt; i++)
    {
        tmstamp = (time_t *)(cch_data + (i * ent_sz));
        cch_thrmo_value = (zwsetp_t *)(tmstamp + 1);
        if ((cch_thrmo_value->type == thrmo_value->type)
            && (cch_thrmo_value->unit == thrmo_value->unit))
        {
            found = 1;
            break;
        }
    }

    if (found)
    {   //Check whether cache value is stale
        if ((cch_thrmo_value->precision != thrmo_value->precision)
            || (cch_thrmo_value->size != thrmo_value->size))
        {
            cache_chg = 1;
        }
        else
        {
            if(memcmp(cch_thrmo_value->data, thrmo_value->data, thrmo_value->size) != 0)
            {
                cache_chg = 1;
            }
        }
    }
    else
    {   //Allocate memory for new entry
        tmp_buf = realloc(intf->cch_data, ent_sz * (intf->cch_dat_cnt + 1));
        if (!tmp_buf)
        {
            return 1;
        }
        cch_data = tmp_buf;
        tmstamp = (time_t *)(cch_data + (intf->cch_dat_cnt * ent_sz));
        cch_thrmo_value = (zwsetp_t *)(tmstamp + 1);

        //Save the newly allocated buffer
        intf->cch_data = tmp_buf;
        intf->cch_dat_cnt++;

        cache_chg = 1;
    }
    //Update cache
    *tmstamp = *time_stamp;
    *cch_thrmo_value = *thrmo_value;

    return cache_chg;
}


/**
zwif_bsnsr_cache_updt - Update binary sensor value cache
@param[in] intf        Interface
@param[in] snsr_value  Sensor value
@param[in] time_stamp  Time stamp
@return  Non-zero if cached value is different from the live report value; else return zero
*/
static int zwif_bsnsr_cache_updt(zwif_p intf, zwbsnsr_t *snsr_value, time_t *time_stamp)
{
    int         i;
    int         ent_sz;     //entry size
    int         found = 0;
    int         cache_chg = 0;
    void        *tmp_buf;
    time_t      *tmstamp;
    uint8_t     *cch_data;
    zwbsnsr_t   *cch_snsr_value;

    ent_sz = sizeof(time_t) + sizeof(zwbsnsr_t);

    if ((intf->cch_dat_cnt == 0) || (intf->cch_dat_item_sz != ent_sz))
    {
        return 1;
    }

    cch_data = intf->cch_data;

    //Find matching type
    for (i=0; i<intf->cch_dat_cnt; i++)
    {
        tmstamp = (time_t *)(cch_data + (i * ent_sz));
        cch_snsr_value = (zwbsnsr_t *)(tmstamp + 1);
        if (cch_snsr_value->type == snsr_value->type)
        {
            found = 1;
            break;
        }
    }

    if (found)
    {   //Check whether cache value is stale
        if (cch_snsr_value->state != snsr_value->state)
        {
            cache_chg = 1;
        }
    }
    else
    {   //Allocate memory for new entry
        tmp_buf = realloc(intf->cch_data, ent_sz * (intf->cch_dat_cnt + 1));
        if (!tmp_buf)
        {
            return 1;
        }
        cch_data = tmp_buf;
        tmstamp = (time_t *)(cch_data + (intf->cch_dat_cnt * ent_sz));
        cch_snsr_value = (zwbsnsr_t *)(tmstamp + 1);

        //Save the newly allocated buffer
        intf->cch_data = tmp_buf;
        intf->cch_dat_cnt++;

        cache_chg = 1;
    }
    //Update cache
    *tmstamp = *time_stamp;
    *cch_snsr_value = *snsr_value;

    return cache_chg;
}


/**
zwif_meter_cache_updt - Update meter reading cache
@param[in] intf        Interface
@param[in] meter_dat   current value and unit of the meter
@param[in] time_stamp  Time stamp
@return  Non-zero if cached value is different from the live report value; else return zero
*/
static int zwif_meter_cache_updt(zwif_p intf, zwmeter_dat_p meter_dat, time_t *time_stamp)
{
    int         i;
    int         ent_sz;     //entry size
    int         found = 0;
    int         cache_chg = 0;
    void        *tmp_buf;
    time_t      *tmstamp;
    uint8_t     *cch_data;
    zwmeter_dat_p   cch_meter_value;

    ent_sz = sizeof(time_t) + sizeof(zwmeter_dat_t);

    if ((intf->cch_dat_cnt == 0) || (intf->cch_dat_item_sz != ent_sz))
    {
        return 1;
    }

    cch_data = intf->cch_data;

    //Find matching type and unit
    for (i=0; i<intf->cch_dat_cnt; i++)
    {
        tmstamp = (time_t *)(cch_data + (i * ent_sz));
        cch_meter_value = (zwmeter_dat_t *)(tmstamp + 1);
        if ((cch_meter_value->type == meter_dat->type)
            && (cch_meter_value->unit == meter_dat->unit))
        {
            found = 1;
            break;
        }
    }

    if (found)
    {   //Check whether cache value is stale
        if ((cch_meter_value->precision != meter_dat->precision)
            || (cch_meter_value->size != meter_dat->size))
        {
            cache_chg = 1;
        }
        else
        {
            if(memcmp(cch_meter_value->data, meter_dat->data, meter_dat->size) != 0)
            {
                cache_chg = 1;
            }
        }
    }
    else
    {   //Allocate memory for new entry
        tmp_buf = realloc(intf->cch_data, ent_sz * (intf->cch_dat_cnt + 1));
        if (!tmp_buf)
        {
            return 1;
        }
        cch_data = tmp_buf;
        tmstamp = (time_t *)(cch_data + (intf->cch_dat_cnt * ent_sz));
        cch_meter_value = (zwmeter_dat_t *)(tmstamp + 1);

        //Save the newly allocated buffer
        intf->cch_data = tmp_buf;
        intf->cch_dat_cnt++;

        cache_chg = 1;
    }
    //Update cache
    *tmstamp = *time_stamp;
    *cch_meter_value = *meter_dat;

    return cache_chg;
}

#if 0
/**
zwif_alrm_cache_updt - Update alarm report value cache
@param[in] intf         Interface
@param[in] alrm_info    Alarm report value
@param[in] alrm_info_sz Size of the alarm report
@param[in] time_stamp   Time stamp
@return  Non-zero if cached value is different from the live report value; else return zero
*/
static int zwif_alrm_cache_updt(zwif_p intf, zwalrm_p alrm_info, int alrm_info_sz, time_t *time_stamp)
{
    int     i;
    int     found = 0;
    int     cache_chg = 0;
    void    *tmp_buf;
    time_t  *tmstamp;
    cch_1lvl_t *p1lvl = (cch_1lvl_t *)intf->cch_data;
    zwalrm_p	cch_alrm_value;

	if ((intf->cch_dat_cnt == 0) || (intf->b2lvlcch != 1))
	{
		return 1;
	}

	for (i = 0; i < intf->cch_dat_cnt; i++, p1lvl++)
	{
		if(p1lvl && p1lvl->pcch_ele)
		{
			tmstamp = (time_t *)p1lvl->pcch_ele;
			cch_alrm_value = (zwalrm_p)(tmstamp + 1);

			if((cch_alrm_value->type == alrm_info->type) &&
			   (cch_alrm_value->ex_type == alrm_info->ex_type) &&
			   (cch_alrm_value->ex_event == alrm_info->ex_event))
			{
				found = 1;
				break;
			}
		}
	}

	if(found)
	{
        if (p1lvl->cch_ele_sz != (sizeof(time_t) + alrm_info_sz))
        {
            cache_chg = 1;
        }
        else if (memcmp(cch_alrm_value, alrm_info, alrm_info_sz) != 0)
        {
            cache_chg = 1;
        }

		//Need to free then calloc again coz the old data and new data may have diff size
		free(p1lvl->pcch_ele);
		p1lvl->pcch_ele = NULL;
	}
	else
	{   //Allocate memory for new entry
		tmp_buf = realloc(intf->cch_data, intf->cch_dat_item_sz * (intf->cch_dat_cnt + 1));
		if (!tmp_buf)
		{
			return 1;
		}
		//Save the newly allocated buffer
		intf->cch_data = tmp_buf;

		p1lvl = (cch_1lvl_t *)intf->cch_data;
		p1lvl += intf->cch_dat_cnt; //move the pointer to the last, newly allocated struct

        cache_chg = 1;
	}

	p1lvl->pcch_ele = calloc(1, sizeof(time_t) + alrm_info_sz);
	if(p1lvl->pcch_ele)
	{
		p1lvl->cch_ele_sz = sizeof(time_t) + alrm_info_sz;
		memcpy(p1lvl->pcch_ele, time_stamp, sizeof(time_t));
		memcpy(((time_t *)p1lvl->pcch_ele) + 1, alrm_info, alrm_info_sz);

		if(!found)
			intf->cch_dat_cnt++;
	}
	else
	{
		return 1;
	}

    return cache_chg;
}
#endif


/**
zwif_alrm_cache_updt - Update alarm report value cache
@param[in] intf        Interface
@param[in] alrm_info   alarm report value
@param[in] alrm_info_sz   size of the alarm report
@param[in] time_stamp  Time stamp
@return
*/
static void zwif_alrm_cache_updt(zwif_p intf, zwalrm_p alrm_info, int alrm_info_sz, time_t *time_stamp)
{
    int     i;
    int     found = 0;
    void    *tmp_buf;
    time_t  *tmstamp;
    cch_1lvl_t *p1lvl = (cch_1lvl_t *)intf->cch_data;
    zwalrm_p	cch_alrm_value;

	if ((intf->cch_dat_cnt == 0) || (intf->b2lvlcch != 1))
	{
		return;
	}

	for (i = 0; i < intf->cch_dat_cnt; i++, p1lvl++)
	{
		if(p1lvl && p1lvl->pcch_ele)
		{
			tmstamp = (time_t *)p1lvl->pcch_ele;
			cch_alrm_value = (zwalrm_p)(tmstamp + 1);

			if((cch_alrm_value->type == alrm_info->type) &&
			   (cch_alrm_value->ex_type == alrm_info->ex_type) &&
			   (cch_alrm_value->ex_event == alrm_info->ex_event))
			{
				found = 1;
				break;
			}
		}
	}

	if(found)
	{
		//Need to free then calloc again coz the old data and new data may have diff size
		free(p1lvl->pcch_ele);
		p1lvl->pcch_ele = NULL;
	}
	else
	{   //Allocate memory for new entry
		tmp_buf = realloc(intf->cch_data, intf->cch_dat_item_sz * (intf->cch_dat_cnt + 1));
		if (!tmp_buf)
		{
			return;
		}
		//Save the newly allocated buffer
		intf->cch_data = tmp_buf;

		p1lvl = (cch_1lvl_t *)intf->cch_data;
		p1lvl += intf->cch_dat_cnt; //move the pointer to the last, newly allocated struct


	}
	p1lvl->pcch_ele = calloc(1, sizeof(time_t) + alrm_info_sz);
	if(p1lvl->pcch_ele)
	{
		p1lvl->cch_ele_sz = sizeof(time_t) + alrm_info_sz;
		memcpy(p1lvl->pcch_ele, time_stamp, sizeof(time_t));
		memcpy(((time_t *)p1lvl->pcch_ele) + 1, alrm_info, alrm_info_sz);

		if(!found)
			intf->cch_dat_cnt++;
	}
	else
	{
		return;
	}
}


/**
zwif_alrm_rec_convert - Find a matched alarm record from database and perform conversion
@param[in]		intf        Interface
@param[in, out] zwalrm_p	Address of alarm info pointer
@return      ZW_ERR_XXX
@note	Conversion will be performed if all the critiria are matched for a match record in database.
*/
static int zwif_alrm_rec_convert(zwif_p intf, zwalrm_p *ppalarm_info)
{
	int                     res = ZW_ERR_NONE;
	zwnode_p                node = intf->ep->node;
	zwnet_p                 nw = node->net;
	zwalrm_p                palarm_info = *ppalarm_info;
	if_rec_alarm_match_t  *alarm_rec;
	if_rec_alarm_result_t *result_rec;

	plt_mtx_lck(nw->mtx);
	//Check whether device configuration record has alarm records
	if (node->dev_cfg_valid)
	{
		alarm_rec = (if_rec_alarm_match_t *)zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_ALARM);

		for (; alarm_rec != NULL; alarm_rec = alarm_rec->next)
		{
			if (alarm_rec->type != -1)
			{
				if (alarm_rec->type != palarm_info->type)
					continue;
			}

			if (alarm_rec->isRange != -1)
			{
				if ((alarm_rec->isRange == 0) &&
					(alarm_rec->level_low != palarm_info->level))
					continue;

				if ((alarm_rec->isRange == 1) &&
					((alarm_rec->level_low > palarm_info->level) ||
					(alarm_rec->level_high < palarm_info->level)))
					continue;
			}

			if (alarm_rec->ex_type != -1)
			{
				if (alarm_rec->ex_type != palarm_info->ex_type)
					continue;
			}

			if (alarm_rec->ex_event != -1)
			{
				if (alarm_rec->ex_event != palarm_info->ex_event)
					continue;
			}

			if (alarm_rec->ex_evt_len != -1)
			{
				if (alarm_rec->ex_evt_len != palarm_info->ex_evt_len)
					continue;

				if (memcmp(alarm_rec->pEx_evt_prm, palarm_info->ex_evt_prm, alarm_rec->ex_evt_len))
					continue;
			}

			//Everything match, start the conversion
			result_rec = alarm_rec->pResult;

			if (result_rec->type != -1) palarm_info->type = (uint8_t)result_rec->type;
			if (result_rec->level != -1) palarm_info->level = (uint8_t)result_rec->level;
			if (result_rec->ex_type != -1) palarm_info->ex_type = (uint8_t)result_rec->ex_type;
			if (result_rec->ex_event != -1) palarm_info->ex_event = (uint8_t)result_rec->ex_event;
			if (result_rec->ex_evt_type != -1) palarm_info->ex_evt_type = (uint8_t)result_rec->ex_evt_type;

			if (result_rec->ex_evt_len != -1)
			{
                //if (result_rec->ex_evt_len > palarm_info->ex_evt_len)
                //Realloc memory if result size is different from the original size
                if (result_rec->ex_evt_len != palarm_info->ex_evt_len)
                {
                    zwalrm_p ptmp_alarm_info;

                    //ptmp_alarm_info = realloc(palarm_info, sizeof(zwalrm_t) + result_rec->ex_evt_len -1);
                    //Don't minus 1.  That will break the structure zwalrm_t if result_rec->ex_evt_len = 0
                    ptmp_alarm_info = realloc(palarm_info, sizeof(zwalrm_t) + result_rec->ex_evt_len);

                    if (!ptmp_alarm_info)
                    {
                        res = ZW_ERR_MEMORY;
                        break;
                    }
                    palarm_info = ptmp_alarm_info;
                    palarm_info->ex_evt_len = result_rec->ex_evt_len;
                }
                memcpy(palarm_info->ex_evt_prm, result_rec->pEx_evt_prm, result_rec->ex_evt_len);
			}

			if (result_rec->ex_type != -1 || result_rec->ex_event != -1 ||
				result_rec->ex_evt_len != -1 || result_rec->ex_evt_type != -1)
				palarm_info->ex_info = 1;

			*ppalarm_info = palarm_info;
			break;
		}
	}

	plt_mtx_ulck(nw->mtx);

	return res;
}

/**
zwif_fill_alrm_info_struct - Fill/update alarm_info struct based on the alarm result database record
@param[in]		pResult     Device specific alarm result record
@param[in, out] ppalarm_info	Address of alarm info pointer
@return      ZW_ERR_XXX
@note	Conversion will be performed if all the critiria are matched for a match record in database.
*/
static int zwif_fill_alrm_info_struct(if_rec_alarm_result_t *pResult, zwalrm_p *ppalarm_info)
{
	zwalrm_p palarm_info = *ppalarm_info;
	int      res = ZW_ERR_NONE;
	int		 ex_evt_len = pResult->ex_evt_len > 0 ? pResult->ex_evt_len : 0;

	if (palarm_info == NULL)
	{
		palarm_info = calloc(1, sizeof(zwalrm_t) + ex_evt_len);

		if (!palarm_info)
		{
			return ZW_ERR_MEMORY;
		}
	}

	if (pResult->type != -1) palarm_info->type = (uint8_t)pResult->type;
	if (pResult->level != -1) palarm_info->level = (uint8_t)pResult->level;
	if (pResult->ex_type != -1) palarm_info->ex_type = (uint8_t)pResult->ex_type;
	if (pResult->ex_event != -1) palarm_info->ex_event = (uint8_t)pResult->ex_event;
	if (pResult->ex_evt_type != -1) palarm_info->ex_evt_type = (uint8_t)pResult->ex_evt_type;

	if (ex_evt_len > 0)
	{
		memcpy(palarm_info->ex_evt_prm, pResult->pEx_evt_prm, pResult->ex_evt_len);
		palarm_info->ex_evt_len = ex_evt_len;
	}


	if (pResult->ex_type != -1 || pResult->ex_event != -1 ||
		pResult->ex_evt_len != -1 || pResult->ex_evt_type != -1)
		palarm_info->ex_info = 1;

	*ppalarm_info = palarm_info;

	return res;
}


/**
zwif_alrm_snsr_cache_updt - Update alarm sensor value cache
@param[in] intf        Interface
@param[in] snsr_value  Alarm Sensor value
@param[in] time_stamp  Time stamp
@return  Non-zero if cached value is different from the live report value; else return zero
*/
static int zwif_alrm_snsr_cache_updt(zwif_p intf, zw_alrm_snsr_t *snsr_value, time_t *time_stamp)
{
	int         i;
	int         ent_sz;     //entry size
	int         found = 0;
    int         cache_chg = 0;
	void        *tmp_buf;
	time_t      *tmstamp;
	uint8_t     *cch_data;
	zw_alrm_snsr_t   *cch_snsr_value;

	ent_sz = sizeof(time_t) + sizeof(zw_alrm_snsr_t);

	if ((intf->cch_dat_cnt == 0) || (intf->cch_dat_item_sz != ent_sz))
	{
		return 1;
	}

	cch_data = intf->cch_data;

	//Find matching type
	for (i = 0; i<intf->cch_dat_cnt; i++)
	{
		tmstamp = (time_t *)(cch_data + (i * ent_sz));
		cch_snsr_value = (zw_alrm_snsr_t *)(tmstamp + 1);
		//Use type only and no soure_node_id because for alarm sensor get, there is only 1 param - type. Since
		//get call is not based on source node id, caching reports with different id will be meaningless since cannot
		//return back.
		if (cch_snsr_value->type == snsr_value->type)
		{
			found = 1;
			break;
		}
	}

    if (found)
    {   //Check whether cache value is stale
        if (cch_snsr_value->state != snsr_value->state)
        {
            cache_chg = 1;
        }
    }
    else
	{   //Allocate memory for new entry
		tmp_buf = realloc(intf->cch_data, ent_sz * (intf->cch_dat_cnt + 1));
		if (!tmp_buf)
		{
			return 1;
		}
		cch_data = tmp_buf;
		tmstamp = (time_t *)(cch_data + (intf->cch_dat_cnt * ent_sz));
		cch_snsr_value = (zw_alrm_snsr_t *)(tmstamp + 1);

		//Save the newly allocated buffer
		intf->cch_data = tmp_buf;
		intf->cch_dat_cnt++;

        cache_chg = 1;
	}
	//Update cache
	*tmstamp = *time_stamp;
	*cch_snsr_value = *snsr_value;

    return cache_chg;
}


/**
zwif_alrm_evt_param_prop - Check whether the alarm event parameter type is proprietary
@param[in]	alrm	Alarm type, ZW_ALRM_XXX
@param[in]	evt	    Alarm event, ZW_ALRM_EVT_XXX
@return     Non-zero if alarm event parameter type is proprietary; else return zero
*/
static int zwif_alrm_evt_param_prop(uint8_t alrm, uint8_t evt)
{
    const static uint8_t alrm_type_event[] = {
        ZW_ALRM_CO, ZW_ALRM_EVT_CO_TEST,
        ZW_ALRM_CO2, ZW_ALRM_EVT_CO2_TEST,
        ZW_ALRM_LOCK, ZW_ALRM_EVT_BARRIER_INIT,
        ZW_ALRM_LOCK, ZW_ALRM_EVT_BARRIER_MOTOR_TIME_EXCEED,
        ZW_ALRM_LOCK, ZW_ALRM_EVT_BARRIER_VACAT_MODE,
        ZW_ALRM_LOCK, ZW_ALRM_EVT_BARRIER_BEAM_OBST,
        ZW_ALRM_LOCK, ZW_ALRM_EVT_BARRIER_SNR_ERR,
        ZW_ALRM_LOCK, ZW_ALRM_EVT_BARRIER_LOW_BATT,
        ZW_ALRM_HEALTH, ZW_ALRM_EVT_VOLATILE_ORGANIC_COMPOUND_LVL,
        ZW_ALRM_WATER_VLV, ZW_ALRM_EVT_VALVE_OP,
        ZW_ALRM_WATER_VLV, ZW_ALRM_EVT_MSTR_VALVE_OP,
        ZW_ALRM_WATER_VLV, ZW_ALRM_EVT_CUR_ALRM,
        ZW_ALRM_WATER_VLV, ZW_ALRM_EVT_MSTR_CUR_ALRM,
        ZW_ALRM_IRRIGATION, ZW_ALRM_EVT_SCHED_START,
        ZW_ALRM_IRRIGATION, ZW_ALRM_EVT_SCHED_FIN,
        ZW_ALRM_IRRIGATION, ZW_ALRM_EVT_VLV_TBL_RUN_START,
        ZW_ALRM_IRRIGATION, ZW_ALRM_EVT_VLV_TBL_RUN_FIN
    };

    int tot_bytes;
    int i;

    tot_bytes = sizeof(alrm_type_event);

    for (i=0; i<tot_bytes; i += 2)
    {
        if ((alrm_type_event[i] == alrm) && (alrm_type_event[i+1] == evt))
        {
            return 1;
        }
    }

    return 0;
}


/**
zwif_alrm_evt_param_loc - Check whether the alarm event parameter type is location string
@param[in]	alrm	Alarm type, ZW_ALRM_XXX
@param[in]	evt	    Alarm event, ZW_ALRM_EVT_XXX
@return     Non-zero if alarm event parameter type is location string; else return zero
*/
static int zwif_alrm_evt_param_loc(uint8_t alrm, uint8_t evt)
{
    const static uint8_t alrm_type_evt_loc[] = {
        ZW_ALRM_SMOKE, ZW_ALRM_EVT_SMOKE_L,
        ZW_ALRM_CO, ZW_ALRM_EVT_CO_L,
        ZW_ALRM_CO2, ZW_ALRM_EVT_CO2_L,
        ZW_ALRM_HEAT, ZW_ALRM_EVT_OVERHEAT_L,
        ZW_ALRM_HEAT, ZW_ALRM_EVT_TEMP_RISE_L,
        ZW_ALRM_HEAT, ZW_ALRM_EVT_UNDRHEAT_L,
        ZW_ALRM_WATER, ZW_ALRM_EVT_LEAK_L,
        ZW_ALRM_WATER, ZW_ALRM_EVT_LVL_L,
        ZW_ALRM_BURGLAR, ZW_ALRM_EVT_INTRUSION_L,
        ZW_ALRM_BURGLAR, ZW_ALRM_EVT_GLASS_L,
        ZW_ALRM_BURGLAR, ZW_ALRM_EVT_MOTION_DET_L,
        ZW_ALRM_GAS, ZW_ALRM_EVT_COMBUST_GAS_DET_L,
        ZW_ALRM_GAS, ZW_ALRM_EVT_TOXIC_GAS_DET_L
    };

    int tot_bytes;
    int i;

    tot_bytes = sizeof(alrm_type_evt_loc);

    for (i=0; i<tot_bytes; i += 2)
    {
        if ((alrm_type_evt_loc[i] == alrm) && (alrm_type_evt_loc[i+1] == evt))
        {
            return 1;
        }
    }

    return 0;
}


/**
zwif_rep_hdlr - Interface report handler
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@param[in]	cmd_len	The length of cmd_buf
@param[in]	rx_sts  Receive status [deprecated]
@return     Non-zero if the report is handled; else return zero
*/
int zwif_rep_hdlr(zwif_p intf, uint8_t *cmd_buf, uint16_t cmd_len, uint8_t rx_sts)
{
    int         i;
    int         single_ep_sm_sts;   // Single endpoint state-machine status: 0=idle; 1=running
    int         cache_chg;
    zwifd_t     ifd;
    void        *report_cb;
    void        *sm_rpt_cb;
    time_t      *tm_stamp_p;
    uint16_t    cls;
    uint8_t     rpt_cmd;

    //Check whether there is a callback function for this interface
    if (!zwif_cls_cmd_get(cmd_buf, cmd_len, &cls, &rpt_cmd))
    {   //Couldn't parse the command class and command
        return 0;
    }
    //Initialize
    sm_rpt_cb = report_cb = NULL;
    cache_chg = 0;

    for (i=0; i<intf->rpt_num; i++)
    {
        if (intf->rpt[i].rpt_cmd == rpt_cmd)
        {
            report_cb = intf->rpt[i].rpt_cb;
            sm_rpt_cb = intf->rpt[i].sm_rpt_cb;
            break;
        }
    }

    single_ep_sm_sts = intf->ep->node->net->single_ep_job.sts;

    if (!(report_cb || sm_rpt_cb))
    {   //Wake up notification command does not require a user-defined callback
		//Basic and sensor alarm may have a database entry to convert to another report
        if (!(cmd_buf[0] == COMMAND_CLASS_WAKE_UP && cmd_buf[1] == WAKE_UP_NOTIFICATION) &&
			!(cmd_buf[0] == COMMAND_CLASS_BASIC && cmd_buf[1] == BASIC_SET) &&
			!(cmd_buf[0] == COMMAND_CLASS_SENSOR_ALARM))
        {
#ifdef ALARM_2_DLOCK_RPT_CONVERSION
            if (cmd_buf[0] != COMMAND_CLASS_ALARM)
#endif
            return 0;
        }
    }

    switch (cls)
    {
        case COMMAND_CLASS_VERSION:
            if (report_cb)
            {
                if (cmd_buf[1] == VERSION_COMMAND_CLASS_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_ver_fn    rpt_cb;
                        rpt_cb = (zwrep_ver_fn)report_cb;
                        //Callback the registered function
                        rpt_cb(intf, cmd_buf[2], cmd_buf[3]);
                    }
                }
                else if (cmd_buf[1] == VERSION_REPORT)
                {
                    if (cmd_len >= 7)
                    {
                        zwrep_zwver_fn  rpt_cb;
                        ext_ver_t       *ext_ver = NULL;
                        uint16_t        proto_ver;
                        uint16_t        app_ver;

                        rpt_cb = (zwrep_zwver_fn)report_cb;
                        proto_ver = ((uint16_t)cmd_buf[3])<<8 | cmd_buf[4];
                        app_ver = ((uint16_t)cmd_buf[5])<<8 | cmd_buf[6];

                        if ((cmd_len >= 9) && (intf->ver > 1))
                        {   //Version 2
                            uint16_t    fw_ver;
                            uint8_t     fw_cnt;
                            int         i;
                            int         j;


                            fw_cnt = cmd_buf[8];
                            if ((cmd_len - 9) >= (fw_cnt * 2))
                            {
                                ext_ver = (ext_ver_t *)malloc(sizeof(ext_ver_t) + (fw_cnt*2));
                                if (ext_ver)
                                {
                                    ext_ver->hw_ver = cmd_buf[7];
                                    ext_ver->fw_cnt = fw_cnt;
                                    j=9;
                                    for (i=0; i<fw_cnt; i++)
                                    {
                                        fw_ver = cmd_buf[j++];
                                        fw_ver = (fw_ver << 8) | cmd_buf[j++];
                                        ext_ver->fw_ver[i] = fw_ver;
                                    }
                                }
                            }
                        }

                        //Callback the registered function
                        rpt_cb(intf, proto_ver, app_ver, cmd_buf[2], ext_ver);
                        free(ext_ver);
                    }
                }
            }
            break;

        case COMMAND_CLASS_BASIC:
            {
                if (cmd_buf[1] == BASIC_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_ts_fn    rpt_cb;
                        uint8_t        lvl;

                        if (!intf->cch_data)
                        {   //No previous cache
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&lvl, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (lvl != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_ts_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_ts_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
				else if (cmd_buf[1] == BASIC_SET)
				{
					if (cmd_len >= 3)
					{
						zwnode_p        node = intf->ep->node;
						zwnet_p         nw = node->net;
						if_rec_basic_match_t  *basic_rec;
						zwif_p			intf_alarm = NULL;
						time_t          time_stamp;

						plt_mtx_lck(nw->mtx);
						//Check whether device configuration record has alarm records
						if (node->dev_cfg_valid)
						{
							basic_rec = (if_rec_basic_match_t *)zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_BASIC);

							for (; basic_rec != NULL; basic_rec = basic_rec->next)
							{
								//Find the matching basic set record
								if (basic_rec->command == BASIC_SET && basic_rec->value == cmd_buf[2])
								{
									if (basic_rec->resultType == IF_REC_TYPE_ALARM)
									{
										intf_alarm = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_ALARM);

										if (intf_alarm)
										{
											zwrep_alrm_fn       rpt_cb = NULL;
											zwalrm_p            alrm_info = NULL;

											for (i = 0; i<intf_alarm->rpt_num; i++)
											{
												if (intf_alarm->rpt[i].rpt_cmd == ALARM_REPORT)
												{
													rpt_cb = intf_alarm->rpt[i].rpt_cb;
													break;
												}
											}

											if (rpt_cb)
											{
												if (zwif_fill_alrm_info_struct(basic_rec->pResult, &alrm_info) == ZW_ERR_NONE)
												{
													//Cache the report and callback
													util_time_stamp(&time_stamp);

													if ((intf_alarm->ver == 1) || (alrm_info->ex_type != 0))
													{
														if (!intf_alarm->cch_data)
														{   //No cached report, cache it.
															//Cached alarm report is per alarm type, per alarm event and variable length.
															//Will use 2 level cache data format
															zwif_dat_cache_alloc(intf_alarm, sizeof(zwalrm_t) + alrm_info->ex_evt_len, TRUE);
															if (intf_alarm->cch_data && intf_alarm->cch_dat_cnt && ((cch_1lvl_t *)intf_alarm->cch_data)->pcch_ele)
															{
																cch_1lvl_t *p1lvl = (cch_1lvl_t *)intf_alarm->cch_data;

																//Update cache
																memcpy(p1lvl->pcch_ele, &time_stamp, sizeof(time_t));
																memcpy(((time_t *)p1lvl->pcch_ele) + 1, alrm_info, sizeof(zwalrm_t) + alrm_info->ex_evt_len);
															}
														}
														else
														{   //Got at least one cached report
															zwif_alrm_cache_updt(intf_alarm, alrm_info, sizeof(zwalrm_t) + alrm_info->ex_evt_len, &time_stamp);
														}
													}

                                                    zwif_get_desc(intf_alarm, &ifd);
                                                    rpt_cb(&ifd, alrm_info, time_stamp);

													free(alrm_info);
												}
											}
										}
									}
									break;
								}
							}
						}
						plt_mtx_ulck(nw->mtx);
					}

					return 0;	//For basic set, it is still considered 'unhandled' command
				}
            }
            break;

#ifdef  TEST_EXT_CMD_CLASS
        case COMMAND_CLASS_EXT_TEST:
            if (report_cb)
            {
                if (rpt_cmd == BASIC_REPORT)
                {
                    if (cmd_len > 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[3]);
                    }
                }
            }
            break;
#endif
        case COMMAND_CLASS_SWITCH_MULTILEVEL:
            {
                if (cmd_buf[1] == SWITCH_MULTILEVEL_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_ts_fn rpt_cb;
                        uint8_t     lvl;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&lvl, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (lvl != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_ts_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_ts_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
                else if (cmd_buf[1] == SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3)
                {
                    if (cmd_len >= 4)
                    {
                        if_mul_switch_data_t    *mul_switch_data;
                        zwrep_lvl_sup_fn        rpt_cb;

                        //Save to cache
                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            mul_switch_data = (if_mul_switch_data_t *)malloc(sizeof(if_mul_switch_data_t));
                        }
                        else
                        {   //Use previously allocated memory
                            mul_switch_data = (if_mul_switch_data_t *)intf->data;
                        }

                        if (mul_switch_data)
                        {
                            intf->data_cnt = 1;
                            intf->data_item_sz = sizeof(if_mul_switch_data_t);
                            intf->data = mul_switch_data;
                            mul_switch_data->pri_type = cmd_buf[2] & 0x1F;
                            mul_switch_data->sec_type = cmd_buf[3] & 0x1F;
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_lvl_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2] & 0x1F, cmd_buf[3] & 0x1F, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_lvl_sup_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2] & 0x1F, cmd_buf[3] & 0x1F, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_SWITCH_BINARY:
            {
                if (cmd_buf[1] == SWITCH_BINARY_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_switch_fn    rpt_cb;
                        uint8_t            sw_state;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&sw_state, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (sw_state != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_switch_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_switch_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_NODE_NAMING:
            {
                if (cmd_buf[1] == NODE_NAMING_NODE_LOCATION_REPORT
                    || cmd_buf[1] == NODE_NAMING_NODE_NAME_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_name_fn   rpt_cb;

                        if (cmd_len > (ZW_LOC_STR_MAX + 3))
                        {
                            cmd_len = ZW_LOC_STR_MAX + 3;
                        }

                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_name_fn)sm_rpt_cb;
                            rpt_cb(intf, (char *)(cmd_buf + 3), cmd_len - 3, cmd_buf[2] & 0x07);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_name_fn)report_cb;
                            rpt_cb(intf, (char *)(cmd_buf + 3), cmd_len - 3, cmd_buf[2] & 0x07);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_SENSOR_BINARY:
            {
                if (cmd_buf[1] == SENSOR_BINARY_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwbsnsr_t           snsr_dat = {0};
                        time_t              time_stamp;
                        zwrep_bsensor_fn    rpt_cb;

                        snsr_dat.state = cmd_buf[2];

                        if (cmd_len == 3)
                        {   //version 1, try to map the sensor type using device configuration database
                            zwnet_p     nw;
                            zwnode_p    node;

                            node = intf->ep->node;
                            nw = node->net;

                            plt_mtx_lck(nw->mtx);
                            if (node->dev_cfg_valid)
                            {
                                if_rec_bsnsr_t   *bsnsr_rec;
                                bsnsr_rec = zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_BIN_SENSOR);
                                if (bsnsr_rec)
                                {
                                    snsr_dat.type = bsnsr_rec->type;
                                }
                            }
                            plt_mtx_ulck(nw->mtx);
                        }
                        else
                        {   //version 2
                            snsr_dat.type = cmd_buf[3];
                        }

                        util_time_stamp(&time_stamp);

                        if (!intf->cch_data)
                        {   //No cached report, cache it.
                            cache_chg = 1;

                            zwif_dat_cache_alloc(intf, sizeof(zwbsnsr_t), FALSE);
                            if (intf->cch_data && intf->cch_dat_cnt)
                            {   //Update cache
                                memcpy(intf->cch_data, &time_stamp, sizeof(time_t));
                                memcpy(((time_t *)intf->cch_data) + 1, &snsr_dat, sizeof(snsr_dat));
                            }
                        }
                        else
                        {   //Got at least one cached report
                            cache_chg = zwif_bsnsr_cache_updt(intf, &snsr_dat, &time_stamp);
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_bsensor_fn)sm_rpt_cb;
                            rpt_cb(&ifd, snsr_dat.state, snsr_dat.type, time_stamp);
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_bsensor_fn)report_cb;
                            rpt_cb(&ifd, snsr_dat.state, snsr_dat.type, time_stamp);
                        }
                    }
                }
                else if (cmd_buf[1] == SENSOR_BINARY_SUPPORTED_SENSOR_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_bsensor_sup_fn rpt_cb;
                        uint8_t             *sensor_dat;
                        uint8_t             snsr_type[248];
                        int                 i;
                        int                 max_snsr_type;
                        uint8_t             type_len;

                        type_len = 0;
                        max_snsr_type = (cmd_len - 2) * 8;

                        for (i = 0; i < max_snsr_type; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                snsr_type[type_len++] = i;
                            }
                        }

                        //Save to cache
                        if (type_len > 0)
                        {
                            if (intf->data_cnt == 0)
                            {
                                //Allocate memory for data
                                sensor_dat = (uint8_t *)malloc(type_len);
                            }
                            else
                            {   //Use previously allocated memory
                                sensor_dat = (uint8_t *)intf->data;
                            }

                            if (sensor_dat)
                            {
                                //Save the sensor types
                                memcpy(sensor_dat, snsr_type, type_len);

                                intf->data_cnt = type_len;
                                intf->data_item_sz = sizeof(uint8_t);
                                intf->data = sensor_dat;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_bsensor_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, type_len, snsr_type, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_bsensor_sup_fn)report_cb;
                            rpt_cb(&ifd, type_len, snsr_type, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_SENSOR_MULTILEVEL:
            {
                if (cmd_buf[1] == SENSOR_MULTILEVEL_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        time_t          time_stamp;
                        zwsensor_t      snsr_value = {0};
                        zwrep_sensor_fn rpt_cb;

                        snsr_value.type = cmd_buf[2];
                        snsr_value.precision = cmd_buf[3] >> 5;
                        snsr_value.unit = (cmd_buf[3] >> 3) & 0x03;
                        snsr_value.size = cmd_buf[3] & 0x07;

                        //Check sensor value size
                        if (memchr(data_storage_sz, snsr_value.size, sizeof(data_storage_sz)) == NULL)
                        {
                            return 1;
                        }

                        if (cmd_len >= (4 + snsr_value.size))
                        {
                            memcpy(snsr_value.data, cmd_buf + 4, snsr_value.size);
                            util_time_stamp(&time_stamp);

                            if (!intf->cch_data)
                            {   //No cached report, cache it.
                                cache_chg = 1;
                                zwif_dat_cache_alloc(intf, sizeof(zwsensor_t), FALSE);
                                if (intf->cch_data && intf->cch_dat_cnt)
                                {   //Update cache
                                    memcpy(intf->cch_data, &time_stamp, sizeof(time_t));
                                    memcpy(((time_t *)intf->cch_data) + 1, &snsr_value, sizeof(snsr_value));
                                }
                            }
                            else
                            {   //Got at least one cached report
                                cache_chg = zwif_snsr_cache_updt(intf, &snsr_value, &time_stamp);
                            }

                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_sensor_fn)sm_rpt_cb;
                                rpt_cb(&ifd, &snsr_value, time_stamp);
                            }

                            if (report_cb && cache_chg)
                            {   //Callback to application
                                rpt_cb = (zwrep_sensor_fn)report_cb;
                                rpt_cb(&ifd, &snsr_value, time_stamp);
                            }
                        }
                    }
                }
                else if (cmd_buf[1] == SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_sensor_sup_fn rpt_cb;
                        uint8_t             snsr_type[248];
                        int                 i;
                        int                 max_snsr_type;
                        if_sensor_data_t    *sensor_dat;
                        uint8_t             snsr_type_cnt;


                        snsr_type_cnt = 0;
                        max_snsr_type = (cmd_len - 2) * 8;

                        for (i = 0; i < max_snsr_type; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                snsr_type[snsr_type_cnt++] = i + 1;
                            }
                        }

                        //Save to cache
                        if (snsr_type_cnt > 0)
                        {
                            if (intf->data_cnt == 0)
                            {
                                //Allocate memory for data
                                sensor_dat = (if_sensor_data_t *)calloc(snsr_type_cnt, sizeof(if_sensor_data_t));
                            }
                            else
                            {   //Use previously allocated memory
                                sensor_dat = (if_sensor_data_t *)intf->data;
                            }

                            if (sensor_dat)
                            {
                                //Save the sensor types
                                for (i=0; i<snsr_type_cnt; i++)
                                {
                                    sensor_dat[i].sensor_type = snsr_type[i];
                                }

                                intf->data_cnt = snsr_type_cnt;
                                intf->data_item_sz = sizeof(if_sensor_data_t);
                                intf->data = sensor_dat;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_sensor_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, snsr_type_cnt, snsr_type, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_sensor_sup_fn)report_cb;
                            rpt_cb(&ifd, snsr_type_cnt, snsr_type, 1);
                        }
                    }
                }
                else if (cmd_buf[1] == SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_sensor_unit_fn    rpt_cb;

                        //Save to cache
                        if (intf->data_cnt > 0)
                        {
                            int                 i;
                            if_sensor_data_t    *sensor_dat = (if_sensor_data_t *)intf->data;

                            //Search for the sensor type
                            for (i=0; i<intf->data_cnt; i++)
                            {
                                if (sensor_dat[i].sensor_type == cmd_buf[2])
                                {
                                    //Save the supported sensor units
                                    sensor_dat[i].sensor_unit = cmd_buf[3] & 0x0F;
                                    break;
                                }
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_sensor_unit_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], cmd_buf[3] & 0x0F, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_sensor_unit_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], cmd_buf[3] & 0x0F, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ASSOCIATION:
            {
                if (cmd_buf[1] == ASSOCIATION_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        int                 i;
                        int                 merge_rpt = 0;  //Flag to indicate if previous reports have been merged
                        zwnet_p             nw;
                        grp_member_t        *grp_mbr;
                        uint8_t             grp_mbr_cnt;
                        zwrep_group_fn      rpt_cb;

                        grp_mbr_cnt = cmd_len - 5;
                        grp_mbr = (grp_member_t *)calloc(grp_mbr_cnt, sizeof(grp_member_t));

                        if (!grp_mbr)
                        {
                            break;
                        }

                        nw = intf->ep->node->net;

                        for (i=0; i<grp_mbr_cnt; i++)
                        {
                            grp_mbr[i].node_id = cmd_buf[5 + i];
                        }

                        //Check if there is a previous report to consolidate
                        if (nw->asc_rpt.grp_mbr_cnt > 0)
                        {
                            //Check whether group id and max. node supported match
                            if (nw->asc_rpt.group_id == cmd_buf[2] &&
                                nw->asc_rpt.max_node == cmd_buf[3])
                            {
                                grp_member_t *grp_mbr_tmp;
                                grp_mbr_tmp = realloc(nw->asc_rpt.grp_mbr_buf,
                                                      (grp_mbr_cnt + nw->asc_rpt.grp_mbr_cnt) * sizeof(grp_member_t));
                                if (grp_mbr_tmp)
                                {
                                    nw->asc_rpt.grp_mbr_buf = grp_mbr_tmp;
                                    //Add additional members to buffer
                                    for (i=0; i<grp_mbr_cnt; i++)
                                    {
                                        nw->asc_rpt.grp_mbr_buf[nw->asc_rpt.grp_mbr_cnt++] = grp_mbr[i];
                                    }
                                }
                                merge_rpt = 1;
                                free(grp_mbr);
                            }
                            else
                            {   //Group id and max. node supported do not match

                                //Free the association report storage
                                free(nw->asc_rpt.grp_mbr_buf);
                            }
                        }

                        if (!merge_rpt)
                        {
                            //Create new association report storage based on current report
                            nw->asc_rpt.group_id = cmd_buf[2];
                            nw->asc_rpt.max_node = cmd_buf[3];
                            nw->asc_rpt.grp_mbr_buf = grp_mbr;
                            nw->asc_rpt.grp_mbr_cnt = grp_mbr_cnt;
                        }

                        //Check whether this is the final report
                        if (cmd_buf[4] == 0)
                        {
                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_group_fn)sm_rpt_cb;
                                rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], nw->asc_rpt.grp_mbr_cnt, nw->asc_rpt.grp_mbr_buf);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_group_fn)report_cb;
                                rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], nw->asc_rpt.grp_mbr_cnt, nw->asc_rpt.grp_mbr_buf);
                            }

                            //Free the association report storage
                            free(nw->asc_rpt.grp_mbr_buf);
                            nw->asc_rpt.grp_mbr_buf = NULL;
                            nw->asc_rpt.grp_mbr_cnt = 0;
                        }
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_GROUPINGS_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        uint8_t               *max_grp;
                        zwrep_group_sup_fn    rpt_cb;

                        //Save to cache
                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            max_grp = (uint8_t *)malloc(1);
                        }
                        else
                        {   //Use previously allocated memory
                            max_grp = (uint8_t *)intf->data;
                        }

                        if (max_grp)
                        {
                            intf->data_cnt = intf->data_item_sz = 1;
                            intf->data = max_grp;
                            *max_grp = cmd_buf[2];

                            //Check whether to mirror the max_grp to multi-channel association interface
                            if (intf->cls == COMMAND_CLASS_ASSOCIATION)
                            {
                                zwif_p  mc_assoc_intf;
                                mc_assoc_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2);
                                if (mc_assoc_intf)
                                {
                                    if (mc_assoc_intf->data_cnt == 0)
                                    {
                                        //Allocate memory for data
                                        max_grp = (uint8_t *)malloc(1);
                                    }
                                    else
                                    {   //Use previously allocated memory
                                        max_grp = (uint8_t *)mc_assoc_intf->data;
                                    }

                                    if (max_grp)
                                    {
                                        mc_assoc_intf->data_cnt = mc_assoc_intf->data_item_sz = 1;
                                        mc_assoc_intf->data = max_grp;
                                        *max_grp = cmd_buf[2];
                                    }
                                }
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_group_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_group_sup_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], 1);
                        }
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_SPECIFIC_GROUP_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_group_actv_fn    rpt_cb;

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_group_actv_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2]);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_group_actv_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2]);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_IP_ASSOCIATION:
            if (report_cb)
            {
                if (cmd_buf[1] == IP_ASSOCIATION_REPORT)
                {
                    if (cmd_len >= 23)
                    {
                        zwnet_p             nw;
                        uint8_t             rpt_idx;
                        if_ip_grp_dat_t     *ip_grp_dat;
                        zwrep_group_fn      rpt_cb;
                        rpt_cb = (zwrep_group_fn)report_cb;

                        nw = intf->ep->node->net;

                        rpt_idx = cmd_buf[3];

                        //Check for first report
                        if (rpt_idx == 1)
                        {
                            if (!zwif_ip_assoc_1st_rpt_hdlr(nw, intf, cmd_buf))
                                break;
                        }
                        else if (rpt_idx > 1)
                        {   //Subsequent report
                            if (!zwif_ip_assoc_other_rpt_hdlr(nw, intf, cmd_buf))
                                break;
                        }
                        else
                        {   //Invalid report index
                            break;
                        }

                        //Check whether this is the final report
                        ip_grp_dat = (if_ip_grp_dat_t *)intf->tmp_data;
                        zwif_get_desc(intf, &ifd);

                        if ((ip_grp_dat->tot_entries == 0) || (ip_grp_dat->tot_entries == rpt_idx))
                        {
                            //Callback
                            rpt_cb(&ifd, cmd_buf[2],
                                   (ip_grp_dat->tot_entries == 0)? 1 : ip_grp_dat->tot_entries,//estimate max. count based on number of entries
                                   ip_grp_dat->valid_entries, ip_grp_dat->grp_mbr);

                            //Free the temporary report storage
                            free(intf->tmp_data);
                            intf->tmp_data = NULL;
                        }
                        else
                        {   //Get next report
                            uint8_t     assoc_get_cmd[4] = {COMMAND_CLASS_IP_ASSOCIATION, IP_ASSOCIATION_GET, 0, 0};

                            assoc_get_cmd[2] = cmd_buf[2];
                            assoc_get_cmd[3] = ip_grp_dat->index + 1;

                            //Send the command
                            zwif_exec_ex(&ifd, assoc_get_cmd, 4, zwif_tx_sts_cb, NULL,
                                         ZWIF_OPT_SKIP_MUL_Q, NULL);

                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION:
            if (report_cb)
            {
                if (cmd_buf[1] == COMMAND_RECORDS_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 7)
                    {
                        zwrep_grp_cmd_sup_fn    rpt_cb;
                        zwgrp_cmd_cap_t         cmd_cap;

                        rpt_cb = (zwrep_grp_cmd_sup_fn)report_cb;
                        cmd_cap.configurable = cmd_buf[2] & 0x01;
                        cmd_cap.config_type = (cmd_buf[2]>>1) & 0x01;
                        cmd_cap.max_len = cmd_buf[2]>>2;
                        cmd_cap.free_rec = (((uint16_t)cmd_buf[3]) << 8) | cmd_buf[4];
                        cmd_cap.total_rec = (((uint16_t)cmd_buf[5]) << 8) | cmd_buf[6];

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &cmd_cap);
                    }
                }
                else if (cmd_buf[1] == COMMAND_CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwrep_grp_cmd_fn    rpt_cb;
                        rpt_cb = (zwrep_grp_cmd_fn)report_cb;

                        if (cmd_len >= (6 + cmd_buf[5]))
                        {
                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], cmd_buf + 6, cmd_buf[5]);
                        }
                    }
                }
            }
            break;


        case COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2:
            {
                if (cmd_buf[1] == MULTI_CHANNEL_ASSOCIATION_REPORT_V2)
                {
                    if (cmd_len >= 5)
                    {
                        int                 i;
                        int                 have_ep;        //flag to determine whether a node has endpoint
                        int                 is_ep_bit_addr; //flag to determine whether endpoint is bit-addressable
                        int                 merge_rpt = 0;  //Flag to indicate if previous reports have been merged
                        unsigned            allocated_mbr;
                        zwnet_p             nw;
                        grp_member_t        *grp_mbr;
                        zwrep_group_fn      rpt_cb;
                        uint8_t             ep_id;
                        uint8_t             node_id;
                        uint8_t             grp_mbr_cnt;
                        uint8_t             valid_ep_cnt;

                        //Find all the endpoints in this report
                        grp_mbr_cnt = cmd_len - 5;  //Estimate maximum number of members
                        grp_mbr = (grp_member_t *)calloc(grp_mbr_cnt, sizeof(grp_member_t));

                        if (!grp_mbr)
                        {
                            break;
                        }

                        allocated_mbr = grp_mbr_cnt;

                        nw = intf->ep->node->net;

                        valid_ep_cnt = 0;
                        have_ep = 0;    //init to node without endpoint

                        for (i=0; i<grp_mbr_cnt; i++)
                        {
                            //Check for the marker to mark the end of node without endpoints
                            if (!have_ep
                                && (cmd_buf[5 + i] == MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V2))
                            {
                                have_ep = 1;
                                continue;
                            }

                            node_id = cmd_buf[5 + i];

                            if (have_ep)
                            {
                                i++; //point to endpoint id
                                ep_id = cmd_buf[5 + i];

                                //Find the endpoint
                                is_ep_bit_addr = 0;
                                if ((intf->ver >= 2) && (ep_id & 0x80))
                                {
                                    //Bit-address is set:
                                    //Bit 0 is End Point 1, bit 1 is End Point 2 … bit 6 is End Point 7
                                    grp_member_t *  grp_mbr_tmp;
                                    unsigned        num_of_ep;
                                    int             j;

                                    is_ep_bit_addr = 1;

                                    //Count number of endpoints
                                    num_of_ep = 0;
                                    for (j=0; j<7; j++)
                                    {
                                        if ((ep_id >> j) & 0x01)
                                        {
                                            num_of_ep++;
                                        }
                                    }

                                    //Re-allocate memory
                                    grp_mbr_tmp = realloc(grp_mbr,
                                                          (allocated_mbr + num_of_ep) * sizeof(grp_member_t));
                                    if (grp_mbr_tmp)
                                    {
                                        allocated_mbr += num_of_ep;
                                        grp_mbr = grp_mbr_tmp;
                                        //Add enpoints to the member array
                                        for (j=0; j<7; j++)
                                        {
                                            if ((ep_id >> j) & 0x01)
                                            {
                                                grp_mbr[valid_ep_cnt].ep_id = j+1;
                                                grp_mbr[valid_ep_cnt].node_id = node_id;
                                                valid_ep_cnt++;
                                            }
                                        }
                                    }
                                }

                                if (!is_ep_bit_addr)
                                {   //Bit address not set
                                    grp_mbr[valid_ep_cnt].ep_id = ep_id;
                                    grp_mbr[valid_ep_cnt].node_id = node_id;
                                    valid_ep_cnt++;
                                }
                            }
                            else
                            {   //Node without endpoint
                                grp_mbr[valid_ep_cnt].ep_id = VIRTUAL_EP_ID;
                                grp_mbr[valid_ep_cnt].node_id = node_id;
                                valid_ep_cnt++;
                            }
                        }

                        //Check is there a previous report to consolidate
                        if (nw->asc_rpt.grp_mbr_cnt > 0)
                        {
                            //Check whether group id and max. node supported match
                            if (nw->asc_rpt.group_id == cmd_buf[2] &&
                                nw->asc_rpt.max_node == cmd_buf[3])
                            {
                                grp_member_t *grp_mbr_tmp;
                                grp_mbr_tmp = realloc(nw->asc_rpt.grp_mbr_buf,
                                                      (valid_ep_cnt + nw->asc_rpt.grp_mbr_cnt) * sizeof(grp_member_t));
                                if (grp_mbr_tmp)
                                {
                                    nw->asc_rpt.grp_mbr_buf = grp_mbr_tmp;
                                    //Add additional enpoints to the member buffer
                                    for (i=0; i<valid_ep_cnt; i++)
                                    {
                                        nw->asc_rpt.grp_mbr_buf[nw->asc_rpt.grp_mbr_cnt++] = grp_mbr[i];
                                    }
                                }
                                free(grp_mbr);
                                merge_rpt = 1;
                            }
                            else
                            {   //Group id and max. node supported do not match

                                //Free the association report storage
                                free(nw->asc_rpt.grp_mbr_buf);
                            }
                        }

                        if (!merge_rpt)
                        {
                            //Create new association report storage based on current report
                            nw->asc_rpt.group_id = cmd_buf[2];
                            nw->asc_rpt.max_node = cmd_buf[3];
                            nw->asc_rpt.grp_mbr_buf = grp_mbr;
                            nw->asc_rpt.grp_mbr_cnt = valid_ep_cnt;

                        }

                        //Check whether this is the final report
                        if (cmd_buf[4] == 0)
                        {
                            zwif_get_desc(intf, &ifd);

                            //Check if controller node id presents
                            for (i=0; i<nw->asc_rpt.grp_mbr_cnt; i++)
                            {
                                if (nw->asc_rpt.grp_mbr_buf[i].node_id == nw->ctl.nodeid)
                                {   //Undo previous hack in zwif_group_add(). Set controller's epid to zero
                                    nw->asc_rpt.grp_mbr_buf[i].ep_id = 0;
                                }
                            }

                            //Callback
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_group_fn)sm_rpt_cb;
                                rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], nw->asc_rpt.grp_mbr_cnt, nw->asc_rpt.grp_mbr_buf);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_group_fn)report_cb;
                                rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], nw->asc_rpt.grp_mbr_cnt, nw->asc_rpt.grp_mbr_buf);
                            }

                            //Free the association report storage
                            free(nw->asc_rpt.grp_mbr_buf);
                            nw->asc_rpt.grp_mbr_buf = NULL;
                            nw->asc_rpt.grp_mbr_cnt = 0;
                        }
                    }
                }
                else if (cmd_buf[1] == MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        uint8_t               *max_grp;
                        zwrep_group_sup_fn    rpt_cb;

                        //Save to cache
                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            max_grp = (uint8_t *)malloc(1);
                        }
                        else
                        {   //Use previously allocated memory
                            max_grp = (uint8_t *)intf->data;
                        }

                        if (max_grp)
                        {
                            intf->data_cnt = intf->data_item_sz = 1;
                            intf->data = max_grp;
                            *max_grp = cmd_buf[2];
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_group_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_group_sup_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_MULTI_CHANNEL_V2:
            if (report_cb)
            {
                switch (cmd_buf[1])
                {
                    case MULTI_INSTANCE_REPORT:
                        if (cmd_len >= 4)
                        {
                            zwrep_mul_inst_fn    rpt_cb;
                            rpt_cb = (zwrep_mul_inst_fn)report_cb;
                            //Callback the registered function
                            rpt_cb(intf, cmd_buf[2], cmd_buf[3]);
                        }
                        break;

                    case MULTI_CHANNEL_END_POINT_REPORT_V2:
                        if (cmd_len >= 4)
                        {
                            zwrep_mc_ep_fn    rpt_cb;
                            rpt_cb = (zwrep_mc_ep_fn)report_cb;
                            //Callback the registered function
                            rpt_cb(intf, cmd_buf[3] & 0x7F, cmd_buf[2]);
                        }
                        break;

                    case MULTI_CHANNEL_CAPABILITY_REPORT_V2:
                        if (cmd_len >= 6)
                        {
                            ep_cap_t            ep_cap;
                            zwrep_mc_cap_fn     rpt_cb;
                            uint16_t            *cmd_cls_sec;
                            uint8_t             cmd_cnt_sec;

                            rpt_cb = (zwrep_mc_cap_fn)report_cb;

                            ep_cap.ep = cmd_buf[2] & 0x7F;
                            ep_cap.ep_dynamic = (cmd_buf[2] & 0x80)? 1:0;
                            ep_cap.gen = cmd_buf[3];
                            ep_cap.spec = cmd_buf[4];

                            if (util_cmd_cls_parse(cmd_buf + 5, cmd_len - 5, &ep_cap.cmd_cls, &ep_cap.cmd_cnt, &cmd_cls_sec, &cmd_cnt_sec))
                            {
                                //Callback the registered function
                                rpt_cb(intf, &ep_cap);

                                free(ep_cap.cmd_cls);
                                free(cmd_cls_sec);
                            }
                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_WAKE_UP:
            {
                switch (cmd_buf[1])
                {
                    case WAKE_UP_NOTIFICATION:
                        {
                            debug_zwapi_msg(&intf->ep->node->net->plt_ctx, "Wake up notification received");

                            //Dequeue the commands
                            if (zwif_cmd_dequeue(intf) == 0)
                            {   //No command in the queue
                                zwnode_post_wkup_notif_handler(intf->ep->node);
                            }
                            else
                            {   //There are commands in the queue

                                //Already handled by zwnode_cmd_q_thrd()
                            }
                        }
                        break;

                    case WAKE_UP_INTERVAL_REPORT:
                        if (cmd_len >= 6)
                        {
                            zwif_wakeup_p      wake_up;
                            zwrep_wakeup_fn    rpt_cb;

                            wake_up = &intf->ep->node->net->wake_up;

                            //Clear the wake up setting
                            memset(wake_up, 0, sizeof(zwif_wakeup_t));

                            wake_up->cur =((uint32_t)(cmd_buf[2])) << 16
                                          | ((uint32_t)(cmd_buf[3])) << 8
                                          | cmd_buf[4];

                            wake_up->node.nodeid = cmd_buf[5];
                            wake_up->node.net = intf->ep->node->net;

                            //Save the interval
                            intf->ep->node->wkup_intv = wake_up->cur;

                            zwif_get_desc(intf, &ifd);
                            if (intf->ver >= 2)
                            {   //Get wake up interval capabilities report

                                //Get from cache first
                                if (intf->data_cnt)
                                {
                                    if_wkup_data_t  *wkup_dat;
                                    wkup_dat = (if_wkup_data_t *)intf->data;

                                    wake_up->min = wkup_dat->min;
                                    wake_up->max = wkup_dat->max;
                                    wake_up->def = wkup_dat->def;
                                    wake_up->interval = wkup_dat->interval;
                                }
                                else
                                {   //Get from the sleeping node
                                    zwif_get_report(&ifd, NULL, 0,
                                                    WAKE_UP_INTERVAL_CAPABILITIES_GET_V2, zwif_tx_sts_cb);
                                    break;
                                }
                            }

                            if (report_cb)
                            {
                                rpt_cb = (zwrep_wakeup_fn)report_cb;
                                //Callback the registered function
                                rpt_cb(&ifd, wake_up);
                            }

                        }
                        break;

                    case WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2:
                        if (cmd_len >= 14)
                        {
                            if_wkup_data_t     *wkup_dat;
                            zwif_wakeup_p      wake_up;
                            zwrep_wakeup_fn    rpt_cb;
                            rpt_cb = (zwrep_wakeup_fn)report_cb;

                            wake_up = &intf->ep->node->net->wake_up;

                            wake_up->min =((uint32_t)(cmd_buf[2])) << 16
                                          | ((uint32_t)(cmd_buf[3])) << 8
                                          | cmd_buf[4];

                            wake_up->max =((uint32_t)(cmd_buf[5])) << 16
                                          | ((uint32_t)(cmd_buf[6])) << 8
                                          | cmd_buf[7];

                            wake_up->def =((uint32_t)(cmd_buf[8])) << 16
                                          | ((uint32_t)(cmd_buf[9])) << 8
                                          | cmd_buf[10];

                            wake_up->interval =((uint32_t)(cmd_buf[11])) << 16
                                               | ((uint32_t)(cmd_buf[12])) << 8
                                               | cmd_buf[13];

                            //Cache the capabilities
                            if (wake_up->max)
                            {
                                //Free any existing interface data
                                if (intf->data_cnt > 0)
                                {
                                    free(intf->data);
                                }

                                intf->data_cnt = 1;
                                intf->data_item_sz = sizeof(if_wkup_data_t);

                                wkup_dat = (if_wkup_data_t *)malloc(intf->data_item_sz);

                                if (wkup_dat)
                                {
                                    wkup_dat->min = wake_up->min;
                                    wkup_dat->max = wake_up->max;
                                    wkup_dat->def = wake_up->def;
                                    wkup_dat->interval = wake_up->interval;

                                    //Assign to interface
                                    intf->data = wkup_dat;
                                }
                                else
                                {
                                    intf->data_cnt = 0;
                                }
                            }

                            if (report_cb)
                            {
                                zwif_get_desc(intf, &ifd);
                                //Callback the registered function
                                rpt_cb(&ifd, wake_up);
                            }
                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_METER_TBL_MONITOR:
            {
                switch (cmd_buf[1])
                {
                    case METER_TBL_TABLE_ID_REPORT:
                        if (cmd_len >= 3)
                        {
                            zwmeter_p   meterd;     //Meter descriptor
                            uint8_t     str_len;    //Character string length


                            meterd = &intf->ep->node->net->meter_desc;

                            //Clear the meter descriptor setting
                            memset(meterd, 0, sizeof(zwmeter_t));

                            //Check whether admin number can be set
                            if (zwif_find_cls(intf->ep->intf, COMMAND_CLASS_METER_TBL_CONFIG))
                            {
                                meterd->caps = ZW_METER_CAP_CFG;
                            }
                            meterd->caps |= ZW_METER_CAP_MON;


                            str_len = cmd_buf[2] & 0x1F;

                            if (str_len > 0)
                            {
                                if (str_len > ZW_ID_STR_MAX)
                                {
                                    str_len = ZW_ID_STR_MAX;
                                }
                                memcpy(meterd->id, cmd_buf + 3, str_len);
                            }

                            //Get Admin number report
                            zwif_get_desc(intf, &ifd);
                            zwif_get_report(&ifd, NULL, 0,
                                            METER_TBL_TABLE_POINT_ADM_NO_GET, zwif_tx_sts_cb);

                        }
                        break;

                    case METER_TBL_TABLE_POINT_ADM_NO_REPORT:
                        if (cmd_len >= 3)
                        {
                            zwmeter_p           if_meter_data;
                            zwmeter_p           meter_desc;
                            zwrep_meterd_fn     rpt_cb;
                            uint8_t             str_len;    //Character string length

                            meter_desc = &intf->ep->node->net->meter_desc;

                            str_len = cmd_buf[2] & 0x1F;

                            if (str_len > 0)
                            {
                                if (str_len > ZW_ADMIN_STR_MAX)
                                {
                                    str_len = ZW_ADMIN_STR_MAX;
                                }

                                if ((str_len + 3) > cmd_len)
                                {
                                    str_len = cmd_len - 3;
                                }

                                memcpy(meter_desc->admin, cmd_buf + 3, str_len);
                            }

                            //Save to cache
                            if (intf->data_cnt == 0)
                            {
                                //Allocate memory for data
                                if_meter_data = (zwmeter_t *)malloc(sizeof(zwmeter_t));
                            }
                            else
                            {   //Use previously allocated memory
                                if_meter_data = (zwmeter_t *)intf->data;
                            }

                            if (if_meter_data)
                            {
                                intf->data_cnt = 1;
                                intf->data_item_sz = sizeof(zwmeter_t);
                                intf->data = if_meter_data;
                                *if_meter_data = *meter_desc;
                            }

                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_meterd_fn)sm_rpt_cb;
                                rpt_cb(&ifd, meter_desc, 1);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_meterd_fn)report_cb;
                                rpt_cb(&ifd, meter_desc, 1);
                            }

                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_METER:
            {
                if (cmd_buf[1] == METER_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        time_t              time_stamp;
                        zwmeter_dat_t       meter_value = {0};
                        zwrep_meter_fn      rpt_cb;

                        meter_value.type = cmd_buf[2] & 0x1F;
                        meter_value.precision = cmd_buf[3] >> 5;
                        meter_value.unit = (cmd_buf[3] >> 3) & 0x03;
                        meter_value.size = cmd_buf[3] & 0x07;

                        if (meter_value.size > 4)
                        {
                            return 1;
                        }

                        if (cmd_len < (meter_value.size + 4))
                        {
                            break;
                        }
                        memcpy(meter_value.data, cmd_buf + 4, meter_value.size);

                        //Check if version 2 and above meter command class supported
                        if (intf->ver >= 2)
                        {
                            if (intf->ver >= 3)
                            {
                                meter_value.unit += ((cmd_buf[2] & 0x80) >> 5);
                            }

                            meter_value.rate_type = (cmd_buf[2] & 0x60) >> 5;

                            //Check whether delta time field is present
                            if ((cmd_len - 4 - meter_value.size) >= 2)
                            {
                                meter_value.delta_time = (((uint16_t)cmd_buf[4 + meter_value.size]) << 8)
                                                         | (uint16_t)cmd_buf[5 + meter_value.size];
                            }

                            //Check whether previous meter value field is present
                            if (meter_value.delta_time > 0)
                            {
                                if ((cmd_len - 6 - meter_value.size) >= meter_value.size)
                                {
                                    memcpy(meter_value.prv_data, cmd_buf + 6 + meter_value.size, meter_value.size);
                                }
                            }
                        }

                        util_time_stamp(&time_stamp);

                        if (!intf->cch_data)
                        {
                            //No cached report, cache it.
                            cache_chg = 1;
                            zwif_dat_cache_alloc(intf, sizeof(zwmeter_dat_t), FALSE);
                            if (intf->cch_data && intf->cch_dat_cnt)
                            {   //Update cache
                                memcpy(intf->cch_data, &time_stamp, sizeof(time_t));
                                memcpy(((time_t *)intf->cch_data) + 1, &meter_value, sizeof(meter_value));
                            }
                        }
                        else
                        {   //Got at least one cached report
                            cache_chg = zwif_meter_cache_updt(intf, &meter_value, &time_stamp);
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_meter_fn)sm_rpt_cb;
                            rpt_cb(&ifd, &meter_value, time_stamp);
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_meter_fn)report_cb;
                            rpt_cb(&ifd, &meter_value, time_stamp);
                        }
                    }
                }
                else if (cmd_buf[1] == METER_SUPPORTED_REPORT_V2)
                {
                    if (cmd_len >= 4)
                    {
                        zwmeter_cap_p           if_meter_data;
                        zwrep_meter_sup_fn      rpt_cb;
                        zwmeter_cap_t           meter_cap;

                        meter_cap.reset_cap = cmd_buf[2] >> 7;
                        meter_cap.type = cmd_buf[2] & 0x1F;
                        meter_cap.unit_sup = cmd_buf[3];

                        //Save to cache
                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            if_meter_data = (zwmeter_cap_t *)malloc(sizeof(zwmeter_cap_t));
                        }
                        else
                        {   //Use previously allocated memory
                            if_meter_data = (zwmeter_cap_t *)intf->data;
                        }

                        if (if_meter_data)
                        {
                            intf->data_cnt = 1;
                            intf->data_item_sz = sizeof(zwmeter_cap_t);
                            intf->data = if_meter_data;
                            *if_meter_data = meter_cap;
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_meter_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, &meter_cap, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_meter_sup_fn)report_cb;
                            rpt_cb(&ifd, &meter_cap, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_METER_PULSE:
            if (report_cb)
            {
                if (cmd_buf[1] == METER_PULSE_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwrep_pulsemeter_fn    rpt_cb;
                        uint32_t               pulse_cnt;


                        //Convert byte stream into 32-bit unsigned integer
                        pulse_cnt = ((uint32_t)cmd_buf[2]) << 24
                                    | ((uint32_t)cmd_buf[3]) << 16
                                    | ((uint32_t)cmd_buf[4]) << 8
                                    | cmd_buf[5];

                        rpt_cb = (zwrep_pulsemeter_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, pulse_cnt);
                    }
                }
            }
            break;

        case COMMAND_CLASS_SIMPLE_AV_CONTROL:
            {
                switch (cmd_buf[1])
                {
                    case SIMPLE_AV_CONTROL_REPORT:
                        if (cmd_len >= 3)
                        {
                            uint8_t             rpt_num;
                            zwif_av_cap_t       *av_cap;

                            if (single_ep_sm_sts && (sm_rpt_cb == zwnet_av_raw_rpt_cb))
                            {
                                zwnet_av_raw_rpt_cb(intf, cmd_len, cmd_buf);
                                break;
                            }

                            av_cap = &intf->ep->node->net->av_cap;

                            //Initialize setting
                            if (av_cap->bit_mask_buf)
                            {
                                free(av_cap->bit_mask_buf);
                            }
                            memset(av_cap, 0, sizeof(zwif_av_cap_t));


                            //Save the total number of reports
                            av_cap->total_rpt = cmd_buf[2];

                            //Get the first report
                            zwif_get_desc(intf, &ifd);
                            rpt_num = 1;
                            zwif_get_report(&ifd, &rpt_num, 1,
                                            SIMPLE_AV_CONTROL_SUPPORTED_GET, zwif_tx_sts_cb);

                        }
                        break;

                    case SIMPLE_AV_CONTROL_SUPPORTED_REPORT:
                        if (cmd_len >= 3)
                        {
                            zwif_av_cap_t       *av_cap;
                            uint8_t             rpt_num;        //Report number of this report
                            uint16_t            bit_mask_len;   //Bit mask length of this report
                            uint8_t             *tmp_buf;

                            //Check whether to callback to single ep state-machine
                            if (single_ep_sm_sts && (sm_rpt_cb == zwnet_av_raw_rpt_cb))
                            {
                                zwnet_av_raw_rpt_cb(intf, cmd_len, cmd_buf);
                                break;
                            }

                            av_cap = &intf->ep->node->net->av_cap;

                            rpt_num = cmd_buf[2];
                            bit_mask_len = cmd_len - 3;

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
                                       cmd_buf + 3, bit_mask_len);

                                av_cap->rpt_num = rpt_num;
                                av_cap->bit_mask_len += bit_mask_len;

                            }

                            zwif_get_desc(intf, &ifd);

                            //Check whether this is the last report
                            if (av_cap->total_rpt == rpt_num)
                            {
                                //Last report
                                zwrep_av_fn         rpt_cb;

                                rpt_cb = (zwrep_av_fn)report_cb;

                                //Save to cache
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

                                //Callback the registered function
                                if (report_cb)
                                {
                                    rpt_cb(&ifd, av_cap->bit_mask_len, av_cap->bit_mask_buf, 1);
                                }

                                //Free memory
                                free(av_cap->bit_mask_buf);
                                av_cap->bit_mask_buf = NULL;

                            }
                            else
                            {
                                //There is at least one report pending, get next report
                                rpt_num = av_cap->rpt_num + 1;
                                zwif_get_report(&ifd, &rpt_num, 1,
                                                SIMPLE_AV_CONTROL_SUPPORTED_GET, zwif_tx_sts_cb);
                            }

                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
            if (report_cb)
            {
                if (cmd_buf[1] == MANUFACTURER_SPECIFIC_REPORT)
                {
                    if (cmd_len >= 8)
                    {
                        uint16_t        manf_pdt_ids[3];
                        zwrep_manf_fn   rpt_cb;

                        rpt_cb = (zwrep_manf_fn)report_cb;

                        manf_pdt_ids[0] = ((uint16_t)cmd_buf[2]) << 8 | cmd_buf[3];
                        manf_pdt_ids[1] = ((uint16_t)cmd_buf[4]) << 8 | cmd_buf[5];
                        manf_pdt_ids[2] = ((uint16_t)cmd_buf[6]) << 8 | cmd_buf[7];

                        //Callback the registered function
                        rpt_cb(intf, manf_pdt_ids);
                    }
                }
                else if (cmd_buf[1] == DEVICE_SPECIFIC_REPORT_V2)
                {
                    if (cmd_len >= 4)
                    {
                        dev_id_t        dev_id = {0};
                        zwrep_devid_fn  rpt_cb;

                        rpt_cb = (zwrep_devid_fn)report_cb;

                        dev_id.type = cmd_buf[2] & 0x07;
                        dev_id.format = cmd_buf[3] >> 5;
                        dev_id.len = cmd_buf[3] & 0x1F;

                        if (cmd_len >= (4 + dev_id.len))
                        {
                            memcpy(dev_id.dev_id, cmd_buf + 4, dev_id.len);
                            //Callback the registered function
                            rpt_cb(intf, &dev_id);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_CONFIGURATION:
            {
                if (cmd_buf[1] == CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwconfig_t         param;
                        zwrep_config_fn    rpt_cb;

                        param.param_num = cmd_buf[2];
                        param.size = cmd_buf[3] & 0x07;

                        //Check parameter size
                        if (memchr(data_storage_sz, param.size, sizeof(data_storage_sz)) == NULL)
                        {
                            return 1;
                        }

                        if (cmd_len >= (4 + param.size))
                        {
                            memcpy(param.data, cmd_buf + 4, param.size);

                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_config_fn)sm_rpt_cb;
                                rpt_cb(&ifd, &param);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_config_fn)report_cb;
                                rpt_cb(&ifd, &param);
                            }
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK:
            {
                if (cmd_buf[1] == DOOR_LOCK_OPERATION_REPORT)
                {
                    if (cmd_len >= 7)
                    {
                        zwdlck_op_t         op_status;
                        zwdlck_op_t         cch_op_status;
                        zwrep_dlck_op_fn    rpt_cb;

                        op_status.mode = cmd_buf[2];
                        op_status.out_mode = cmd_buf[3] >> 4;
                        op_status.in_mode = cmd_buf[3] & 0x0F;
                        op_status.cond = cmd_buf[4];
                        op_status.tmout_min = cmd_buf[5];
                        op_status.tmout_sec = cmd_buf[6];

                        if (!intf->cch_data)
                        {
                            cache_chg = 1;
                            zwif_dat_cache_alloc(intf, sizeof(if_dlck_cch_data_t), FALSE);
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&cch_op_status, tm_stamp_p + 1, sizeof(op_status));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (memcmp(&cch_op_status, &op_status, sizeof(op_status)) != 0)
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &op_status, sizeof(op_status));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_dlck_op_fn)sm_rpt_cb;
                            rpt_cb(&ifd, &op_status, (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_dlck_op_fn)report_cb;
                            rpt_cb(&ifd, &op_status, (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
                else if (cmd_buf[1] == DOOR_LOCK_CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwdlck_cfg_t        config;
                        zwrep_dlck_cfg_fn   rpt_cb;
                        if_dlck_cch_data_t  *dlck_cch = NULL;

                        config.type = cmd_buf[2];
                        config.out_sta = cmd_buf[3] >> 4;
                        config.in_sta = cmd_buf[3] & 0x0F;
                        config.tmout_min = cmd_buf[4];
                        config.tmout_sec = cmd_buf[5];

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(if_dlck_cch_data_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            dlck_cch = (if_dlck_cch_data_t *)(((time_t *)intf->cch_data) + 1);

                            util_time_stamp(&dlck_cch->cfg_ts);

                            if (memcmp(&dlck_cch->cfg, &config, sizeof(config)) != 0)
                            {   //Update cache
                                cache_chg = 1;
                                dlck_cch->cfg = config;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_dlck_cfg_fn)sm_rpt_cb;
                            rpt_cb(&ifd, &config, (dlck_cch)? dlck_cch->cfg_ts : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_dlck_cfg_fn)report_cb;
                            rpt_cb(&ifd, &config, (dlck_cch)? dlck_cch->cfg_ts : util_time_get());
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_USER_CODE:
            {
                if (cmd_buf[1] == USER_CODE_REPORT)
                {
                    if (cmd_len >= 4)//Allow 0 length user code
                    {
                        zwusrcod_t          usr_cod;
                        zwrep_usr_cod_fn    rpt_cb;

                        usr_cod.id = cmd_buf[2];
                        usr_cod.id_sts = cmd_buf[3];
                        usr_cod.code_len = cmd_len - 4;

                        if (usr_cod.code_len > MAX_USRCOD_LENGTH)
                        {   //Maximum allowed user code length exceeded
                            break;
                        }

                        memcpy(usr_cod.code, cmd_buf + 4, usr_cod.code_len);

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_usr_cod_fn)sm_rpt_cb;
                            rpt_cb(&ifd, &usr_cod);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_usr_cod_fn)report_cb;
                            rpt_cb(&ifd, &usr_cod);
                        }
                    }
                }
                else if (cmd_buf[1] == USERS_NUMBER_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        uint8_t             *max_usr_codes;
                        zwrep_usr_sup_fn    rpt_cb;

                        //Save to cache
                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            max_usr_codes = (uint8_t *)malloc(1);
                        }
                        else
                        {   //Use previously allocated memory
                            max_usr_codes = (uint8_t *)intf->data;
                        }

                        if (max_usr_codes)
                        {
                            intf->data_cnt = intf->data_item_sz = 1;
                            intf->data = max_usr_codes;
                            *max_usr_codes = cmd_buf[2];
                        }


                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_usr_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_usr_sup_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ALARM:
            {
                if (cmd_buf[1] == ALARM_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        time_t              time_stamp;
                        zwalrm_p            alrm_info;
                        zwrep_alrm_fn       rpt_cb = NULL;
                        zwif_p              dlck_intf = NULL;

                        alrm_info = (zwalrm_p)calloc(1, sizeof(zwalrm_t) + cmd_len);

                        if (!alrm_info)
                        {
                            return 1;
                        }

                        alrm_info->type = cmd_buf[2];
                        alrm_info->level = cmd_buf[3];

                        if (cmd_len == 4)
                        {   //Version 1
                            //Check for mappings to Z-wave alarm type and event
							zwif_alrm_rec_convert(intf, &alrm_info);

       //                     zwnode_p        node = intf->ep->node;
       //                     zwnet_p         nw = node->net;
							//if_rec_alarm_match_t  *alarm_rec;
       //                     int             bLevelMatched = 0;

       //                     plt_mtx_lck(nw->mtx);
       //                     //Check whether device configuration record has vendor specific alarm types
       //                     if (node->dev_cfg_valid)
       //                     {
							//	alarm_rec = (if_rec_alarm_match_t *)zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_ALARM);

       //                         while (alarm_rec)
       //                         {
       //                             if ((alarm_rec->type == alrm_info->type)
       //                                 && (alarm_rec->zw_alrm != 0))
       //                             {   //Found alarm type
       //                                 alrm_info->ex_info = 1;
       //                                 alrm_info->ex_status = ZW_ALRM_STS_ACTIVATED;
       //                                 alrm_info->ex_type = alarm_rec->zw_alrm;

       //                                 //Find event mapping
       //                                 for (i=0; i<alarm_rec->level_cnt; i++)
       //                                 {
       //                                     if ((alarm_rec->level[i].isRange == 0) &&
       //                                         (alarm_rec->level[i].level_low == alrm_info->level))
       //                                         bLevelMatched = 1;

       //                                     else if ((alarm_rec->level[i].isRange == 1) &&
       //                                              (alarm_rec->level[i].level_low <= alrm_info->level) &&
       //                                              (alarm_rec->level[i].level_high >= alrm_info->level))
       //                                         bLevelMatched = 1;

       //                                     if ((bLevelMatched == 1)
       //                                         && (alarm_rec->level[i].zw_evt != 0))
       //                                     {   //Found event
       //                                         alrm_info->ex_event = alarm_rec->level[i].zw_evt;
       //                                         break;
       //                                     }
       //                                 }

       //                                 if (i >= alarm_rec->level_cnt)
       //                                 {   //No event found
       //                                     alrm_info->ex_event = ZW_ALRM_EVT_UNKNOWN;
       //                                 }
       //                                 break;
       //                             }
       //                             alarm_rec = alarm_rec->next;
       //                         }
       //                     }
       //                     plt_mtx_ulck(nw->mtx);
                        }
                        else if ((cmd_len >= 6) && (cmd_len < 8))
                        {   //To handle the case:
                            //If Notification Status = 0xFE, no further fields will be appended to the report,
                            //thus the report command is structured as Notification Report (V1 Alarm Type = 0,
                            //V1 Alarm Level = 0, Zensor Net Source Node ID = 0, Notification Status = 0xFE).
                            alrm_info->ex_info = 1;
                            alrm_info->ex_zensr_nodeid = cmd_buf[4];
                            alrm_info->ex_status = cmd_buf[5];

							zwif_alrm_rec_convert(intf, &alrm_info);
                        }
                        else if (cmd_len >= 8)
                        {   //version 2 and above
                            uint8_t param_len;

                            alrm_info->ex_info = 1;
                            alrm_info->ex_zensr_nodeid = cmd_buf[4];
                            alrm_info->ex_status = cmd_buf[5];
                            alrm_info->ex_type = cmd_buf[6];
                            alrm_info->ex_event = cmd_buf[7];
                            alrm_info->ex_has_sequence = (cmd_buf[8] & 0x80) >> 7;

                            param_len = cmd_buf[8] & 0x1F;
                            //Check whether event comes with parameter
                            if ((cmd_len >= 9) && (param_len > 0))
                            {
                                if (cmd_len < (9 + param_len))
                                {
                                    free(alrm_info);
                                    return 1;
                                }

                                //Check for system failure event parameters
                                if ((cmd_buf[6] == ZW_ALRM_SYSTEM)
                                    && ((cmd_buf[7] == ZW_ALRM_EVT_HW_OEM_CODE) || (cmd_buf[7] == ZW_ALRM_EVT_SW_OEM_CODE)))
                                {

                                    memcpy(alrm_info->ex_evt_prm, cmd_buf + 9, param_len);
                                    alrm_info->ex_evt_len = param_len;
                                    alrm_info->ex_evt_type = ZW_ALRM_PARAM_OEM_ERR_CODE;
                                }
                                else if (param_len >= 3)
                                {
                                    if ((cmd_buf[6] == ZW_ALRM_LOCK) &&
                                        ((cmd_buf[7] == ZW_ALRM_EVT_KEYPAD_LCK) || (cmd_buf[7] == ZW_ALRM_EVT_KEYPAD_ULCK)))
                                    {
                                        if (cmd_buf[9] == COMMAND_CLASS_USER_CODE &&
                                            cmd_buf[10] == USER_CODE_REPORT)
                                        {
                                            //cmd_buf[9] = COMMAND_CLASS_USER_CODE
                                            //cmd_buf[10] = USER_CODE_REPORT
                                            //cmd_buf[11] = User Identifier
                                            //cmd_buf[12] = User ID Status
                                            alrm_info->ex_evt_type = ZW_ALRM_PARAM_USRID;
                                            alrm_info->ex_evt_len = 1;
                                            alrm_info->ex_evt_prm[0] = cmd_buf[11];
                                        }
                                    }
                                    else if (zwif_alrm_evt_param_loc(cmd_buf[6], cmd_buf[7]))
                                    {
                                        if (cmd_buf[9] == COMMAND_CLASS_NODE_NAMING &&
                                            cmd_buf[10] == NODE_NAMING_NODE_LOCATION_REPORT)
                                        {
                                            //cmd_buf[9] = COMMAND_CLASS_NODE_NAMING
                                            //cmd_buf[10] = NODE_NAMING_NODE_LOCATION_REPORT
                                            //cmd_buf[11] = Char. Presentation
                                            //cmd_buf[12] = Node location char 1
                                            uint8_t str_len = param_len - 3;

                                            //Check whether to convert into utf8
                                            if ((cmd_buf[11] & 0x07) == CHAR_PRES_UTF16)
                                            {
                                                uint8_t *utf16_buf;
                                                uint8_t utf8_str[ZW_LOC_STR_MAX + 1];
                                                int     result;

                                                utf16_buf = (uint8_t *)calloc(1, str_len + 2);//additional 2 bytes for null characters
                                                if (!utf16_buf)
                                                {
                                                    free(alrm_info);
                                                    return 1;
                                                }
                                                memcpy(utf16_buf, cmd_buf + 12, str_len);

                                                //convert into utf8
#if defined(_WINDOWS) || defined(WIN32)
                                                result = plt_utf16_to_8((const char *)utf16_buf, utf8_str, ZW_LOC_STR_MAX + 1, 1);
#else
                                                result = plt_utf16_to_8((const uint16_t *)utf16_buf, utf8_str, ZW_LOC_STR_MAX + 1, 1);
#endif

                                                free(utf16_buf);
                                                if (result == 0)
                                                {
                                                    size_t utf8_len;

                                                    utf8_len = strlen((const char *)utf8_str);

                                                    if (utf8_len > cmd_len)
                                                    {   //Not enough memory, have to realloc
                                                        zwalrm_p  tmp_buf;
                                                        tmp_buf = (zwalrm_p) realloc(alrm_info, sizeof(zwalrm_t) + utf8_len);
                                                        if (tmp_buf)
                                                        {
                                                            alrm_info = tmp_buf;
                                                        }
                                                        else
                                                        {
                                                            free(alrm_info);
                                                            return 1;
                                                        }
                                                    }

                                                    //Copy node location
                                                    memcpy(alrm_info->ex_evt_prm, utf8_str, utf8_len);
                                                    alrm_info->ex_evt_prm[utf8_len] = '\0';
                                                    alrm_info->ex_evt_len = utf8_len + 1; //include NULL char

                                                }
                                            }
                                            else
                                            {
                                                //Check for valid UTF-8 string
                                                str_len = plt_utf8_chk(cmd_buf + 12, str_len);

                                                //Copy node location to the node
                                                memcpy(alrm_info->ex_evt_prm, cmd_buf + 12, str_len);
                                                alrm_info->ex_evt_len = str_len + 1; //include NULL char

                                            }
                                            alrm_info->ex_evt_type = ZW_ALRM_PARAM_LOC;

                                        }
                                    }
                                    else if ((cmd_buf[6] == ZW_ALRM_CLOCK) && (cmd_buf[7] == ZW_ALRM_EVT_TIME_REMAINING))
                                    {
                                        //Event Parm 1 = hour(s) Event Parm 2 = minute(s) Event Parm 3 = second(s)
                                        //Cannot be listed in NIF, MUST be described in product manual.
                                        memcpy(alrm_info->ex_evt_prm, cmd_buf + 9, param_len);
                                        alrm_info->ex_evt_len = param_len;
                                        alrm_info->ex_evt_type = ZW_ALRM_PARAM_PROPRIETARY;
                                    }
                                }
                                else if (param_len == 1)
                                {
                                    if (zwif_alrm_evt_param_prop(cmd_buf[6], cmd_buf[7]))
                                    {
                                        memcpy(alrm_info->ex_evt_prm, cmd_buf + 9, param_len);
                                        alrm_info->ex_evt_len = param_len;
                                        alrm_info->ex_evt_type = ZW_ALRM_PARAM_PROPRIETARY;
                                    }
                                }

                                //If after going through the CC spec param list and the param is still not found,
                                //it means the param is not defined in the CC spec, use ZW_ALRM_PARAM_UNKNOWN
                                if (alrm_info->ex_evt_type == 0)
                                {
                                    memcpy(alrm_info->ex_evt_prm, cmd_buf + 9, param_len);
                                    alrm_info->ex_evt_len = param_len;
                                    alrm_info->ex_evt_type = ZW_ALRM_PARAM_UNKNOWN;
                                }
                            }

                            //Check sequence number
                            if ((alrm_info->ex_has_sequence != 0) && (param_len + 9 < cmd_len))
                            {
                                alrm_info->ex_sequence_no = cmd_buf[cmd_len - 1];
                            }

							zwif_alrm_rec_convert(intf, &alrm_info);
                        }

                        util_time_stamp(&time_stamp);

                        //For v1, always cache
                        //For v2 or above, if notification type != 0, notification event != 0 or 0xFE (unknown event), cache the report
                        //As we do not have an application that needs cached data for alarm CC, we are not sure how the application expects
                        //the data to be, until we have such case, the caching may need to change.
                        //eg. if "no more alarm pending" response (status 0xFE, without type and event) is also needed to be cached, then
                        //the Get command itself may need to be cached as well, otherwise no way to do comparison for type/event based on the
                        //response from the device.

                        //Remove checking of notification events. i.e. even if the event is 0 or 0xFE, we also cache it.
                        //This is because we have a v2 vision smoke detector which sends 0xFE (unknown event) in a valid
                        //alarm report when triggered
                        //if((intf->ver == 1) ||
                        //	(alrm_info->ex_type != 0 && alrm_info->ex_event != 0 && alrm_info->ex_event != 0xFE))

                        if ((intf->ver == 1) ||(alrm_info->ex_type != 0))
                        {
                            int alrm_info_size;
                            alrm_info_size = sizeof(zwalrm_t) +
                                        ((alrm_info->ex_info && alrm_info->ex_evt_len)? alrm_info->ex_evt_len : 0);

                            if (!intf->cch_data)
                            {   //No cached report, cache it.
                                //Cached alarm report is per alarm type, per alarm event and variable length.
                                //Will use 2 level cache data format
                                zwif_dat_cache_alloc(intf, alrm_info_size, TRUE);
                                if (intf->cch_data && intf->cch_dat_cnt && ((cch_1lvl_t *)intf->cch_data)->pcch_ele)
                                {
                                    cch_1lvl_t *p1lvl = (cch_1lvl_t *)intf->cch_data;

                                    //Update cache
                                    memcpy(p1lvl->pcch_ele, &time_stamp, sizeof(time_t));
                                    memcpy(((time_t *)p1lvl->pcch_ele) + 1, alrm_info, alrm_info_size);
                                }
                            }
                            else
                            {   //Got at least one cached report
                                zwif_alrm_cache_updt(intf, alrm_info, alrm_info_size, &time_stamp);
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_alrm_fn)sm_rpt_cb;
                            rpt_cb(&ifd, alrm_info, time_stamp);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_alrm_fn)report_cb;
                            rpt_cb(&ifd, alrm_info, time_stamp);
                        }

#ifdef ALARM_2_DLOCK_RPT_CONVERSION
                        //If the same endpoint has both doorlock and alarm CC, convert alarm report
                        //type Access control (0x06) manual lock, manual unlock, RF lock, RF Unlock,
                        //Keypad lock, Keypad unlock to appropiate Doorlock report
                        dlck_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_DOOR_LOCK);

                        if (dlck_intf != NULL)
                        {
                            if (alrm_info->ex_type == ZW_ALRM_LOCK) //0x06
                            {
                                uint8_t dlck_cmd_buf[20];
                                uint16_t dlck_cmd_len = 0;

                                memset(dlck_cmd_buf, 0, 20);

                                if (dlck_intf->ver <= 2)
                                    dlck_cmd_len = 7;
                                else if (dlck_intf->ver >= 3)
                                    dlck_cmd_len = 9;

                                dlck_cmd_buf[0] = COMMAND_CLASS_DOOR_LOCK;
                                dlck_cmd_buf[1] = DOOR_LOCK_OPERATION_REPORT;

                                dlck_cmd_buf[5] = 0xFE; //Lock Time out minutes
                                dlck_cmd_buf[6] = 0xFE; //Lock Time out seconds

                                if (alrm_info->ex_event == ZW_ALRM_EVT_MANUAL_LCK ||
                                    alrm_info->ex_event == ZW_ALRM_EVT_RF_LCK ||
                                    alrm_info->ex_event == ZW_ALRM_EVT_KEYPAD_LCK)
                                {
                                    //Fabricate a doorlock report
                                    dlck_cmd_buf[2] = ZW_DOOR_SEC;
                                    zwif_rep_hdlr(dlck_intf, dlck_cmd_buf, dlck_cmd_len, rx_sts);
                                }
                                else if (alrm_info->ex_event == ZW_ALRM_EVT_MANUAL_ULCK ||
                                         alrm_info->ex_event == ZW_ALRM_EVT_RF_ULCK ||
                                         alrm_info->ex_event == ZW_ALRM_EVT_KEYPAD_ULCK)
                                {
                                    //Fabricate a doorlock report
                                    dlck_cmd_buf[2] = ZW_DOOR_UNSEC;
                                    dlck_cmd_buf[4] = 0x02; //Door condition
                                    zwif_rep_hdlr(dlck_intf, dlck_cmd_buf, dlck_cmd_len, rx_sts);
                                }

                            }
                        }
#endif
                        free(alrm_info);
                    }
                }
                else if (cmd_buf[1] == ALARM_TYPE_SUPPORTED_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
						zwnode_p			node;
						if_rec_alarm_match_t *alarm_rec;
                        if_alarm_data_t     *alarm_dat;
                        zwrep_alrm_sup_fn   rpt_cb;
                        uint8_t             ztype[248];
                        int                 i;
                        int                 max_alrm_type;
                        uint8_t             ztype_len;
                        uint8_t             bitmsk_len;

                        ztype_len = 0;
                        bitmsk_len = (cmd_buf[2] & 0x1F);

                        if (cmd_len >= (3 + bitmsk_len))
                        {
                            max_alrm_type = bitmsk_len * 8;

                            for (i=0; i < max_alrm_type; i++)
                            {
                                if ((cmd_buf[(i>>3) + 3] >> (i & 0x07)) & 0x01)
                                {
                                    ztype[ztype_len++] = i;
                                }
                            }

							//Go through database to add the supported types in DB as well
							node = intf->ep->node;
							if (node->dev_cfg_valid)
							{
								alarm_rec = zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_ALARM);

								if (alarm_rec)
								{
									while (alarm_rec && (ztype_len < 248))
									{
										//go through both match block and result block to return all the types
										if (alarm_rec->ex_type != -1)
										{
											UNIQUELY_ADD_TO_ARRAY(ztype, alarm_rec->ex_type, ztype_len)
										}
										if (alarm_rec->pResult->ex_type != -1)
										{
											UNIQUELY_ADD_TO_ARRAY(ztype, alarm_rec->pResult->ex_type, ztype_len)
										}
										alarm_rec = alarm_rec->next;
									}
								}

							}
                            //Save to cache

                            intf->data_item_sz = sizeof(if_alarm_data_t) + ((ztype_len - 1)*sizeof(if_alarm_type_evt_t));

                            if (intf->data_cnt == 0)
                            {
                                //Allocate memory for data
                                alarm_dat = (if_alarm_data_t *)calloc(1, intf->data_item_sz);
                            }
                            else
                            {   //Use previously allocated memory
                                alarm_dat = (if_alarm_data_t *)intf->data;
                            }

                            if (alarm_dat)
                            {
                                alarm_dat->have_vtype = cmd_buf[2] >> 7;
                                alarm_dat->type_evt_cnt = ztype_len;
                                for (i=0; i<alarm_dat->type_evt_cnt; i++)
                                {
                                    alarm_dat->type_evt[i].ztype = ztype[i];
                                }

                                //Assign to interface
                                intf->data = alarm_dat;
                                intf->data_cnt = 1;
                            }
                            else
                            {
                                intf->data_cnt = 0;
                            }

                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_alrm_sup_fn)sm_rpt_cb;
                                rpt_cb(&ifd, cmd_buf[2] >> 7, ztype_len, ztype, 1);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_alrm_sup_fn)report_cb;
                                rpt_cb(&ifd, cmd_buf[2] >> 7, ztype_len, ztype, 1);
                            }
                        }
                    }
                }
                else if (cmd_buf[1] == EVENT_SUPPORTED_REPORT_V3)
                {
                    if (cmd_len >= 4)
                    {
						zwnode_p			node;
						if_rec_alarm_match_t *alarm_rec;
                        zwrep_alrm_evt_fn   rpt_cb;
                        uint8_t             sup_evt[248];
                        int                 i;
                        int                 max_evt;
                        uint8_t             evt_len;

                        evt_len = cmd_buf[3] & 0x1F;
						//TODO: implement here!!!

                        //Allow event lenght of zero.  Spec states:
                        //If a device receives an un-supported Notification Type or Notification Type = 0xFF,
                        //the receiving device MUST respond with Event Supported Report command with the
                        //Notification Type specified in the Event Supported Get command and the
                        //Number of Bit Masks field set to 0.

                        //if (evt_len && (cmd_len >= (evt_len + 4)))
                        if (cmd_len >= (evt_len + 4))
                        {
                            //Save to cache
							if (intf->data_cnt)
							{
								if_alarm_data_t     *alarm_dat;
								uint8_t             evt_bitmask_len;

								alarm_dat = (if_alarm_data_t *)intf->data;

								//Find matching alarm type in cache
								for (i = 0; i < alarm_dat->type_evt_cnt; i++)
								{
									if (alarm_dat->type_evt[i].ztype == cmd_buf[2])
									{   //Found alarm type
										evt_bitmask_len = (evt_len > MAX_EVT_BITMASK_LEN) ? MAX_EVT_BITMASK_LEN : evt_len;
										memcpy(alarm_dat->type_evt[i].evt, cmd_buf + 4, evt_bitmask_len);
										alarm_dat->type_evt[i].evt_len = evt_bitmask_len;
										break;
									}
								}

								//Check database record, if alarm record exist, add the coresponding event in as well
								node = intf->ep->node;
								if (node->dev_cfg_valid)
								{
									alarm_rec = zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_ALARM);

									while (alarm_rec)
									{
										//go through both match block and result block to return all the events with the same type
										if ((alarm_rec->ex_type == cmd_buf[2]) &&
											(alarm_rec->ex_event != -1) &&
											(alarm_rec->ex_event != 0xFE))
										{
											UNIQUELY_ADD_TO_BITMASK(alarm_dat->type_evt[i].evt, alarm_rec->ex_event, 1, MAX_EVT_BITMASK_LEN)
										}
										if (alarm_rec->pResult->ex_type == cmd_buf[2] &&
											(alarm_rec->pResult->ex_event != -1) &&
											(alarm_rec->pResult->ex_event != 0xFE))
										{
											UNIQUELY_ADD_TO_BITMASK(alarm_dat->type_evt[i].evt, alarm_rec->pResult->ex_event, 1, MAX_EVT_BITMASK_LEN)
										}
										alarm_rec = alarm_rec->next;
									}
								}
							}

                            max_evt = evt_len * 8;
                            evt_len = 0;

                            for (i=0; i < max_evt; i++)
                            {
                                if ((cmd_buf[(i>>3) + 4] >> (i & 0x07)) & 0x01)
                                {
                                    sup_evt[evt_len++] = i;
                                }
                            }

                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_alrm_evt_fn)sm_rpt_cb;
                                rpt_cb(&ifd, cmd_buf[2], evt_len, sup_evt, 1);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_alrm_evt_fn)report_cb;
                                rpt_cb(&ifd, cmd_buf[2], evt_len, sup_evt, 1);
                            }
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_BATTERY:
            {
                if (cmd_buf[1] == BATTERY_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_ts_fn    rpt_cb;
                        uint8_t        lvl;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&lvl, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (lvl != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_ts_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_ts_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_MODE:
            {
                if (cmd_buf[1] == THERMOSTAT_FAN_MODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_fan_md_fn       rpt_cb;
                        uint8_t                     off;

                        if (intf->ver >= 3)
                        {
                            off = (cmd_buf[2] & 0x80)? 1 : 0;
                        }
                        else
                        {
                            off = 0;
                        }

                        //Mask out the reserved bits
                        cmd_buf[2] &= 0x0F;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint16_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            uint16_t fan_md_cch;
                            uint16_t cached_fan_md;

                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&cached_fan_md, tm_stamp_p + 1, sizeof(uint16_t));

                            util_time_stamp((time_t *)intf->cch_data);
                            fan_md_cch = off;   //high byte stores the "off"
                            fan_md_cch = (fan_md_cch << 8) | cmd_buf[2];//low byte stores the "fan mode"

                            if (cached_fan_md != fan_md_cch)
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &fan_md_cch, sizeof(uint16_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_fan_md_fn)sm_rpt_cb;
                            rpt_cb(&ifd, off, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_fan_md_fn)report_cb;
                            rpt_cb(&ifd, off, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
                else if (cmd_buf[1] == THERMOSTAT_FAN_MODE_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        if_thrmo_fan_mode_data_t    *thrmo_fan_mode_dat;
                        zwrep_thrmo_fan_md_sup_fn   rpt_cb;
                        uint8_t             fan_mod[248];
                        int                 i;
                        int                 max_fan_mod;
                        uint8_t             fan_mod_len;
                        uint8_t             off;

                        fan_mod_len = 0;
                        max_fan_mod = (cmd_len - 2) * 8;

                        for (i = 0; i < max_fan_mod; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                fan_mod[fan_mod_len++] = i;
                            }
                        }

                        off = (intf->ver >= 2)? 1 : 0;

                        //Save to cache
                        intf->data_item_sz = sizeof(if_thrmo_fan_mode_data_t) + fan_mod_len - 1;

                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            thrmo_fan_mode_dat = (if_thrmo_fan_mode_data_t *)malloc(intf->data_item_sz);
                        }
                        else
                        {   //Use previously allocated memory
                            thrmo_fan_mode_dat = (if_thrmo_fan_mode_data_t *)intf->data;
                        }

                        if (thrmo_fan_mode_dat)
                        {
                            thrmo_fan_mode_dat->off = off;
                            thrmo_fan_mode_dat->mode_len = fan_mod_len;
                            memcpy(thrmo_fan_mode_dat->mode, fan_mod, fan_mod_len);

                            //Assign to interface
                            intf->data = thrmo_fan_mode_dat;
                            intf->data_cnt = 1;
                        }
                        else
                        {
                            intf->data_cnt = 0;
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_fan_md_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, off, fan_mod_len, fan_mod, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_fan_md_sup_fn)report_cb;
                            rpt_cb(&ifd, off, fan_mod_len, fan_mod, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_STATE:
            {
                if (cmd_buf[1] == THERMOSTAT_FAN_STATE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_fan_sta_fn      rpt_cb;
                        uint8_t                     fan_sta;

                        //Mask out the reserved bits
                        cmd_buf[2] &= 0x0F;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&fan_sta, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (fan_sta != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_fan_sta_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_fan_sta_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_MODE:
            {
                if (cmd_buf[1] == THERMOSTAT_MODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_md_fn   rpt_cb;
                        uint8_t             thrmo_md;

                        //Mask out the reserved bits
                        cmd_buf[2] &= 0x1F;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&thrmo_md, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (thrmo_md != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_md_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_md_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
                else if (cmd_buf[1] == THERMOSTAT_MODE_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        if_thrmo_mode_data_t    *thrmo_mode_dat;
                        zwrep_thrmo_md_sup_fn   rpt_cb;
                        uint8_t             sup_mod[248];
                        int                 i;
                        int                 max_sup_mod;
                        uint8_t             sup_mod_len;

                        sup_mod_len = 0;
                        max_sup_mod = (cmd_len - 2) * 8;

                        for (i = 0; i < max_sup_mod; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                sup_mod[sup_mod_len++] = i;
                            }
                        }

                        //Save to cache
                        intf->data_item_sz = sizeof(if_thrmo_mode_data_t) + sup_mod_len - 1;

                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            thrmo_mode_dat = (if_thrmo_mode_data_t *)malloc(intf->data_item_sz);
                        }
                        else
                        {   //Use previously allocated memory
                            thrmo_mode_dat = (if_thrmo_mode_data_t *)intf->data;
                        }

                        if (thrmo_mode_dat)
                        {
                            thrmo_mode_dat->mode_len = sup_mod_len;
                            memcpy(thrmo_mode_dat->mode, sup_mod, sup_mod_len);

                            //Assign to interface
                            intf->data = thrmo_mode_dat;
                            intf->data_cnt = 1;
                        }
                        else
                        {
                            intf->data_cnt = 0;
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_md_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, sup_mod_len, sup_mod, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_md_sup_fn)report_cb;
                            rpt_cb(&ifd, sup_mod_len, sup_mod, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:
            {
                if (cmd_buf[1] == THERMOSTAT_OPERATING_STATE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_op_sta_fn   rpt_cb;
                        uint8_t                 op_sta;

                        if (!intf->cch_data)
                        {
                            zwif_dat_cache_alloc(intf, sizeof(uint8_t), FALSE);
                            cache_chg = 1;
                        }

                        //Mask out the reserved bits
                        cmd_buf[2] &= 0x0F;

                        if (intf->cch_data && intf->cch_dat_cnt)
                        {
                            //Check whether the cache is stale
                            tm_stamp_p = (time_t *)intf->cch_data;
                            memcpy(&op_sta, tm_stamp_p + 1, sizeof(uint8_t));

                            util_time_stamp((time_t *)intf->cch_data);

                            if (op_sta != cmd_buf[2])
                            {   //Update cache
                                cache_chg = 1;
                                memcpy(((time_t *)intf->cch_data) + 1, &cmd_buf[2], sizeof(uint8_t));
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_op_sta_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }

                        if (report_cb && cache_chg)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_op_sta_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (intf->cch_data)? (*(time_t *)intf->cch_data) : util_time_get());
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETBACK:
            if (report_cb)
            {
                if (cmd_buf[1] == THERMOSTAT_SETBACK_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_thrmo_setb_fn     rpt_cb;
                        uint8_t                 state;

                        rpt_cb = (zwrep_thrmo_setb_fn)report_cb;

                        if (cmd_buf[3] == 0x79)
                        {
                            state = ZW_THRMO_SETB_STA_FROST_PROCT;
                        }
                        else if (cmd_buf[3] == 0x7A)
                        {
                            state = ZW_THRMO_SETB_STA_ENER_SAVE;
                        }
                        else if ((cmd_buf[3] >= 0x7B) && (cmd_buf[3] < 0x7F))
                        {   //Invalid
                            return 1;
                        }
                        else if (cmd_buf[3] == 0x7F)
                        {
                            state = ZW_THRMO_SETB_STA_UNUSED;
                        }
                        else
                        {
                            state = ZW_THRMO_SETB_STA_SETB;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2] & 0x03, state, cmd_buf[3]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETPOINT:
            {
                if (cmd_buf[1] == THERMOSTAT_SETPOINT_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        time_t              time_stamp;
                        zwsetp_t            setp_val = {0};
                        zwrep_thrmo_setp_fn rpt_cb;

                        setp_val.type = cmd_buf[2] & 0x0F;
                        setp_val.precision = cmd_buf[3] >> 5;
                        setp_val.unit = (cmd_buf[3] >> 3) & 0x03;
                        setp_val.size = cmd_buf[3] & 0x07;

                        //Check setpoint value size
                        if (memchr(data_storage_sz, setp_val.size, sizeof(data_storage_sz)) == NULL)
                        {
                            return 1;
                        }

                        if (cmd_len >= (4 + setp_val.size))
                        {
                            memcpy(setp_val.data, cmd_buf + 4, setp_val.size);

                            util_time_stamp(&time_stamp);

                            cache_chg = 1;

                            //Do not cache setpoint type 0
                            if (setp_val.type > 0)
                            {
                                if (!intf->cch_data)
                                {   //No cached report, cache it.
                                    zwif_dat_cache_alloc(intf, sizeof(zwsetp_t), FALSE);
                                    if (intf->cch_data && intf->cch_dat_cnt)
                                    {   //Update cache
                                        memcpy(intf->cch_data, &time_stamp, sizeof(time_t));
                                        memcpy(((time_t *)intf->cch_data) + 1, &setp_val, sizeof(setp_val));
                                    }
                                }
                                else
                                {   //Got at least one cached report
                                    cache_chg = zwif_thrmo_setp_cache_updt(intf, &setp_val, &time_stamp);
                                }
                            }

                            zwif_get_desc(intf, &ifd);
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_thrmo_setp_fn)sm_rpt_cb;
                                rpt_cb(&ifd, &setp_val, time_stamp);
                            }

                            if (report_cb && cache_chg)
                            {   //Callback to application
                                rpt_cb = (zwrep_thrmo_setp_fn)report_cb;
                                rpt_cb(&ifd, &setp_val, time_stamp);
                            }
                        }
                    }
                }
                else if (cmd_buf[1] == THERMOSTAT_SETPOINT_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        if_thrmo_setp_data_t    *thrmo_setp_dat;
                        zwrep_thrmo_setp_sup_fn rpt_cb;
                        uint8_t             sup_type[248];
                        int                 i;
                        int                 max_sup_type;
                        uint8_t             sup_type_len;

                        sup_type_len = 0;
                        max_sup_type = (cmd_len - 2) * 8;

                        //Map type 0 to type 1 for Danfoss Living Connect workaround
                        if (cmd_buf[2] & 0x01)
                        {
                            sup_type[sup_type_len++] = 1;
                        }

                        for (i = 1; i < max_sup_type; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                sup_type[sup_type_len++] = (i >= 3)? (i + 4) : i;
                            }
                        }

                        //Save to cache
                        intf->data_item_sz = sizeof(if_thrmo_setp_data_t) + sup_type_len - 1;

                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            thrmo_setp_dat = (if_thrmo_setp_data_t *)malloc(intf->data_item_sz);
                        }
                        else
                        {   //Use previously allocated memory
                            thrmo_setp_dat = (if_thrmo_setp_data_t *)intf->data;
                        }


                        if (thrmo_setp_dat)
                        {
                            thrmo_setp_dat->setp_len = sup_type_len;
                            memcpy(thrmo_setp_dat->setp, sup_type, sup_type_len);

                            //Assign to interface
                            intf->data = thrmo_setp_dat;
                            intf->data_cnt = 1;
                        }
                        else
                        {
                            intf->data_cnt = 0;
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_thrmo_setp_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, sup_type_len, sup_type, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_thrmo_setp_sup_fn)report_cb;
                            rpt_cb(&ifd, sup_type_len, sup_type, 1);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_CLOCK:
            if (report_cb)
            {
                if (cmd_buf[1] == CLOCK_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_clock_fn    rpt_cb;
                        rpt_cb = (zwrep_clock_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]>>5, cmd_buf[2] & 0x1F, cmd_buf[3]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
            if (report_cb)
            {
                if (cmd_buf[1] == SCHEDULE_REPORT)
                {
                    if (cmd_len >= 30)
                    {
                        zwrep_clmt_ctl_schd_fn  rpt_cb;
                        zwcc_shed_t             sched;
                        uint8_t                 *cmdptr;
                        uint8_t                 state;


                        sched.weekday = cmd_buf[2] & 0x07;

                        rpt_cb = (zwrep_clmt_ctl_schd_fn)report_cb;

                        cmdptr = cmd_buf + 3;

                        sched.total = 0;

                        for (i=0; i < 9; i++)
                        {
                            if (cmdptr[2] == 0x79)
                            {
                                state = ZW_THRMO_SETB_STA_FROST_PROCT;
                            }
                            else if (cmdptr[2] == 0x7A)
                            {
                                state = ZW_THRMO_SETB_STA_ENER_SAVE;
                            }
                            else if ((cmdptr[2] >= 0x7B) && (cmdptr[2] < 0x7F))
                            {   //Invalid
                                return 1;
                            }
                            else if (cmdptr[2] == 0x7F)
                            {   //Unused state
                                break;
                            }
                            else
                            {
                                state = ZW_THRMO_SETB_STA_SETB;
                            }

                            sched.swpts[i].state = state;
                            sched.swpts[i].hour = cmdptr[0] & 0x1F;
                            sched.swpts[i].minute = cmdptr[1] & 0x3F;
                            if (state == ZW_THRMO_SETB_STA_SETB)
                            {
                                sched.swpts[i].tenth_deg = cmdptr[2];
                            }

                            //Update switch point counter
                            sched.total++;

                            //Update pointer
                            cmdptr += 3;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &sched);
                    }
                }
                else if (cmd_buf[1] == SCHEDULE_CHANGED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_clmt_ctl_schd_chg_fn    rpt_cb;
                        rpt_cb = (zwrep_clmt_ctl_schd_chg_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }

                }
                else if (cmd_buf[1] == SCHEDULE_OVERRIDE_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_clmt_ctl_schd_ovr_fn    rpt_cb;
                        zwcc_shed_ovr_t               schd_ovr;
                        rpt_cb = (zwrep_clmt_ctl_schd_ovr_fn)report_cb;

                        if (cmd_buf[3] == 0x79)
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_FROST_PROCT;
                        }
                        else if (cmd_buf[3] == 0x7A)
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_ENER_SAVE;
                        }
                        else if ((cmd_buf[3] >= 0x7B) && (cmd_buf[3] < 0x7F))
                        {   //Invalid
                            return 1;
                        }
                        else if (cmd_buf[3] == 0x7F)
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_UNUSED;
                        }
                        else
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_SETB;
                            schd_ovr.tenth_deg = cmd_buf[3];
                        }

                        schd_ovr.type = cmd_buf[2] & 0x03;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &schd_ovr);
                    }

                }

            }
            break;

        case COMMAND_CLASS_PROTECTION:
            {
                if (cmd_buf[1] == PROTECTION_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_prot_fn     rpt_cb;

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_prot_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2], (cmd_len >= 4)? cmd_buf[3] : ZW_RFPROT_UNPROT);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_prot_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2], (cmd_len >= 4)? cmd_buf[3] : ZW_RFPROT_UNPROT);
                        }
                    }
                }
                else if (cmd_buf[1] == PROTECTION_SUPPORTED_REPORT_V2)
                {
                    if (cmd_len >= 7)
                    {
                        zwprot_sup_t        *if_prot_data;
                        zwrep_prot_sup_fn   rpt_cb;
                        zwprot_sup_t        sup = {0};
                        int                 i;

                        sup.excl_ctl = (cmd_buf[2] & 0x02)? 1 : 0;
                        sup.tmout = cmd_buf[2] & 0x01;
                        //Init
                        sup.lprot_len = 0;
                        sup.rfprot_len = 0;

                        for (i = 0; i < 16; i++)
                        {
                            if ((cmd_buf[(i>>3) + 3] >> (i & 0x07)) & 0x01)
                            {
                                sup.lprot[sup.lprot_len++] = i;
                            }
                            if ((cmd_buf[(i>>3) + 5] >> (i & 0x07)) & 0x01)
                            {
                                sup.rfprot[sup.rfprot_len++] = i;
                            }
                        }

                        //Save to cache
                        if (intf->data_cnt == 0)
                        {
                            //Allocate memory for data
                            if_prot_data = (zwprot_sup_t *)malloc(sizeof(zwprot_sup_t));
                        }
                        else
                        {   //Use previously allocated memory
                            if_prot_data = (zwprot_sup_t *)intf->data;
                        }

                        if (if_prot_data)
                        {
                            intf->data_cnt = 1;
                            intf->data_item_sz = sizeof(zwprot_sup_t);
                            intf->data = if_prot_data;
                            *if_prot_data = sup;
                        }

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_prot_sup_fn)sm_rpt_cb;
                            rpt_cb(&ifd, &sup, 1);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_prot_sup_fn)report_cb;
                            rpt_cb(&ifd, &sup, 1);
                        }
                    }
                }
                else if (cmd_buf[1] == PROTECTION_EC_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_prot_ec_fn    rpt_cb;

                        zwif_get_desc(intf, &ifd);
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_prot_ec_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2]);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_prot_ec_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2]);
                        }
                    }
                }
                else if (cmd_buf[1] == PROTECTION_TIMEOUT_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_prot_tmout_fn    rpt_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_prot_tmout_fn)sm_rpt_cb;
                            rpt_cb(&ifd, cmd_buf[2]);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_prot_tmout_fn)report_cb;
                            rpt_cb(&ifd, cmd_buf[2]);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_APPLICATION_STATUS:
            if (report_cb)
            {
                if (cmd_buf[1] == APPLICATION_BUSY)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_appl_busy_fn     rpt_cb;

                        rpt_cb = (zwrep_appl_busy_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], cmd_buf[3]);

                    }
                }
                else if (cmd_buf[1] == APPLICATION_REJECTED_REQUEST)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_appl_reject_fn   rpt_cb;

                        rpt_cb = (zwrep_appl_reject_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_INDICATOR:
            if (report_cb)
            {
                if (cmd_buf[1] == INDICATOR_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
            if (report_cb)
            {
                if (cmd_buf[1] == FIRMWARE_UPDATE_MD_GET)
                {
                    if (cmd_len >= 5)
                    {
                        zw_fw_tx_fn     fw_tx_fn;
                        uint16_t        frag_num;
                        uint8_t         rpt_cnt;

                        fw_tx_fn = (zw_fw_tx_fn)report_cb;

                        rpt_cnt = cmd_buf[2];
                        frag_num = cmd_buf[3];
                        frag_num = (frag_num << 8) | cmd_buf[4];

                        fw_tx_fn(intf, frag_num, rpt_cnt);
                    }
                }
                else if (cmd_buf[1] == FIRMWARE_MD_REPORT)
                {
                    if (cmd_len >= 8)
                    {
                        if_fw_tmp_dat_t     *fw_data;
                        uint16_t            *fw_id;
                        int                 i;
                        int                 j;
                        zwfw_info_t         fw_info;
                        zwrep_fw_info_fn    rpt_cb;
                        rpt_cb = (zwrep_fw_info_fn)report_cb;

                        fw_info.vid = cmd_buf[2];
                        fw_info.vid = (fw_info.vid << 8) | cmd_buf[3];
                        fw_info.zw_fw_id = cmd_buf[4];
                        fw_info.zw_fw_id = (fw_info.zw_fw_id << 8) | cmd_buf[5];
                        fw_info.chksum = cmd_buf[6];
                        fw_info.chksum = (fw_info.chksum << 8) | cmd_buf[7];

                        if (cmd_len == 8)
                        {   //version 1 & 2
                            fw_info.upgrade_flg = ZW_FW_UPGD_YES;
                            fw_info.other_fw_cnt = 0;
                            fw_info.other_fw_id = NULL;
                            fw_info.max_frag_sz = (intf->ver == 1)? ZW_FW_FRAG_SZ_V1 : ZW_FW_FRAG_SZ_V2;
                            fw_info.fixed_frag_sz = 1;
                        }
                        else if (cmd_len >= 12)
                        {   //version 3
                            fw_info.upgrade_flg = cmd_buf[8];
                            fw_info.other_fw_cnt = cmd_buf[9];
                            fw_info.max_frag_sz = cmd_buf[10];
                            fw_info.max_frag_sz = (fw_info.max_frag_sz << 8) | cmd_buf[11];
                            fw_info.fixed_frag_sz = 0;

                            if (fw_info.other_fw_cnt > 0)
                            {
                                if (((cmd_len - 12)/2) == fw_info.other_fw_cnt)
                                {
                                    fw_id = calloc(fw_info.other_fw_cnt, sizeof(uint16_t));

                                    if (!fw_id)
                                    {
                                        return 1;
                                    }

                                    fw_info.other_fw_id = fw_id;

                                    for (i=0, j=0; i<fw_info.other_fw_cnt; i++)
                                    {
                                        fw_id[i] = cmd_buf[12+(j++)];
                                        fw_id[i] = (fw_id[i] << 8) | cmd_buf[12+(j++)];
                                    }
                                }
                                else
                                {
                                    return 1;
                                }
                            }
                            else
                            {
                                fw_info.other_fw_id = NULL;
                            }
                        }
                        else
                        {   //error
                            return 1;
                        }

                        //Save the maximum fragment size
                        if (!intf->tmp_data)
                        {
                            fw_data = (if_fw_tmp_dat_t *)calloc(1, sizeof(if_fw_tmp_dat_t));
                            if (!fw_data)
                            {
                                free(fw_info.other_fw_id);
                                return 1;
                            }
                            //Assign to interface
                            intf->tmp_data = fw_data;
                        }
                        fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;
                        fw_data->max_frag_sz = fw_info.max_frag_sz;
                        fw_data->fixed_frag_sz = fw_info.fixed_frag_sz;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &fw_info);
                        free(fw_info.other_fw_id);
                    }
                }
                else if (cmd_buf[1] == FIRMWARE_UPDATE_MD_REQUEST_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fw_updt_sts_fn    rpt_cb;
                        rpt_cb = (zwrep_fw_updt_sts_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == FIRMWARE_UPDATE_MD_STATUS_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        if_fw_tmp_dat_t         *fw_data;
                        zwrep_fw_updt_cmplt_fn  rpt_cb;
                        int                     wait_tm_valid = 0;
                        uint16_t                wait_tm = 0;

                        rpt_cb = (zwrep_fw_updt_cmplt_fn)report_cb;

                        //Stop any further firmware fragment transfer
                        fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;
                        if (fw_data)
                        {
                            fw_data->frag_sz = 0;
                            //Close the firmware file
                            fclose(fw_data->fw_file);
                            fw_data->fw_file = NULL;
                        }

                        if ((intf->ver >=3) && (cmd_buf[2] == ZW_FW_UPDT_CMPLT_OK_RESTART)
                            && (cmd_len >= 5))
                        {
                            wait_tm_valid = 1;
                            wait_tm = cmd_buf[3];
                            wait_tm = (wait_tm << 8) | cmd_buf[4];

                            if (wait_tm == 0)
                            {
                                wait_tm_valid = 0;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], wait_tm, wait_tm_valid);

                        //Wait target to restart
                        if (fw_data && wait_tm_valid && intf->ep->node->restart_cb)
                        {
                            if (fw_data->restrt_tmr_ctx)
                            {
                                plt_tmr_stop(&ifd.net->plt_ctx, fw_data->restrt_tmr_ctx);
                                fw_data->restrt_tmr_ctx = NULL;
                            }

                            fw_data->nw = ifd.net;
                            zwnode_get_desc(intf->ep->node, &fw_data->node);

                            //Start timer
                            fw_data->restrt_tmr_ctx = plt_tmr_start(&ifd.net->plt_ctx, wait_tm*1000, zwif_fw_restrt_tout_cb, fw_data);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ZIP_GATEWAY:
            if (report_cb)
            {
                if (cmd_buf[1] == GATEWAY_MODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwnet_p             nw = intf->ep->node->net;
                        zwrep_gw_mode_fn    rpt_cb;

                        rpt_cb = (zwrep_gw_mode_fn)report_cb;
                        zwif_get_desc(intf, &ifd);

                        if (cmd_buf[2] == ZW_GW_STAND_ALONE)
                        {
                            //Stop timer
                            plt_mtx_lck(nw->mtx);
                            plt_tmr_stop(&nw->plt_ctx, nw->gw_mode_qry.tmr_ctx);
                            nw->gw_mode_qry.tmr_ctx = NULL;
                            plt_mtx_ulck(nw->mtx);

                            //Callback the registered function
                            rpt_cb(&ifd, 0, ZW_GW_STAND_ALONE, NULL);
                        }
                        else if (cmd_buf[2] == ZW_GW_PORTAL)
                        {   //Get portal profile
                            int result;
                            uint8_t prof_num = 1;

                            result = zwif_get_report_ex(&ifd, &prof_num, 1, GATEWAY_PEER_GET,
                                                        zwif_tx_sts_cb, ZWIF_RPT_GET_ZIP);
                            if (result < 0)
                            {
                                //Stop timer
                                plt_mtx_lck(nw->mtx);
                                plt_tmr_stop(&nw->plt_ctx, nw->gw_mode_qry.tmr_ctx);
                                nw->gw_mode_qry.tmr_ctx = NULL;
                                plt_mtx_ulck(nw->mtx);

                                //Callback the registered function
                                rpt_cb(&ifd, 2, ZW_GW_PORTAL, NULL);
                                break;
                            }
                        }
                    }
                }
                else if (cmd_buf[1] == GATEWAY_PEER_REPORT)
                {
                    if (cmd_len >= 23)
                    {
                        zwnet_p             nw = intf->ep->node->net;
                        zwrep_gw_mode_fn    rpt_cb;
                        zwgw_portal_prof_t  portal_prof;

                        rpt_cb = (zwrep_gw_mode_fn)report_cb;
                        zwif_get_desc(intf, &ifd);

                        //Stop timer
                        plt_mtx_lck(nw->mtx);
                        plt_tmr_stop(&nw->plt_ctx, nw->gw_mode_qry.tmr_ctx);
                        nw->gw_mode_qry.tmr_ctx = NULL;
                        plt_mtx_ulck(nw->mtx);


                        if (cmd_buf[3] && (cmd_buf[2] == 1))
                        {   //Profile is valid
                            memcpy(portal_prof.addr6, cmd_buf + 4, 16);
                            portal_prof.port = cmd_buf[20];
                            portal_prof.port = (portal_prof.port << 8) | cmd_buf[21];

                            portal_prof.name_len = cmd_buf[22] & 0x3F;
                            if (portal_prof.name_len)
                            {
                                memcpy(portal_prof.portal_name, cmd_buf + 23, portal_prof.name_len);
                            }
                            //Callback the registered function
                            rpt_cb(&ifd, 0, ZW_GW_PORTAL, &portal_prof);
                        }
                        else
                        {
                            //Profile is invalid
                            rpt_cb(&ifd, 0, ZW_GW_PORTAL, NULL);
                        }
                    }
                }
                else if (cmd_buf[1] == UNSOLICITED_DESTINATION_REPORT)
                {
                    if (cmd_len >= 20)
                    {
                        uint16_t                dst_port;
                        zwrep_gw_unsolicit_fn   rpt_cb;

                        rpt_cb = (zwrep_gw_unsolicit_fn)report_cb;
                        zwif_get_desc(intf, &ifd);

                        dst_port = cmd_buf[18];
                        dst_port = (dst_port << 8) | cmd_buf[19];

                        rpt_cb(&ifd, cmd_buf + 2, dst_port);

                    }
                }
                else if (cmd_buf[1] == GATEWAY_WIFI_CONFIG_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwrep_gw_wifi_cfg_fn    rpt_cb;
                        zw_wifi_cfg_t           cfg = {0};
                        uint16_t                idx;

                        rpt_cb = (zwrep_gw_wifi_cfg_fn)report_cb;

                        //SSID
                        if (cmd_buf[2] > 32)
                        {
                            break;
                        }
                        cfg.ssid_len = cmd_buf[2];

                        if (cmd_len < (3 + cfg.ssid_len + 2)) // +2 is for security type & key length fields
                        {
                            break;
                        }
                        memcpy(cfg.ssid, cmd_buf + 3, cfg.ssid_len);
                        idx = 3 + cfg.ssid_len;

                        //Security type
                        cfg.sec_type = cmd_buf[idx] & 0x07;
                        cfg.wep_idx = (cmd_buf[idx] >> 6) + 1;
                        idx++;

                        //Key or password
                        if (cmd_buf[idx] > 64)
                        {
                            break;
                        }

                        cfg.key_len = cmd_buf[idx++];
                        if (cmd_len < (5 + cfg.ssid_len + cfg.key_len))
                        {
                            break;
                        }
                        memcpy(cfg.key, cmd_buf + idx, cfg.key_len);

                        zwif_get_desc(intf, &ifd);

                        rpt_cb(&ifd, &cfg);

                    }
                }
                else if (cmd_buf[1] == GATEWAY_WIFI_ENCRYPT_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_gw_wifi_sec_fn    rpt_cb;

                        rpt_cb = (zwrep_gw_wifi_sec_fn)report_cb;
                        zwif_get_desc(intf, &ifd);

                        rpt_cb(&ifd, cmd_buf + 2, cmd_len - 2, 1);

                    }
                }

            }
            break;

        case COMMAND_CLASS_ZWAVEPLUS_INFO:
            {
                if (cmd_buf[1] == ZWAVEPLUS_INFO_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwplus_info_t   info = {0};
                        zwrep_info_fn   rpt_cb;

                        info.zwplus_ver = cmd_buf[2];
                        info.role_type = cmd_buf[3];
                        info.node_type = cmd_buf[4];

                        if (cmd_len >= 9)
                        {
                            info.instr_icon = cmd_buf[5];
                            info.instr_icon = (info.instr_icon << 8) | cmd_buf[6];
                            info.usr_icon = cmd_buf[7];
                            info.usr_icon = (info.usr_icon << 8) | cmd_buf[8];
                        }

                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_info_fn)sm_rpt_cb;
                            rpt_cb(intf, &info);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_info_fn)report_cb;
                            rpt_cb(intf, &info);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_GRP_INFO:
            {
                if (cmd_buf[1] == ASSOCIATION_GROUP_NAME_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_grp_name_fn     rpt_cb;

                        if (cmd_buf[3] > (cmd_len - 4))
                        {
                            return 1;
                        }

                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_grp_name_fn)sm_rpt_cb;
                            rpt_cb(intf, cmd_buf[2], cmd_buf[3], cmd_buf + 4);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_grp_name_fn)report_cb;
                            rpt_cb(intf, cmd_buf[2], cmd_buf[3], cmd_buf + 4);
                        }
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_GROUP_INFO_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_grp_info_fn   rpt_cb;
                        zw_grp_info_ent_t   *grp_info;
                        int                 i;
                        uint8_t             *grp_info_ptr;
                        uint16_t            profile;
                        uint16_t            evt_code;
                        uint8_t             grp_cnt;


                        grp_cnt = cmd_buf[2] & 0x3F;

                        if ((grp_cnt * 7) > (cmd_len - 3))
                        {
                            return 1;
                        }

                        grp_info = (zw_grp_info_ent_t *)calloc(1, (grp_cnt * sizeof(zw_grp_info_ent_t)));

                        if (grp_info)
                        {
                            grp_info_ptr = cmd_buf + 3;

                            for (i=0; i<grp_cnt; i++)
                            {
                                grp_info[i].grp_num =  *grp_info_ptr++;
                                if (*grp_info_ptr++ != 0)
                                {   //Mode not equals to zero, discard the whole report
                                    free(grp_info);
                                    return 1;
                                }
                                profile = *grp_info_ptr++;
                                profile = (profile << 8) | *grp_info_ptr++;
                                grp_info_ptr++;
                                evt_code = *grp_info_ptr++;
                                evt_code = (evt_code << 8) | *grp_info_ptr++;

                                grp_info[i].profile = profile;
                                grp_info[i].evt_code = evt_code;
                            }
                        }

                        if (single_ep_sm_sts && sm_rpt_cb)
                        {   //Callback to single state-machine
                            rpt_cb = (zwrep_grp_info_fn)sm_rpt_cb;
                            rpt_cb(intf, grp_cnt, (cmd_buf[2] & 0x40)? 1 : 0, grp_info);
                        }

                        if (report_cb)
                        {   //Callback to application
                            rpt_cb = (zwrep_grp_info_fn)report_cb;
                            rpt_cb(intf, grp_cnt, (cmd_buf[2] & 0x40)? 1 : 0, grp_info);
                        }
                        free(grp_info);
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_GROUP_COMMAND_LIST_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_grp_cmd_lst_fn    rpt_cb;
                        uint8_t                 lst_len;
                        uint8_t                 cmd_ent_cnt;
                        grp_cmd_ent_t           *cmd_lst;

                        lst_len = cmd_buf[3];

                        if (lst_len > (cmd_len - 4))
                        {
                            return 1;
                        }

                        //Allocate memory for the command list
                        cmd_ent_cnt = lst_len >> 1;  //assuming the worst case with each command class equals to 1-byte
                        cmd_lst = (grp_cmd_ent_t *)calloc(1, (cmd_ent_cnt * sizeof(grp_cmd_ent_t)));

                        if (cmd_lst)
                        {
                            int         i;
                            uint8_t     *cmd_lst_ptr;
                            uint16_t    cls;
                            uint8_t     cmd;

                            cmd_lst_ptr = cmd_buf + 4;
                            cmd_ent_cnt = 0;

                            for (i=0; i<lst_len; i++)
                            {
                                if (*cmd_lst_ptr >= 0xF1)
                                {   //Extended command class
                                    if ((i + 2) < lst_len)
                                    {
                                        cls = *cmd_lst_ptr++;
                                        cls = (cls << 8) | (*cmd_lst_ptr++);
                                        cmd = *cmd_lst_ptr++;
                                        i += 2;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                else
                                {   //Normal command class
                                    if ((i + 1) < lst_len)
                                    {
                                        cls = *cmd_lst_ptr++;
                                        cmd = *cmd_lst_ptr++;
                                        i++;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                cmd_lst[cmd_ent_cnt].cls = cls;
                                cmd_lst[cmd_ent_cnt].cmd = cmd;
                                cmd_ent_cnt++;
                            }
                            if (single_ep_sm_sts && sm_rpt_cb)
                            {   //Callback to single state-machine
                                rpt_cb = (zwrep_grp_cmd_lst_fn)sm_rpt_cb;
                                rpt_cb(intf, cmd_buf[2], cmd_ent_cnt, cmd_lst);
                            }

                            if (report_cb)
                            {   //Callback to application
                                rpt_cb = (zwrep_grp_cmd_lst_fn)report_cb;
                                rpt_cb(intf, cmd_buf[2], cmd_ent_cnt, cmd_lst);
                            }
                            free(cmd_lst);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ZIP_PORTAL:
            if (report_cb)
            {
                if (cmd_buf[1] == GATEWAY_CONFIGURATION_STATUS)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_cfg_sts_fn    rpt_cb;
                        rpt_cb = (zwrep_cfg_sts_fn)report_cb;
#ifdef TCP_PORTAL
                        //Change the TLS tunnel server source address
                        if (cmd_buf[2] == 0xFF)
                        {   //Gateway configuration set is successful
                            tpt_svr_ipv6_addr_set(&intf->ep->node->net->appl_ctx.ssn_ctx.frm_ctx.tpt_ctx,
                                                  intf->ep->node->net->tls_tunnel_svr_ipv6);
                            memcpy(intf->ep->node->net->zip_pan, intf->ep->node->net->tls_pan_prefix, IPV6_ADDR_LEN);
                        }
#endif
                        zwif_get_desc(intf, &ifd);
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == GATEWAY_CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 68)
                    {
                        zwportal_cfg_t      cfg;
                        zwrep_gw_cfg_fn     rpt_cb;

                        rpt_cb = (zwrep_gw_cfg_fn)report_cb;

                        //copy the Z/IP gateway LAN Ipv6 addr
                        memcpy(cfg.lan_ipv6_addr, cmd_buf + 2, IPV6_ADDR_LEN);

                        //copy the prefix length of gateway LAN ipv6 addr -- 1byte
                        cfg.lan_ipv6_prefix_len = cmd_buf[2 + IPV6_ADDR_LEN];

                        //copy the portal Ipv6 prefix (tunnel prefix)
                        memcpy(cfg.portal_ipv6_prefix, cmd_buf + 3 + IPV6_ADDR_LEN, IPV6_ADDR_LEN);

                        //copy the prefix length of portal ipv6 addr -- 1byte
                        cfg.portal_ipv6_prefix_len = cmd_buf[3 + (2 * IPV6_ADDR_LEN)];

                        //copy default gateway Ipv6 addr
                        memcpy(cfg.dflt_gw, cmd_buf + 4 + (2 * IPV6_ADDR_LEN), IPV6_ADDR_LEN);

                        //copy PAN prefix Ipv6 addr (with 64 prefix length)
                        memcpy(cfg.pan_prefix, cmd_buf + 4 + (3 * IPV6_ADDR_LEN), IPV6_ADDR_LEN);

                        zwif_get_desc(intf, &ifd);
                        rpt_cb(&ifd, &cfg);
                    }
                }
            }
            break;

		case COMMAND_CLASS_POWERLEVEL:
            if (report_cb)
            {
                if (cmd_buf[1] == POWERLEVEL_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_power_level_fn       rpt_cb;
                        uint8_t                    byTimeout;

                        rpt_cb = (zwrep_power_level_fn)report_cb;

						if(cmd_len >= 4)
						{
                           byTimeout = cmd_buf[3];
                        }
                        else
                        {
                            byTimeout = 0;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], byTimeout);
                    }
                }
                else if (cmd_buf[1] == POWERLEVEL_TEST_NODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_power_level_test_fn   rpt_cb;
                        uint8_t             byStatus;
                        uint16_t			wFramCnt;

                        rpt_cb = (zwrep_power_level_test_fn)report_cb;

						if(cmd_len >= 4)
						{
							byStatus = cmd_buf[3];
						}
						else
						{
							byStatus = POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES;
						}

                        if(cmd_len >= 6)
						{
							wFramCnt = (cmd_buf[4] << 8) + cmd_buf[5];
						}
						else
						{
							wFramCnt = 0;
						}

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], byStatus, wFramCnt);
                    }
                }
            }
            break;

		case COMMAND_CLASS_SECURITY:
            if (report_cb)
            {
                if (cmd_buf[1] == SECURITY_COMMANDS_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwif_sec_cmd_cls_t  *cmd_cls;
                        uint8_t             *tmp_buf;
                        uint16_t            cmd_cls_len;   //command classes length of this report

                        cmd_cls = &intf->ep->node->net->sec_cmd_cls;

                        cmd_cls_len = cmd_len - 3;

                        if (cmd_cls_len)
                        {
                            //Store the supported command classes
                            if (cmd_cls->cmd_cls_buf)
                            {
                                //Re-allocate memory
                                tmp_buf = realloc(cmd_cls->cmd_cls_buf,
                                                  cmd_cls->buf_len + cmd_cls_len);
                            }
                            else
                            {
                                //Allocate memory
                                tmp_buf = malloc(cmd_cls_len);
                                cmd_cls->buf_len = 0;
                            }

                            if (tmp_buf)
                            {
                                cmd_cls->cmd_cls_buf = tmp_buf;
                                memcpy(cmd_cls->cmd_cls_buf + cmd_cls->buf_len,
                                       cmd_buf + 3, cmd_cls_len);

                                cmd_cls->buf_len += cmd_cls_len;

                            }
                        }

                        //Check whether this is the last report
                        if (cmd_buf[2] == 0)
                        {
                            //Last report
                            zwrep_sup_sec_fn    rpt_cb = (zwrep_sup_sec_fn)report_cb;
                            uint16_t            *cmd_cls_sec;
                            uint16_t            *cmd_class;
                            uint8_t             cmd_cnt_sec;
                            uint8_t             cmd_cnt;

                            if(util_cmd_cls_parse(cmd_cls->cmd_cls_buf, cmd_cls->buf_len, &cmd_class, &cmd_cnt,
                                                  &cmd_cls_sec, &cmd_cnt_sec))
                            {
                                //Callback the registered function
                                rpt_cb(intf, cmd_class, cmd_cnt);

                                free(cmd_class);
                                free(cmd_cls_sec);
                            }
                            else if (!cmd_cls->cmd_cls_buf)
                            {   //Empty report (i.e. no command class)

                                //Callback the registered function
                                rpt_cb(intf, NULL, 0);
                            }

                            //Free memory
                            free(cmd_cls->cmd_cls_buf);
                            cmd_cls->cmd_cls_buf = NULL;

                        }
                    }
                }
            }
            break;

		case COMMAND_CLASS_CENTRAL_SCENE:
		{
			if (cmd_buf[1] == CENTRAL_SCENE_NOTIFICATION)
			{
				if (cmd_len >= 5)
				{
					zwcsc_notif_t       csc_dat = {0};
					zwcsc_notif_p		pcsc_dat_cached = NULL;
					time_t              time_stamp;
					zwrep_csc_fn		rpt_cb;
					uint8_t				bDiscardRpt = 0;
					rpt_cb = (zwrep_csc_fn)report_cb;

					csc_dat.seqNo = cmd_buf[2];
					csc_dat.keyAttr = cmd_buf[3] & CENTRAL_SCENE_NOTIFICATION_PROPERTIES1_KEY_ATTRIBUTES_MASK;
					csc_dat.sceneNo = cmd_buf[4];

					util_time_stamp(&time_stamp);

					if (!intf->cch_data)
					{   //No cached report, cache it.
						zwif_dat_cache_alloc(intf, sizeof(zwcsc_notif_t), FALSE);
						if (intf->cch_data && intf->cch_dat_cnt)
						{   //Update cache
							memcpy(intf->cch_data, &time_stamp, sizeof(time_t));
							memcpy(((time_t *)intf->cch_data) + 1, &csc_dat, sizeof(csc_dat));
						}
					}
					else
					{   //Got at least one cached report
						//Check the sequence number is increasing.
						pcsc_dat_cached = (zwcsc_notif_p)(((time_t *)intf->cch_data) + 1);
						if ((csc_dat.seqNo > pcsc_dat_cached->seqNo) ||
							(csc_dat.seqNo == 0 && pcsc_dat_cached->seqNo == 0xFF))
						{
							memcpy(((time_t *)intf->cch_data) + 1, &csc_dat, sizeof(csc_dat));
						}
						else
							//discard this report
							bDiscardRpt = 1;
					}

					if ((bDiscardRpt == 0) && rpt_cb)
					{   //Always invoke callback since key attributes shouldn't be cached!
						zwif_get_desc(intf, &ifd);
						//Callback the registered function
						rpt_cb(&ifd, &csc_dat, time_stamp);
					}
				}
			}
			else if (cmd_buf[1] == CENTRAL_SCENE_SUPPORTED_REPORT)
			{
				/* KA Array returned in the following format:
				* v1 interface -- sameKA 1, scene_cnt 5, KA_array_len 0, KA Array is NULL
				* V2 interface or above --
				* eg 1.
				* sameKA 1, scene_cnt 5, KA_array_len 4
				*  KA_array: [3] [0] [1] [2]
				*
				* eg 2.
				* sameKA 0, scene_cnt 5, KA_array_len 4
				* KA_array: [3] [0] [1] [2]
				*			[2] [0] [5] [0]		//Only have 2 valid KA in this row
				*			[0] [0] [0] [0]		//No valid KA in this row
				*			[3] [0] [3] [5]
				*			[1] [1] [0] [0]		//Only have 1 valid KA in this row
				*/
				if (cmd_len >= 3)
				{
					zwrep_csc_sup_fn	rpt_cb;
					uint8_t             scene_cnt = 0, sameKA = 1, KA_array_len = 0;
					uint8_t				KA_array_temp[25][25]; //max 3 byte bitmask means max 24 supported KA per scene
																//Change this if there is more than 25 Scenes/buttons supported by the device
					uint8_t				byNofBitMaskByte = 0;
					uint8_t				*KA_array = NULL, *pKA;
                    if_csc_data_t       *csc_sup_dat;
					int                 i = 0, max_KA, j = 1, max_row = 0;
                    int				    KA_Array_sz = 0;

                    //Clear array
					memset(KA_array_temp, 0, 25 * 25);

					scene_cnt = cmd_buf[2];

					if (cmd_len > 3)	//v2 inerface or above
					{
						sameKA = cmd_buf[3] & CENTRAL_SCENE_SUPPORTED_REPORT_PROPERTIES1_IDENTICAL_BIT_MASK_V2;
						byNofBitMaskByte = (cmd_buf[3] & CENTRAL_SCENE_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASK_BYTES_MASK_V2) >>
											CENTRAL_SCENE_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASK_BYTES_SHIFT_V2;

						if (sameKA)
						{
							//Should only have supported KA for scene 1
							if (cmd_len < 4 + byNofBitMaskByte)
								break;

							max_KA = byNofBitMaskByte * 8;
							max_row = 1;
							//cmd_len = 4 + byNofBitMaskByte;
						}
						else
						{
							//Should have supported KA for all the scenes
							if (cmd_len < 4 + byNofBitMaskByte * scene_cnt)
								break;

							max_KA = byNofBitMaskByte * scene_cnt * 8;
							max_row = scene_cnt;
							//cmd_len = 4 + byNofBitMaskByte * scene_cnt;
						}

						for (i = 0; i < max_KA; i++)
						{
							//Beginning of the support KA byte for a new scene, reinit the column index to 1.
							//column 0 is for length
							if ((i % (8 * byNofBitMaskByte)) == 0)
							{
								j = 1;
							}
							if ((cmd_buf[(i >> 3) + 4] >> (i & 0x07)) & 0x01)
							{
								KA_array_temp[(i >> 3) / byNofBitMaskByte][j++] = i;
								KA_array_temp[(i >> 3) / byNofBitMaskByte][0]++;
							}
						}

						//Get the longest row in the array
						for (i = 0; i < max_row; i++)
						{
							if (KA_array_temp[i][0] > (KA_array_len - 1))
								KA_array_len = KA_array_temp[i][0] + 1;
						}

						KA_array = (uint8_t *)calloc(max_row, KA_array_len);

						if (!KA_array)
						{
							break;
						}

						pKA = KA_array;
						for (i = 0; i < max_row; i++)
						{
							memcpy(pKA, &KA_array_temp[i][0], KA_array_len);
							pKA += KA_array_len;
						}
					}

                    //Save to cache

                    //Free any existing data
                    if (intf->data_cnt > 0)
                    {
                        free(intf->data);
                    }

                    if (sameKA)		//if sameKA == 1
                    {
                        KA_Array_sz = KA_array_len;
                    }
                    else
                    {
                        KA_Array_sz = KA_array_len * scene_cnt;
                    }

                    intf->data_cnt = 1;
                    intf->data_item_sz = sizeof(if_csc_data_t) + KA_Array_sz - 1;

                    csc_sup_dat = (if_csc_data_t *)calloc(1, intf->data_item_sz);

                    if (csc_sup_dat)
                    {
                        csc_sup_dat->scene_cnt = scene_cnt;
                        csc_sup_dat->sameKA = sameKA;
                        csc_sup_dat->KA_array_len = KA_array_len;
                        memcpy(csc_sup_dat->KA_array, KA_array, KA_Array_sz);

                        //Assign to interface
                        intf->data = csc_sup_dat;
                    }
                    else
                    {
                        intf->data_cnt = 0;
                    }

					zwif_get_desc(intf, &ifd);

                    if (single_ep_sm_sts && sm_rpt_cb)
                    {   //Callback to single state-machine
                        rpt_cb = (zwrep_csc_sup_fn)sm_rpt_cb;
                        rpt_cb(&ifd, scene_cnt, sameKA, KA_array_len, KA_array, 1);
                    }

                    if (report_cb)
                    {   //Callback to application
                        rpt_cb = (zwrep_csc_sup_fn)report_cb;
                        rpt_cb(&ifd, scene_cnt, sameKA, KA_array_len, KA_array, 1);
                    }

					free(KA_array);
				}
			}
		}
		break;

		case COMMAND_CLASS_SENSOR_ALARM:
		{
			if (cmd_buf[1] == SENSOR_ALARM_REPORT)
			{
				if (cmd_len >= 7)
				{
					zw_alrm_snsr_t      snsr_dat = { 0 };
					time_t              time_stamp;
					zwrep_alrm_snsr_fn  rpt_cb;
					zwnode_p            node = intf->ep->node;
					zwnet_p             nw = node->net;
					zwif_p			    intf_alarm = NULL;
					if_rec_alrm_snsr_match_t  *alrm_snsr_rec = NULL;

                    snsr_dat.src_node_id = cmd_buf[2];
					snsr_dat.type = cmd_buf[3];
					snsr_dat.state = cmd_buf[4];
					snsr_dat.activetime = (cmd_buf[5] << 8) + cmd_buf[6];

					util_time_stamp(&time_stamp);

					if (!intf->cch_data)
					{   //No cached report, cache it.
						zwif_dat_cache_alloc(intf, sizeof(zw_alrm_snsr_t), FALSE);
						if (intf->cch_data && intf->cch_dat_cnt)
						{   //Update cache
							memcpy(intf->cch_data, &time_stamp, sizeof(time_t));
							memcpy(((time_t *)intf->cch_data) + 1, &snsr_dat, sizeof(zw_alrm_snsr_t));
						}
                        cache_chg = 1;
					}
					else
					{   //Got at least one cached report
						cache_chg = zwif_alrm_snsr_cache_updt(intf, &snsr_dat, &time_stamp);
					}

					zwif_get_desc(intf, &ifd);
					if (single_ep_sm_sts && sm_rpt_cb)
					{   //Callback to single state-machine
						rpt_cb = (zwrep_alrm_snsr_fn)sm_rpt_cb;
						rpt_cb(&ifd, &snsr_dat, time_stamp);
					}

					if (report_cb && cache_chg)
					{   //Callback to application
						rpt_cb = (zwrep_alrm_snsr_fn)report_cb;
						rpt_cb(&ifd, &snsr_dat, time_stamp);
					}

					plt_mtx_lck(nw->mtx);
					//Check whether device configuration record has alarm records
					if (node->dev_cfg_valid)
					{
						alrm_snsr_rec = (if_rec_alrm_snsr_match_t *)zwdev_if_get(node->dev_cfg_rec.ep_rec, intf->ep->epid, IF_REC_TYPE_ALRM_SNSR);

						for (; alrm_snsr_rec != NULL; alrm_snsr_rec = alrm_snsr_rec->next)
						{
							//Find the matching alrm_snsr set record
							if (alrm_snsr_rec->type != -1)
							{
								if (alrm_snsr_rec->type != snsr_dat.type)
									continue;
							}

							if (alrm_snsr_rec->isRange != -1)
							{
								if ((alrm_snsr_rec->isRange == 0) &&
									(alrm_snsr_rec->state_low != snsr_dat.state))
									continue;

								if ((alrm_snsr_rec->isRange == 1) &&
									((alrm_snsr_rec->state_low > snsr_dat.state) ||
									(alrm_snsr_rec->state_high < snsr_dat.state)))
									continue;
							}


							if (alrm_snsr_rec->resultType == IF_REC_TYPE_ALARM)
							{
								intf_alarm = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_ALARM);

								if (intf_alarm)
								{
									zwrep_alrm_fn   rpt_cb = NULL;
									zwalrm_p        alrm_info = NULL;

									for (i = 0; i < intf_alarm->rpt_num; i++)
									{
										if (intf_alarm->rpt[i].rpt_cmd == ALARM_REPORT)
										{
											rpt_cb = intf_alarm->rpt[i].rpt_cb;
											break;
										}
									}

									if (rpt_cb)
									{
										if (zwif_fill_alrm_info_struct(alrm_snsr_rec->pResult, &alrm_info) == ZW_ERR_NONE)
										{
											//Cache the report and callback

											if ((intf_alarm->ver == 1) || (alrm_info->ex_type != 0))
											{
												if (!intf_alarm->cch_data)
												{   //No cached report, cache it.
													//Cached alarm report is per alarm type, per alarm event and variable length.
													//Will use 2 level cache data format
													zwif_dat_cache_alloc(intf_alarm, sizeof(zwalrm_t) + alrm_info->ex_evt_len, TRUE);
													if (intf_alarm->cch_data && intf_alarm->cch_dat_cnt && ((cch_1lvl_t *)intf_alarm->cch_data)->pcch_ele)
													{
														cch_1lvl_t *p1lvl = (cch_1lvl_t *)intf_alarm->cch_data;

														//Update cache
														memcpy(p1lvl->pcch_ele, &time_stamp, sizeof(time_t));
														memcpy(((time_t *)p1lvl->pcch_ele) + 1, alrm_info, sizeof(zwalrm_t) + alrm_info->ex_evt_len);
													}
												}
												else
												{   //Got at least one cached report
													zwif_alrm_cache_updt(intf_alarm, alrm_info, sizeof(zwalrm_t) + alrm_info->ex_evt_len, &time_stamp);
												}
											}

                                            zwif_get_desc(intf_alarm, &ifd);
                                            rpt_cb(&ifd, alrm_info, time_stamp);

											free(alrm_info);
										}
									}
								}
							}
							break;
						}
					}
					plt_mtx_ulck(nw->mtx);

				}
			}
			else if (cmd_buf[1] == SENSOR_ALARM_SUPPORTED_REPORT)
			{
				if (cmd_len >= 4)
				{
					zwrep_alrm_snsr_sup_fn rpt_cb;
					uint8_t             *sensor_dat;
					uint8_t				bitmsk_len;
					uint8_t             snsr_type[248] = { 0 };
					int                 i;
					int                 max_snsr_type;
					uint8_t             type_len;

					bitmsk_len = zwmin(cmd_buf[2], cmd_len - 3);

					type_len = 0;
					max_snsr_type = bitmsk_len * 8;

					for (i = 0; i < max_snsr_type; i++)
					{
						if ((cmd_buf[(i >> 3) + 3] >> (i & 0x07)) & 0x01)
						{
							snsr_type[type_len++] = i;
						}
					}

					//Save to cache
					if (type_len > 0)
					{
						if (intf->data_cnt == 0)
						{
							//Allocate memory for data
							sensor_dat = (uint8_t *)malloc(type_len);
						}
						else
						{   //Use previously allocated memory
							sensor_dat = (uint8_t *)intf->data;
						}

						if (sensor_dat)
						{
							//Save the sensor types
							memcpy(sensor_dat, snsr_type, type_len);

							intf->data_cnt = type_len;
							intf->data_item_sz = sizeof(uint8_t);
							intf->data = sensor_dat;
						}
					}

					zwif_get_desc(intf, &ifd);
					if (single_ep_sm_sts && sm_rpt_cb)
					{   //Callback to single state-machine
						rpt_cb = (zwrep_alrm_snsr_sup_fn)sm_rpt_cb;
						rpt_cb(&ifd, type_len, snsr_type, 1);
					}

					if (report_cb)
					{   //Callback to application
						rpt_cb = (zwrep_alrm_snsr_sup_fn)report_cb;
						rpt_cb(&ifd, type_len, snsr_type, 1);
					}
				}
			}
		}
		break;

        case COMMAND_CLASS_MAILBOX:
            if (report_cb)
            {
                if (cmd_buf[1] == MAILBOX_CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 22)
                    {
                        mailbox_cfg_t   mbx_cfg = {0};
                        mbx_cfg_fn      rpt_cb;

                        rpt_cb = (mbx_cfg_fn)report_cb;

                        mbx_cfg.supp_mode = (cmd_buf[2] >> 3) & 0x03;
                        mbx_cfg.mode = cmd_buf[2] & 0x07;
                        mbx_cfg.frame_cap = cmd_buf[3];
                        mbx_cfg.frame_cap = (mbx_cfg.frame_cap << 8) | cmd_buf[4];
                        memcpy(mbx_cfg.forward_dest, cmd_buf + 5, IPV6_ADDR_LEN);
                        mbx_cfg.forward_port = cmd_buf[21];

                        //Callback the registered function
                        rpt_cb(intf, &mbx_cfg);
                    }
                }
            }
            break;

    }
    return 1;
}


/**
zwif_tx_sts_cb - callback function to display transmit status
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	node_id     Node id
@return
*/
void zwif_tx_sts_cb(appl_layer_ctx_t *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        //debug_zwapi_msg(&nw->plt_ctx, "Send data completed successfully");
    }
    else
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "Send data completed with error:%d, node id:%u", tx_sts, node_id);
    }
}


/**
zwif_tx_sts_get - translate low-level transmit status to high-level transmit status
@param[in]	tx_sts		The transmit status
@return high-level transmit status
*/
uint8_t zwif_tx_sts_get(int8_t tx_sts)
{
    uint8_t hl_tx_sts;

    switch (tx_sts)
    {
        case ZWHCI_NO_ERROR:
            hl_tx_sts = ZWNET_TX_OK;
            break;

        case SESSION_ERROR_SND_FRM_TMOUT:
            hl_tx_sts = ZWNET_TX_NO_ACK;
            break;

        case SESSION_ERROR_UNREACHABLE:
            hl_tx_sts = ZWNET_TX_NOROUTE;
            break;

        case SESSION_ERROR_DEST_BUSY:
            hl_tx_sts = ZWNET_TX_DEST_BUSY;
            break;

        case SESSION_ERROR_SYSTEM:
            hl_tx_sts = ZWNET_TX_SYS_ERR;
            break;

        default:
            hl_tx_sts = ZWNET_TX_SYS_ERR;

    }
    return hl_tx_sts;
}


/**
zwif_exec_cb - execute action on an interface callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@param[in]	node_id     Node id
@return
*/
void zwif_exec_cb(appl_layer_ctx_t *appl_ctx, int8_t tx_sts, void *user_prm, uint8_t node_id)
{
    zwnet_p nw = (zwnet_p)appl_ctx->data;

    //Callback higher-level application
    if (nw->init.appl_tx)
    {
        nw->init.appl_tx(nw->init.user, zwif_tx_sts_get(tx_sts));
    }
}


/**
zwnode_mul_cmd_tmout_cb - Multi command timeout callback
@param[in] data     Pointer to node
@return
*/
static void    zwnode_mul_cmd_tmout_cb(void *data)
{
    int         ret;
    zwnode_p    node = (zwnode_p)data;
    zwnet_p     nw = node->net;
    util_lst_t  *cmd_lst_hd;
    zwnoded_t   noded;
    uint8_t     max_sz;

    plt_mtx_lck(nw->mtx);
    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
    node->mul_cmd_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Send multi command
    cmd_lst_hd = node->mul_cmd_q_hd;
    max_sz = (node->crc_cap)? (MAX_ZWAVE_PKT_SIZE - CRC16_OVERHEAD) : MAX_ZWAVE_PKT_SIZE;

    //Reset the command queue for the node
    node->mul_cmd_q_hd = NULL;
    node->mul_cmd_q_cnt = 0;

    ret = zwnode_get_desc(node, &noded);
    plt_mtx_ulck(nw->mtx);

    if (ret == ZW_ERR_NONE)
    {
        zwnode_mul_cmd_send(&noded, &cmd_lst_hd, max_sz);
    }
}


/**
zwnode_wait_rpt_flg_clr - Reset the wait report flag
@param[in]	net	        Network
@param[in]	nodeid	    Node id
@return
*/
static void zwnode_wait_rpt_flg_clr(zwnet_p net, uint8_t nodeid)
{
    zwnode_p            node;

    //debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_rpt_flg_clr");
    plt_mtx_lck(net->mtx);

    node = zwnode_find(&net->ctl, nodeid);

    if (!node)
    {
        plt_mtx_ulck(net->mtx);
        return;
    }

    zwnode_wait_rpt_tmout_cb(node);

    plt_mtx_ulck(net->mtx);
}


/**
zwif_multi_cmd_cmp - Compare two multi-command
@param[in]	s1	multi-command 1
@param[in]	s2	multi-command 2
@return     zero if s1 equals to s2; else return non-zero
*/
static int zwif_multi_cmd_cmp(uint8_t *s1, uint8_t *s2)
{
    mul_cmd_q_ent_t  *mul_cmd1 = (mul_cmd_q_ent_t *)s1;
    mul_cmd_q_ent_t  *mul_cmd2 = (mul_cmd_q_ent_t *)s2;

    if ((mul_cmd1->ifd.epid == mul_cmd2->ifd.epid)
        && (mul_cmd1->cmd_id == mul_cmd2->cmd_id)
        && (mul_cmd1->dat_len == mul_cmd2->dat_len))
    {
        return memcmp(mul_cmd1->dat_buf, mul_cmd2->dat_buf, mul_cmd1->dat_len);
    }

    return 1;
}


/**
zwif_exec_ex - execute action on an interface with extra parameters
@param[in]	ifd         interface
@param[in]	cmd_buf		command and parameters
@param[in]	buf_len		length of cmd_buf in bytes
@param[in]	cb		    callback function for transmit status
@param[in]	user	    user parameter of callback function
@param[in]	opt		    option, ZWIF_OPT_XXX
@param[in]	xtra		extra parameter
@return	ZW_ERR_xxx
*/
int zwif_exec_ex(zwifd_p ifd, uint8_t *cmd_buf, int buf_len,
                 tx_cmplt_cb_t cb, void *user, int opt, void *xtra)
{
    int                 result;
    int                 wait_rpt = 0;   //Flag to indicate the command is expecting a response report
    appl_snd_data_t     prm = {0};
    uint8_t             *cmd;
    zwnode_p            node;
    zwnet_p             nw = ifd->net;
#ifdef CRC16_ENCAP
    int                 crc16_cap;      //Flag to indicate the node supports CRC-16 encapsulation
    uint8_t             crc16_cmd[MAX_ZWAVE_PKT_SIZE];
#endif
    //uint8_t             multi_ch_cmd[MAX_ZWAVE_PKT_SIZE];
    int                 len;
    int                 max_pkt_size = MAX_ZWAVE_PKT_SIZE;
#ifdef WKUP_BY_MAILBOX_ACK
    uint8_t             sleep_cap;
#endif

    if (!nw->cb_thrd_run)
    {
        return ZW_ERR_SHUTDOWN;
    }

    //Initialize command to the original input command
    cmd = cmd_buf;
    len = buf_len;

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, ifd->nodeid);
    if (!node)
    {
        plt_mtx_ulck(nw->mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

#ifdef WKUP_BY_MAILBOX_ACK
    sleep_cap = node->sleep_cap;
#endif

    //--------------------------------------------------------
    // Check for node that requires wakeup beam (FLIRS device)
    //--------------------------------------------------------
    if (opt & ZWIF_OPT_WAIT_RPT_SET)
    {   //Caller must set the parameters: cb and user
        wait_rpt = 1;
        //debug_zwapi_msg(&nw->plt_ctx, "ZWIF_OPT_WAIT_RPT_SET");
    }

    if (opt & ZWIF_OPT_WAIT_REPORT)
    {
        if (zwnode_get_rpt(cmd, len, &node->wait_cmd, &node->wait_rpt))
        {   //Caller's parameters: cb and user will be overwritten

            wait_rpt = 1;
            //debug_zwapi_msg(&nw->plt_ctx, "ZWIF_OPT_WAIT_REPORT");

            //Restart wait report timer in GET command transmit status callback
            cb = zwnode_wait_tx_cb;
			user = (void *)((uintptr_t)node->nodeid);
        }
    }

    if (((opt & ZWIF_OPT_SKIP_WK_BEAM) == 0)
        && (node->sensor))//Check whether the node supports SENSOR_MODE_WAKEUP_1000 or SENSOR_MODE_WAKEUP_250
    {   //FLIRS device
        int q_cmd = 0;      //Flag to indicate whether to queue the command
        if (node->wait_rpt_flg)
        {   //Queue the command
            q_cmd = 1;
        }
        else
        {
            //No report is pending
            if (node->wait_cmd_q_cnt > 0)
            {
                //There is pending command in the queue, append the command
                q_cmd = 1;
                debug_zwapi_msg(&nw->plt_ctx, "Wait command queue not empty");
            }
        }

        if (q_cmd)
        {
            mul_cmd_q_ent_t  *mul_cmd;

            mul_cmd = (mul_cmd_q_ent_t *)calloc(1, sizeof(mul_cmd_q_ent_t) + len);

            if (!mul_cmd)
            {
                if ((opt & ZWIF_OPT_POLL) == 0)
                {
                    node->cmd_num = 0;
                }
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_MEMORY;
            }

            mul_cmd->ifd = *ifd;
            if (opt & ZWIF_OPT_POLL)
            {
                mul_cmd->cmd_id = ZW_CID_POLL;
                mul_cmd->cmd_num = 1;
            }
            else
            {
                mul_cmd->cmd_id = node->cmd_id;
                mul_cmd->cmd_num = node->cmd_num;
            }
            mul_cmd->dat_len = len;
            memcpy(mul_cmd->dat_buf, cmd, len);

            //Check for extra parameter
            if ((opt & ZWIF_OPT_Q_EXTRA) && xtra)
            {
                mul_cmd->extra = xtra;
            }

            result = util_list_add_no_dup(nw->mtx, &node->wait_cmd_q_hd,
                                          (uint8_t *)mul_cmd, sizeof(mul_cmd_q_ent_t) + len,
                                          zwif_multi_cmd_cmp);

            if (result == 0)
            {
                node->wait_cmd_q_cnt++;
            }
            else if (result == 1)
            {   //Duplicate command
                if (mul_cmd->extra)
                {
                    zwif_cmd_q_xtra_rm(nw, &mul_cmd->extra);
                }
            }

            if ((opt & ZWIF_OPT_POLL) == 0)
            {
                node->cmd_num = 0;
            }

            //Submit request to wait thread for execution of queued commands
            result = util_list_add(nw->mtx, &nw->wait_q_req_hd,
                                   &node->nodeid, sizeof(uint8_t));
            if (result == 0)
            {
                result = ZW_ERR_QUEUED;
                plt_sem_post(nw->wait_q_sem);
            }
            else
            {
                result = ZW_ERR_MEMORY;
            }

            plt_mtx_ulck(nw->mtx);
            free(mul_cmd);
            return result;

        }

        //Check whether the command is GET command with REPORT as response
        if (zwnode_get_rpt(cmd, len, &node->wait_cmd, &node->wait_rpt))
        {
            wait_rpt = 1;

            //Restart wait report timer in GET command transmit status callback
            cb = (opt & ZWIF_OPT_POLL)? zwpoll_beam_tx_cb : zwnode_wait_tx_cb;
			user = (void *)((uintptr_t)node->nodeid);
        }
    }

    //-------------------------------------------------
    //Determine the maximum size of input
    //-------------------------------------------------
    if ((opt & ZWIF_OPT_SKIP_CHK) == 0)
    {
        if ((ifd->propty & IF_PROPTY_SECURE) == 0)
        {   //unsecure message
            if (node->crc_cap)
            {   //crc-16 message
                max_pkt_size -= CRC16_OVERHEAD;
            }
        }

        if (node->mul_cmd_ctl)
        {   //multi command message
            max_pkt_size -= 4;
        }

        if (ifd->epid > 0)
        {   //multi channel message
            max_pkt_size -= 4;
        }

        if (len > max_pkt_size)
        {
            plt_mtx_ulck(nw->mtx);
            return ZW_ERR_TOO_LARGE;
        }
    }

    //-------------------------------------------------
    // Multi-channel encapsulation
    //-------------------------------------------------

    //Determine whether the destination is an real endpoint
//  if (((opt & ZWIF_OPT_SKIP_MUL_CH) == 0)
//      && (ifd->epid > 0))
    if (ifd->epid > 0)
    {
#if 1
        //
        // Work-around for Astral 2-gang dimmer
        //
        if ((node->vid == 0x40) && (node->vtype == 0x04) && (node->pid == 0x0102))
        {   //Use destination endpoint id as source endpoint id
            prm.ep_addr.src_ep = ifd->epid;//src endpoint
        }
        prm.ep_addr.dst_ep = ifd->epid;// destination endpoint
#else
        //Use multi-channel encapsulation command
        uint8_t  mul_ch_ver;

        //Get the multi-channel version
        mul_ch_ver = node->mul_ch_ver;

        multi_ch_cmd[0] = COMMAND_CLASS_MULTI_CHANNEL_V2;
        if (mul_ch_ver == 1)
        {
            multi_ch_cmd[1] = MULTI_INSTANCE_CMD_ENCAP;
            multi_ch_cmd[2] = ifd->epid;
            memcpy(multi_ch_cmd + 3, cmd, len);

        }
        else //version 2
        {
            multi_ch_cmd[1] = MULTI_CHANNEL_CMD_ENCAP_V2;
            //
            // Work-around for Astral 2-gang dimmer
            //
            if ((node->vid == 0x40) && (node->vtype == 0x04) && (node->pid == 0x0102))
            {   //Use destination endpoint id as source endpoint id
                multi_ch_cmd[2] = ifd->epid;//src endpoint
            }
            else
            {
                multi_ch_cmd[2] = 0;//src endpoint
            }
            multi_ch_cmd[3] = ifd->epid;// destination endpoint
            memcpy(multi_ch_cmd + 4, cmd, len);
        }

        //Adjustment
        len += ((mul_ch_ver == 1)? 3 : 4);
        cmd = multi_ch_cmd;
#endif
    }

    //----------------------------------------------------------------
    // Command queuing (sleeping node) or Multi command encapsulation
    //----------------------------------------------------------------

    if (((opt & ZWIF_OPT_SKIP_MUL_Q) == 0)
        && (node->mul_cmd_ctl || node->enable_cmd_q))
    {
        int             process_cmd;    //Flag to indicate whether to continue processing the command
        mul_cmd_q_ent_t *wkup_beam_cmd = NULL;

        if ((opt & ZWIF_OPT_CMD_ENTRY) && xtra)
        {
            wkup_beam_cmd = (mul_cmd_q_ent_t *)xtra;
            process_cmd = wkup_beam_cmd->cmd_num;
        }
        else
        {
            process_cmd = (opt & ZWIF_OPT_POLL)? 1 : node->cmd_num;
        }

        if (process_cmd)
        {
            mul_cmd_q_ent_t  *mul_cmd;

            if (node->mul_cmd_ctl && !node->enable_cmd_q)
            {
                //Restart multi command timer
                plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
                node->mul_cmd_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNODE_MUL_CMD_TMOUT, zwnode_mul_cmd_tmout_cb, node);

            }

            mul_cmd = (mul_cmd_q_ent_t *)calloc(1, sizeof(mul_cmd_q_ent_t) + len);

            if (!mul_cmd)
            {
                if ((opt & ZWIF_OPT_POLL) == 0)
                {
                    node->cmd_num = 0;
                }
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_MEMORY;
            }

            mul_cmd->ifd = *ifd;
            mul_cmd->dat_len = len;
            memcpy(mul_cmd->dat_buf, cmd, len);

            //Check for command entry in xtra param
            if (wkup_beam_cmd)
            {
                mul_cmd->cmd_id = wkup_beam_cmd->cmd_id;
                mul_cmd->cmd_num = wkup_beam_cmd->cmd_num;
                mul_cmd->extra = wkup_beam_cmd->extra;

            }
            else
            {
                if (opt & ZWIF_OPT_POLL)
                {
                    mul_cmd->cmd_id = ZW_CID_POLL;
                    mul_cmd->cmd_num = 1;
                }
                else
                {
                    mul_cmd->cmd_id = node->cmd_id;
                    mul_cmd->cmd_num = node->cmd_num;
                }

                //Check for extra parameter
                if ((opt & ZWIF_OPT_Q_EXTRA) && xtra)
                {
                    mul_cmd->extra = xtra;
                }

                //Reset the command number
                if ((opt & ZWIF_OPT_POLL) == 0)
                {
                    node->cmd_num = 0;
                }
            }

            //Set wait report flag
            if (wait_rpt)
            {
                mul_cmd->wait_rpt = 1;
            }

            result = util_list_add_no_dup(nw->mtx, &node->mul_cmd_q_hd,
                                          (uint8_t *)mul_cmd, sizeof(mul_cmd_q_ent_t) + len,
                                          zwif_multi_cmd_cmp);

            if (result == 0)
            {
                result = ZW_ERR_QUEUED;
                node->mul_cmd_q_cnt++;

                if (wait_rpt)
                {
                    //Set wait report flag
                    node->wait_rpt_flg = 1;
                }
            }
            else if (result == 1)
            {   //Duplicate command
                result = ZW_ERR_QUEUED;
                if (mul_cmd->extra)
                {
                    zwif_cmd_q_xtra_rm(nw, &mul_cmd->extra);
                }
            }
            else
            {
                result = ZW_ERR_MEMORY;
            }

            plt_mtx_ulck(nw->mtx);
            free(mul_cmd);
            return result;
        }
    }

    //-------------------------------------------------
    //Release network mutex
    //-------------------------------------------------
    if (wait_rpt)
    {
        //Set wait report flag
        node->wait_rpt_flg = 1;
    }

#ifdef CRC16_ENCAP
    crc16_cap = node->crc_cap;
#endif

    plt_mtx_ulck(nw->mtx);


    //-------------------------------------------------
    // Security message encapsulation
    //-------------------------------------------------
    if ((ifd->propty & IF_PROPTY_SECURE)
        || (opt & ZWIF_OPT_SECURE))
    {
        prm.flag = ZIP_FLAG_SECURE;
    }


#ifdef CRC16_ENCAP
    //-------------------------------------------------------------------------------------
    // CRC-16 encapsulation
    // Note: CRC-16 encapsulation and security message encapsulation are mutually exclusive
    //-------------------------------------------------------------------------------------

    else if (crc16_cap)
    {
        uint16_t crc;

        //Encapsulate with CRC-16
        crc16_cmd[0] = COMMAND_CLASS_CRC_16_ENCAP;
        crc16_cmd[1] = CRC_16_ENCAP;
        memcpy(crc16_cmd + 2, cmd, len);
        crc = zwutl_crc16_chk(CRC_INIT, crc16_cmd, len + 2);
        crc16_cmd[len + 2] = (crc >> 8);
        crc16_cmd[len + 3] = (crc & 0x00ff);

        //Adjust command buffer pointer
        cmd = crc16_cmd;
        len += CRC16_OVERHEAD;
    }
#endif


    //-------------------------------------------------
    // Send the command through Z/IP router
    //-------------------------------------------------
    prm.dat_len = len;
    prm.dat_buf = cmd;
    prm.node_id = ifd->nodeid;

#ifdef WKUP_BY_MAILBOX_ACK
    if (sleep_cap)
    {
        prm.flag |= ZIP_FLAG_MORE_INFO;
    }
#endif

    //Force every command sent to the Z/IP router to require ACK before sending the next command.
    //This is important especially sending security encapsulation messages.
    if (cb == NULL)
    {
        cb = zwif_tx_sts_cb;
    }

    result = zw_send_data(&nw->appl_ctx, &prm, cb, user);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwif_exec with error:%d", result);
        if (result == APPL_ERROR_WAIT_CB)
        {
            result = ZW_ERR_BUSY;
        }
        else
        {
            result = ZW_ERR_OP_FAILED;
        }

        //For node that requires wakeup beam
        if (wait_rpt)
        {   //Undo set node->wait_rpt_flg
            zwnode_wait_rpt_flg_clr(nw, ifd->nodeid);
        }
    }

    if ((result == 0) && wait_rpt)
    {
        result = ZW_ERR_QUEUED;
    }

    return result;
}


/**
zwif_exec - execute action on an interface
@param[in]	ifdesc      interface
@param[in]	cmd_buf		command and parameters
@param[in]	buf_len		length of cmd in bytes
@param[in]	cb		    callback function for transmit status
@return	ZW_ERR_XXX
*/
int zwif_exec(zwifd_p ifdesc, uint8_t *cmd_buf, int buf_len, tx_cmplt_cb_t cb)
{
    return zwif_exec_ex(ifdesc, cmd_buf, buf_len, cb, NULL, 0, NULL);
}


/**
zwif_get_report_ex - get interface report through report callback
@param[in]	ifd	        interface
@param[in]	param	    Parameter for the report get command
@param[in]	len     	Length of param
@param[in]	get_rpt_cmd Command to get the report
@param[in]	cb		    callback function for transmit status
@param[in]	flag		flag, ZWIF_RPT_GET_XXX
@return		ZW_ERR_xxx
*/
int zwif_get_report_ex(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd, tx_cmplt_cb_t cb, int flag)
{
    int     result;
    int     hdr_len;
    uint8_t cmd[32];

    //Check if the parameter length
    if (len > 29)
    {
        return ZW_ERR_MEMORY;
    }

    //Check for extended command class (2-byte command class)
    if (ifd->cls & 0xFF00)
    {
        hdr_len = 3;
        cmd[0] = ifd->cls >> 8 ;
        cmd[1] = (ifd->cls & 0x00FF);
        cmd[2] = get_rpt_cmd;
    }
    else
    {
        hdr_len = 2;
        cmd[0] = (uint8_t)ifd->cls;
        cmd[1] = get_rpt_cmd;
    }

    if (len > 0)
    {
        memcpy(cmd + hdr_len, param, len);
    }

    result = zwif_exec_ex(ifd, cmd, len + hdr_len, cb, NULL,
                          (flag == ZWIF_RPT_GET_ZIP)? ZWIF_OPT_GW_PORTAL : 0, NULL);

    if (result < 0)
    {
        debug_zwapi_msg(&ifd->net->plt_ctx, "zwif_get_report with error:%d", result);
        return ZW_ERR_OP_FAILED;
    }
    return result;

}


/**
zwif_get_report - get interface report through report callback
@param[in]	ifd	        interface
@param[in]	param	    Parameter for the report get command
@param[in]	len     	Length of param
@param[in]	get_rpt_cmd Command to get the report
@param[in]	cb		    callback function for transmit status
@return		ZW_ERR_xxx
*/
int zwif_get_report(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd, tx_cmplt_cb_t cb)
{
    return zwif_get_report_ex(ifd, param, len, get_rpt_cmd, cb, ZWIF_RPT_GET_NORM);
}


/**
@}
*/

