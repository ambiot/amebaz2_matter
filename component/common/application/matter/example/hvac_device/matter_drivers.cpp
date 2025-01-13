#include <matter_drivers.h>
#include <matter_interaction.h>
#include <gpio_api.h>
#include <gpio_irq_api.h>

#include <room_aircon_driver.h>
#include <fan_driver.h>
#include <temp_hum_sensor_driver.h>
#include <thermostat_driver.h>
#include <thermostat_ui_driver.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/util/attribute-table.h>

using namespace ::chip::app;

#define FAN_PIN         PA_23
#define PWM_PIN         PA_17

uint32_t identifyTimerCount;
constexpr uint32_t kIdentifyTimerDelayMS = 250;

// Set identify cluster and its callback on ep1
static Identify gIdentify1 = {
    chip::EndpointId{ 1 }, matter_driver_on_identify_start, matter_driver_on_identify_stop, Clusters::Identify::IdentifyTypeEnum::kVisibleIndicator, matter_driver_on_trigger_effect,
};

MatterRoomAirCon aircon;
MatterRoomAirCon::FanControl airconFan;

MatterFan roomFan;

MatterThermostatUI thermostatUI;
MatterThermostat roomThermostat;

static DeviceType device_type;

CHIP_ERROR matter_driver_fan_init(chip::EndpointId ep)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    EmberAfStatus status;
    DataModel::Nullable<uint8_t> FanPercentSettingValue;
    chip::app::Clusters::FanControl::FanModeEnum FanModeValue;

    ChipLogProgress(DeviceLayer, "Fan on Endpoint%d", ep);

    device_type = DeviceType::FAN_DEVICE;
    roomFan.SetEp(ep);
    roomFan.Init(PWM_PIN);

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    // get percent setting value
    status = Clusters::FanControl::Attributes::PercentSetting::Get(ep, FanPercentSettingValue);
    VerifyOrExit(status == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);
    // get fan mode
    status = Clusters::FanControl::Attributes::FanMode::Get(ep, &FanModeValue);
    VerifyOrExit(status == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);

    roomFan.setFanSpeedPercent(FanPercentSettingValue.Value());
    roomFan.setFanMode((uint8_t) FanModeValue);

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

exit:
    if (err == CHIP_ERROR_INTERNAL) {
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }
    return err;
}

CHIP_ERROR matter_driver_room_aircon_init(chip::EndpointId ep)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    EmberAfStatus status;
    DataModel::Nullable<uint8_t> FanPercentSettingValue;
    chip::app::Clusters::FanControl::FanModeEnum FanModeValue;

    ChipLogProgress(DeviceLayer, "Aircon on Endpoint%d", ep);

    device_type = DeviceType::AIRCON_DEVICE;
    aircon.SetEp(ep);
    airconFan.Init(FAN_PIN);

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    // get percent setting value
    status = Clusters::FanControl::Attributes::PercentSetting::Get(ep, FanPercentSettingValue);
    VerifyOrExit(status == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);
    // get fan mode
    status = Clusters::FanControl::Attributes::FanMode::Get(ep, &FanModeValue);
    VerifyOrExit(status == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);

    airconFan.setFanSpeedPercent(FanPercentSettingValue.Value());
    airconFan.setFanMode((uint8_t) FanModeValue);

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

exit:
    return err;
}

CHIP_ERROR matter_driver_thermostat_init(chip::EndpointId ep)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    EmberAfStatus getstatus;
    DataModel::Nullable<int16_t> temp;
    int16_t OccupiedCoolingSetpoint;
    int16_t OccupiedHeatingSetpoint;
    uint8_t SystemMode;

    ChipLogProgress(DeviceLayer, "Thermostat on Endpoint%d", ep);

    device_type = DeviceType::THERMOSTAT_DEVICE;
    roomThermostat.SetEp(ep);
    roomThermostat.Init();
    thermostatUI.Init();

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    getstatus = Clusters::Thermostat::Attributes::LocalTemperature::Get(ep, temp);
    VerifyOrExit(getstatus == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);
    getstatus = Clusters::Thermostat::Attributes::OccupiedCoolingSetpoint::Get(ep, &OccupiedCoolingSetpoint);
    VerifyOrExit(getstatus == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);
    getstatus = Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::Get(ep, &OccupiedHeatingSetpoint);
    VerifyOrExit(getstatus == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);
    getstatus = Clusters::Thermostat::Attributes::SystemMode::Get(1, &SystemMode);
    VerifyOrExit(getstatus == EMBER_ZCL_STATUS_SUCCESS, err = CHIP_ERROR_INTERNAL);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    if (temp.IsNull()) {
        thermostatUI.SetLocalTemperature(0);
    }
    else {
        thermostatUI.SetLocalTemperature(temp.Value());
    }

    thermostatUI.SetOccupiedCoolingSetpoint(OccupiedCoolingSetpoint);
    thermostatUI.SetOccupiedHeatingSetpoint(OccupiedHeatingSetpoint);
    thermostatUI.SetSystemMode((uint8_t)SystemMode);

    thermostatUI.UpdateDisplay();

exit:
    if (err == CHIP_ERROR_INTERNAL) {
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }

    return CHIP_NO_ERROR;
}

