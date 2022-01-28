#pragma once

#include <stdio.h>

//-----------------------------------------------------------------------------

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

//-----------------------------------------------------------------------------

#define QR_SERVER_DEBUG
#ifdef QR_SERVER_DEBUG
# define PRINTF_D(...) fprintf(stderr, __VA_ARGS__)
#else
# define PRINTF_D(...) {}
#endif // QR_SERVER_DEBUG

