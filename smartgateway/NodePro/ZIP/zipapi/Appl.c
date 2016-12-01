
#include <Appl.h>


hl_appl_ctx_t ApplCtx ;


/**
hl_nw_tx_cb - Callback function to notify application transmit data status
@param[in]	user	    The high-level api context
param[in]	tx_sts	    Transmit status ZWNET_TX_xx
@return
*/
/**/
void hl_nw_tx_cb(void *user, uint8_t tx_sts)
{
    static const char    *tx_cmplt_sts[] = {"ok",
        "timeout: no ACK received",
        "system error",
        "destination host needs long response time",
        "frame failed to reach destination host"
    };

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        printf("Higher level appl send data completed successfully\n");
    }
    else
    {
        printf("Higher level appl send data completed with error:%s\n",
               (tx_sts < sizeof(tx_cmplt_sts)/sizeof(char *))? tx_cmplt_sts[tx_sts]  : "unknown");
    }
}

/**
hl_nw_node_cb - Callback function to notify node is added, deleted, or updated
@param[in]	user	    The high-level api context
@param[in]	noded	Node
@param[in]	mode	    The node status
@return
*/
/**/
void hl_nw_node_cb(void *user, zwnoded_p noded, int mode)
{
    printf("Callback function to notify node is added, deleted, or updated\n");
    switch (mode)
    {
        case ZWNET_NODE_ADDED:
            {
                printf("\nNode:%u added\n", (unsigned)noded->nodeid);
            }
            break;

        case ZWNET_NODE_REMOVED:
            {
                printf("\nNode:%u removed\n", (unsigned)noded->nodeid);
            }
            break;
        case ZWNET_NODE_UPDATED:
            {
                printf("\nNode:%u updated\n", (unsigned)noded->nodeid);
            }
            break;
        case ZWNET_NODE_STATUS_ALIVE:
            {
                printf("\nNode:%u alive\n", (unsigned)noded->nodeid);
            }
            break;
        case ZWNET_NODE_STATUS_DOWN:
            {
                printf("\nNode:%u down or sleep\n", (unsigned)noded->nodeid);
            }
            break;

        default:
            printf("Node mode unknown\n");
            break;
    }
}

