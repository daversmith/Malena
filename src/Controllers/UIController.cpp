
#include "UIController.h"

namespace ml {



    void UIController::onUpdate(std::function<void()> f) {
        proxy.onUpdate(f);
    }
} // namespace ml

