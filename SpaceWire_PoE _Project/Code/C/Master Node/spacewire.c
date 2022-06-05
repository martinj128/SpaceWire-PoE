#include "spacewire.h"

static uint16_t eth_index = 0; //variable to hold current index in eth rx buffer
uint16_t dataRead = 0;         //variable to hold amount of data read from eth rx buffer, updated within readSWPacket()
//uint8_t *swPacketMultiFrameBuffer[SW_NCHAR_BUFFER_SIZE];


static const char *TAG = "spacewire";
QueueHandle_t puttyQueue;

void receiveSWPackets(esp_eth_handle_t eth_handle, uint8_t *eth_rx_buffer, uint32_t len, void *priv) //MAC dest addr is already determined if correct
{
    eth_index = 0;
    uint8_t ethFrameBuffer[24];
    uint16_t frameLength = 0;
    uint16_t dataToRead = 0;

    for (int i = 0; i < 6; i++) //get MAC destination address from payload
    {
        ethFrameBuffer[i] = eth_rx_buffer[eth_index];
        eth_index++;
    }

    for (int i = 6; i < 12; i++) //get MAC destination address from payload
    {
        ethFrameBuffer[i] = eth_rx_buffer[eth_index];
        eth_index++;
    }

    frameLength |= (uint16_t)(eth_rx_buffer[eth_index] << 8);
    eth_index++;
    frameLength |= (uint16_t)(eth_rx_buffer[eth_index]);
    eth_index++;
    dataToRead = frameLength - 14; //to not count mac addr's and len bytes in data to read

    ESP_LOGI(TAG, "Received Ethernet Frame\n\t\t\t MAC Dest Addr: %02x:%02x:%02x:%02x:%02x:%02x\n\t\t\t Source Addr: %02x:%02x:%02x:%02x:%02x:%02x\n\t\t\t SW Data Length: %d",
             ethFrameBuffer[0], ethFrameBuffer[1], ethFrameBuffer[2], ethFrameBuffer[3], ethFrameBuffer[4], ethFrameBuffer[5],
             ethFrameBuffer[6], ethFrameBuffer[7], ethFrameBuffer[8], ethFrameBuffer[9], ethFrameBuffer[10], ethFrameBuffer[11], dataToRead);

    while (dataToRead > 0)
    { //process the ethernet buffer until all data processed/read for ethernet payload, no need to check if NULL value
        if(processSWPacketHeader(eth_rx_buffer))
        {//header-byte found
            dataToRead = dataToRead - dataRead;
            ESP_LOGI(TAG, "Eth Frame Data to read: %d", dataToRead);
        }
        else 
        {//Throw error and exit function
            ESP_LOGI(TAG, "Header-Byte not found");
            return;
        }
    }
    ESP_LOGI(TAG, "Finished Reading Frame");
}

uint8_t processSWPacketHeader(uint8_t *eth_rx_buffer)
{
    uint8_t swPacketType = eth_rx_buffer[eth_index];
    uint8_t err = 1;

    ESP_LOGI(TAG, "SW Packet_Type: %c", swPacketType);

    switch (swPacketType)
    {
    case EOP_HEADER:
        //contains an EOP marker within payload
        //indicate to check for EOP when processing, get length, get packet number etc.
        ESP_LOGI(TAG, "SW Packet HeaderByte = '%c' EOP_HEADER", EOP_HEADER);
        readSWPacket(eth_rx_buffer);
        err = 1;
        break;
    case EEP_HEADER:
        //contains of EEP marker within payload
        //send request for packet again
        err = 1;
        break;
    case CCODE_HEADER:
        //payload contains Control Code in payload
        ESP_LOGI(TAG, "SW Packet HeaderByte = '%c' CCODE HEADER", CCODE_HEADER);
        readSWCCode(eth_rx_buffer);
        err = 1;
        break;
    case MULTIPLE_FRAMES_HEADER:
        //payload is maxxed on buffer and unfinished, to be completed in next Eth frame
        //indicate payload not all received, keep in buffer, get number etc.
        ESP_LOGI(TAG, "SpaceWire Packet in multiple Eth Frames");
        readSWPacketMultipleFrames(eth_rx_buffer);
        err = 1;
        break;

    default:
        //header type not found, throw error
        err = 0;
        break;
    }
return err;
}

