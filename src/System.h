#ifndef VULKAN_SYSTEM_H
#define VULKAN_SYSTEM_H

#include <entt/entt.hpp>
#include <cassert>

namespace SGE {
    class System {
    public:
        virtual bool run(entt::registry *m_world);
    };

    void windowSizeUpdate(entt::registry &m_world, int width, int height) {
        for (auto &&[item, ui] : m_world.view<UIComponent>().each()) {
            if (ui.scalingFunction) {
                ui.scalingFunction(ui.xTop, ui.yTop, ui.xBottom, ui.yBottom, ui.xScale, ui.yScale, width, height);
            }
        }
    }
}


#endif //VULKAN_SYSTEM_H
