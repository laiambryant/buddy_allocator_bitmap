CCOPTS=-Wall -g -std=gnu99 
VGOPTS=--leak-check=full --show-leak-kinds=all --track-origins=yes --verbose 
GPROF_OPTS=-pg
LIBS=-lm
CC=gcc

OUT_DIR=OUT/
LOG_DIR=OUT/Logs/
BINS_DIR=OUT/Bins/
DUMPS_DIR=OUT/Dumps/
TEST_DIR=Tests/
OBJS_DIR=OUT/Objs/
ASM_DIR=OUT/ASM/

BINS= $(BINS_DIR)*

buddy: buddy.o
	@echo "Linking objects..."
	$(CC) $(CCOPTS) -o Bud_test Main.c Bitmap.*  BuddyAllocator.* Bitmap_tree.* $(LIBS)
	@echo "Running executable..."
	./Bud_test
	mv Bud_test $(BINS_DIR)

buddy.o: buddy.s
	@echo "Compiling Objects..."
	$(CC) $(CCOPTS) -c $(ASM_DIR)*.s  $(LIBS)
	mv *.o  $(OBJS_DIR)
	rm -rf *.h.gch

buddy.s:
	@echo "Generating assembly code..."
	$(CC) $(CCOPTS) -S Main.c Bitmap.* BuddyAllocator.* Bitmap_tree.*  $(LIBS)
	mv *.s $(ASM_DIR)
	rm -rf *.h.gch

buddyVG:
	@echo "Running valgrind on program..."
	$(CC) $(CCOPTS) -o Bud_test Main.c Bitmap.*  BuddyAllocator.* Bitmap_tree.* $(LIBS)
	valgrind $(VGOPTS)--log-file=bud-valgrind-out.txt \./Bud_test
	mv bud-valgrind-out.txt  $(LOG_DIR)
	mv Bud_test $(BINS_DIR)
	rm -rf gmon.out 

buddyGProf:
	@echo "Running GProf on program..."
	$(CC) $(CCOPTS) $(GPROF_OPTS) -o Bud_test Main.c Bitmap.* BuddyAllocator.* Bitmap_tree.* $(LIBS)
	./Bud_test
	gprof Bud_test gmon.out > Bud_TestGprof.txt
	mv Bud_TestGprof.txt $(LOG_DIR)
	rm -rf gmon.out Bud_test

buddyDump: buddy.o
	@echo "Generating memory dumps..."
	objdump -d $(OBJS_DIR)Bitmap.o > Bitmap_dump.txt 
	objdump -d $(OBJS_DIR)BuddyAllocator.o > Buddy_dump.txt 
	objdump -d $(OBJS_DIR)Main.o > Main_dump.txt 
	objdump -d $(OBJS_DIR)Bitmap_tree.o>Bitmap_tree_dump.txt
	mv *.txt $(ASM_DIR)

buddyDebugFull: buddyGProf buddyVG buddyDump
	@echo "Full Debug..."

dirSetup:
	@echo "Setting up directories"
	mkdir $(OUT_DIR) $(LOG_DIR) $(BINS_DIR) $(OBJS_DIR) $(ASM_DIR)

BitmapTest:
	@echo "Running Bitmap test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Bitmap_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out bm_test
	./bm_test
	mv bm_test $(BINS_DIR)
	rm -rf *.h.gch

InitTest:
	@echo "Running init test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Init_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out init_test
	./init_test
	mv init_test $(BINS_DIR)
	rm -rf *.h.gch


ConsistTest:
	@echo "Running init test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Mem_consist_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out consist_test
	./consist_test
	mv consist_test $(BINS_DIR)
	rm -rf *.h.gch

Errors_test_noMem: clean
	@echo "Running Error (No mem) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out err_test
	./err_test 1
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch


Errors_test_outOfRange: clean
	@echo "Running Error (Out Of Range free) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out err_test
	./err_test 2
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch


Errors_test_unalignedFree: clean
	@echo "Running Error (Unaligned free) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out err_test
	./err_test 3
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch


Errors_test_doubleFree: clean
	@echo "Running Error (Double Freee) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out err_test
	./err_test 4
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch

clean:
	@echo "Cleaning..."
	rm -rf *.o *.txt *.h.gch $(ASM_DIR)*.txt $(DUMPS_DIR)*.txt $(BINS) $(OBJS_DIR)*.o $(LOG_DIR)*.txt $(ASM_DIR)*.s $(BINS_DIR)a.out a.out.dSYM
