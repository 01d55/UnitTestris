// Compatibility header.

// If using autotools, use generated config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// If compiling on Windows, use hand-maintained stdafx.h
#ifdef WIN32
#include <stdafx.h>
#endif // WIN32
#endif // HAVE_CONFIG_H
