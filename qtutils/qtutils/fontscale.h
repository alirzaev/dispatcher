#pragma once

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define FONT_SCALE_FACTOR 1.2
#elif defined(Q_OS_LINUX)
#define FONT_SCALE_FACTOR 1.1
#else
#define FONT_SCALE_FACTOR 1.0
#endif

#define STANDARD_DPI 92
