#pragma once

#ifdef OS_WIN
#ifdef USE_LIB
#define API_PREFIX __declspec(dllexport)
#else
#define API_PREFIX __declspec(dllimport)
#endif
#elif defined(OS_LINUX)
#define API_PREFIX
#endif