/* Identify Cluster Functions */
void matter_driver_on_identify_start(Identify * identify)
{
    ChipLogProgress(Zcl, "OnIdentifyStart");
}

void matter_driver_on_identify_stop(Identify * identify)
{
    ChipLogProgress(Zcl, "OnIdentifyStop");
}

void matter_driver_on_trigger_effect(Identify * identify)
{
    switch (identify->mCurrentEffectIdentifier)
    {
    case Clusters::Identify::EffectIdentifierEnum::kBlink:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kBlink");
        break;
    case Clusters::Identify::EffectIdentifierEnum::kBreathe:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kBreathe");
        break;
    case Clusters::Identify::EffectIdentifierEnum::kOkay:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kOkay");
        break;
    case Clusters::Identify::EffectIdentifierEnum::kChannelChange:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kChannelChange");
        break;
    default:
        ChipLogProgress(Zcl, "No identifier effect");
        return;
    }
}

void IdentifyTimerHandler(chip::System::Layer *systemLayer, void *appState, CHIP_ERROR error)
{
    if (identifyTimerCount)
    {
        identifyTimerCount--;
    }
}

void matter_driver_OnIdentifyPostAttributeChangeCallback(uint8_t *value)
{
    // timerCount represents the number of callback executions before we stop the timer.
    // value is expressed in seconds and the timer is fired every 250ms, so just multiply value by 4.
    // Also, we want timerCount to be odd number, so the ligth state ends in the same state it starts.
    identifyTimerCount = (*value) * 4;

exit:
    return;
}

/* Uplink and Downlink handler */
void matter_driver_uplink_update_handler(AppEvent *aEvent)
{
    chip::app::ConcreteAttributePath path = aEvent->path;
    DataModel::Nullable<uint8_t> FanPercentSettingValue;
    chip::EndpointId ep;

    ChipLogProgress(Zcl, "Uplink: mClusterId = 0x%x, mAttributeId = 0x%x", path.mClusterId, path.mAttributeId);

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    switch (path.mClusterId)
    {
    case Clusters::FanControl::Id:
        if (path.mAttributeId == Clusters::FanControl::Attributes::PercentSetting::Id) {
            if (device_type == DeviceType::AIRCON_DEVICE) {
                airconFan.setFanSpeedPercent(aEvent->value._u8);
            } else if (device_type == DeviceType::FAN_DEVICE) {
                roomFan.setFanSpeedPercent(aEvent->value._u8);
            }
        }
        else if (path.mAttributeId == Clusters::FanControl::Attributes::FanMode::Id) {
            if (device_type == DeviceType::AIRCON_DEVICE) {
                airconFan.setFanMode(aEvent->value._u8);
            } else if (device_type == DeviceType::FAN_DEVICE) {
                roomFan.setFanMode(aEvent->value._u8);
            }
        }
        break;
    case Clusters::Identify::Id:
        {
            matter_driver_OnIdentifyPostAttributeChangeCallback(&(aEvent->value._u8));
        }
        break;
    case Clusters::Thermostat::Id:
        {
            if (path.mAttributeId == Clusters::Thermostat::Attributes::LocalTemperature::Id) {
                thermostatUI.SetLocalTemperature(aEvent->value._u16);
                roomThermostat.Do();
            }
            if (path.mAttributeId == Clusters::Thermostat::Attributes::OccupiedCoolingSetpoint::Id) {
                thermostatUI.SetOccupiedCoolingSetpoint(aEvent->value._u16);
                roomThermostat.Do();
            }
            if (path.mAttributeId == Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::Id) {
                thermostatUI.SetOccupiedHeatingSetpoint(aEvent->value._u16);
                roomThermostat.Do();
            }
            if (path.mAttributeId == Clusters::Thermostat::Attributes::SystemMode::Id) {
                thermostatUI.SetSystemMode(aEvent->value._u8);
                roomThermostat.Do();
            }
            thermostatUI.UpdateDisplay();
        }
        break;
    case Clusters::OnOff::Id:
        if (path.mAttributeId == Clusters::OnOff::Attributes::OnOff::Id) {
            if (device_type == DeviceType::AIRCON_DEVICE) {
                airconFan.setFanMode((aEvent->value._u8 == 1) ? 4 /* kOn */ : 0 /* kOff */);
                ep = aircon.GetEp();
                if (aEvent->value._u8 == 1) {
                    Clusters::FanControl::Attributes::FanMode::Set(ep, Clusters::FanControl::FanModeEnum::kOn);
                }
                else {
                    Clusters::FanControl::Attributes::FanMode::Set(ep, Clusters::FanControl::FanModeEnum::kOff);
                }
            } else if (device_type == DeviceType::FAN_DEVICE) {
                roomFan.setFanMode((aEvent->value._u8 == 1) ? 4 /* kOn */ : 0 /* kOff */);
                ep = roomFan.GetEp();
            }
        }
        break;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

exit:
    return;
}

void matter_driver_downlink_update_handler(AppEvent *aEvent)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();

    //ChipLogProgress(Zcl, "Downlink");

    switch (aEvent->Type)
    {
    default:
        break;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}
