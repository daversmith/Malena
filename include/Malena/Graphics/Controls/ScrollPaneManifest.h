#ifndef MALENA_SCROLLPANEMANIFEST_H
#define MALENA_SCROLLPANEMANIFEST_H

#include <Malena/Core/malena_export.h>
#include <Malena/Manifests/Manifest.h>

namespace ml
{
	class MALENA_API ScrollPaneManifest : public ml::Manifest
	{
	public:
		enum class State { IDLE, SCROLLING };
		enum class Flag { VERTICAL, HORIZONTAL, DIRTY };
	};

}

#endif //SCROLLPANEMANIFEST_H
