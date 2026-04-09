// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_EXPORT_H
#define MALENA_EXPORT_H

#pragma once

#ifdef _WIN32
#  ifdef MALENA_EXPORTS
#    define MALENA_API __declspec(dllexport)
#  else
#    define MALENA_API
#  endif
#else
#  define MALENA_API
#endif

#endif // MALENA_EXPORT_H
