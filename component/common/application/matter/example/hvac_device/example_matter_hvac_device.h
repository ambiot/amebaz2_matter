/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/

#ifndef EXAMPLE_MATTER_HVAC_DEVICE_H
#define EXAMPLE_MATTER_HVAC_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Create Matter Task (for Matter Room Air-Conditioner).
 */
void example_matter_task_init_thread(void);
void example_matter_add_aircon(void);
void example_matter_add_fan(void);
void example_matter_add_thermostat(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_MATTER_HVAC_DEVICE_H */