/**
hl_nw_notify_cb - Callback function to notify the status of current operation
@param[in]	user	The high-level api context
@param[in]	op		Network operation ZWNET_OP_XXX
@param[in]	sts		The status of current operation
@return
*/
/**/
void hl_nw_notify_cb(void *user, uint8_t op, uint16_t sts)
{
    hl_appl_ctx_t *hl_appl = (hl_appl_ctx_t *)user;

    //Check whether the status is progress status of discovering each detailed node information
    if (sts & OP_GET_NI_TOTAL_NODE_MASK)
    {
        uint16_t    total_nodes;
        uint16_t    cmplt_nodes;

        total_nodes = (sts & OP_GET_NI_TOTAL_NODE_MASK) >> 8;
        cmplt_nodes = sts & OP_GET_NI_NODE_CMPLT_MASK;
        printf("Get node info %u/%u completed\n", cmplt_nodes, total_nodes);
        return;
    }

    switch (op)
    {
        case ZWNET_OP_INITIALIZE:
            APPL_DBG("\nInitialization status:%u\n",(unsigned)sts);
            if (sts == OP_DONE)
            {
                zwnetd_p nw_desp;
                nw_desp = zwnet_get_desc(hl_appl->zwnet);
                printf("network id:%08X, Z/IP controller id:%u\n", nw_desp->id, nw_desp->ctl_id);
                hl_appl->init_status = 1;
                // APPL_MSG("\n(1) Add node\n(x) Exit\n");
                // APPL_MSG("Select your choice:\n");
                APPL_DBG("\nInitialization status: DONE\n");
            }
            else
            {
                APPL_MSG("Press 'x' to exit ...\n");
            }
            break;

        case ZWNET_OP_ADD_NODE:
            APPL_DBG("Add node status:%u\n",(unsigned)sts);
            if (sts == OP_DONE)
            {   //Clear add node DSK callback control & status
                hl_appl->sec2_cb_enter = 0;
                hl_appl->sec2_cb_exit = 1;

                hl_appl->add_status = ADD_NODE_STS_DONE;
            }
            else if (sts == OP_FAILED)
            {   //Clear add node DSK callback control & status
                hl_appl->sec2_cb_enter = 0;
                hl_appl->sec2_cb_exit = 1;

                hl_appl->add_status = ADD_NODE_STS_UNKNOWN;
            }


            if (hl_appl->add_status != ADD_NODE_STS_PROGRESS)
            {
                // APPL_MSG("\n(1) Add node\n(x) Exit\n");
                // APPL_MSG("Select your choice:\n");
            }
            break;

        case ZWNET_OP_RM_NODE:
            printf("Remove node status:%u\n",(unsigned)sts);
            if (sts == OP_DONE)
            {
                hl_appl->rm_status = RM_NODE_STS_DONE;
            }
            else if (sts == OP_FAILED)
            {
                hl_appl->rm_status = RM_NODE_STS_UNKNOWN;
            }


            if (hl_appl->rm_status != RM_NODE_STS_PROGRESS)
            {
                // printf("\n(1) Remove node\n(x) Exit\n");
                // printf("Select your choice:\n");
            }
            break;

        case ZWNET_OP_RESET:
            printf("Reset status:%u\n",(unsigned)sts);
            if (sts == OP_DONE)
            {
                hl_appl->rst_status = RESET_NW_STS_DONE;
            }
            else if (sts == OP_FAILED)
            {
                hl_appl->rst_status = RESET_NW_STS_UNKNOWN;
            }

            if (hl_appl->rst_status != RESET_NW_STS_PROGRESS)
            {
                printf("Press 'x' to exit ...\n");
            }
            break;
        default:
            printf("hl_nw_notify_cb op:%u, status:%u\n", (unsigned)op, (unsigned)sts);
    }
}
/**/




/**
hex2bin - Convert hex character to binary
@param[in] c        hex character
@return  Value of hex character on success, negative value on failure
*/
int hex2bin(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c-'0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c-'a' + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c-'A' + 10;
    }
    else
    {
        return -1;
    }
}


/**
hexstring_to_bin - Convert ASCII hexstring to binary string
@param[in] psk_str   ASCII hexstring
@param[in] psk_len   ASCII hexstring length (must be even number)
@param[out] psk_bin  Binary string
@return  Zero on success, non-zero on failure
*/
int hexstring_to_bin(char *psk_str, int psk_len, uint8_t *psk_bin)
{
    int i = 0;
    int val;

    while(psk_len > 0)
    {
      val = hex2bin(*psk_str++);
      if(val < 0)
          return -1;
      psk_bin[i]  = (val & 0x0F) << 4;

      val = hex2bin(*psk_str++);
      if(val < 0)
          return -1;
      psk_bin[i] |= (val & 0x0F);

      i++;
      psk_len -= 2;
    }

    return 0;
}

