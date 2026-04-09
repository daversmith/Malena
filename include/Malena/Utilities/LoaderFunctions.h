// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/3/25.
//

#ifndef LOADERFUNCTIONS_H
#define LOADERFUNCTIONS_H
/**
 * @file LoaderFunctions.h
 * @ingroup Utilities
 */
#include <Malena/Core/malena_export.h>
#include <string>
namespace ml
{
    template <typename Resource>
    bool fileLoader(Resource& r, const std::string& path) {return r.loadFromFile(path);}

    template <typename Resource>
    bool fileOpener(Resource& r, const std::string& path){return r.openFromFile(path);}

    template <typename Resource>
    bool returnTrue(Resource& r, const std::string& path){return true;}
}
#endif //LOADERFUNCTION_H
