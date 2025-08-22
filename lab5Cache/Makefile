LIBS = -lpthread
CC = gcc -Iinclude -I./src
CFLAGS = -O3  -funroll-loops -Wall 

.PHONY: default all clean

default: testMul benchmarkMul benchmarkBlock
all: default


MATRIX_OBJ = matrixMul.o matrix.o
TEST_OBJ = testMul.o
BENCHMARK_BLOCK = benchmarkBlock.o
BENCHMARK_MUL = benchmarkMul.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

testMul: $(MATRIX_OBJ) $(TEST_OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

benchmarkMul: $(MATRIX_OBJ) $(BENCHMARK_MUL)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

benchmarkBlock: $(MATRIX_OBJ) $(BENCHMARK_BLOCK)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)



clean:
	-rm -f *.o
	-rm -f benchmarkMul testMul benchmarkBlock