/**
config_param_get - get configuration parameters
@param[in] cfg_file     Configuration file name
@param[out] host_port   Host port
@param[out] router      Z/IP router IP address
@param[out] psk         DTLS pre-shared key (PSK)
@return         ZW_ERR_XXX
*/
int config_param_get(char *cfg_file, uint16_t *host_port, char *router, char *psk)
{
    FILE        *file;
    const char  delimiters[] = " =\r\n";
    char        line[384];
    char        *prm_name;
    char        *prm_val;

    //Initialize output
    *router = '\0';
    *psk = '\0';
    *host_port = 0;

    //Open config file
    if (!cfg_file)
    {
        return ZW_ERR_FILE_OPEN;
    }

    file = fopen(cfg_file, "rt");
    if (!file)
    {
        return ZW_ERR_FILE_OPEN;
    }

    while (fgets(line, 384, file))
    {
        if (*line == '#')
        {   //Skip comment line
            continue;
        }

        //Check if '=' exists
        if (strchr(line, '='))
        {
            //Get the parameter name and value
            prm_name = strtok(line, delimiters);

            if (prm_name)
            {
                prm_val = strtok(NULL, delimiters);

                if (!prm_val)
                {
                    continue;
                }

                //Compare the parameter name
                if (strcmp(prm_name, "ZipLanPort") == 0)
                {
                    unsigned port;
                    if (sscanf(prm_val, "%u", &port) == 1)
                    {
                        *host_port = (uint16_t)port;
                    }
                }
                else if (strcmp(prm_name, "ZipRouterIP") == 0)
                {
                    strcpy(router, prm_val);
                }
                else if (strcmp(prm_name, "DTLSPSK") == 0)
                {
                    strcpy(psk, prm_val);
                }
            }
        }
    }

    fclose(file);

    return 0;
}


/**
lib_init - Initialize library
@param[in]	hl_appl		        The high-level api context
@param[in]	host_port		    Host listening port
@param[in]	zip_router_ip		Z/IP router IP address in numerical form
@param[in]	use_ipv4		    Flag to indicate zip_router_ip is in IPv4 or IPv6 format. 1= IPv4; 0= IPv6
@param[in]	dev_cfg_file_name	Device specific configuration database file name
@param[in]	dtls_psk		    DTLS pre-shared key
@param[in]	dtls_psk_len		DTLS pre-shared key length (in bytes)
@param[in]	pref_dir		    Full path of directory for storing network/user preference files
@return  0 on success, negative error number on failure
*/
extern void GoNwNodeCB(void *user, zwnoded_p noded, int mode);
extern void GoNwNotifyCB(void *user, uint8_t op, uint16_t sts);
extern void GoNwTxCB(void *user, uint8_t tx_sts);
/**/
int lib_init(hl_appl_ctx_t *hl_appl, uint16_t host_port, uint8_t *zip_router_ip, int use_ipv4,
             char *dev_cfg_file_name, uint8_t *dtls_psk, uint8_t dtls_psk_len, char *pref_dir)
{
    int                 result;
    zwnet_init_t        zw_init = {0};

    zw_init.user = hl_appl; //high-level application context
	
//     zw_init.node = hl_nw_node_cb;
//    zw_init.notify = hl_nw_notify_cb;
//    zw_init.appl_tx = hl_nw_tx_cb;

    zw_init.node = GoNwNodeCB;
    zw_init.notify = GoNwNotifyCB;
	zw_init.appl_tx = GoNwTxCB;
	
    zw_init.pref_dir = pref_dir;
    //zw_init.print_txt_fn = printf;
    zw_init.net_info_dir = "/home/eric/Desktop/Go/SmartGateway/HAL/src/HAL";
    zw_init.host_port = host_port;
    zw_init.use_ipv4 = use_ipv4;
    memcpy(zw_init.zip_router, zip_router_ip, (use_ipv4)? IPV4_ADDR_LEN : IPV6_ADDR_LEN);
    zw_init.dev_cfg_file = dev_cfg_file_name;
    zw_init.dev_cfg_usr = NULL;
    zw_init.dtls_psk_len = dtls_psk_len;
    if (dtls_psk_len)
    {
        memcpy(zw_init.dtls_psk, dtls_psk, dtls_psk_len);
    }
    //Unhandled command handler
    zw_init.unhandled_cmd = NULL;

    //Init ZW network
    result = zwnet_init(&zw_init, &hl_appl->zwnet);

    if (result != 0)
    {
        printf("zwnet_init with error:%d\n", result);

        //Display device configuration file error
        if (zw_init.err_loc.dev_ent)
        {
            printf("Parsing device configuration file error loc:\n");
            printf("Device entry number:%u\n", zw_init.err_loc.dev_ent);
            if (zw_init.err_loc.ep_ent)
            {
                printf("Endpoint entry number:%u\n", zw_init.err_loc.ep_ent);
            }

            if (zw_init.err_loc.if_ent)
            {
                printf("Interface entry number:%u\n", zw_init.err_loc.if_ent);
            }
        }
        return result;
    }

    return 0;
}
/**/


