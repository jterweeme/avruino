#include "w5100.h"
#include "socket.h"

static uint16_t local_port;
extern W5100Class *g_w5100;

/**
 * @brief	This Socket function initialize the channel in perticular mode, and set the port and wait for W5100 done it.
 * @return 	1 for success else 0.
 */
uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
    if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP) ||
        (protocol == SnMR::IPRAW) || (protocol == SnMR::MACRAW) || (protocol == SnMR::PPPOE))
    {
        close(s);
        g_w5100->writeSnMR(s, protocol | flag);

        if (port != 0)
        {
            g_w5100->writeSnPORT(s, port);
        } 
        else
        {
            local_port++; // if don't set the source port, set local_port number.
            g_w5100->writeSnPORT(s, local_port);
        }

        g_w5100->execCmdSn(s, Sock_OPEN);
        return 1;
    }

    return 0;
}

void close(SOCKET s)
{
    g_w5100->execCmdSn(s, Sock_CLOSE);
    g_w5100->writeSnIR(s, 0xFF);
}


uint8_t listen(SOCKET s)
{
    if (g_w5100->readSnSR(s) != SnSR::INIT)
        return 0;

    g_w5100->execCmdSn(s, Sock_LISTEN);
    return 1;
}

uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port)
{
    if (((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
        ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
        (port == 0x00))
    {
        return 0;
    }

    // set destination IP
    g_w5100->writeSnDIPR(s, addr);
    g_w5100->writeSnDPORT(s, port);
    g_w5100->execCmdSn(s, Sock_CONNECT);
    return 1;
}

/**
 * @brief	This function used for disconnect the socket and parameter is "s" which represent the socket number
 * @return	1 for success else 0.
 */
void disconnect(SOCKET s)
{
    g_w5100->execCmdSn(s, Sock_DISCON);
}


/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len)
{
    uint8_t status=0;
    uint16_t ret=0;
    uint16_t freesize=0;

    ret = len > g_w5100->SSIZE ? g_w5100->SSIZE : len;

    do 
    {
        freesize = g_w5100->getTXFreeSize(s);
        status = g_w5100->readSnSR(s);

        if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
        {
            ret = 0; 
            break;
        }
    } 
    while (freesize < ret);

    // copy data
    g_w5100->send_data_processing(s, (uint8_t *)buf, ret);
    g_w5100->execCmdSn(s, Sock_SEND);

    /* +2008.01 bj */
    while ((g_w5100->readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
    {
        /* m2008.01 [bj] : reduce code */
        if (g_w5100->readSnSR(s) == SnSR::CLOSED )
        {
            close(s);
            return 0;
        }
    }

    g_w5100->writeSnIR(s, SnIR::SEND_OK);
    return ret;
}

int16_t recv(SOCKET s, uint8_t *buf, int16_t len)
{
    // Check how much data is available
    int16_t ret = g_w5100->getRXReceivedSize(s);

    if (ret == 0)
    {
        // No data available.
        uint8_t status = g_w5100->readSnSR(s);

        if (status == SnSR::LISTEN || status == SnSR::CLOSED || status == SnSR::CLOSE_WAIT)
        {
            // The remote end has closed its side of the connection, so this is the eof state
            ret = 0;
        }
        else
        {
            ret = -1; // The connection is still up, but there's no data waiting to be read
        }
    }
    else if (ret > len)
    {
        ret = len;
    }

    if (ret > 0)
    {
        g_w5100->recv_data_processing(s, buf, ret);
        g_w5100->execCmdSn(s, Sock_RECV);
    }
    return ret;
}


/**
 * @brief	Returns the first byte in the receive queue (no checking)
 * 		
 * @return
 */
uint16_t peek(SOCKET s, uint8_t *buf)
{
    g_w5100->recv_data_processing(s, buf, 1, 1);
    return 1;
}

/**
 * @brief	This function is an application I/F function which is used to send the data for other then TCP mode. 
 * 		Unlike TCP transmission, The peer's destination address and the port is needed.
 * 		
 * @return	This function return send data size for success else -1.
 */
uint16_t sendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port)
{
    uint16_t ret = len > g_w5100->SSIZE ? g_w5100->SSIZE : len;

    if (((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
        ((port == 0x00)) ||(ret == 0)) 
    {
        ret = 0;
    }
    else
    {
        g_w5100->writeSnDIPR(s, addr);
        g_w5100->writeSnDPORT(s, port);
        g_w5100->send_data_processing(s, (uint8_t *)buf, ret);  // copy data
        g_w5100->execCmdSn(s, Sock_SEND);

        while ((g_w5100->readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
        {
            if (g_w5100->readSnIR(s) & SnIR::TIMEOUT)
            {
                g_w5100->writeSnIR(s, (SnIR::SEND_OK | SnIR::TIMEOUT));
                return 0;
            }
        }

        g_w5100->writeSnIR(s, SnIR::SEND_OK);
    }
    return ret;
}


/**
 * @brief	This function is an application I/F function which is used to receive the data in other then
 * 	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well. 
 * 	
 * @return	This function return received data size for success else -1.
 */
uint16_t recvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port)
{
    uint8_t head[8];
    uint16_t data_len=0;
    uint16_t ptr=0;

    if (len > 0)
    {
        ptr = g_w5100->readSnRX_RD(s);
        switch (g_w5100->readSnMR(s) & 0x07)
        {
        case SnMR::UDP :
            g_w5100->read_data(s, (uint8_t *)ptr, head, 0x08);
            ptr += 8;
            // read peer's IP address, port number.
            addr[0] = head[0];
            addr[1] = head[1];
            addr[2] = head[2];
            addr[3] = head[3];
            *port = head[4];
            *port = (*port << 8) + head[5];
            data_len = head[6];
            data_len = (data_len << 8) + head[7];
            g_w5100->read_data(s, (uint8_t *)ptr, buf, data_len); // data copy.
            ptr += data_len;
            g_w5100->writeSnRX_RD(s, ptr);
            break;
        case SnMR::IPRAW :
            g_w5100->read_data(s, (uint8_t *)ptr, head, 0x06);
            ptr += 6;
            addr[0] = head[0];
            addr[1] = head[1];
            addr[2] = head[2];
            addr[3] = head[3];
            data_len = head[4];
            data_len = (data_len << 8) + head[5];
            g_w5100->read_data(s, (uint8_t *)ptr, buf, data_len); // data copy.
            ptr += data_len;
            g_w5100->writeSnRX_RD(s, ptr);
            break;
        case SnMR::MACRAW:
            g_w5100->read_data(s,(uint8_t*)ptr,head,2);
            ptr+=2;
            data_len = head[0];
            data_len = (data_len<<8) + head[1] - 2;
            g_w5100->read_data(s,(uint8_t*) ptr,buf,data_len);
            ptr += data_len;
            g_w5100->writeSnRX_RD(s, ptr);
            break;
        default:
            break;
        }
        g_w5100->execCmdSn(s, Sock_RECV);
    }
    return data_len;
}


uint16_t igmpsend(SOCKET s, const uint8_t * buf, uint16_t len)
{
    uint16_t ret=0;

    if (len > g_w5100->SSIZE) 
        ret = g_w5100->SSIZE; // check size not to exceed MAX size.
    else 
        ret = len;

    if (ret == 0)
        return 0;

    g_w5100->send_data_processing(s, (uint8_t *)buf, ret);
    g_w5100->execCmdSn(s, Sock_SEND);

    while ((g_w5100->readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
    {
        //status = g_w5100->readSnSR(s);

        if (g_w5100->readSnIR(s) & SnIR::TIMEOUT)
        {
            /* in case of igmp, if send fails, then socket closed */
            /* if you want change, remove this code. */
            close(s);
            return 0;
        }
    }

    g_w5100->writeSnIR(s, SnIR::SEND_OK);
    return ret;
}

uint16_t bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
    uint16_t ret = len > g_w5100->getTXFreeSize(s) ? g_w5100->getTXFreeSize(s) : len;
    g_w5100->send_data_processing_offset(s, offset, buf, ret);
    return ret;
}

int startUDP(SOCKET s, uint8_t* addr, uint16_t port)
{
    if (((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
        ((port == 0x00))) 
    {
        return 0;
    }

    g_w5100->writeSnDIPR(s, addr);
    g_w5100->writeSnDPORT(s, port);
    return 1;
}

int sendUDP(SOCKET s)
{
    g_w5100->execCmdSn(s, Sock_SEND);
		
    while ((g_w5100->readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
    {
        if (g_w5100->readSnIR(s) & SnIR::TIMEOUT)
        {
            g_w5100->writeSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
            return 0;
        }
    }

    g_w5100->writeSnIR(s, SnIR::SEND_OK);
    return 1; // sent ok
}

