#ifndef REALLOCARRAY_H
#define REALLOCARRAY_H
// --------

#include <stdlib.h>

// --------

#if defined __ANDROID_API__ && __ANDROID_API__ <= 29

void* reallocarray(void* ptr, size_t nmemb, size_t size);

#endif

// --------
#endif

