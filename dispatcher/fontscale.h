#pragma once

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define FONT_SCALE_FACTOR 1.2
#elif defined(Q_OS_MACOS)
#define FONT_SCALE_FACTOR 1.1
#elif defined(Q_OS_LINUX)
#define FONT_SCALE_FACTOR 1.1
#endif
