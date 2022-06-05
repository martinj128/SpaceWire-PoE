#include "EthernetSW.h"

#define PHY_ADDR 1
#define PHY_RESET 5
#define PHY_MDIO 18
#define PHY_MDC 23
#define FRAME_BUFFER 1518

static const char *TAG = "ethernet";
esp_eth_handle_t eth_handle = NULL;
uint8_t* mac_addr_ptr;
uint8_t mac_dest_addr[6] = {0x7c, 0x9e, 0xbd, 0xcf, 0x00, 0x03}; //mac addr of master 
unsigned char hello[] = "SpaceWire N-Char test";
unsigned char timeCode[] = "1";
unsigned char multiplePackets[] = "*SW Packet 1'*SW Packet 2'*SW Packet 3'";
unsigned char swmix[] = "*SW Packet 1'#1*SW Packet 2'*SW Packet 3'#2#3";
unsigned char frame1[] = "&SW Packet Frame 1+";
unsigned char frame2[] = "&SW Packet Frame 2'";

spaceWirePoEPackets_t nCharTest; 
spaceWirePoEPackets_t swFrame1; 
spaceWirePoEPackets_t swFrame2;

void sendTask(void *pvParameters)
{
    //N-char test data struct
    nCharTest.dest_addr = mac_dest_addr;
    nCharTest.source_addr = mac_addr_ptr;
    nCharTest.data = multiplePackets;
    nCharTest.len = sizeof(multiplePackets)-1;

    //frame1 test data struct
    swFrame1.dest_addr = mac_dest_addr;
    swFrame1.source_addr = mac_addr_ptr;
    swFrame1.data = frame1;
    swFrame1.len = sizeof(frame1)-1;

    //frame2 test data struct
    swFrame2.dest_addr = mac_dest_addr;
    swFrame2.source_addr = mac_addr_ptr;
    swFrame2.data = frame2;
    swFrame2.len = sizeof(frame2)-1;
    
    while(1)
    {
        //sendSWPacket((uint8_t *)timeCode, (sizeof(timeCode)-1),CCODE_HEADER, mac_dest_addr);   //-1 on size to remove null for string, cause problems
        sendSWPackets(swFrame1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        sendSWPackets(swFrame2);
        vTaskDelay(10000/portTICK_PERIOD_MS);
    }

}

// Event handler for Ethernet events
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    mac_addr_ptr = mac_addr;
    //we can get the ethernet driver handle from event data
    eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id)
    {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
                 xTaskCreate(sendTask, "sendEthTask", 4096, NULL, 5, NULL); //upon connect event, start send task
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

void setupEthernet(void)
{
    //setup ethernet event handler callback
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));

    //Configure Ethernet Interface on ESP32
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG(); //MAC config object
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); //PHY config esp_eth_phy_new_ip101() can be used
    phy_config.phy_addr = PHY_ADDR;
    phy_config.reset_gpio_num = PHY_RESET;
    mac_config.smi_mdc_gpio_num = PHY_MDC;
    mac_config.smi_mdio_gpio_num = PHY_MDIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config); //create ethernet MAC instance against MAC config
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); //create ethernet PHY instance for IP101 COnfig

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); //default eth config for mac and phy instances
    esp_eth_handle_t eth_handle = NULL;

    config.stack_input = (void *)receiveSWPackets;                 //pass ethernet frames received to rxSWpackets function to process
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle)); //after driver installed, obtain ethernet handler

    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
}
