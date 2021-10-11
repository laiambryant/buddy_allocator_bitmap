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
