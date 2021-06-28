#include <cstdio>
#include <synchapi.h>

class Player {
private:
	int HP;
	int bombs;
public:
	static int cnt;
	
	Player() : HP(9453), bombs(5) {}

	void shoot() {
		bombs -= 1;
	}

	void show() {
		printf("\r                                                                           ");
		printf("\r%d    HP: %d    bombs: %d    HP address: 0x%012llx", cnt, HP, bombs, &HP);
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