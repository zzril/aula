#ifndef REALLOCARRAY_H
#define REALLOCARRAY_H
// --------

#if defined __ANDROID_API__ && __ANDROID_API__ <= 29

// --------

#include <stddef.h>

// --------

void *reallocarray(void *ptr, size_t nmemb, size_t size);

// --------

#endif

// --------
#endif

