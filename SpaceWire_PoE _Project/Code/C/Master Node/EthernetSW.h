#ifndef _ETHERNETSW_H_
#define _ETHERNETSW_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <string.h>
#include "spacewire.h"

extern uint8_t* mac_addr_ptr;
extern esp_eth_handle_t eth_handle;

typedef struct spaceWirePoEPackets{
    uint8_t* dest_addr;
    uint8_t* source_addr;
    uint8_t* data;
    uint16_t len;
} spaceWirePoEPackets_t;

void setupEthernet(void);

#endif