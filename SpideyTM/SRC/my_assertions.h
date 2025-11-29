#pragma once

#ifndef MY_ASSERTIONS_H
#define MY_ASSERTIONS_H

extern int FAIL_VALIDATION;
void validate_i32(int cur, int expected, const char *name, int line);
#define VALIDATE_I32(x,y) validate_i32(x, y, __FILE__, __LINE__);

void validate_class(int cur, int expected, const char *cls, const char *member);
#define VALIDATE(cls, member, expected) validate_class(offsetof(cls, member), expected, #cls, #member);


void validate_size(int cur, int expected, const char *name);
#define VALIDATE_SIZE(cls, size) validate_size(sizeof(cls), size, #cls);

void validate_val(int, int, const char* name);
#define VALIDATE_VAL(val, exp) validate_val(val, exp, #val);

template<bool b>

struct StaticAssert{};



template<>

struct StaticAssert<true>
{
	static void sass() {}
};

int* get_thunk_address(void* first,...);

void validate_vtable_index(
		unsigned int expected_index,
		const int* known_address,
		const char *cls,
		const char *name);
#define VALIDATE_VTABLE(cls, member, expected) {\
	validate_vtable_index(expected, get_thunk_address(0, &cls::member), #cls, #member);\
}

#define VALIDATE_VTABLE_POLY(cls, member, func, expected) {\
	validate_vtable_index(expected, get_thunk_address(0, func), #cls, #member);\
}

#endif
