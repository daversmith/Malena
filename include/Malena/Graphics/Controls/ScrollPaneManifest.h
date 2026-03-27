#ifndef SCROLLPANEMANIFEST_H
#define SCROLLPANEMANIFEST_H

#include <Malena/Manifests/Manifest.h>

namespace ml
{
	class ScrollPaneManifest : public ml::Manifest
	{
	public:
		enum class State { IDLE, SCROLLING };
		enum class Flag { VERTICAL, HORIZONTAL, DIRTY };
	};

}

#endif //SCROLLPANEMANIFEST_H
