#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>
#include <stdbool.h>

// 메모리 풀 구조체
typedef struct MemoryPool {
    void* blocks;         // 메모리 블록 배열 포인터
    void** freeList;      // 사용 가능한 블록 포인터 배열
    size_t blockSize;     // 각 블록 크기
    int capacity;         // 총 블록 수
    int freeCount;        // 사용 가능한 블록 수
} MemoryPool;

// 메모리 풀 초기화
bool MemoryPool_Init(MemoryPool* pool, size_t blockSize, int capacity);
// 메모리 풀에서 블록 할당
void* MemoryPool_Alloc(MemoryPool* pool);
// 메모리 풀에 블록 반환
void MemoryPool_Free(MemoryPool* pool, void* ptr);
// 메모리 풀 정리 및 모든 메모리 해제
void MemoryPool_Destroy(MemoryPool* pool);

#endif // MEMORY_POOL_H 