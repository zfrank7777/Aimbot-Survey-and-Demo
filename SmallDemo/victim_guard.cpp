#include <cstdio>
#include <cstdlib>
#include <synchapi.h>
#include <windows.h>

#define GUARD_DIFF 9487

class Player {
private:
	int HP, HP_guard;
	int bombs;
public:
	static int cnt;
	
	Player() : HP(9453), bombs(5) {
		HP_guard = HP + GUARD_DIFF;
	}

	void shoot() {
		bombs -= 1;
	}

	void show() {
		if((HP+GUARD_DIFF) != HP_guard){
			MessageBoxA(NULL, "No Cheating!",  "Memory Modification Detected!", MB_OK | MB_SETFOREGROUND);
			exit(1);
		}
		printf("\r                                                                          ");
		printf("\r%d    HP: %d    bombs: %d", cnt, HP, bombs);
		cnt += 1;
	}
};

int Player::cnt = 0;

int main() {
	Player player1;
	while(1){
		player1.show();
		Sleep(1000);
	}
	return 0;
}