/**
nw_init - initialization network
@param[in] hl_appl   user application context
@return  0 on success; otherwise negative number
*/
/**/
extern int CConfigParamGet(char *cfg_file, uint16_t *host_port, char *router, char *psk);

int nw_init(hl_appl_ctx_t *hl_appl)
{
    int             ret;
    char            zip_gw_addr_str[100];
    uint16_t        host_port;                  ///< Host listening port
    char            psk_str[384];
    int             psk_len;
    uint8_t         dtls_psk[MAX_DTLS_PSK/2];   ///< DTLS pre-shared key
    uint8_t         zip_gw_ip[16];              ///< Z/IP gateway address in IPv4 or IPv6
    
    //Read config file to get configuration parameters
    ret = config_param_get("app.cfg", &host_port, zip_gw_addr_str, psk_str);
//ret = CConfigParamGet("config.json", &host_port, zip_gw_addr_str, psk_str);
    if (ret != 0)
    {
        printf("Error: couldn't get config param from file: app.cfg\n");
        return ret;
    }

    //Check DTLS pre-shared key validity
    psk_len = strlen(psk_str);

    if (psk_len > 0)
    {
        if (psk_len > MAX_DTLS_PSK)
        {
            printf("PSK string length is too long\n");
            return ZW_ERR_VALUE;
        }
        if (psk_len % 2)
        {
            printf("PSK string length should be even\n");
            return ZW_ERR_VALUE;
        }
        //Convert ASCII hexstring to binary string
        ret = hexstring_to_bin(psk_str, psk_len, dtls_psk);
        if (ret != 0)
        {
            printf("PSK string is not hex string\n");
            return ZW_ERR_VALUE;
        }
    }
printf("psk_str:%s\n",psk_str);
printf("psk_len:%d\n",psk_len);
//for(i = 0;i< psk_len;i++){
//	printf("dtls_psk[%d]=%x\n",i,dtls_psk[i]);
//}



    //Convert IPv4 / IPv6 address string to numeric equivalent
    ret = zwnet_ip_aton(zip_gw_addr_str, zip_gw_ip, &hl_appl->use_ipv4);

    if (ret != 0)
    {
        printf("Invalid Z/IP router IP address:%s\n", zip_gw_addr_str);
        return ZW_ERR_IP_ADDR;
    }
	printf("Z/IP router IP address:%s\n", zip_gw_addr_str);

    //Initialize library
    ret = lib_init(hl_appl, host_port, zip_gw_ip, hl_appl->use_ipv4, "zwave_device_rec.txt" ,
                  dtls_psk, psk_len/2, NULL);

    if (ret < 0)
    {
        printf("lib_init with error: %d\n", ret);
    }
    return ret;
}
/**/
int nwInit(void)
{
	return (nw_init(&ApplCtx));
}
/*                               Add Node                            */
/**
prompt_str - prompt for a string from user
@param[in] disp_str   The prompt string to display
@param[in] out_buf_sz The size of out_str buffer
@param[out] out_str   The buffer where the user input string to be stored
@return          The out_str if successful; else NULL.
*/
char  *prompt_str(hl_appl_ctx_t *hl_appl, const char *disp_str, int out_buf_sz, char *out_str)
{
    int retry;
    puts(disp_str);
    retry = 3;
    while (retry-- > 0)
    {
        if (fgets(out_str, out_buf_sz, stdin) && (*out_str) && ((*out_str) != '\n'))
        {
            char *newline;
            //Remove newline character

            newline = strchr(out_str, '\n');
            if (newline)
            {
                *newline = '\0';
            }
            return out_str;
        }
    }
    return NULL;
}

