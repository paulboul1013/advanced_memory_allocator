#include "LinearAllocator.h"
#include <stdlib.h>
#include "Utils.h"
#include <cassert>
#include <algorithm>

#ifdef _DEBUG
#include <iostream>
#endif

LinearAllocator::LinearAllocator(const std::size_t totalSize): Allocator(totalSize){}


void LinearAllocator::Init(){
    if (m_start_ptr!=nullptr){
        free(m_start_ptr);
    }
    m_start_ptr = malloc(m_totalSize);
    m_offset = 0;
}

LinearAllocator::~LinearAllocator(){
    free(m_start_ptr);
    m_start_ptr = nullptr;
}

void *LinearAllocator::Allocate(const std::size_t size,const std::size_t alignment){
    std::size_t padding=0;
    std::size_t paddingAddress=0;
    const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

    if (alignment !=0 && m_offset % alignment != 0){
        //Alignment is required . Find the next aligned memory address and update 
        padding = Utils::CalculatePadding(currentAddress,alignment);
    }
    
    if (m_offset + padding + size > m_totalSize){
        return nullptr;
    }

    m_offset += padding + size;

    #ifdef _DEBUG
    std::cout << "========== 線性配置器分配記憶體 ==========" << std::endl;
    std::cout << "  要求大小: " << size << " bytes" << std::endl;
    std::cout << "  對齊要求: " << alignment << " bytes" << std::endl;
    std::cout << "  對齊填充: " << padding << " bytes" << std::endl;
    std::cout << "  當前位址: " << (void*) currentAddress << std::endl;
    std::cout << "  新偏移量: " << m_offset << " bytes" << std::endl;
    std::cout << "  剩餘空間: " << (m_totalSize - m_offset) << " bytes" << std::endl;
    std::cout << "===========================================" << std::endl;
    #endif

    m_used= m_offset;
    m_peak= std::max(m_peak,m_used);

    return (void *)currentAddress;
}

void LinearAllocator::Free(void * ptr){
    assert(false && "Use Reset() method");
}

void LinearAllocator::Reset(){
    m_offset=0;
    m_used=0;
    m_peak=0;
}