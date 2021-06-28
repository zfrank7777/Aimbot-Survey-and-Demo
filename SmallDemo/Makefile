CC=g++
CFLAGS=-lpsapi -w

all:
	$(CC) search.cpp $(CFLAGS) -o search.exe
	$(CC) victim.cpp $(CFLAGS) -o victim.exe
	$(CC) victim_close_privilege.cpp $(CFLAGS) -o victim_close_privilege.exe
	$(CC) victim_check_debugger.cpp $(CFLAGS) -o victim_check_debugger.exe
	$(CC) victim_guard.cpp $(CFLAGS) -o victim_guard.exe
