#include <iostream>
#include "Benchmark.h"
#include "LinearAllocator.h"
#include "CAllocator.h"
#include "free_tree_allocator.h"

using namespace std;


int main(){


    const std::size_t A=static_cast<std::size_t>(1e9);//定義1GB記憶體

    const std::vector<std::size_t> ALLOCATION_SIZES {32, 64, 256, 512, 1024};//定義5種分配大小
    const std::vector<std::size_t> ALIGNMENTS={8, 8, 8, 8, 8};//定義5種對齊大小

    LinearAllocator * linearAllocator = new LinearAllocator(A);

    CAllocator * cAllocator = new CAllocator();
   

    FreeTreeAllocator * freeTreeAllocator = new FreeTreeAllocator(A);

    Benchmark benchmark(OPERATIONS);


    cout<<"C ALLOCATOR----------------------------"<<endl;
    benchmark.MultipleFree(cAllocator,ALLOCATION_SIZES,ALIGNMENTS);
    cAllocator->Reset();

    delete cAllocator;

    cout<<"LINEAR ALLOCATOR----------------------------"<<endl;


    benchmark.MultipleAllocation(linearAllocator,ALLOCATION_SIZES,ALIGNMENTS);
    linearAllocator->Reset();

    delete linearAllocator;


    cout<<"FREE TREE ALLOCATOR----------------------------"<<endl;


    // benchmark.MultipleFree(freeTreeAllocator,ALLOCATION_SIZES,ALIGNMENTS);
    benchmark.MultipleAllocation(freeTreeAllocator,ALLOCATION_SIZES,ALIGNMENTS);


    delete freeTreeAllocator;


    return 0;
}



