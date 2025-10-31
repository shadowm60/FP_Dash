#ifndef _CAN_H_
#define _CAN_H_
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"

#define TX_GPIO_NUM     21
#define RX_GPIO_NUM     22

#define RX_QUEUE_SIZE   10 //number of messages to be stored
#define TX_QUEUE_SIZE   10 //number of messages to be stored


#define RX_TASK_PRIO            8
#define TX_TASK_PRIO            9
#define PROC_TSK_PRIO           10


void init_can(void);
void init_can_tasks(void);
void can_send_message(twai_message_t *msg);

#endif