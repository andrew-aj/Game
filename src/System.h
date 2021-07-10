#ifndef VULKAN_SYSTEM_H
#define VULKAN_SYSTEM_H

#include <entt/entt.hpp>

namespace SGE {
    class System {
    public:
        virtual bool run(entt::registry *m_world);
    };
}


#endif //VULKAN_SYSTEM_H
