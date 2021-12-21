

#ifndef GENERATIONS_DEVICECLASS_H
#define GENERATIONS_DEVICECLASS_H

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "Common/interface/RefCntAutoPtr.hpp"

namespace SGE {

    class DeviceClass {
    public:
        static Diligent::IRenderDevice *getDevice() {
            return m_pDevice;
        }

        static Diligent::IDeviceContext *getContext() {
            return m_pImmediateContext;
        }

        static Diligent::ISwapChain *getSwapChain() {
            return m_pSwapChain;
        }

        static Diligent::IEngineFactory *getEngineFactory() {
            return m_pDevice->GetEngineFactory();
        }

        static Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        static Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        static Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_pSwapChain;
    };

    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> DeviceClass::m_pDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> DeviceClass::m_pImmediateContext;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> DeviceClass::m_pSwapChain;

}
#endif //GENERATIONS_DEVICECLASS_H
