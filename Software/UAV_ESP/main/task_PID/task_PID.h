#ifndef __TASK_PID_H__
#define __TASK_PID_H__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <esp_timer.h>
#include <nvs_flash.h>
#include <nvs.h>

#include "../../Hardware/PID/PID.h"
#include "../../Hardware/PWM/PWM.h"
#include "../../Hardware/CONTROL/CONTROL.h"
#include "../../Hardware/LED/LED.h"
#include "../../Hardware/BUZZER/BUZZER.h"
#include "../../System/sys/sys.h"
void task_PID(void*);

extern PID_key[4][3];
extern nvs_handle_t PID_handle;

#endif