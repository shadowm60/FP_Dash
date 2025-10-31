#include "can.h"

//todo: later we should probe or get a config on the baudrate to be used
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);


static QueueHandle_t tx_task_queue;
static QueueHandle_t rx_task_queue;

static const char *TAG = "FP_Dash_CAN";


void can_send_message(twai_message_t *msg) {
    if (xQueueSend(tx_task_queue, msg, portMAX_DELAY) != pdPASS) {
        ESP_LOGW(TAG, "Failed to push sent msg to queue!");
    }
}

static void can_receive_task(void *arg) {
    esp_err_t result;
    while (1) {
        twai_message_t rx_msg;
        result = twai_receive(&rx_msg, portMAX_DELAY);
        if (result == ESP_OK) {
            //received a message, push it to queue
            if (xQueueSend(rx_task_queue, &rx_msg, portMAX_DELAY) != pdPASS) {
                ESP_LOGW(TAG, "Failed to push msg to queue!");
            }
        }
    }
    vTaskDelete(NULL);
}

static void can_send_task(void *arg) {
    while (1) {
        twai_message_t msg;
        if (xQueueReceive(tx_task_queue, &msg, portMAX_DELAY) == pdPASS) {
            twai_transmit(&msg, portMAX_DELAY);
        }        
    }
    vTaskDelete(NULL);    
}

static void can_process_task(void *arg) {
    while(1) {
        twai_message_t msg;
        twai_message_t dbgmsg;
        if (xQueueReceive(rx_task_queue, &msg, portMAX_DELAY) == pdPASS) {
            //process message
            dbgmsg = msg;
            dbgmsg.identifier += 1;
            can_send_message(&dbgmsg);
        }
    }
    vTaskDelete(NULL);
}


void init_can( void ) {
    tx_task_queue = xQueueCreate(RX_QUEUE_SIZE, sizeof(twai_message_t));
    rx_task_queue = xQueueCreate(TX_QUEUE_SIZE, sizeof(twai_message_t));

    if ((tx_task_queue == NULL) || (rx_task_queue == NULL)) {
        ESP_LOGE(TAG, "Failed to create Queue");
    }

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(EXAMPLE_TAG, "Driver installed");

}

void init_can_tasks(void) {

    xTaskCreatePinnedToCore(can_receive_task, "TWAI_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(can_send_task, "TWAI_tx", 4096, NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(can_process_task, "TWAI_ctrl", 4096, NULL, PROC_TSK_PRIO, NULL, tskNO_AFFINITY);
}
//send from isr example
//BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//xQueueSendFromISR(myQueue, &valueToSend, &xHigherPriorityTaskWoken);
//portYIELD_FROM_ISR(xHigherPriorityTaskWoken);


//static const twai_message_t ping_message = {
//    // Message type and format settings
//    .extd = 0,              // Standard Format message (11-bit ID)
//    .rtr = 0,               // Send a data frame
//    .ss = 1,                // Is single shot (won't retry on error or NACK)
//    .self = 0,              // Not a self reception request
//    .dlc_non_comp = 0,      // DLC is less than 8
//    // Message ID and payload
//    .identifier = ID_MASTER_PING,
//    .data_length_code = 0,
//    .data = {0},
//};