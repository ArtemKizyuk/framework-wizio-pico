////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <reent.h>
#include <sys/_timeval.h>
#include <sys/stat.h>

__attribute__((always_inline)) __inline void __disable_irq(void) { __asm volatile("cpsid i" : : : "memory"); }
__attribute__((always_inline)) __inline void __enable_irq(void) { __asm volatile("cpsie i" : : : "memory"); }

int _isatty(int fd) { return -1; }

_off_t _lseek_r(struct _reent *ignore, int fd, _off_t where, int whence) { return -1; }

int _fstat_r(struct _reent *ignore, int fd, struct stat *st) { return -1; }

int _close_r(struct _reent *ignore, int fd) { return -1; }

// Executed before main (crt0.S)
void system_init(void)
{
    void add_bootloader(void);
    add_bootloader(); // need to link boot2
}



#ifdef USE_FREERTOS
#include <string.h>

extern void *pvPortMalloc(size_t xWantedSize);
extern void vPortFree(void *pv);

void *malloc(size_t size)
{
    return pvPortMalloc(size);
}
void *_malloc_r(struct _reent *ignore, size_t size) { return malloc(size); }

void free(void *p)
{
    if (p)
        vPortFree(p);
}
void _free_r(struct _reent *ignore, void *ptr) { free(ptr); }

void *realloc(void *mem, size_t newsize)
{
    void *new = malloc(newsize);
    if ((new) && (mem))
        memcpy(new, mem, newsize);
    free(mem);
    return new;
}
void *_realloc_r(struct _reent *ignored, void *ptr, size_t size) { return realloc(ptr, size); }

void *calloc(size_t element, size_t size)
{
    size_t total = element * size;
    void *ret = malloc(total);
    if (ret)
        memset(ret, 0, total);
    return ret;
}
void *_calloc_r(struct _reent *ignored, size_t element, size_t size) { return calloc(element, size); }
#endif