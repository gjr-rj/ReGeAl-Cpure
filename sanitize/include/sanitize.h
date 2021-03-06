#ifndef _SANITIZE_H
#define _SANITIZE_H

#include <stdlib.h>

#define STZ_MAX_SIZE_NAME_FUNC 100

#ifdef _RUN_SANITIZE
void stzLogMemory(void);
void *stzMalloc(size_t size, const char *functionName, unsigned line);
void stzfree(void *ptr);
unsigned int getSanitizeAmountAllocated(void);

#ifdef _RUN_SMOKE
void getSanitizeLastFuncName(char *funcName);
unsigned int getSanitizeLastLine(void);
unsigned long getSanitizeLastAddr(void);
size_t getSanitizeLastSize(void);
#endif /* #ifdef _RUN_SMOKE */

#define LOG_SANITIZE stzLogMemory
#define COMMON_MALLOC(a) stzMalloc(a, __FUNCTION__, __LINE__)
#define COMMON_FREE(a) stzfree(a)
#define COMMON_AMOUNT() getSanitizeAmountAllocated()
#else /* #ifdef _RUN_SANITIZE */
#define LOG_SANITIZE()
#define COMMON_MALLOC malloc
#define COMMON_FREE free
#define COMMON_AMOUNT() 0
#endif /* #ifdef _RUN_SANITIZE */

#endif /* #ifndef _SANITIZE_H */