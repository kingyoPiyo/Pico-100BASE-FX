/********************************************************
* Title    : UDP
* Date     : 2022/01/10
* Design   : kingyo
********************************************************/
#ifndef __UDP_H__
#define __UDP_H__

#include <stdint.h>

void udp_init(void);
void udp_packet_init(uint32_t *buf, uint32_t in_data);
void udp_payload_update(uint32_t *buf, uint32_t in_data);

#endif //__UDP_H__
