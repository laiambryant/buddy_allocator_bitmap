CCOPTS=-Wall -g -std=gnu99
VGOPTS=--leak-check=full --show-leak-kinds=all  --track-origins=yes  --verbose 
DBOPTS=-r
LIBS=-lm
CC=gcc
DEBUGGER=lldb

OUT_DIR=OUT/
LOG_DIR=OUT/Logs/
BINS_DIR=OUT/Bins/
DUMPS_DIR=OUT/Dumps/
TEST_DIR=Tests/
OBJS_DIR=OUT/Objs/
ASM_DIR=OUT/ASM/

BINS=$(BINS_DIR)bm_test $(BINS_DIR)Bud_test

buddy: buddy.o
	$(CC) $(CCOPTS) $(OBJS_DIR)*.o $(LIBS)
	./a.out
	mv a.out $(BINS_DIR)
 
buddy.o: buddy.s
	$(CC) $(CCOPTS) -c $(ASM_DIR)*.s
	mv *.o $(OBJS_DIR)

buddy.s:
	$(CC) $(CCOPTS) -S -O2 Main.c Bitmap.*  BuddyAllocator.* pool_allocator.* Bitmap_tree.*
	rm -rf *.h.gch
	mv *.s $(ASM_DIR)

BitmapTest:
	$(CC) $(CCOPTS) $(TEST_DIR)Bitmap_test.c Bitmap.* pool_allocator.* Bitmap_tree.* $(LIBS)
	mv a.out bm_test
	./bm_test
	mv bm_test $(BINS_DIR)
	rm -rf *.h.gch

OSX_buddyDebug: buddy
	sudo $(DEBUGGER) $(BINS_DIR)a.out $(DBOPTS) > $(LOG_DIR)debug.txt

clean:
	rm -rf *.o *.txt *.h.gch $(OUT_DIR)*.txt $(DUMPS_DIR)*.txt $(BINS) $(OBJS_DIR)*.o $(LOG_DIR)*.txt $(ASM_DIR)*.s $(BINS_DIR)a.out a.out.dSYM