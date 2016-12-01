

#ifndef __APPL_H__
#define __APPL_H__


#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <zip_api.h>


typedef struct hl_appl_ctx{

    uint8_t *gw_addr_buf;   ///< Gateway addresses buffer
    char    **gw_name;      ///< Gateway names corresponding to entries in gw_addr_buf
    uint8_t gw_addr_cnt;    ///< Number of gateway addresses in gw_addr_buf

    zwnet_p zwnet;         ///< Network handle
    int     use_ipv4;       ///< Flag to indicate whether to use IPv4 or IPv6. 1=IPv4; 0=IPv6

    volatile int init_status;   ///< Network initialization status. 0=unknown; 1=done
    volatile int rm_status;     ///< Remove node status.  0=unknown; 1=in progress; 2=done
    volatile int rst_status;    ///< Reset network status.  0=unknown; 1=in progress; 2=done
    volatile int add_status;    ///< Add node status.  0=unknown; 1=in progress; 2=done

    volatile unsigned   sec2_cb_enter;///< Control security 2 callback entry bitmask, see SEC2_ENTER_XXX. bit set = allowed callback, 0 = not allowed
    volatile int    sec2_cb_exit;  ///< Security 2 callback status. 1 = exited callback, 0 = waiting or still in the callback
    uint8_t         sec2_add_node; ///< Flag to determine whether to use security 2 when adding node
    sec2_add_prm_t  sec2_add_prm;  ///< Add node with security 2 parameters

    zwnoded_t   node[10];
} hl_appl_ctx_t;
//typedef struct hl_appl_ctx hl_appl_ctx_t;

#define  MAX_DTLS_PSK       64                 //Maximum DTLS pre-shared key hex string length


typedef enum node_status{
    ADD_NODE_STS_UNKNOWN = 0,     ///<Add node status: unknown
    ADD_NODE_STS_PROGRESS,        ///<Add node status: in progress
    ADD_NODE_STS_DONE,            ///<Add node status: done

    RM_NODE_STS_UNKNOWN=0,          ///<Remove node status: unknown
    RM_NODE_STS_PROGRESS,         ///<Remove node status: in progress
    RM_NODE_STS_DONE,             ///<Remove node status: done

    RESET_NW_STS_UNKNOWN=0,       ///<Reset network status: unknown
    RESET_NW_STS_PROGRESS,      ///<Reset network status: in progress
    RESET_NW_STS_DONE,          ///<Reset network status: done
}node_status_t;

enum sec2{
	SEC2_ENTER_KEY_REQ = 1,
	SEC2_ENTER_DSK,
};

extern hl_appl_ctx_t ApplCtx ;

extern  void hl_nw_tx_cb(void *user, uint8_t tx_sts);
extern  void hl_nw_node_cb(void *user, zwnoded_p noded, int mode);
extern  void hl_nw_notify_cb(void *user, uint8_t op, uint16_t sts);
extern  int hex2bin(char c);
extern  int hexstring_to_bin(char *psk_str, int psk_len, uint8_t *psk_bin);
extern  int config_param_get(char *cfg_file, uint16_t *host_port, char *router, char *psk);
extern  int lib_init(hl_appl_ctx_t *hl_appl, uint16_t host_port, uint8_t *zip_router_ip, int use_ipv4,
          char *dev_cfg_file_name, uint8_t *dtls_psk, uint8_t dtls_psk_len, char *pref_dir);
extern  int nw_init(hl_appl_ctx_t *hl_appl);
extern  char  *prompt_str(hl_appl_ctx_t *hl_appl, const char *disp_str, int out_buf_sz, char *out_str);
extern  unsigned prompt_hex(hl_appl_ctx_t *hl_appl, char *str);
extern  char prompt_char(hl_appl_ctx_t *hl_appl, char *str);
extern  int prompt_yes(hl_appl_ctx_t *hl_appl, char *str);
extern  void hl_add_node_s2_cb(void *usr_param, sec2_add_cb_prm_t *cb_param);
extern  int hl_add_node(hl_appl_ctx_t *hl_appl);
extern  void gw_discvr_cb(uint8_t *gw_addr, uint8_t gw_addr_cnt, int ipv4, void *usr_param, int rpt_num, int total_rpt, char **gw_name);
extern  void get_addr_buf(uint8_t * buf, uint16_t buf_size, uint8_t addr[]);

extern int nwInit(void);


#define APPL_DEBUG

#ifdef APPL_DEBUG
#define APPL_WARNING(x,arg...)          printf("[APPL_WARNING] "x,##arg)
#define APPL_DBG(x,arg...)              printf(x,##arg)		//printf("[APPL_DBG] "x,##arg)
//#define APPL_DBG(...)              	printf("[APPL_DBG] "__VA_ARGS__)
//#define APPL_DBG(...)              	printf(__VA_ARGS__)
//#define APPL_DBG(x, ...)              	printf(x, __VA_ARGS__)

#define APPL_ERR(x,arg...)              printf("[APPL_ERR] "x,##arg)
#define APPL_MSG(x,arg...)              printf("[APPL_MSG] "x,##arg)


#else
#define APPL_WARNING(x,arg...)
#define APPL_DBG(x,arg...)
#define APPL_ERR(x,arg...)
#define APPL_MSG(x,arg...)
#endif



#endif