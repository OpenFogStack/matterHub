/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "DeviceCallbacks.h"
#include <common/CHIPDeviceManager.h>
#include <common/Esp32AppServer.h>

#include "AppTask.h"
#include "BindingHandler.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "shell_extension/launch.h"

#include <app/server/OnboardingCodesUtil.h>

// SPDX: MIT
// Copyright 2021 Brian Starkey <stark3y@gmail.com>
// Portions from lvgl example: https://github.com/lvgl/lv_port_esp32/blob/master/main/main.c
//
// 
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_log.h>

#include "i2c_manager.h"
#include "m5core2_axp192.h"

#include "lvgl.h"
#include "lvgl_helpers.h"

#define LV_TICK_PERIOD_MS 1
using namespace ::chip;
using namespace ::chip::DeviceManager;

static const char * TAG = "light-switch-app";

static AppDeviceCallbacks EchoCallbacks;

static void InitServer(intptr_t context)
{
    // Print QR Code URL
    PrintOnboardingCodes(chip::RendezvousInformationFlags(CONFIG_RENDEZVOUS_MODE));

    Esp32AppServer::Init(); // Init ZCL Data Model and CHIP App Server AND Initialize device attestation config

    InitBindingHandler();
}



enum toggle_id {
	TOGGLE_LED = 0,
};

extern "C" {static void toggle_event_cb(lv_obj_t *toggle, lv_event_t event)
{
	if(event == LV_EVENT_VALUE_CHANGED) {
		bool state = lv_switch_get_state(toggle);
		enum toggle_id *id =(toggle_id*) lv_obj_get_user_data(toggle);

		// Note: This is running in the GUI thread, so prolonged i2c
		// comms might cause some jank
		switch (*id) {
		case TOGGLE_LED:
			m5core2_led(state);
			break;
		}
	}
}}

extern "C" {static void gui_timer_tick(void *arg)
{
	// Unused
	(void) arg;

	lv_tick_inc(LV_TICK_PERIOD_MS);
}}

extern "C"{ static void gui_thread(void *pvParameter)
{
	(void) pvParameter;

	static lv_color_t bufs[2][DISP_BUF_SIZE];
	static lv_disp_buf_t disp_buf;
	uint32_t size_in_px = DISP_BUF_SIZE;

	// Set up the frame buffers
	lv_disp_buf_init(&disp_buf, &bufs[0], &bufs[1], size_in_px);

	// Set up the display driver
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = disp_driver_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);

	// Register the touch screen. All of the properties of it
	// are set via the build config
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.read_cb = touch_driver_read;
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&indev_drv);

	// Timer to drive the main lvgl tick
	const esp_timer_create_args_t periodic_timer_args = {
		.callback = &gui_timer_tick,
		.name = "periodic_gui"
	};
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));


	// Full screen root container
	lv_obj_t *root = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_size(root, 320, 240);
	lv_cont_set_layout(root, LV_LAYOUT_COLUMN_MID);
	// Don't let the containers be clicked on
	lv_obj_set_click(root, false);

	// Create rows of switches for different functions
	struct {
		const char *label;
		bool init;
		enum toggle_id id;
	} switches[] = {
		{ "LED",     true,  TOGGLE_LED },
	};
	for (int i = 0; i < sizeof(switches) / sizeof(switches[0]); i++) {
		lv_obj_t *row = lv_cont_create(root, NULL);
		lv_cont_set_layout(row, LV_LAYOUT_ROW_MID);
		lv_obj_set_size(row, 300, 0);
		lv_cont_set_fit2(row, LV_FIT_NONE, LV_FIT_TIGHT);
		// Don't let the containers be clicked on
		lv_obj_set_click(row, false);

		lv_obj_t *toggle = lv_switch_create(row, NULL);
		if (switches[i].init) {
			lv_switch_on(toggle, LV_ANIM_OFF);
		}
		lv_obj_set_user_data(toggle, &switches[i].id);
		lv_obj_set_event_cb(toggle, toggle_event_cb);

		lv_obj_t *label = lv_label_create(row, NULL);
		lv_label_set_text(label, switches[i].label);
	}

	while (1) {
		vTaskDelay(10 / portTICK_PERIOD_MS);

		lv_task_handler();
	}

	// Never returns
}
}
extern "C" void m5_lib_init(void)
{
	printf("Hello world!\n");

	/* Print chip information */
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
			CONFIG_IDF_TARGET,
			chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
			(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

	printf("silicon revision %d, ", chip_info.revision);

	printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

	printf("Free heap: %d\n", esp_get_free_heap_size());
	
	m5core2_init();

	lvgl_i2c_locking(i2c_manager_locking());

	lv_init();
	lvgl_driver_init();

	// Needs to be pinned to a core
	xTaskCreatePinnedToCore(gui_thread, "gui", 4096*2, NULL, 0, NULL, 1);
}
extern "C" void app_main()
{
    // Initialize the ESP NVS layer.
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs_flash_init() failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "==================================================");
    ESP_LOGI(TAG, "chip-esp32-light-switch-example starting");
    ESP_LOGI(TAG, "==================================================");
	m5_lib_init();
#if CONFIG_ENABLE_CHIP_SHELL
    chip::LaunchShell();
#endif // CONFIG_ENABLE_CHIP_SHELL

    CHIPDeviceManager & deviceMgr = CHIPDeviceManager::GetInstance();

    CHIP_ERROR error = deviceMgr.Init(&EchoCallbacks);
    if (error != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "device.Init() failed: %s", ErrorStr(error));
        return;
    }
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    if (ThreadStackMgr().InitThreadStack() != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to initialize Thread stack");
        return;
    }
    if (ThreadStackMgr().StartThreadTask() != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to launch Thread task");
        return;
    }
#endif

    chip::DeviceLayer::PlatformMgr().ScheduleWork(InitServer, reinterpret_cast<intptr_t>(nullptr));

    error = GetAppTask().StartAppTask();
    if (error != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "GetAppTask().StartAppTask() failed : %s", ErrorStr(error));
    }
}