/**
prompt_hex - prompt for an hexadecimal unsigned integer input from user
@param[in] str   The prompt string to display
@return          The unsigned integer that user has input
*/
unsigned prompt_hex(hl_appl_ctx_t *hl_appl, char *str)
{
    char user_input_str[36];
    unsigned  ret;

    if (prompt_str(hl_appl, str, 36, user_input_str))
    {
#ifdef USE_SAFE_VERSION
        if (sscanf_s(user_input_str, "%x", &ret) == 1)
        {
            return ret;
        }
#else
        if (sscanf(user_input_str, "%x", &ret) == 1)
        {
            return ret;
        }
#endif
    }
    return 0;
}

/**
prompt_char - prompt for a character input from user
@param[in] str   The prompt string to display
@return          The character that user has input. Null character on error.
*/
char prompt_char(hl_appl_ctx_t *hl_appl, char *str)
{
    char ret[80];

    if (prompt_str(hl_appl, str, 80, ret))
    {
        return ret[0];
    }
    return 0;
}



/**
prompt_yes - prompt for yes or no from user
@param[in] str   The prompt string to display
@return          1 = user has input yes, 0 =  user has input no
*/
int prompt_yes(hl_appl_ctx_t *hl_appl, char *str)
{
    char c;

    c = prompt_char(hl_appl, str);
    if (c == 'y' || c == 'Y')
    {
        return 1;
    }
    return 0;
}



/**
hl_add_node_s2_cb - callback for add node with security 2 operation to report Device Specific Key (DSK)
@param[in]	usr_param  user supplied parameter when calling zwnet_add
@param[in]	cb_param   DSK related callback parameters
*/
void hl_add_node_s2_cb(void *usr_param, sec2_add_cb_prm_t *cb_param)
{
    hl_appl_ctx_t *hl_appl = (hl_appl_ctx_t *)usr_param;
    int           res;

    if (cb_param->cb_type == S2_CB_TYPE_REQ_KEY)
    {
        uint8_t granted_key;

        if (hl_appl->sec2_cb_enter & SEC2_ENTER_KEY_REQ)
        {   //Requested keys callback is allowed
            hl_appl->sec2_cb_enter &= ~SEC2_ENTER_KEY_REQ;
        }
        else
        {
            printf("\nNot allowed to processed Security 2 requested keys callback!\n");
            return;
        }

        printf("\nDevice requested keys bit-mask: %02Xh\n", cb_param->cb_prm.req_key);

        printf("Key (bit-mask in hex) :\n");
        printf("                      Security 2 key 0 (01)\n");
        printf("                      Security 2 key 1 (02)\n");
        printf("                      Security 2 key 2 (04)\n");
        printf("                      Security 0       (80)\n");

        granted_key = prompt_hex(hl_appl, "Grant keys bit-mask (hex):");

        res = zwnet_add_sec2_grant_key(hl_appl->zwnet, granted_key);

        if (res != 0)
        {
            printf("zwnet_add_sec2_grant_key with error: %d\n", res);
        }

        //Check whether if there is DSK callback pending
        if (!(hl_appl->sec2_cb_enter))
        {   //No callback pending
            hl_appl->sec2_cb_exit = 1;
        }
    }
    else
    {
        sec2_dsk_cb_prm_t   *dsk_prm;
        int                 accept;
        char                dsk_str[200];

        if (hl_appl->sec2_cb_enter & SEC2_ENTER_DSK)
        {   //DSK callback is allowed
            hl_appl->sec2_cb_enter &= ~SEC2_ENTER_DSK;
        }
        else
        {
            printf("\nNot allowed to processed Security 2 DSK callback!\n");
            return;
        }

        dsk_prm = &cb_param->cb_prm.dsk;

        if (dsk_prm->pin_required)
        {
            printf("\nReceived DSK: XXXXX%s\n", dsk_prm->dsk);
        }
        else
        {
            printf("\nReceived DSK: %s\n", dsk_prm->dsk);
        }

        accept = prompt_yes(hl_appl, "Do you accept this device to be added securely (y/n)?:");

        printf("You %s the device.\n", (accept)? "accepted" : "rejected");

        if (accept && dsk_prm->pin_required)
        {
            if (prompt_str(hl_appl, "Enter 5-digit PIN that matches the received DSK:", 200, dsk_str))
            {

#ifdef USE_SAFE_VERSION
                strcat_s(dsk_str, 200, dsk_prm->dsk);
#else
                strcat(dsk_str, dsk_prm->dsk);
#endif
            }
        }

        res = zwnet_add_sec2_accept(hl_appl->zwnet, accept, (dsk_prm->pin_required)? dsk_str : dsk_prm->dsk);

        if (res != 0)
        {
            printf("zwnet_add_sec2_accept with error: %d\n", res);
        }

        hl_appl->sec2_cb_exit = 1;
    }
}


