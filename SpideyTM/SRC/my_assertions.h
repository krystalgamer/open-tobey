#pragma once

extern int FAIL_VALIDATION;
void validate_i32(int cur, int expected, const char *name, int line);
#define VALIDATE_I32(x,y) validate_i32(x, y, __FILE__, __LINE__);

void validate_class(int cur, int expected, const char *cls, const char *member);
#define VALIDATE(cls, member, expected) validate_class(offsetof(cls, member), expected, #cls, #member);


void validate_size(int cur, int expected, const char *name);
#define VALIDATE_SIZE(cls, size) validate_size(sizeof(cls), size, #cls);

template<bool b>

struct StaticAssert{};



template<>

struct StaticAssert<true>
{
	static void sass() {}
};
