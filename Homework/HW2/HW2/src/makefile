HW2:HW2.o FM_struc_copy.o
	g++ -std=c++11 -O3 -o ../bin/hw2 HW2.o FM_struc_copy.o

FM_struc_copy.o:FM_struc_copy.cpp FM_struc_copy.h
	g++ -std=c++11 -O3 -c FM_struc_copy.cpp

HW2.o:HW2.cpp
	g++ -std=c++11 -O3 -c HW2.cpp

clean:
	rm -f ../bin/hw2
	rm -f HW2.o
	rm -f FM_struc_copy.o
