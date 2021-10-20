CCOPTS=-Wall -g -std=gnu99
VGOPTS=--leak-check=full --show-leak-kinds=all  --track-origins=yes  --verbose 
DBOPTS=
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
	@echo "Linking obj files..."
	$(CC) $(CCOPTS) $(OBJS_DIR)*.o $(LIBS)
	@echo "Executing program"
	./a.out
	mv a.out $(BINS_DIR)
 
buddy.o: buddy.s
	@echo "Creating obj files..."
	$(CC) $(CCOPTS) -c $(ASM_DIR)*.s
	mv *.o $(OBJS_DIR)

buddy.s:
	@echo "Generating asm code..."
	$(CC) $(CCOPTS) -S -O2 Main.c Bitmap.*  BuddyAllocator.*  Bitmap_tree.*
	rm -rf *.h.gch
	mv *.s $(ASM_DIR)

BitmapTest: 
	@echo "Running bitmap test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Bitmap_test.c Bitmap.* Bitmap_tree.* $(LIBS)
	mv a.out bm_test
	./bm_test
	mv bm_test $(BINS_DIR)
	rm -rf *.h.gch

InitTest:
	@echo "Running init test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Init_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out bm_test
	./bm_test
	mv bm_test $(BINS_DIR)
	rm -rf *.h.gch

ConsistTest:
	@echo "Running init test..."
	$(CC) $(CCOPTS) $(TEST_DIR)Mem_consist_test.c Bitmap.* BuddyAllocator.*  Bitmap_tree.* $(LIBS)
	mv a.out bm_test
	./bm_test
	mv bm_test $(BINS_DIR)
	rm -rf *.h.gch

OSX_buddyDebug: buddy
	@echo "Running OSX debugging... give access to folder"
	sudo $(DEBUGGER) $(BINS_DIR)a.out $(DBOPTS) > $(LOG_DIR)debug.txt

clean:
	@echo "Cleaning..."
	rm -rf *.o *.txt *.h.gch $(OUT_DIR)*.txt $(DUMPS_DIR)*.txt $(BINS) $(OBJS_DIR)*.o $(LOG_DIR)*.txt $(ASM_DIR)*.s $(BINS_DIR)a.out a.out.dSYM