#ifndef	_SOCKET_H_
#define	_SOCKET_H_
#include "w5100.h"

extern uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);
extern void close(SOCKET s); // Close socket
extern uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port);
extern void disconnect(SOCKET s); // disconnect the connection
extern uint8_t listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len); // Send data (TCP)
extern int16_t recv(SOCKET s, uint8_t * buf, int16_t len);	// Receive data (TCP)
extern uint16_t peek(SOCKET s, uint8_t *buf);
extern uint16_t sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);
extern uint16_t recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port);
extern uint16_t igmpsend(SOCKET s, const uint8_t * buf, uint16_t len);
extern int startUDP(SOCKET s, uint8_t* addr, uint16_t port);
uint16_t bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len);
int sendUDP(SOCKET s);
#endif