/**
hl_add_node - Add node
@param[in]	hl_appl  Application context
@return zero if successful; else negative error number
*/
int hl_add_node(hl_appl_ctx_t *hl_appl)
{
    int     res;
//    char    dsk_str[200];
    zwnetd_p netdesc;

    netdesc = zwnet_get_desc(hl_appl->zwnet);

    if (netdesc->ctl_cap & ZWNET_CTLR_CAP_S2)
    {
        printf("Controller supports security 2.\n");
        hl_appl->sec2_add_node = 1;
    }
    else
    {
        hl_appl->sec2_add_node = 0;
    }

    if (hl_appl->sec2_add_node)
    {
        hl_appl->sec2_add_prm.dsk = NULL;

// TODO I don't know what's this purpose:DSK, Security 2(S2)
//      So make prompt_no
        // if (prompt_yes(hl_appl, "Pre-enter Device Specific Key (DSK) (y/n)?:"))
        // {
        //     if (prompt_str(hl_appl, "DSK:", 200, dsk_str))
        //     {
        //         hl_appl->sec2_add_prm.dsk = dsk_str;
        //     }
        // }

        hl_appl->sec2_add_prm.usr_param = hl_appl;

        hl_appl->sec2_add_prm.cb = hl_add_node_s2_cb;

    }

    res = zwnet_add(hl_appl->zwnet, 1, (hl_appl->sec2_add_node)? &hl_appl->sec2_add_prm : NULL);

    if (res == 0)
    {
        if (hl_appl->sec2_add_node)
        {
            int wait_count;

            hl_appl->sec2_cb_enter = SEC2_ENTER_KEY_REQ;

            if (!hl_appl->sec2_add_prm.dsk)
            {   //No pre-entered DSK, requires DSK callback
                hl_appl->sec2_cb_enter |= SEC2_ENTER_DSK;
            }

            hl_appl->sec2_cb_exit = 0;

            printf("Waiting for Requested keys and/or DSK callback in 60 seconds...\n");

            //Wait for S2 callback to exit
            wait_count = 600;    //Wait for 60 seconds
            while (wait_count-- > 0)
            {
                if (hl_appl->sec2_cb_exit == 1)
                    break;
                plt_sleep(100);
                printf("Waiting for Requested keys and/or DSK callback in %d seconds...\n",wait_count/10);
            }
            // printf("Waiting without timetick\n");
        }
    }

    return res;

}


