#ifndef REALLOCARRAY_H
#define REALLOCARRAY_H
// --------

#if defined __ANDROID_API__ && __ANDROID_API__ <= 29

void* reallocarray(void* ptr, size_t nmemb, size_t size);

#else

// Ensure that including this header always
// provides a `reallocarray` declaration:
#include <stdlib.h>

#endif

// --------
#endif

