object = main.o sqlite3.o Sqlite3Forensics.o
out : ${object}
	g++ -o out ${object} -lpthread -ldl
main.o : Sqlite3Forensics.h
	g++ -c main.cpp
sqlite3.o : sqlite3.h
	gcc -c sqlite3.c
Sqlite3Forensics.o : 
	g++ -c Sqlite3Forensics.cpp
.PHONY : clean
clean:
	rm out ${object}
