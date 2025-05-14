#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>

bool MemoryPool_Init(MemoryPool* pool, size_t blockSize, int capacity) {
    pool->blockSize = blockSize;
    pool->capacity = capacity;
    pool->freeCount = capacity;
    
    // 모든 블록을 위한 메모리 할당
    pool->blocks = malloc(blockSize * capacity);
    if (!pool->blocks) return false;
    
    // 사용 가능한 블록 목록을 위한 메모리 할당
    pool->freeList = malloc(sizeof(void*) * capacity);
    if (!pool->freeList) {
        free(pool->blocks);
        return false;
    }
    
    // 사용 가능한 블록 목록 초기화
    for (int i = 0; i < capacity; i++) {
        pool->freeList[i] = (char*)pool->blocks + i * blockSize;
    }
    
    return true;
}

void* MemoryPool_Alloc(MemoryPool* pool) {
    if (pool->freeCount == 0) return NULL;
    
    // 사용 가능한 블록 가져오기
    void* ptr = pool->freeList[--pool->freeCount];
    
    // 메모리 초기화 (선택적)
    memset(ptr, 0, pool->blockSize);
    
    return ptr;
}

void MemoryPool_Free(MemoryPool* pool, void* ptr) {
    // 주어진 포인터가 유효한지 확인
    char* blockStart = (char*)pool->blocks;
    char* blockEnd = blockStart + (pool->capacity * pool->blockSize);
    
    if ((char*)ptr >= blockStart && (char*)ptr < blockEnd && pool->freeCount < pool->capacity) {
        // 블록을 사용 가능한 목록에 추가
        pool->freeList[pool->freeCount++] = ptr;
    }
}

void MemoryPool_Destroy(MemoryPool* pool) {
    if (pool->blocks) {
        free(pool->blocks);
        pool->blocks = NULL;
    }
    
    if (pool->freeList) {
        free(pool->freeList);
        pool->freeList = NULL;
    }
    
    pool->capacity = 0;
    pool->freeCount = 0;
} 