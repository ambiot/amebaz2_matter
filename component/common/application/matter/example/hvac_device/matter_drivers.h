/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <matter_events.h>

#include <app/clusters/identify-server/identify-server.h>
#include <platform/CHIPDeviceLayer.h>

enum class DeviceType {
    THERMOSTAT_DEVICE = 0,
    FAN_DEVICE = 1,
    AIRCON_DEVICE = 2,
};

/**
 * @brief  Callback function for starting the identify process.
 * @param[in]  identify: Pointer to the Identify structure.
 */
void matter_driver_on_identify_start(Identify *identify);

/**
 * @brief  Callback function for stopping the identify process.
 * @param[in]  identify: Pointer to the Identify structure.
 */
void matter_driver_on_identify_stop(Identify *identify);

/**
 * @brief  Callback function for triggering an effect during the identify process.
 * @param[in]  identify: Pointer to the Identify structure.
 */
void matter_driver_on_trigger_effect(Identify *identify);

/**
 * @brief  Initialize the Device Driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_room_aircon_init(chip::EndpointId ep);
CHIP_ERROR matter_driver_fan_init(chip::EndpointId ep);
CHIP_ERROR matter_driver_thermostat_init(chip::EndpointId ep);

/**
 * @brief  Update uplink handler when receiving commands from Matter Controller.
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_uplink_update_handler(AppEvent *aEvent);

/**
 * @brief  Update downlink handler when receiving commands from external (e.g., GPIO, PWM).
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_downlink_update_handler(AppEvent *aEvent);
