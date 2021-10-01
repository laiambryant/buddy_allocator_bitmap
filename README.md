# buddy_allocator_bitmap

This is my project for my Operating Systems Course in University La Sapienza. It extends and modifies an implementation of a buddy allocator you can find at the following link: *https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/tree/master/source/06_memory_allocation/buddy_allocator*.

This implementations of a buddy allocator uses a bitmap for bookkeping purposes and will be also used in another of my projects you can find at the following link: *https://github.com/laiambryant/NES_EMU*.

The docs will be structured in 4 parts:
  1. How to run the tests on the allocator
  2. Explaination of how the allocator works
  3. Choices made while making the allocator
  4. References

### How to run the tests on the allocator
  I made a very simple makefile to handle all compiling purposes. There are 4 make commands:
    
  1. <code> make buddy </code> links object files, dependancy on make buddy.o, and runs it
    
  2. <code> make buddy.o </code> compiles object files dependancy on buddy.s
    
  3. <code> make buddy.s </code> generates assembly code
    
  4. <code> make buddyGProf </code> compiles with GProf options and runs program. Runs gprof on the Gmon and moves the output to the OUT sub-directory
    
  5. <code> make BitmapTest </code> compiles and runs using the main in Tests/Bitmap_test.c
    
  6. <code> make OSXDebug </code> debugs program through lldb
    
  7. <code> make clean </code> removes executable, objects, precompiled headers and ouput of other tests.


### Explaination of how the allocator works


### Choices made while making the allocator


### References


/*
        if(level == 0){
            BitMap_setBit(b_alloc->tree->BitMap, 1, ALLOCATED);
            return (Buddy_item*) b_alloc->memory;
        }
        DATA_MAX first_idx = tree_first_free_node_level(b_alloc->tree, level);
        if(first_idx==0)
            first_idx = tree_first_node_level(b_alloc->tree, level+1);
        if(!(first_idx-(tree_first_node_level(b_alloc->tree, first_idx)))){
            offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level);
        }
        else {
            DATA_MAX idx=tree_first_node_level(b_alloc->tree, first_idx);
            DATA_MAX free_idx= tree_first_free_node_level(b_alloc->tree, level);
            offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level) * (first_idx-idx);
            //offset =((idx - (1 << level))) << (b_alloc->num_levels - free_idx) * b_alloc->min_bucket_size;

        }

        if(DEBUG){
            printf("Buddies on level %d: %d\n", level, tree_buddiesOnLevel(b_alloc->tree, level));
            printf("%d * (%d - %d) * (%d - %d)\n", b_alloc->min_bucket_size, b_alloc->num_levels, level, first_idx, tree_first_node_level(b_alloc->tree, first_idx));
            printf("[Address]:%p \t[Offset]: %d,\n", b_alloc->memory+offset, offset);
            tree_print(b_alloc->tree, F_CONCAT);
        }

        Buddy_item* item = b_alloc->memory + offset;
        item->mem=b_alloc->memory+offset;
        item->idx=first_idx;
        BitMap_setBit(b_alloc->tree->BitMap, first_idx, ALLOCATED);
        printf("Item addr: %p\t Item idx: %d\n", item->mem, item->idx);
        return item;
        */