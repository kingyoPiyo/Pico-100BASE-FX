/********************************************************
* Title    : Pico-100BASE-FX
* Date     : 2022/01/09
* Design   : kingyo
********************************************************/
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
//#include "hardware/irq.h"
#include "hardware/dma.h"
#include "ser_100base_fx.pio.h"
#include <stdint.h>
#include "udp.h"

#define DEF_TX_PIN          (3)
#define DEF_DBG_PIN         (10)
#define DEF_SW0_PIN         (16)

#define DBG_HI gpio_put(DEF_DBG_PIN, true);
#define DBG_LO gpio_put(DEF_DBG_PIN, false);


int main()
{
    uint32_t DMA_SER_WR;
    uint32_t tx_buf[2][DEF_TX_BUF_SIZE+2];
    uint32_t buf_sel = 0;
    uint32_t lp_cnt = 0;
    PIO pio_ser_wr = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio_ser_wr, &ser_100base_fx_program);

    set_sys_clock_khz(250000, true);
    ser_100base_fx_program_init(pio_ser_wr, sm, offset, DEF_TX_PIN);

    udp_init();
    udp_packet_init(tx_buf[0], lp_cnt);
    udp_packet_init(tx_buf[1], lp_cnt);

    // GPIO Settings
    gpio_init(DEF_DBG_PIN);
    gpio_set_dir(DEF_DBG_PIN, GPIO_OUT);
    gpio_init(DEF_SW0_PIN);
    gpio_set_dir(DEF_SW0_PIN, GPIO_IN);

    // DMA channel setting
    DMA_SER_WR = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(DMA_SER_WR);
    channel_config_set_dreq(&c, pio_get_dreq(pio_ser_wr, sm, true));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    dma_channel_configure(
        DMA_SER_WR,             // Channel to be configured
        &c,                     // The configuration we just created
        &pio_ser_wr->txf[0],    // Destination address
        tx_buf[0],              // Source address
        (DEF_TX_BUF_SIZE+2),    // Number of transfers
        false                   // Don't start yet
    );

    
    while (true)
    {
        // Send data only while button is pressed.
        while(gpio_get(DEF_SW0_PIN)){};

        // DMA Start
        //dma_channel_abort(DMA_SER_WR);
        dma_channel_set_read_addr(DMA_SER_WR, tx_buf[buf_sel], true);

        // UDP Packet update
        DBG_HI
        lp_cnt++;
        udp_payload_update(tx_buf[1-buf_sel], lp_cnt);
        DBG_LO

        // Wait for DMA
        dma_channel_wait_for_finish_blocking(DMA_SER_WR);

        // Wait for IPG
        sleep_us(1);

        // TX Buffer change
        buf_sel = 1 - buf_sel;
    }
}
