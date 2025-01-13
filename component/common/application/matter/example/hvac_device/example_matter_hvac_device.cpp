#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#include <basic_types.h>
#include <wifi_constants.h>
#include <wifi/wifi_conf.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_data_model.h>
#include <matter_data_model_presets.h>
#include <matter_drivers.h>
#include <matter_interaction.h>

#include <app/ConcreteAttributePath.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

#if defined(CONFIG_EXAMPLE_MATTER_HVAC_DEVICE) && CONFIG_EXAMPLE_MATTER_HVAC_DEVICE

#define DEVICE_TYPE_ROOT_NODE       0x0016
#define DEVICE_TYPE_ROOM_AIRCON     0x0072
#define DEVICE_TYPE_FAN             0x002B
#define DEVICE_TYPE_THERMOSTAT      0x0301

// Device Version for dynamic endpoints:
#define DEVICE_VERSION_DEFAULT 1

EmberAfDeviceType rootNodeDeviceTypes[] = {
    { DEVICE_TYPE_ROOT_NODE, DEVICE_VERSION_DEFAULT },
};

EmberAfDeviceType RoomAirConDeviceTypes[] = {
    { DEVICE_TYPE_ROOM_AIRCON, DEVICE_VERSION_DEFAULT },
};

EmberAfDeviceType FanDeviceTypes[] = {
    { DEVICE_TYPE_FAN, DEVICE_VERSION_DEFAULT },
};

EmberAfDeviceType ThermostatDeviceTypes[] = {
    { DEVICE_TYPE_THERMOSTAT, DEVICE_VERSION_DEFAULT },
};

Node& node = Node::getInstance();
chip::EndpointId AirconEpId;
chip::EndpointId FanEpId;
chip::EndpointId ThermostatEpId;
uint8_t device_type = 0;
extern "C" void example_matter_add_aircon(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    EndpointConfig roomAirconEndpointConfig;
    
    ChipLogProgress(DeviceLayer, "Matter Room Air-Conditioner Example");
    
    Presets::Endpoints::matter_room_air_conditioner_preset(&roomAirconEndpointConfig);
    AirconEpId = node.addEndpoint(roomAirconEndpointConfig, Span<const EmberAfDeviceType>(RoomAirConDeviceTypes));

    node.enableAllEndpoints();

    err = matter_driver_room_aircon_init(AirconEpId);
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_room_aircon_init failed!\n");
    }
}

extern "C" void example_matter_add_fan(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    EndpointConfig FanEndpointConfig;
    
    ChipLogProgress(DeviceLayer, "Matter Fan Example");
    
    Presets::Endpoints::matter_fan_preset(&FanEndpointConfig);
    FanEpId = node.addEndpoint(FanEndpointConfig, Span<const EmberAfDeviceType>(FanDeviceTypes));
    node.enableAllEndpoints();

    err = matter_driver_fan_init(FanEpId);
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_fan_init failed!\n");
    }
}

extern "C" void example_matter_add_thermostat(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    EndpointConfig ThermostatEndpointConfig;

    ChipLogProgress(DeviceLayer, "Matter Thermostat Example");

    Presets::Endpoints::matter_thermostat_preset(&ThermostatEndpointConfig);   
    ThermostatEpId = node.addEndpoint(ThermostatEndpointConfig, Span<const EmberAfDeviceType>(ThermostatDeviceTypes));
    node.enableAllEndpoints();

    err = matter_driver_thermostat_init(ThermostatEpId);
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_thermostat_init failed!\n");
    }
}

static void example_matter_task_init_thread(void *pvParameters)
{
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE))) {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "Matter Task Initialization....!");

    CHIP_ERROR err = CHIP_NO_ERROR;
    chip::EndpointId endpoint = 1;

    initPref();

    err = matter_core_start();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_core_start failed!");
    }

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!");
    }

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!");
    }

    EndpointConfig rootNodeEndpointConfig;
    Presets::Endpoints::matter_root_node_preset(&rootNodeEndpointConfig);
    node.addEndpoint(rootNodeEndpointConfig, Span<const EmberAfDeviceType>(rootNodeDeviceTypes));
    node.enableAllEndpoints();

    if (device_type == 1) {
        example_matter_add_thermostat();
    } else if (device_type == 2) {
        example_matter_add_fan();
    } else if (device_type == 3) {
        example_matter_add_aircon();
    }

    vTaskDelete(NULL);
}

extern "C" void example_matter_task_init(void)
{
    if (xTaskCreate(example_matter_task_init_thread, ((const char*)"example_matter_task_init_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_task_init) failed", __FUNCTION__);
    }
}

#endif /* CONFIG_EXAMPLE_MATTER_HVAC_DEVICE */