void readSWPacket(uint8_t *eth_rx_buffer)
{
    uint16_t sw_packet_index = 0;
    uint8_t *swPacketBuffer = (uint8_t *)malloc(SW_NCHAR_BUFFER_SIZE);

    eth_index++; //increase index by 1 to account for headerbyte

    while (eth_rx_buffer[eth_index] != EOP) //not equal char '"'
    {
        swPacketBuffer[sw_packet_index] = eth_rx_buffer[eth_index];
        sw_packet_index++;
        eth_index++;
    }

    dataRead = sw_packet_index + 2; //+2 to account for headerbyte and EOP Marker byte read
    eth_index++;                    //+1 to move address to next headerbyte in loop

    ESP_LOGI(TAG, "EOP found in payload '%c'\n\t\t\t SW Packet Data Length %d bytes", EOP, dataRead);

    //unsigned char* buffer = convertToCharArray2(swPacketBuffer, dataRead-2); //-2 to not read headerbyte and EOP
    //ESP_LOGI(TAG, "Payload: %s", buffer);

    swPacketBuffer[sw_packet_index] = 0;

    //xQueueSend(puttyQueue, (void *)&swPacketBuffer, portMAX_DELAY); //send to queue, which sends to putty terminal like uart
}

void readSWCCode(uint8_t *eth_rx_buffer){

    uint8_t *swCCodeBuffer = (uint8_t *)malloc(SW_CCODE_BUFFER_SIZE);
    eth_index++; //increase index by 1 to account for headerbyte
    swCCodeBuffer[0] = eth_rx_buffer[eth_index];   //get c-code data (time/interrupt)
    swCCodeBuffer[1] = 0;
    dataRead = 2;    //CCode Header and Data therefore 2
    //xQueueSend(puttyQueue, (void *)&swCCodeBuffer, portMAX_DELAY); //send to queue, which sends to putty terminal like uart
}

void readSWPacketMultipleFrames(uint8_t *eth_rx_buffer)
{   
    static uint8_t *swPacketMultiFrameBuffer[SW_NCHAR_BUFFER_SIZE];
    static uint16_t sw_packet_index = 0; 
    static uint8_t frameNumber = 0;
    static uint16_t packetSize = 0;
    
    dataRead = 0;

    frameNumber++;  //increase frame number count

    ESP_LOGI(TAG, "Frame No: %d", frameNumber);

    eth_index++;    //increase index by 1 to move from headerbyte
    dataRead++;     //increase by 1 for headerbyte
    
    while(1)
    {
        switch(eth_rx_buffer[eth_index])
        {
            case END_OF_FRAME:
                dataRead++;
                //sw_packet_index++;
                packetSize = packetSize + dataRead;
                ESP_LOGI(TAG, "End of Frame, DataRead: %d", dataRead);
                eth_index++;                    //+1 to move address to next headerbyte in loop
                goto exit_loop;

            case EOP:
                eth_index++;
                dataRead++;
                packetSize = packetSize + dataRead;
                ESP_LOGI(TAG, "EOP Found, %d bytes read in Frame", dataRead);
                ESP_LOGI(TAG, "Total bytes in SW Packet: %d", packetSize);

                for(int i=0; i < packetSize-4; i++){
                    ESP_LOGI(TAG, "%c" ,(char)swPacketMultiFrameBuffer[i]);
                }

                char *char_buffer =  convertToCharArray((uint8_t *)swPacketMultiFrameBuffer, packetSize-4);
                ESP_LOGI(TAG, "Size of SW Packet: %d\nSW Packet: %s", packetSize-4, char_buffer);
                //xQueueSend(puttyQueue, (void *)&swPacketMultiFrameBuffer, portMAX_DELAY); //send to queue, which sends to putty terminal like uart
                memset(swPacketMultiFrameBuffer, 0, packetSize-4);
                sw_packet_index = 0;
                frameNumber = 0;
                packetSize = 0;
                goto exit_loop;
        
            default:
                swPacketMultiFrameBuffer[sw_packet_index] = eth_rx_buffer[eth_index];
                sw_packet_index++;
                eth_index++;
                dataRead++;
                break;    
        }
    }
exit_loop: return;
}

