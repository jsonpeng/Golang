package zwave

//command class
const (
	COMMAND_CLASS_ZIP_ND                       = 0x58
	COMMAND_CLASS_ZIP                          = 0x23
	COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY     = 0x52
	COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION = 0x34
	COMMAND_CLASS_SWITCH_BINARY                = 0x25
)

//command 0x23
const (
	COMMAND_ZIP_PACKET              = 0x02
	COMMAND_NODE_INFO_CACHED_GET    = 0x03
	COMMAND_NODE_INFO_CACHED_REPORT = 0x04
)

//command  0x58
const (
	COMMAND_NODE_LIST_GET    = 0x01
	COMMAND_NODE_LIST_REPORT = 0x02
)

//command 0x52
const (
	COMMAND_ZIP_NODE_ADVERTISEMENT    = 0x01
	COMMAND_ZIP_NODE_SOLICITATION     = 0x03
	COMMAND_ZIP_INV_NODE_SOLICITATION = 0x04
)

//command 0x34
const (
	COMMAND_NODE_ADD           = 0x01
	COMMAND_NODE_ADD_STATUS    = 0x02
	COMMAND_NODE_REMOVE        = 0x03
	COMMAND_NODE_REMOVE_STATUS = 0x04
)

//command 0x25
const (
	SWITCH_BINARY_SET    = 0x01
	SWITCH_BINARY_GET    = 0x02
	SWITCH_BINARY_REPORT = 0x03
)
