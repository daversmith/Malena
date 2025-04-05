
#include <Malena/Controllers/UIController.h>

#include <functional>

namespace ml
{


	void UIController::onUpdate(std::function<void()> f)
	{
		proxy.onUpdate(f);
	}

} // namespace ml
