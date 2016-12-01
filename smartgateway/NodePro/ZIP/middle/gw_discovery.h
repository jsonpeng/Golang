#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../../include/zip_api.h"


typedef struct
{
    int     use_ipv4;       ///< Flag to indicate whether to use IPv4 or IPv6. 1=IPv4; 0=IPv6
    uint8_t *gw_addr_buf;   ///< Gateway addresses buffer
    char    **gw_name;      ///< Gateway names corresponding to entries in gw_addr_buf
    uint8_t gw_addr_cnt;    ///< Number of gateway addresses in gw_addr_buf

} hl_appl_ctx_t;


int gw_discovery();
