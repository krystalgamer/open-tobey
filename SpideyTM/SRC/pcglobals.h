#ifndef PCGLOBALS_H
#define PCGLOBALS_H

#ifdef _WIN32

typedef float rational_t;

// @TODO
#define STUB(str)

#define CONCAT(x, y) x##y

#define EXPAND(x, y) CONCAT(x, y)
#define PADDING(x) EXPAND(unsigned char pad_, __LINE__)[(x)]

#define PADDING_VIRTUAL() EXPAND(virtual void my_padding_virtual_, __LINE__)(void) {}


#endif

#endif
