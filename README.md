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
    
  1. <code> make buddyAlloc </code> compiles and runs the main with all the tests
    
  2. <code> make buddy.o </code> compiles object files
    
  3. <code> make buddyVG </code> compiles and runs program through valgrind. Valgrind output is then moved into the OUT sub-directory
    
  4. <code> make buddyGProf </code> compiles with GProf options and runs program. Runs gprof on the Gmon and moves the output to the OUT sub-directory
    
  5. <code> make buddyDump </code> runs <code> make buddy.o </code> and then runs objdump. All the dumps are then moved to the OUT/DUMP sub-directory
    
  6. <code> make buddyDebugFull </code> runs <code> make buddyVG </code>, <code> make buddyGProf </code>, <code> make buddyDump </code>
    
  7. <code> make clean </code> removes executable, objects, precompiled headers and ouput of other tests.
  
  8. <code>BitmapTest</code> tests bitmap (main is in the /Tests dir)
  
  9. <code>BitmapVG</code> Compiles and runs bitmap test through valgrind(main is in the /Tests dir)


### Explaination of how the allocator works


### Choices made while making the allocator


### References
