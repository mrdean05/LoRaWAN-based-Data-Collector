#ifndef _LORAWAN_H_
#define _LORAWAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include  "LoRaMac.h" 

struct lorawan_sx126x_settings {
    struct {
        uint32_t mosi;
        uint32_t miso;
        uint32_t sck;
    } spi;
};

struct lorawan_abp_settings {
    const char* device_address;
    const char* network_session_key;
    const char* app_session_key;
    const char* channel_mask;
};

struct lorawan_otaa_settings {
    const char* device_eui;
    const char* app_eui;
    const char* app_key;
    const char* channel_mask;
};


int lorawan_init_abp(LoRaMacRegion_t region, const struct lorawan_abp_settings* abp_settings);

int lorawan_init_otaa(LoRaMacRegion_t region, const struct lorawan_otaa_settings* otaa_settings);

int lorawan_join();

int lorawan_is_joined();

int lorawan_process();

int lorawan_process_timeout_ms(uint32_t timeout_ms);

int lorawan_send_unconfirmed(const void* data, uint8_t data_len, uint8_t app_port);

int lorawan_receive(void* data, uint8_t data_len, uint8_t* app_port);

#ifdef __cplusplus
}
#endif

#endif