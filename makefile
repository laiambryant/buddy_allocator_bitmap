CCOPTS=-Wall -Wextra -g -std=gnu99
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

BINS=$(BINS_DIR)*
SRCS=Main.c Bitmap.c BuddyAllocator.c Bitmap_tree.c
HDRS=Bitmap.h BuddyAllocator.h Bitmap_tree.h common.h

buddy: dirSetup buddy.o
	@echo "Linking objects..."
	$(CC) $(CCOPTS) -o Bud_test $(SRCS) $(LIBS)
	@echo "Running executable..."
	./Bud_test
	mv Bud_test $(BINS_DIR)

buddy.o: dirSetup buddy.s
	@echo "Compiling Objects..."
	$(CC) $(CCOPTS) -c $(ASM_DIR)*.s $(LIBS)
	mv *.o $(OBJS_DIR)
	rm -rf *.h.gch

buddy.s: dirSetup
	@echo "Generating assembly code..."
	$(CC) $(CCOPTS) -S $(SRCS) $(LIBS)
	mv *.s $(ASM_DIR)
	rm -rf *.h.gch

buddyVG: dirSetup
	@echo "Running valgrind on program..."
	$(CC) $(CCOPTS) -o Bud_test $(SRCS) $(LIBS)
	valgrind $(VGOPTS) --log-file=$(LOG_DIR)bud-valgrind-out.txt ./Bud_test
	mv Bud_test $(BINS_DIR)

buddyGProf: dirSetup
	@echo "Running GProf on program..."
	$(CC) $(CCOPTS) $(GPROF_OPTS) -o Bud_test $(SRCS) $(LIBS)
	./Bud_test
	gprof Bud_test gmon.out > $(LOG_DIR)Bud_TestGprof.txt
	rm -rf gmon.out Bud_test

buddyDump: buddy.o
	@echo "Generating memory dumps..."
	objdump -d $(OBJS_DIR)Bitmap.o > $(ASM_DIR)Bitmap_dump.txt
	objdump -d $(OBJS_DIR)BuddyAllocator.o > $(ASM_DIR)Buddy_dump.txt
	objdump -d $(OBJS_DIR)Main.o > $(ASM_DIR)Main_dump.txt
	objdump -d $(OBJS_DIR)Bitmap_tree.o > $(ASM_DIR)Bitmap_tree_dump.txt

buddyDebugFull: buddyGProf buddyVG buddyDump
	@echo "Full Debug complete."

dirSetup:
	@mkdir -p $(OUT_DIR) $(LOG_DIR) $(BINS_DIR) $(OBJS_DIR) $(ASM_DIR) $(DUMPS_DIR)

BitmapTest: dirSetup
	@echo "Running Bitmap test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Bitmap_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out bm_test
	./bm_test
	mv bm_test $(BINS_DIR)
	rm -rf *.h.gch

InitTest: dirSetup
	@echo "Running init test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Init_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out init_test
	./init_test
	mv init_test $(BINS_DIR)
	rm -rf *.h.gch

ConsistTest: dirSetup
	@echo "Running memory consistency test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Mem_consist_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out consist_test
	./consist_test
	mv consist_test $(BINS_DIR)
	rm -rf *.h.gch

Errors_test_noMem: dirSetup clean
	@echo "Running Error (No mem) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out err_test
	./err_test 1
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch

Errors_test_outOfRange: dirSetup clean
	@echo "Running Error (Out Of Range free) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out err_test
	./err_test 2
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch

Errors_test_unalignedFree: dirSetup clean
	@echo "Running Error (Unaligned free) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out err_test
	./err_test 3
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch

Errors_test_doubleFree: dirSetup clean
	@echo "Running Error (Double Free) test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Errors_test.c Bitmap.c Bitmap_tree.c BuddyAllocator.c $(LIBS)
	mv a.out err_test
	./err_test 4
	mv err_test $(BINS_DIR)
	rm -rf *.h.gch

test: BitmapTest InitTest ConsistTest
	@echo "All tests passed."

clean:
	@echo "Cleaning..."
	rm -rf *.o *.h.gch a.out $(ASM_DIR)*.txt $(BINS_DIR)* $(OBJS_DIR)*.o $(LOG_DIR)*.txt $(ASM_DIR)*.s
