#include "tcp_socket.h"
#define PORT CONFIG_EXAMPLE_PORT
#define TX_BUF_SIZE 1024

const char *TAG = "tcp_socket";
const char msg[] = "---------SpaceWire Functional Testing---------\n\n\r";
const char SWP[] = "SpaceWire N-Char Packet:\t";
const char SWCCode[] = "SpaceWire C-Code:\t\t";

QueueHandle_t puttyQueue;

static int sock = 0;
int tx_len = 0;

void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    if (addr_family == AF_INET)
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }
    else if (addr_family == AF_INET6)
    {
        bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        ip_protocol = IPPROTO_IPV6;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
    // Note that by default IPV6 binds to both protocols, it is must be disabled
    // if both protocols used at the same time (used in CI)
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
#endif

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {

        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        else
        {
            // Convert ip address to string
            if (source_addr.sin6_family == PF_INET)
            {
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
            }
            else if (source_addr.sin6_family == PF_INET6)
            {
                inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
            }
            ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

            xTaskCreate(sendToPuttyTask, "putty_task", 1024 * 2, NULL, 7, NULL);
        }
        //shutdown(sock, 0);
        //close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

void sendToPuttyTask(void *pvParameters)
{
    uint8_t *data;
    int err = 0;
    uint8_t *tx_buff = (uint8_t *)malloc(TX_BUF_SIZE); //create temporary memory buffer to tx data over tcp/ip
    puttyQueue = xQueueCreate(1, sizeof(uint8_t *));   //create queue for putty task to rx buffer

    err = transmitToPuTTY(sock, (uint8_t *)msg, sizeof(msg)); //send initial message over tcp/ip
    //err = transmitToPuTTY(sock, (uint8_t*)SWP, sizeof(SWP)); //check this sends

    while (1)
    {
        if (xQueueReceive(puttyQueue, &data, portMAX_DELAY))
        { //if queue has received data from uart (elsewhere), proceed to transmit
            uint16_t len = 0;

            while (data[len] != 0)
            { //find length of payload (look for '0')
                len++;
            }

            int tx_len;

            if (len < 2)
            {
                for (tx_len = 0; tx_len < (sizeof(SWCCode) + len); tx_len++)
                { //append static msg of "SpaceWire packet" onto received data for viewing
                    if (tx_len < sizeof(SWCCode))
                    {
                        tx_buff[tx_len] = (uint8_t)SWCCode[tx_len];
                    }
                    else
                    {
                        tx_buff[tx_len] = data[tx_len - sizeof(SWCCode)];
                    }
                }
            }
            else
            {
                for (tx_len = 0; tx_len < (sizeof(SWP) + len); tx_len++)
                { //append static msg of "SpaceWire packet" onto received data for viewing
                    if (tx_len < sizeof(SWP))
                    {
                        tx_buff[tx_len] = (uint8_t)SWP[tx_len];
                    }
                    else
                    {
                        tx_buff[tx_len] = data[tx_len - sizeof(SWP)];
                    }
                }
            }

            tx_buff[tx_len] = (uint8_t)'\n';
            tx_buff[tx_len + 1] = (uint8_t)'\r';
            //ESP_LOGI(TAG, "Bytes to Send %d bytes: %s", len, data);
            err = transmitToPuTTY(sock, tx_buff, tx_len + 2); //transmit to putty tcp/ip, tx_len + 2 for /r/n and index
            if (err)
            {
                goto SOCK_ERROR;
            } //if error detected close/shutdown socket
        }
    }

SOCK_ERROR:
    close(sock);
    vTaskDelete(NULL);
}

int transmitToPuTTY(int const sock, uint8_t *tx_buffer, int len)
{
    //tx_buffer[len] = '\n'; // Send new-line to Putty terminal
    //ESP_LOGI(TAG, "Bytes to Send %d bytes: %s", len, tx_buffer);

    while (len > 0)
    {
        int written = send(sock, tx_buffer, len, 0);
        len -= written;
        if (len < 0)
        {
            return 1;
            break;
        }
    }
    return 0;
}
