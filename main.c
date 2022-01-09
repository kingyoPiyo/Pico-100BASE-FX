/********************************************************
* Title    : Pico-100BASE-FX
* Date     : 2022/01/09
* Design   : kingyo
********************************************************/
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "tx.pio.h"
#include <stdint.h>
#include "udp.h"

#define DEF_TX_PIN          (9)
#define DEF_TX_BUF_SIZE     (72)


int main()
{
    uint32_t tx_buf[DEF_TX_BUF_SIZE+2];
    uint32_t lp_cnt = 0;
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &tx_program);

    set_sys_clock_khz(250000, true);
    tx_program_init(pio, sm, offset, DEF_TX_PIN);

    udp_init();
    udp_packet_init(tx_buf, lp_cnt);
    
    while (true)
    {
        // Send Ethernet Frame
        for (uint32_t i = 0; i < DEF_TX_BUF_SIZE+2; i++)
        {
            tx_10b(pio, sm, tx_buf[i]);
        }

        // UDP Packet update
        lp_cnt++;
        udp_payload_update(tx_buf, lp_cnt);

        //sleep_ms(100);
    }
}