/*                               Gateway Discovery                            */
/**
gw_discvr_cb - gateway discovery callback
@param[in]	gw_addr	    Gateway addresses
@param[in]	gw_addr_cnt Number of gateway addresses returned in gw_addr
@param[in]	ipv4        Flag to indicate the addr parameter is IPv4 or IPv6. 1=IPv4; 0=IPv6
@param[in]	usr_param   User defined parameter used in callback function
@param[in]	rpt_num     Report number that this callback is delivering the gateway addresses report; start from 1
@param[in]	total_rpt   Total reports that will be delivered by callbacks. Each callback delivers one report.
@param[in]	gw_name	    Gateway names corresponding to the gw_ip.  If NULL, it means gateway name information is unavailable.
*/
void gw_discvr_cb(uint8_t *gw_addr, uint8_t gw_addr_cnt, int ipv4, void *usr_param, int rpt_num, int total_rpt, char **gw_name){
    hl_appl_ctx_t   *hl_appl = (hl_appl_ctx_t *)usr_param;
    uint8_t         *tmp_buf;
    char            **tmp_gw_name;
    int             gw_addr_buf_sz;
    int             i;
    int             result;
    int             one_ip_addr_len;    //number of bytes required for storing one IP address. IPv4=4, IPv6=16
    char            addr_str[80];

    printf("\nReceived report:%d/%d with gw count:%u\n", rpt_num, total_rpt, gw_addr_cnt);

    if (total_rpt == 0) {
        printf("The system has no valid IP, please configure it.\n");
        return;
    }

    one_ip_addr_len = (ipv4)? 4 : 16;

    if (gw_addr_cnt > 0){
        //Calculate IP address buffer size for storing new found gateways from this report
        gw_addr_buf_sz = (gw_addr_cnt * one_ip_addr_len);

        if (hl_appl->gw_addr_buf){
		//Expand buffer to store new found gateways from this report
            tmp_buf = realloc(hl_appl->gw_addr_buf, (hl_appl->gw_addr_cnt * one_ip_addr_len) + gw_addr_buf_sz);
        }
        else{
		//Allocate buffer to store new found gateways
            tmp_buf = malloc(gw_addr_buf_sz);
        }

        if (gw_name){
		//Gateway names are available
            if (hl_appl->gw_name){
			//Expand buffer to store new found gateways from this report
                tmp_gw_name = (char **)realloc(hl_appl->gw_name, (hl_appl->gw_addr_cnt + gw_addr_cnt)*sizeof(char *));
            }
            else{
			//Allocate buffer to store new found gateways
                tmp_gw_name = (char **)malloc(gw_addr_cnt * sizeof(char *));
            }
            if (tmp_gw_name){
			//Save gateway names
                hl_appl->gw_name = tmp_gw_name;
                for (i=0; i<gw_addr_cnt; i++){
                    tmp_gw_name[hl_appl->gw_addr_cnt + i] = strdup(gw_name[i]);
                }
            }
        }

        if (tmp_buf){
		//Save gateway IP addresses
            hl_appl->gw_addr_buf = tmp_buf;
            memcpy(hl_appl->gw_addr_buf + (hl_appl->gw_addr_cnt * one_ip_addr_len), gw_addr, gw_addr_buf_sz);
            hl_appl->gw_addr_cnt += gw_addr_cnt;
        }
    }

    if (hl_appl->gw_addr_cnt > 0){
        //Display gateway ip addresses
        printf("\n---Gateways found---\n");
        for (i=0; i<hl_appl->gw_addr_cnt; i++){
            result = zwnet_ip_ntoa(hl_appl->gw_addr_buf + (i * one_ip_addr_len), addr_str, 80, ipv4);
            if (result == 0){
                printf("(%d) %s [%s]\n", i, addr_str, (hl_appl->gw_name)? hl_appl->gw_name[i] : "unknown");
            }
        }
    }
}


void get_addr_buf(uint8_t * buf, uint16_t buf_size, uint8_t addr[]){
	uint16_t i;
	for(i = 0; i < buf_size; i++){
		addr[i] = buf[i];
	}

}
