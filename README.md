# buddy_allocator_bitmap

This is my project for my Operating Systems Course in University La Sapienza. It extends and modifies an implementation of a buddy allocator you can find at the following link: *https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/tree/master/source/06_memory_allocation/buddy_allocator*.

This implementations of a buddy allocator uses a bitmap for bookkeeping 

The docs will be structured in 4 parts:
  1. How to run the tests on the allocator
  2. Explaination of how the allocator works
  3. Choices made while making the allocator
  4. Error Handling

## How to run the tests on the allocator
  I made a very simple makefile to handle all compiling purposes. Before running run <code> make dirSetup </code> to setup the directories.
  <br><br>
  There are 4 make commands:
    
  1. <code> make buddy </code> links object files, dependancy on make buddy.o, and runs it
    
  2. <code> make buddy.o </code> assembles object files dependancy on buddy.s
    
  3. <code> make buddy.s </code> compiles assembly code
    
  4. <code> make buddyGProf </code> compiles with GProf flag and runs executable. Runs gprof on the Gmon and moves the output to the OUT/Logs sub-directory

  5. <code> make buddyVG </code> compiles with Valgrind flag and runs program then runs valgrind on the executable and moves log file to the OUT/Logs sub-directory

  6. <code> make buddyDump </code> runs make buddy.o command and objdump on objects then moves dumps to the OUT/ASM sub-directory

  7. <code> make buddyDebugFull </code> runs <code>make buddyVG </code>, <code>make BuddyDump </code> and <code>make BuddyGProf </code>
    
  8. <code> make BitmapTest </code> compiles and runs using the main in Tests/Bitmap_test.c
    
  9. <code> make InitTest </code> compiles and runs using the main in Tests/Init_test.c
    
  10. <code> make clean </code> removes executable, objects, precompiled headers and ouput of other tests. 
    


## Explanation of how the allocator works
The allocator works as a typical buddy allocator: memory is partitioned in half until the size of the partition is the smallest possible to satisfy the memory request. Should we have a buffer of 1MB for example and should we receive a request for 100 kB of memory, memory will be divided into two 512 kB partitions, then two 256 kB partitions and finally into two 128 kB partitions. Successive partitioning would make the memory returned too small so the chosen “level” is the 4th. The partitioning is abstract in this implementation: we will have a bitmap representation of a tree that will help in finding the address that has to be returned to the user in order to satisfy the memory request and the memory pointed by the address will be preceded by the index of the item in the tree.<br><br>
Each level starts with index 2^level of the Bitmap and ends with index (2^(level+1)-1). Every time the allocator gets a request through <code>BuddyAllocator_malloc</code>, it calculates the smallest possible level to serve the request and returns the first free memory segment for that level. Once it has found the index the allocator goes both up and down the tree and sets all bits corresponding to parent nodes and children nodes. When memory is released with <code>BuddyAllocator_free</code>, the allocator looks for the index of that node in the 4 bits preceding the address. Subsequently children are all freed and parents are freed, but only if buddy of the item is also free. 
Respectively in BuddyAllocator.c, Bitmap.c, Bitmap_tree.c. Bitmap_print and tree_print have 3 output modes: STDOUT that prints debugging info on stdout, F_WRITE that prints on a new file (uses <code>fprintf</code> with "w" flag), F_CONCAT that concats debugging info to an existing file (uses <code>fprintf</code> with "a" flag). 
The allocator buffer is structured in the following way: First we have the Buddyallocator struct, then the Bitmap_tree struct, then the Bitmap Struct followed by the actual bitmap. <br><br>After these first 3 structs we have the memory that can be allocated. Each allocated part of the memory will include, stored in the first 4 bytes of the memory segment, the index it would have in the bitmap. This is done to aid the free function that needs the index of the object to work: <br>
For example if my memory starts at address 0x0004 and the user asks to free address 0x0004 that address could have been returned by a malloc function that requests the whole memory or by a malloc that requests a small amount of bytes at a lower level. We have included the index that item would have in the tree for this exact situation: allocator reads address 0x0004, goes to address 0x0004 - <code>sizeof(index)</code>  and reads the index.  <br><br>
By storing the index associated to that address we can find all data associated to that memory segment (for example the size, level, end of memory segment etc.) inspecting the tree. 
Last choice made was to not use the pool allocator, because it took little to no effort to remove it.


## Error Handling
There are 5 error codes:
1. <code>BA_Success</code>, success
2. <code>BA_NotEnoughMemory</code>, not enough memory to satisfy request
3. <code>BA_UnalignedFree</code>, pointer returned by user to free is not aligned
4. <code>BA_OutOfRange</code>, pointer returned by user to free is not within allocator memory boundaries
5. <code>BA_DoubleFree</code>, pointer returned by user points to a memory location that is already free

The NotEnoughMemory error can only be returned by malloc function, the  BA_UnalignedFree, BA_OutOfRange, BA_DoubleFree errors can only be returned by free function.


## References
Repository:<br><br>
 https://github.com/laiambryant/buddy_allocator_bitmap

Original project: <br><br>
https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/tree/master/source/06_memory_allocation/buddy_allocator.

Various sources:<br><br>
https://stackoverflow.com/questions/29071093/how-to-map-bits-from-a-bitmap-to-physical-pages-in-a-buddy-allocator<br><br>
https://eatplayhate.me/2010/09/04/memory-management-from-the-ground-up-2-foundations/<br><br>
https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/blob/master/slides/os_06_memory_allocators.pdf
