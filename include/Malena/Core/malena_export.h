// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_MALENA_EXPORT_H
#define MALENA_MALENA_EXPORT_H

#pragma once

#ifdef _WIN32
#  ifdef MALENA_EXPORTS
#    define MALENA_API __declspec(dllexport)
#  elif defined(MALENA_SHARED)
#    define MALENA_API __declspec(dllimport)
#  else
#    define MALENA_API
#  endif
#else
#  define MALENA_API
#endif

#endif // MALENA_MALENA_EXPORT_H
