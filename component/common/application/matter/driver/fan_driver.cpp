#include <fan_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterFan::SetEp(EndpointId ep)
{
    mEp = ep;
}

EndpointId MatterFan::GetEp(void)
{
    return mEp;
}

void MatterFan::Init(PinName pin)
{
    mPwm_obj                        = (pwmout_t*) pvPortMalloc(sizeof(pwmout_t));
    pwmout_init(mPwm_obj, pin);
#if defined(CONFIG_PLATFORM_8710C)
    pwmout_period_us(mPwm_obj, 20000); //pwm period = 20ms
    pwmout_start(mPwm_obj);
#endif
}

void MatterFan::deInit(void)
{
    pwmout_free(mPwm_obj);
    vPortFree(mPwm_obj);
}

void MatterFan::setFanMode(uint8_t mode)
{
    if (mMode == mode)
    {
        return;
    }

    mMode = mode;
}

void MatterFan::setFanSpeedPercent(uint8_t percent)
{
    if (mPercent == percent)
    {
        return;
    }

    ChipLogProgress(DeviceLayer, "Setting fan speed to %d\%", percent);
    mPercent = percent;
    float duty_cycle = (float) (percent) / 100;
    pwmout_write(mPwm_obj, duty_cycle);
}

chip::app::Clusters::FanControl::FanModeEnum MatterFan::mapPercentToMode(uint8_t percent)
{
    if (percent >= 80) // high
    {
        return chip::app::Clusters::FanControl::FanModeEnum::kHigh;
    }
    else if (percent >= 40) // medium
    {
        return chip::app::Clusters::FanControl::FanModeEnum::kMedium;
    }
    else if (percent >= 10) // low
    {
        return chip::app::Clusters::FanControl::FanModeEnum::kLow;
    }
    else // off
    {
        return chip::app::Clusters::FanControl::FanModeEnum::kOff;
    }
}

uint8_t MatterFan::mapModeToPercent(uint8_t mode)
{
    using namespace chip::app::Clusters::FanControl;

    if (mode == 1)
    {
        return 30;
    }
    else if (mode == 2)
    {
        return 70;
    }
    else if (mode == 3)
    {
        return 100;
    }
}
