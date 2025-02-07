#include <platform_stdlib.h>
#include <app/server/Server.h>
#include <app/server/OnboardingCodesUtil.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <platform/Ameba/DiagnosticDataProviderImpl.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/CommissionableDataProvider.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>

#include <matter_api.h>

#if defined(CONFIG_ENABLE_AMEBA_OPHOURS) && (CONFIG_ENABLE_AMEBA_OPHOURS == 1)
#define HOUR_PER_MILLISECOND       ( 3600 * 1000 )
#endif

using namespace ::chip::Credentials;
using namespace ::chip::DeviceLayer;

uint8_t matter_get_total_operational_hour(uint32_t *totalOperationalHours)
{
    if (totalOperationalHours == nullptr)
    {
        printf("%s: nullptr\n", __FUNCTION__);
        return -1;
    }

    CHIP_ERROR err;
    DiagnosticDataProvider & diagProvider = chip::DeviceLayer::GetDiagnosticDataProviderImpl();

    if (&diagProvider != NULL)
    {
        err = diagProvider.GetTotalOperationalHours(*totalOperationalHours);
        if (err != CHIP_NO_ERROR)
        {
             printf("%s: GetTotalOperationalHours Failed err=%d\n", __FUNCTION__, err);
             return -1;
        }
    }
    else
    {
        printf("%s: DiagnosticDataProvider is invalid\n", __FUNCTION__);
        return -1;
    }
    return 0;
}

uint8_t matter_set_total_operational_hour(uint32_t time)
{
    CHIP_ERROR err = ConfigurationMgr().StoreTotalOperationalHours(time);

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

#if defined(CONFIG_ENABLE_AMEBA_OPHOURS) && (CONFIG_ENABLE_AMEBA_OPHOURS == 1)
static void matter_op_hours_task(void *pvParameters)
{
    uint32_t cur_hour = 0, prev_hour = 0;
    uint8_t ret = 0;
    char key[] = "temp_hour";

    // 1. Check if "temp_hour" exist in NVS
    if (checkExist(key, key) != DCT_SUCCESS)
    {
        // 2. If "temp_hour" exist, get "temp_hour" and set as "total_hour" into NVS
        if (getPref_u32_new(key, key, &prev_hour) == DCT_SUCCESS)
        {
            ret = matter_set_total_operational_hour(prev_hour);
            if (ret != 0)
            {
                printf("matter_store_total_operational_hour failed, ret=%d\n", ret);
                goto loop;
            }
            // 3. Delete "temp_hour" from NVS
            deleteKey(key, key);
        }
        else
        {
            printf("getPref_u32_new: %s not found\n", key);
            goto loop;
        }
    }

loop:
    while (1)
    {
        // 4. Every hour get Total operational hour
        ret = matter_get_total_operational_hour(&cur_hour);
        if (ret == 0)
        {
            // 5. If "prev_hour" and "cur_hour" differs, enter and store new value into NVS using "temp_hour"
            if (prev_hour != cur_hour)
            {
                prev_hour = cur_hour;
                if (setPref_new(key, key, (uint8_t *) &cur_hour, sizeof(cur_hour)) != DCT_SUCCESS)
                {
                    printf("setPref_new: temp_hour Failed\n");
                }
            }
        }
        vTaskDelay(HOUR_PER_MILLISECOND);
    }

    vTaskDelete(NULL);
}

void matter_op_hours(void)
{
    if (xTaskCreate(matter_op_hours_task, ((const char*)"matter_op_hours_task"), 2048, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
    {
        printf("\n\r%s xTaskCreate(matter_op_hours) failed", __FUNCTION__);
    }
}

extern "C" void matter_op_hours_wrapper(void)
{
    matter_op_hours();
}
#endif
