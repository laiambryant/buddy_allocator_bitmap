CCOPTS=-Wall -g -std=gnu99 
VGOPTS=--leak-check=full --show-leak-kinds=all  --track-origins=yes  --verbose 
LIBS=
CC=gcc
CC_GPROF = -pg
CC_DUMP= -c

OUT_DIR=OUT/
DUMPS_DIR=OUT/Dumps

buddy:
	$(CC) $(CCOPTS) -o Bud_test Main.c Bitmap.* Buddy.* 
	./Bud_test

buddy.o: 
	$(CC) $(CCOPTS) $(CC_DUMP) Main.c Bitmap.* Buddy.* 

buddyVG:
	$(CC) $(CCOPTS) -o Bud_test Main.c Bitmap.* Buddy.* 
	valgrind $(VGOPTS)--log-file=bm-valgrind-out.txt \./Bud_test
	mv bm-valgrind-out.txt $(OUT_DIR)
	rm -rf gmon.out Bud_test

buddyGProf:
	$(CC) $(CCOPTS) $(CC_GPROF) -o Bud_test Main.c Bitmap.* Buddy.* 
	./Bud_test
	gprof Bud_test gmon.out > Bud_TestGprof.txt
	mv Bud_TestGprof.txt $(OUT_DIR)
	rm -rf gmon.out Bud_test

buddyDump: buddy.o
	objdump -d Bitmap.o > Bitmap_dump.txt Buddy.o > Buddy_dump.txt Main.o > Main_dump.txt
	mv *.txt $(DUMPS_DIR)

clean:
	rm -rf *.o Bud_test *.h.gch


