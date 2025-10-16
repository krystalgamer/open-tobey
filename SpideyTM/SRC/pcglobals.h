#ifndef PCGLOBALS_H
#define PCGLOBALS_H

#ifdef _WIN32

typedef float rational_t;

// @TODO
#define STUB(str)

#define CONCAT(x, y) x##y

#define EXPAND(x, y) CONCAT(x, y)
#define PADDING(x) EXPAND(unsigned char pad_, __LINE__)[(x)]


#endif

#endif
