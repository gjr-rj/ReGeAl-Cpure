#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "sanitize.h"

#ifdef _RUN_SANITIZE

#define MAX_SIZE_NAME_FUNC 100

typedef struct _stSanitizeAlloc
{
    char funcName[MAX_SIZE_NAME_FUNC + 1];
    unsigned int line;
    unsigned long addr;
    size_t size;
    struct _stSanitizeAlloc *next;
} * _ptStSanitizeAlloc;

typedef struct
{
    _ptStSanitizeAlloc head;
    _ptStSanitizeAlloc foot;
    unsigned int len;
} _stSanitize;

static _stSanitize sanitize_ = {NULL, NULL, 0};

static void
removeSanitizeMemory_(_ptStSanitizeAlloc no, _ptStSanitizeAlloc prevno)
{
    assert(no != NULL);
    assert((no == sanitize_.head) || (prevno != NULL));

    if (no == sanitize_.head)
    {
        sanitize_.head = sanitize_.head->next;
    }
    else
    {
        prevno->next = no->next;
    }

    free(no);
    sanitize_.len--;
}

static void
addSanitizeMemory_(void *addr,
                   size_t size,
                   const char *functionName,
                   unsigned line)
{
    _ptStSanitizeAlloc no =
        (struct _stSanitizeAlloc *)malloc(sizeof(struct _stSanitizeAlloc));

    if (no != NULL)
    {
        strcpy(no->funcName, functionName);
        no->line = line;
        no->addr = (unsigned long)(addr);
        no->size = size;
        no->next = NULL;

        if (0 == sanitize_.len)
        {
            sanitize_.head = no;
        }
        else
        {
            sanitize_.foot->next = no;
        }

        sanitize_.foot = no;
        sanitize_.len++;
    }
}

void
stzLogMemory(void)
{
    if (sanitize_.len > 0)
    {
        _ptStSanitizeAlloc no;

        printf("**********************************************************\n");
        printf("Sanitize info: %u memory leak\n", sanitize_.len);
        printf("**********************************************************\n");

        for (no = sanitize_.head; no != NULL; no = no->next)
        {
            printf("Memory %lu, size %lu, allocated in function '%s' line %u\n",
                   (unsigned long)(no->addr),
                   (unsigned long)(no->size),
                   no->funcName,
                   no->line);
        }

        printf("**********************************************************\n");
    }
}

void *
stzMalloc(size_t size, const char *functionName, unsigned line)
{
    void *rc = malloc(size);

    if (rc != NULL)
    {
        addSanitizeMemory_(rc, size, functionName, line);
    }

    return rc;
}

void
stzfree(void *ptr)
{
    _ptStSanitizeAlloc no = NULL;
    _ptStSanitizeAlloc prevno = NULL;
    unsigned long addr = (unsigned long)(ptr);

    assert(ptr != NULL);
    for (no = sanitize_.head; no != NULL; no = no->next)
    {
        if (no->addr == addr)
        {
            removeSanitizeMemory_(no, prevno);
            break;
        }

        prevno = no;
    }

    free(ptr);
}

unsigned int
getSanitizeAmountAllocated(void)
{
    return sanitize_.len;
}

#ifdef _RUN_SMOKE
void
getSanitizeLastFuncName(char *funcName)
{
    strcpy(funcName, sanitize_.foot->funcName);
}

unsigned int
getSanitizeLastLine(void)
{
    return sanitize_.foot->line;
}

unsigned long
getSanitizeLastAddr(void)
{
    return sanitize_.foot->addr;
}

size_t
getSanitizeLastSize(void)
{
    return sanitize_.foot->size;
}

#endif /* #ifdef _RUN_SMOKE */

#endif /* #ifdef _RUN_SANITIZE */