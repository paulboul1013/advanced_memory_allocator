#ifndef FREE_TREE_ALLOCATOR_H
#define FREE_TREE_ALLOCATOR_H

#include "Allocator.h"

class FreeTreeAllocator : public Allocator{
    protected:
        void *m_start_ptr=nullptr; //memory start address
        std::size_t m_offset=0; //current offset

    public:
        FreeTreeAllocator* root; //root of the free tree
        size_t size; //allocated memory block size in single node
        FreeTreeAllocator* l; //left child of the node
        FreeTreeAllocator* r; //right child of the node
        
        
    public:
        FreeTreeAllocator(std::size_t totalSize);

        virtual ~FreeTreeAllocator();

        virtual void *Allocate(std::size_t size,const std::size_t alignment = 0) override;

        virtual void Free(void *ptr) override;

        virtual void Init() override;

        virtual void Reset();

        FreeTreeAllocator** find_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* target);

        FreeTreeAllocator* find_predecessor_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* node);

        void remove_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* target);

        void *insert_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* node);

        void inorder_traverse(FreeTreeAllocator* root);

        void free_tree(FreeTreeAllocator* root);
};

#endif