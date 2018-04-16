// DHCP Library v0.3 - April 25, 2009
// Author: Jordan Terrell - blog.jordanterrell.com

#ifndef _DHCP_H_
#define _DHCP_H_
#include "uip_udp.h"

static constexpr uint8_t
    STATE_DHCP_START = 0,
    STATE_DHCP_DISCOVER = 1,
    STATE_DHCP_REQUEST = 2,
    STATE_DHCP_LEASED = 3,
    STATE_DHCP_REREQUEST = 4,
    STATE_DHCP_RELEASE = 5,
    DHCP_SERVER_PORT = 67,
    DHCP_CLIENT_PORT = 68,
    DHCP_BOOTREQUEST = 1,
    DHCP_BOOTREPLY = 2,
    DHCP_DISCOVER = 1,
    DHCP_OFFER = 2,
    DHCP_REQUEST = 3,
    DHCP_DECLINE = 4,
    DHCP_ACK = 5,
    DHCP_NAK = 6,
    DHCP_RELEASE = 7,
    DHCP_INFORM = 8,
    DHCP_HTYPE10MB = 1,
    DHCP_HTYPE100MB = 2,
    DHCP_HLENETHERNET = 6,
    DHCP_HOPS = 0,
    DHCP_SECS = 0,
    DHCP_CHECK_NONE = 0,
    DHCP_CHECK_RENEW_FAIL = 1,
    DHCP_CHECK_RENEW_OK = 2,
    DHCP_CHECK_REBIND_FAIL = 3,
    DHCP_CHECK_REBIND_OK = 4;

#define DHCP_FLAGSBROADCAST	0x8000

#define MAGIC_COOKIE		0x63825363
#define MAX_DHCP_OPT	16
#define HOST_NAME "ENC28J"
#define DEFAULT_LEASE	(900) //default lease time in seconds
enum
{
	padOption		=	0,
	subnetMask		=	1,
	timerOffset		=	2,
	routersOnSubnet		=	3,
	/* timeServer		=	4,
	nameServer		=	5,*/
	dns			=	6,
	/*logServer		=	7,
	cookieServer		=	8,
	lprServer		=	9,
	impressServer		=	10,
	resourceLocationServer	=	11,*/
	hostName		=	12,
	/*bootFileSize		=	13,
	meritDumpFile		=	14,*/
	domainName		=	15,
	/*swapServer		=	16,
	rootPath		=	17,
	extentionsPath		=	18,
	IPforwarding		=	19,
	nonLocalSourceRouting	=	20,
	policyFilter		=	21,
	maxDgramReasmSize	=	22,
	defaultIPTTL		=	23,
	pathMTUagingTimeout	=	24,
	pathMTUplateauTable	=	25,
	ifMTU			=	26,
	allSubnetsLocal		=	27,
	broadcastAddr		=	28,
	performMaskDiscovery	=	29,
	maskSupplier		=	30,
	performRouterDiscovery	=	31,
	routerSolicitationAddr	=	32,
	staticRoute		=	33,
	trailerEncapsulation	=	34,
	arpCacheTimeout		=	35,
	ethernetEncapsulation	=	36,
	tcpDefaultTTL		=	37,
	tcpKeepaliveInterval	=	38,
	tcpKeepaliveGarbage	=	39,
	nisDomainName		=	40,
	nisServers		=	41,
	ntpServers		=	42,
	vendorSpecificInfo	=	43,
	netBIOSnameServer	=	44,
	netBIOSdgramDistServer	=	45,
	netBIOSnodeType		=	46,
	netBIOSscope		=	47,
	xFontServer		=	48,
	xDisplayManager		=	49,*/
	dhcpRequestedIPaddr	=	50,
	dhcpIPaddrLeaseTime	=	51,
	/*dhcpOptionOverload	=	52,*/
	dhcpMessageType		=	53,
	dhcpServerIdentifier	=	54,
	dhcpParamRequest	=	55,
	/*dhcpMsg			=	56,
	dhcpMaxMsgSize		=	57,*/
	dhcpT1value		=	58,
	dhcpT2value		=	59,
	/*dhcpClassIdentifier	=	60,*/
	dhcpClientIdentifier	=	61,
	endOption		=	255
};

typedef struct _RIP_MSG_FIXED
{
	uint8_t  op; 
	uint8_t  htype; 
	uint8_t  hlen;
	uint8_t  hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint8_t  ciaddr[4];
	uint8_t  yiaddr[4];
	uint8_t  siaddr[4];
	uint8_t  giaddr[4];
	uint8_t  chaddr[6];
}RIP_MSG_FIXED;

class DhcpClass
{
private:
    uint32_t _dhcpInitialTransactionId;
    uint32_t _dhcpTransactionId;
    uint8_t  _dhcpMacAddr[6];
    uint8_t  _dhcpLocalIp[4];
    uint8_t  _dhcpSubnetMask[4];
    uint8_t  _dhcpGatewayIp[4];
    uint8_t  _dhcpDhcpServerIp[4];
    uint8_t  _dhcpDnsServerIp[4];
    uint32_t _dhcpLeaseTime;
    uint32_t _dhcpT1, _dhcpT2;
    signed long _renewInSec;
    signed long _rebindInSec;
    signed long _lastCheck;
    uint32_t _timeout, _responseTimeout, _secTimeout;
    uint8_t _dhcp_state;
    UIPUDP _dhcpUdpSocket;
    int request_DHCP_lease();
    void reset_DHCP_lease() { memset(_dhcpLocalIp, 0, 20); }
    void presend_DHCP();
    void send_DHCP_MESSAGE(uint8_t, uint16_t);
    void printByte(char *, uint8_t);
    uint8_t parseDHCPResponse(unsigned long responseTimeout, uint32_t& transactionId);

    uint32_t _toInt32(uint8_t *v)
    { return (uint32_t)v[0] | (uint32_t)v[1] << 8 | (uint32_t)v[2] << 16 | (uint32_t)v[3] << 24; }
public:
    DhcpClass(UIPEthernetClass *eth) : _dhcpUdpSocket(eth) { }
    uint32_t getLocalIp() { return _toInt32(_dhcpLocalIp); }
    uint32_t getSubnetMask() { return _toInt32(_dhcpSubnetMask); }
    uint32_t getGw() { return _toInt32(_dhcpGatewayIp); }
    uint32_t getDhcpServerIp() { return _toInt32(_dhcpDhcpServerIp); }
    uint32_t getDnsServerIp() { return _toInt32(_dhcpDnsServerIp); }
    int beginWithDHCP(uint8_t *, uint32_t timeout = 60000, uint32_t responseTimeout = 4000);
    int checkLease();
};
#endif




