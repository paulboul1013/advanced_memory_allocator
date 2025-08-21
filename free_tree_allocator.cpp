#include <cassert>
#include <cstddef>
#include <stdlib.h>
#include "Utils.h"
#include <algorithm>
#include "free_tree_allocator.h"
#include <iostream>


#ifdef _DEBUG //if debug mode is enabled, include the iostream library
#include <iostream>
#endif

//constructor allocate max memory to the free tree
FreeTreeAllocator::FreeTreeAllocator(std::size_t totalSize) : Allocator(totalSize) {
    this->l=nullptr;
    this->r=nullptr;
    this->root=nullptr;
}

//destructor
FreeTreeAllocator::~FreeTreeAllocator() {
    free(m_start_ptr);
    m_start_ptr = nullptr;
    this->free_tree(this->root);
    this->root=nullptr;

}

void FreeTreeAllocator::Init() {
    if (m_start_ptr!=nullptr){
        free(m_start_ptr);
    }
    m_start_ptr = malloc(m_totalSize);
    m_offset = 0;
    this->l=nullptr;
    this->r=nullptr;
    this->size=m_totalSize;
    this->root=nullptr;
}

//free tree structure
void FreeTreeAllocator::free_tree(FreeTreeAllocator* root) {
    if (root) {
        free_tree(root->l);
        free_tree(root->r);
        delete root;
    }
}

//inorder traverse the free tree
void FreeTreeAllocator::inorder_traverse(FreeTreeAllocator* root) {
    if (root) {
        inorder_traverse(root->l);
        std::cout << root->size << " ";
        inorder_traverse(root->r);
    }
}

//insert the node to the free tree with binary search tree
void *FreeTreeAllocator::insert_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* node) {
    if (!*root) { //if the root is null, insert the node to the root
        *root = node;
        node->l = nullptr;
        node->r = nullptr;
        return (void*)node;
    }
    //find the position to insert the node
    FreeTreeAllocator** current = root;
    while (*current) {
        if (node->size < (*current)->size)
            current = &(*current)->l;
        else
            current = &(*current)->r;
    }
    //insert node 
    *current = node;
    node->l = nullptr;
    node->r = nullptr;
    return (void*)current; //return the address of the node
}

//allocate memory with insert the node to the free tree with binary search tree
void *FreeTreeAllocator::Allocate(std::size_t size,const std::size_t alignment){
    std::size_t padding=0;
    const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

    if (alignment !=0 && m_offset % alignment != 0){
        //Alignment is required . Find the next aligned memory address and update 
        padding = Utils::CalculatePadding(currentAddress,alignment);
    }
    
    if (m_offset + padding + size > m_totalSize){ //if overflow memory return null
        return nullptr;
    }

    m_offset += padding + size;
    void * block_address = nullptr;

    FreeTreeAllocator *new_node=new FreeTreeAllocator(m_offset);
    new_node->size=size;
    block_address=insert_free_tree(&this->root,new_node);

    #ifdef _DEBUG
    std::cout << "========== binary search tree allocator分配記憶體 ==========" << std::endl;
    std::cout << "  要求大小: " << size << " bytes" << std::endl;
    std::cout << "  對齊要求: " << alignment << " bytes" << std::endl;
    std::cout << "  對齊填充: " << padding << " bytes" << std::endl;
    std::cout << "  當前位址: " << (void*) currentAddress << std::endl;
    std::cout << "  插入block的位址: " << (void*) block_address << std::endl;
    std::cout << "  新偏移量: " << m_offset << " bytes" << std::endl;
    std::cout << "  剩餘空間: " << (m_totalSize - m_offset) << " bytes" << std::endl;
    std::cout << "  樹的結構: " << std::endl;
    inorder_traverse(this->root);
    std::cout << std::endl;
    std::cout << "===========================================" << std::endl;
    #endif

    m_used= m_offset;
    m_peak= std::max(m_peak,m_used);

    return (void *)block_address;

}

//find the target node in free tree
FreeTreeAllocator** FreeTreeAllocator::find_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* target) {
    FreeTreeAllocator** current = root;
    while (*current && *current != target) {
        if (target->size < (*current)->size)
            current = &(*current)->l;
        else
            current = &(*current)->r;
    }
    return current;
    
}

//find the predecessor of the target node in free tree
FreeTreeAllocator* FreeTreeAllocator::find_predecessor_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* node){
    if (!node || !node->l)
        return NULL;
    FreeTreeAllocator *current = node->l;
    while (current->r)
        current = current->r;
    return current;
}

//remove the allocated memory target node from the free tree
void FreeTreeAllocator::remove_free_tree(FreeTreeAllocator** root, FreeTreeAllocator* target){
     /* Locate the pointer to the target node in the tree. */
     FreeTreeAllocator **node_ptr = find_free_tree(root, target);
 
     /* If the target node has two children, we need to find a replacement. */
     if ((*node_ptr)->l && (*node_ptr)->r) {
         /* Find the in-order predecessor:
          * This is the rightmost node in the left subtree.
          */
         FreeTreeAllocator **pred_ptr = &(*node_ptr)->l;
         while ((*pred_ptr)->r)
             pred_ptr = &(*pred_ptr)->r;
 
         /* Verify the found predecessor using a helper function (for debugging).
          */
         FreeTreeAllocator *expected_pred = find_predecessor_free_tree(root, *node_ptr);
         assert(expected_pred == *pred_ptr);
 
         /* If the predecessor is the immediate left child. */
         if (*pred_ptr == (*node_ptr)->l) {
             FreeTreeAllocator *old_right = (*node_ptr)->r;
             *node_ptr = *pred_ptr; /* Replace target with its left child. */
             (*node_ptr)->r = old_right; /* Attach the original right subtree. */
             assert(*node_ptr != (*node_ptr)->l);
             assert(*node_ptr != (*node_ptr)->r);
         } else {
             /* The predecessor is deeper in the left subtree. */
             FreeTreeAllocator *old_left = (*node_ptr)->l;
             FreeTreeAllocator *old_right = (*node_ptr)->r;
             FreeTreeAllocator *pred_node = *pred_ptr;
             /* Remove the predecessor from its original location. */
             remove_free_tree(&old_left, *pred_ptr);
             /* Replace the target node with the predecessor. */
             *node_ptr = pred_node;
             (*node_ptr)->l = old_left;
             (*node_ptr)->r = old_right;
             assert(*node_ptr != (*node_ptr)->l);
             assert(*node_ptr != (*node_ptr)->r);
         }
     }
     /* If the target node has one child (or none), simply splice it out. */
     else if ((*node_ptr)->l || (*node_ptr)->r) {
         FreeTreeAllocator *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
         *node_ptr = child;
     } else {
         /* No children: remove the node. */
         *node_ptr = NULL;
     }
 
     /* Clear the removed node's child pointers to avoid dangling references. */
     target->l = NULL;
     target->r = NULL;
}

void FreeTreeAllocator::Reset() {
    this->root=nullptr;
    this->size=this->m_totalSize;
    this->m_offset=0;
    this->m_used=0;
    this->m_peak=0;
}


//配合benchmark的single free和mutliple free相關函式
void FreeTreeAllocator::Free(void * ptr){
    FreeTreeAllocator *node = (FreeTreeAllocator *)ptr;
    remove_free_tree(&this->root,node);
    m_offset -= node->size;
    m_used= m_offset;
    m_peak= std::max(m_peak,m_used);
    delete node;
}
