time: parallel sequential
	time ./par.out test.input
	time ./seq.out test.input
	diff seq.txt par.txt

input: geninput
	./gen.out 10000 > test.input

parallel: 
	g++ -o2 parMatrix.cpp -o par.out -pthread -std=c++0x

sequential: 
	g++ -o2 seqMatrix.cpp -o seq.out

geninput: 
	g++ genInput.cpp -o gen.out

clean: 
	rm -f *.out
	rm -f *.txt
	rm -f test.input