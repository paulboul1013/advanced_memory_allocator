#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> //std::size_t

class Allocator{
    protected:
        std::size_t m_totalSize; //總記憶體大小
        std::size_t m_used; //已使用記憶體大小
        std::size_t m_peak; //記憶體使用巔峰量

    public:
        Allocator(std::size_t  totalSize): m_totalSize(totalSize), m_used(0), m_peak(0){}

        virtual ~Allocator(){ m_totalSize=0; }

        virtual void *Allocate(std::size_t size,const std::size_t alignment = 0) = 0;

        virtual void Free(void *ptr) = 0;

        virtual void Init() = 0;

        friend class Benchmark;
};       


#endif