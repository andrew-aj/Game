#include "DeviceClass.h"

namespace SGE{

    std::unique_ptr<DeviceClass> DeviceClass::m_DeviceClass;

    DeviceClass* DeviceClass::getInstance(){
        if (!m_DeviceClass){
            m_DeviceClass = std::make_unique<DeviceClass>();
        }

        return m_DeviceClass.get();
    }
}

