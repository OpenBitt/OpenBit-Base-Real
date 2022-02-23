#include "Memory/pgfalloc.h"
#include "kernelpanic.h"
#include <kmem.h>

#define PGFALLOC_FAULT "Page Frame Allocator Fault:\nAllocator Function called before initialization."
#define PGFALLOC_DBMP  "Page Frame Allocator Fault:\nAllocator tried to reinitialize page bitmap."


static uint64_t mFreeSize,
                mReservedSize,
                mUsedSize;

static uint64_t pgBmpIndex;

static bmp_t    pgBitmap;
static memseg_t mSegment;

static uint8_t pgfInitialized;


static void __reserve_page__(void* __address) {
    kernel_panic_assert(pgfInitialized >= 2, PGFALLOC_FAULT);
    
    uint64_t _idx = (uint64_t)(__address) / 4096;

    SOFTASSERT(!(kernel_bmp_get(&pgBitmap, _idx)), RETVOID)
    
    kernel_bmp_set(&pgBitmap, _idx, 1);
    mFreeSize     -= 4096;
    mReservedSize += 4096;
}

static void __reserve_pages__(void* __address, uint64_t __pagecount) {
    for (uint64_t _i = 0; _i < __pagecount; _i++)
        __reserve_page__((void*)((uint64_t)(__address) + (_i * 4096))); 
}

static void __unreserve_page__(void* __address) {
    kernel_panic_assert(pgfInitialized >= 2, PGFALLOC_FAULT);
    
    uint64_t _idx = (uint64_t)(__address) / 4096;

    SOFTASSERT(kernel_bmp_get(&pgBitmap, _idx), RETVOID)
    
    kernel_bmp_set(&pgBitmap, _idx, 0);
    mFreeSize     += 4096;
    mReservedSize -= 4096;

    pgBmpIndex = (pgBmpIndex > _idx) ? _idx :
                                       pgBmpIndex;
}

static void __init__bitmap__() {
    kernel_panic_assert(pgfInitialized == 1, PGFALLOC_DBMP);
    
    memset(mSegment._Segment, mSegment._Size, 0);

    pgBitmap = (bmp_t) {
        ._Size = mFreeSize / 4096 / 8 + 1,
        ._Buffer = (uint8_t*)(mSegment._Segment)
    };

    pgfInitialized = 2;
    kernel_allocator_lock(pgBitmap._Buffer, pgBitmap._Size / 4096 + 1);
}

void __free_page__(void* __address) {
    kernel_panic_assert(pgfInitialized >= 2, PGFALLOC_FAULT);

    uint64_t _idx = (uint64_t)(__address) / 4096;

    SOFTASSERT(kernel_bmp_get(&pgBitmap, _idx), RETVOID)
    
    kernel_bmp_set(&pgBitmap, _idx, 0);
    mFreeSize += 4096;
    mUsedSize -= 4096;

    pgBmpIndex = (pgBmpIndex > _idx) ? _idx :
                                       pgBmpIndex;
}

void __lock__page__(void* __address) {
    kernel_panic_assert(pgfInitialized >= 2, PGFALLOC_FAULT);

    uint64_t _idx = (uint64_t)(__address) / 4096;

    SOFTASSERT(!(kernel_bmp_get(&pgBitmap, _idx)), RETVOID)
    
    kernel_bmp_set(&pgBitmap, _idx, 1);
    mFreeSize -= 4096;
    mUsedSize += 4096;
}

void kernel_allocator_initialize() {
    SOFTASSERT(pgfInitialized == 0, RETVOID);

    // Temporary code
    memseg_t  _bmp_seg;
    meminfo_t _meminfo;
    kernel_mmap_info_get(&mFreeSize, NULL, &_bmp_seg, &_meminfo);
    mSegment = _bmp_seg;

    pgfInitialized = TRUE;
    __init__bitmap__();
    
    for (uint64_t _i = 0; _i < _meminfo._MemoryMapSize / _meminfo._DescriptorSize; _i++) {
        efimemdesc_t* _mem_desc = kernel_mmap_entry_get(_i);

        if (_mem_desc->_Type != 7)
            kernel_allocator_reserve(_mem_desc->_PhysicalAddress, _mem_desc->_Pages); 
    }

    pgfInitialized = 3;
}

void kernel_allocator_free(void* __address, uint64_t __pagecount) {
    for (uint64_t _i = 0; _i < __pagecount; _i++)
        __free_page__((void*)((uint64_t)(__address) + (_i * 4096))); 
}

void kernel_allocator_lock(void* __address, uint64_t __pagecount) {
    for (uint64_t _i = 0; _i < __pagecount; _i++)
        __lock__page__((void*)((uint64_t)(__address) + (_i * 4096))); 
}

void kernel_allocator_unreserve(void* __address, uint64_t __pagecount) {
    for (uint64_t _i = 0; _i < __pagecount; _i++)
        __unreserve_page__((void*)((uint64_t)(__address) + (_i * 4096))); 
}

void kernel_allocator_reserve(void* __address, uint64_t __pagecount) {
    for (uint64_t _i = 0; _i < __pagecount; _i++)
        __reserve_page__((void*)((uint64_t)(__address) + (_i * 4096))); 
}

void kernel_allocator_info_get(uint64_t* __freemem, uint64_t* __usedmem, uint64_t* __reservedmem) { 
    ARGRET(__freemem, mFreeSize);
    ARGRET(__usedmem, mUsedSize);
    ARGRET(__reservedmem, mReservedSize);
}

void* kernel_allocator_page_new() {
    kernel_panic_assert(pgfInitialized >= 2, PGFALLOC_FAULT);

    for (; pgBmpIndex < pgBitmap._Size * 8; pgBmpIndex++) {
        if (kernel_bmp_get(&pgBitmap, pgBmpIndex) == 0) {
            void* _page = (void*)(pgBmpIndex * 4096);
            __lock__page__(_page);
            return _page;
        }
    }

    return NULL; // Swap file not implemented yet
}