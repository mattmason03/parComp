time: parallel sequential
	time ./seq.out graphinput.bin 100 > seq.txt
	time ./par.out graphinput.bin 100 > par.txt
	diff seq.txt par.txt

parallel: 
	g++ -o2 bfs_par.cpp -o par.out -pthread -std=c++0x

sequential: 
	gcc -o2 bfs_seq.c -o seq.out

clean: 
	rm -f *.out
	rm -f *.txt