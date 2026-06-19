// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Created by Dave Smith on 10/3/25.
//

#ifndef MALENA_LOADERFUNCTIONS_H
#define MALENA_LOADERFUNCTIONS_H
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