void sendSWPacket(uint8_t *tx_buffer, uint16_t buffer_len, uint8_t sw_packet_type, uint8_t *destination_addr)
{
    uint8_t *sw_tx_buffer = (uint8_t *)malloc(SW_NCHAR_BUFFER_SIZE);
    uint16_t tx_index = 0;
    uint16_t ethFrameLen = 0;
    uint8_t frame_len_msb, frame_len_lsb;

    //build the spacewire packet with ethernet frame requirements
    for (int i = 0; i < 6; i++)
    { // insert destination MAC address
        sw_tx_buffer[tx_index] = destination_addr[i];
        tx_index++;
    }

    for (int i = 0; i < 6; i++)
    { // insert source MAC address
        sw_tx_buffer[tx_index] = mac_addr_ptr[i];
        tx_index++;
    }

    tx_index = SW_No1_HEADER_INDEX;

        switch (sw_packet_type)
    {
    case EOP_HEADER:
        sw_tx_buffer[tx_index] = EOP_HEADER; //place EOP Header in tx_buffer
        ESP_LOGI(TAG, "value %c at index %d", sw_tx_buffer[tx_index], tx_index);
        tx_index++; //move index to payload index start
        for (int i = 0; i < buffer_len; i++)
        {
            sw_tx_buffer[tx_index] = tx_buffer[i];
            ESP_LOGI(TAG, "value %c at index %d", sw_tx_buffer[tx_index], tx_index);
            tx_index++;
        }
        sw_tx_buffer[tx_index] = EOP; //place EOP marker in send string
        ESP_LOGI(TAG, "value %c at index %d", sw_tx_buffer[tx_index], tx_index);
        ethFrameLen = (tx_index + 1); //tx_index+1 to reflect size not index
        break;

    case CCODE_HEADER:
        sw_tx_buffer[tx_index] = CCODE_HEADER; //place C-Code Header into tx_buffer
        ESP_LOGI(TAG, "value %c at index %d", sw_tx_buffer[tx_index], tx_index);
        tx_index++; //move index to payload index start
        for (int i = 0; i < buffer_len; i++) //for loop not strictly needed, although useful for future work
        {
            sw_tx_buffer[tx_index] = tx_buffer[i];
            ESP_LOGI(TAG, "value %c at index %d", sw_tx_buffer[tx_index], tx_index);
            tx_index++;
        }
        //sw_tx_buffer[tx_index] = EOP; //place EOP marker in send string
        ESP_LOGI(TAG, "value %c at index %d", sw_tx_buffer[tx_index], tx_index);
        ethFrameLen = tx_index; //tx_index to reflect size not index for c-code
        break;

    default: 
        break;
    }

    //ethFrameLen = (tx_index + 1); //tx_index+1 to reflect size not index
    frame_len_msb = (ethFrameLen & MSB_BITMASK) >> 8; //get ms8 bits of 16-bit length & 1111111100000000
    frame_len_lsb = ethFrameLen & LSB_BITMASK;        //get ls bits of 16-bit length & 00000000011111111

    sw_tx_buffer[MSB_FRAME_INDEX] = frame_len_msb; //insert len msb into frame
    sw_tx_buffer[LSB_FRAME_INDEX] = frame_len_lsb; //insert len lsb into frame

    if (esp_eth_transmit(eth_handle, sw_tx_buffer, ethFrameLen) != ESP_OK)
    { //transmit SW packet over ethernet
        ESP_LOGE(TAG, "Ethernet send packet failed");
    }
    else
    {
        //Check what has been sent and if correctly
        char *char_buffer = convertToCharArray(sw_tx_buffer, ethFrameLen);
        ESP_LOGI(TAG, "Ethernet Sent %d bytes Containing:\nDest Addr: %02x:%02x:%02x:%02x:%02x:%02x\nSource Addr: %02x:%02x:%02x:%02x:%02x:%02x\nFrame Length Bytes: %02x:%02x\nSW Packet: %s", ethFrameLen,
                 sw_tx_buffer[0], sw_tx_buffer[1], sw_tx_buffer[2], sw_tx_buffer[3], sw_tx_buffer[4], sw_tx_buffer[5], sw_tx_buffer[6],
                 sw_tx_buffer[7], sw_tx_buffer[8], sw_tx_buffer[9], sw_tx_buffer[10], sw_tx_buffer[11], sw_tx_buffer[12], sw_tx_buffer[13], char_buffer);
    }
}

