#include <thermostat_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterThermostat::SetEp(EndpointId ep)
{
    mEp = ep;
}

EndpointId MatterThermostat::GetEp(void)
{
    return mEp;
}

void MatterThermostat::Init(void)
{
    // init thermostat driver code
}

void MatterThermostat::deInit(void)
{
    // deinit thermostat driver code
}

void MatterThermostat::Do(void)
{
    // implement thermostat action here
    ChipLogProgress(DeviceLayer, "Thermostat action");
}
