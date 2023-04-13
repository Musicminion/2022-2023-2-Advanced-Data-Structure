cc = g++
prog = sparsematrix
objects = main.o SparseMatrix.o

$(prog): $(objects)
	$(cc) -o $(prog) $(objects)

main.o: SparseMatrix.h
SparseMatrix.o: SparseMatrix.h

grade:
	./$(prog)
clean:
	rm -f *.o $(prog) output/*.yourans

.PHONY: grade clean