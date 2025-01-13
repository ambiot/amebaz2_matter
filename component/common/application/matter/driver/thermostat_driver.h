/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <platform_stdlib.h>
#include <app/util/attribute-table.h>

using namespace ::chip;
using namespace ::chip::app;

class MatterThermostat
{
public:
    void SetEp(EndpointId ep);
    EndpointId GetEp(void);

    void Init(void);
    void deInit(void);
    void Do(void);

private:
    EndpointId mEp;
};