void sendSWPackets(spaceWirePoEPackets_t frame){

    uint8_t *sw_tx_buffer = (uint8_t *)malloc(SW_NCHAR_BUFFER_SIZE);
    uint16_t tx_index = 0;
    uint16_t ethFrameLen = 0;
    uint8_t frame_len_msb, frame_len_lsb;

    //build the spacewire packet with ethernet frame requirements
    for (int i = 0; i < 6; i++)
    { // insert destination MAC address
        sw_tx_buffer[tx_index] = frame.dest_addr[i];
        tx_index++;
    }
    
    for (int i = 0; i < 6; i++)
    { // insert source MAC address
        sw_tx_buffer[tx_index] = frame.source_addr[i];
        tx_index++;
    }

    tx_index = SW_No1_HEADER_INDEX;

    for(int i=0; i < frame.len; i++)
    {
        sw_tx_buffer[tx_index] = frame.data[i];
        tx_index++;
    }

    ethFrameLen = tx_index; //tx_index+1 to reflect size not index

    frame_len_msb = (ethFrameLen & MSB_BITMASK) >> 8; //get ms8 bits of 16-bit length & 1111111100000000
    frame_len_lsb = ethFrameLen & LSB_BITMASK;        //get ls bits of 16-bit length & 00000000011111111

    sw_tx_buffer[MSB_FRAME_INDEX] = frame_len_msb; //insert len msb into frame
    sw_tx_buffer[LSB_FRAME_INDEX] = frame_len_lsb; //insert len lsb into frame

    if (esp_eth_transmit(eth_handle, sw_tx_buffer, ethFrameLen) != ESP_OK)
    { //transmit SW packet over ethernet
        ESP_LOGE(TAG, "Ethernet send packet failed");
    }
    else
    {
        //Check what has been sent and if correctly
        char *char_buffer = convertToCharArray(sw_tx_buffer, ethFrameLen);
        ESP_LOGI(TAG, "Ethernet Sent %d bytes Containing:\nDest Addr: %02x:%02x:%02x:%02x:%02x:%02x\nSource Addr: %02x:%02x:%02x:%02x:%02x:%02x\nFrame Length Bytes: %02x:%02x\nSW Packet: %s", ethFrameLen,
                 sw_tx_buffer[0], sw_tx_buffer[1], sw_tx_buffer[2], sw_tx_buffer[3], sw_tx_buffer[4], sw_tx_buffer[5], sw_tx_buffer[6],
                 sw_tx_buffer[7], sw_tx_buffer[8], sw_tx_buffer[9], sw_tx_buffer[10], sw_tx_buffer[11], sw_tx_buffer[12], sw_tx_buffer[13], char_buffer);
    }


}

char *convertToCharArray(uint8_t *buffer, uint16_t len)
{
    char *char_buffer = (char *)malloc(len + 1); //+1 to add null-character at end of string
    int size = 0;

    for (int i = 0; i < (len); i++)
    {
        char_buffer[i] = (char)buffer[i]; //convert uint8_t to char array to print, +14 to skip MAC addr and len bytes
        size++;
    }

    char_buffer[size] = '\0'; //null terminate the array to be read like string

    return char_buffer;
}

unsigned char *convertToCharArray2(uint8_t *buffer, uint16_t len)
{
    unsigned char *char_buffer = (unsigned char *)malloc(len + 1); //+1 to add null-character at end of string
    int size = 0;

    for (int i = 0; i < (len); i++)
    {
        char_buffer[i] = (unsigned char)buffer[i]; //convert uint8_t to char array to print, +14 to skip MAC addr and len bytes
        size++;
    }

    char_buffer[size] = 0; //null terminate the array to be read like string

    return char_buffer;
}
