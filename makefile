CCOPTS=-Wall -g -std=gnu99
VGOPTS=--leak-check=full --show-leak-kinds=all  --track-origins=yes  --verbose 
LIBS=-lm
CC=gcc
CC_GPROF = -pg
CC_DUMP= -c

OUT_DIR=OUT/
LOG_DIR=OUT/Logs/
BINS_DIR=OUT/Bins/
TEST_DIR=Tests/
OBJS_DIR=OUT/Objs/
ASM_DIR=OUT/ASM/

BINS=$(BINS_DIR)BM_test $(BINS_DIR)Bud_test

buddy: 
	$(CC) $(CCOPTS) -o Bud_test Main.c Bitmap.* BuddyAllocator.* pool_allocator.* Bitmap_tree.* $(LIBS)
	./Bud_test
	mv Bud_test $(BINS_DIR)

buddy.o: 
	$(CC) $(CCOPTS) $(CC_DUMP) Main.c Bitmap.*  BuddyAllocator.* pool_allocator.* Bitmap_tree.* $(LIBS)
	mv *.o  $(OBJS_DIR)
	rm -rf *.h.gch

buddy.s:
	$(CC) $(CCOPTS) -s Bitmap.*  BuddyAllocator.* pool_allocator.* Bitmap_tree.* $(LIBS)
	mv *.s $(ASM)
	rm -rf *.h.gch

buddyVG:
	$(CC) $(CCOPTS) -o Bud_test Main.c Bitmap.*  BuddyAllocator.* pool_allocator.* Bitmap_tree.* $(LIBS)
	valgrind $(VGOPTS)--log-file=bud-valgrind-out.txt \./Bud_test
	mv bud-valgrind-out.txt  $(LOG_DIR)
	mv Bud_test $(BINS_DIR)
	rm -rf gmon.out 

buddyGProf:
	$(CC) $(CCOPTS) $(CC_GPROF) -o Bud_test Main.c Bitmap.* BuddyAllocator.* pool_allocator.* Bitmap_tree.* $(LIBS)
	./Bud_test
	gprof Bud_test gmon.out > Bud_TestGprof.txt
	mv Bud_TestGprof.txt $(LOG_DIR)
	rm -rf gmon.out Bud_test

buddyDump: buddy.o
	objdump -d $(OBJS_DIR)Bitmap.o > Bitmap_dump.txt 
	objdump -d $(OBJS_DIR)BuddyAllocator.o > Buddy_dump.txt 
	objdump -d $(OBJS_DIR)Main.o > Main_dump.txt 
	objdump -d $(OBJS_DIR)Bitmap_tree.o>Bitmap_tree_dump.txt
	mv *.txt $(ASM_DIR)

buddyDebugFull: buddyGProf buddyVG buddyDump

BitmapTest:
	$(CC) $(CCOPTS) -o BM_test $(TEST_DIR)Bitmap_test.c Bitmap.* pool_allocator.* Bitmap_tree.* $(LIBS)
	./BM_test
	mv BM_test $(BINS_DIR)

BitmapVG:
	$(CC) $(CCOPTS) -o BM_test $(TEST_DIR)Bitmap_test.c Bitmap.* pool_allocator.* Bitmap_tree.* $(LIBS)
	valgrind $(VGOPTS)--log-file=bm-valgrind-out.txt \./BM_test
	mv bm-valgrind-out.txt  $(OUT_DIR)
	mv BM_test $(BINS_DIR)
	rm -rf gmon.out

dirSetup:
	mkdir $(OUT_DIR) $(LOG_DIR) $(BINS_DIR) $(OBJS_DIR) $(ASM_DIR)

clean:
	rm -rf *.o Bud_test *.h.gch $(OUT_DIR)*.txt $(DUMPS_DIR)*.txt $(BINS) $(LOG_DIR)*.txt $(OBJS_DIR)*.o $(OBJS_DIR)*.h.gch $(ASM_DIR)*.txt