#pragma once

#ifndef GENERATIONS_DEVICECLASS_H
#define GENERATIONS_DEVICECLASS_H

#include "Includes.h"

namespace SGE {

    class DeviceClass {
    public:
        static DeviceClass* getInstance();

        static std::unique_ptr<DeviceClass> m_DeviceClass;

        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_pSwapChain;
    };
}
#endif //GENERATIONS_DEVICECLASS_H
