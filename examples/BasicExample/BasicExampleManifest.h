//
// Created by Dave Smith on 3/23/26.
//

#ifndef BASICEXAMPLEMANIFEST_H
#define BASICEXAMPLEMANIFEST_H

#include <Malena/Manifests/Manifest.h>
 struct BasicExampeManifest : public ml::Manifest
 {
 	enum class Images {ICON};
	 inline static const auto _ = []() {
	 	set(
	 		Images::ICON, "/Users/dsmizzle/CLionProjects/MalenaDeferred/Documentation/assets/malena_icon_3.png"
	 	);
		 return 0;
	 }();
 };

#endif //BASICEXAMPLEMANIFEST_H
