#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
using namespace sf;

#define GAME_ROWS_COUNT  8
#define GAME_COLS_COUNT  8

int ts = 57;  // ÿһ����ϷС��������Ĵ�С

bool isMoving = false;
bool isSwap = false;

// ����λ�õĵڼ��ε�������2�ε����Ž�������
int click = 0;

Vector2i pos; //��굥��ʱ��λ��
Vector2i offset(15, 273);

int posX1, posY1; //��һ�ε�����λ��(��¼�к��е���ţ�
int posX2, posY2; //�ڶ��ε�����λ��(��¼�к��е���ţ�

struct Block {
	int x, y; //����ֵ     x ==  col * ts   y == row * ts;
	int row, col;  //�ڼ��У��ڼ���
	int kind; //��ʾ�ڼ���С����
	bool match; //��ʾ�Ƿ����
	int alpha; //͸����

	Block() {
		match = false;
		alpha = 255;
		kind = -1;
	}
} grid[GAME_ROWS_COUNT + 2][GAME_ROWS_COUNT + 2];

void swap(Block p1, Block p2) {
	std::swap(p1.col, p2.col);
	std::swap(p1.row, p2.row);

	grid[p1.row][p1.col] = p1;
	grid[p2.row][p2.col] = p2;
}

void doEvent(RenderWindow *window) {
	Event e;
	while (window->pollEvent(e)) {
		if (e.type == Event::Closed) {
			window->close();
		}

		if (e.type == Event::MouseButtonPressed) {
			if (e.key.code == Mouse::Left) {
				if (!isSwap && !isMoving) click++;
				pos = Mouse::getPosition(*window)- offset;
			}
		}
	}

	if (click == 1) {
		posX1 = pos.x / ts + 1;
		posY1 = pos.y / ts + 1;
	}
	else if (click == 2) {
		posX2 = pos.x / ts + 1;
		posY2 = pos.y / ts + 1;

		// �����ڷ���ͽ���λ��
		if (abs(posX2 - posX1) + abs(posY2 - posY1) == 1) {
			// �������ڵ�����С����
			// �����ֵķ��飬��ô��ʾ��
			swap(grid[posY1][posX1], grid[posY2][posX2]);
			isSwap = 1;
			click = 0;
		}
		else {
			click = 1;
		}
	}
}

void check() {
	for (int i = 1; i <= GAME_ROWS_COUNT; i++) {
		for (int j = 1; j <= GAME_COLS_COUNT; j++) {
			if (grid[i][j].kind == grid[i + 1][j].kind &&
				grid[i][j].kind == grid[i - 1][j].kind) {
				//grid[i - 1][j].match++;
				//grid[i][j].match++;
				//grid[i + 1][j].match++;
				for (int k = -1; k <= 1; k++) grid[i+k][j].match++;
			}

			if (grid[i][j].kind == grid[i][j - 1].kind &&
				grid[i][j].kind == grid[i][j + 1].kind) {
				//grid[i][j - 1].match++;
				//grid[i][j + 1].match++;
				//grid[i][j].match++;
				for (int k = -1; k <= 1; k++) grid[i][j + k].match++;
			}
		}
	}
}

void doMoving() {
	isMoving = false;

	for (int i = 1; i <= GAME_ROWS_COUNT; i++) {
		for (int j = 1; j <= GAME_COLS_COUNT; j++) {
			Block& p = grid[i][j]; // ����p, ����grid[i][j]�ı���
			int dx, dy;

			for (int k = 0; k < 4; k++) {
				dx = p.x - p.col * ts;
				dy = p.y - p.row * ts;

				if (dx) p.x -= dx / abs(dx);
				if (dy) p.y -= dy / abs(dy);
			}

			if (dx || dy) isMoving = true;
		}
	}
}

void xiaochu() {
	for (int i = 1; i <= GAME_ROWS_COUNT; i++) {
		for (int j = 1; j <= GAME_COLS_COUNT; j++) {
			if (grid[i][j].match && grid[i][j].alpha > 10) {
				grid[i][j].alpha -= 10;
				isMoving = true;
			}
		}
	}
}

void huanYuan() {
	if (isSwap && !isMoving) {
		// �����ʱû�в���ƥ��Ч������Ҫ��ԭ
		int score = 0;
		for (int i = 1; i <= GAME_ROWS_COUNT; i++) {
			for (int j = 1; j <= GAME_COLS_COUNT; j++) {
				score += grid[i][j].match;
			}
		}

		if (score == 0) {
			swap(grid[posY1][posX1], grid[posY2][posX2]);
		}

		isSwap = false;
	}
}

void updateGrid() {
	for (int i = GAME_ROWS_COUNT; i > 0; i--) {
		for (int j = 1; j <= GAME_COLS_COUNT; j++) {
			if (grid[i][j].match) {
				for (int k = i - 1; k > 0; k--) {
					if (grid[k][j].match == 0) {
						swap(grid[k][j], grid[i][j]);
						break;
					}
				}

			}
		}
	}

	for (int j = 1; j <= GAME_COLS_COUNT; j++) {
		int n = 0;
		for (int i = GAME_ROWS_COUNT; i > 0; i--) {
			if (grid[i][j].match) {
				grid[i][j].kind = rand() % 7;
				grid[i][j].y = -ts * n;
				n++;
				grid[i][j].match = false;
				grid[i][j].alpha = 255;
			}
		}
	}
}

void drawBlocks(Sprite * sprite, RenderWindow *window) {
	for (int i = 1; i <= GAME_ROWS_COUNT; i++) {
		for (int j = 1; j <= GAME_COLS_COUNT; j++) {
			Block p = grid[i][j];
			sprite->setTextureRect(
				IntRect(p.kind * 52, 0, 52, 52));
			// ����͸����
			sprite->setColor(Color(255, 255, 255, p.alpha));
			sprite->setPosition(p.x, p.y);
			// ��Ϊ����gird�е�Block, ÿ��Block���б꣬�б��Ǵ�1����ģ�
			// �������б���б��������x,y����
			// ���������ƫ�ƣ���Ҫ�ٱ���һЩ��Ҳ�����൱������������������Ͻǵ����ϽǷ���ƫ��һ����λ
			// �����λ�ÿ�����ŵ�0�е�0�У�ʵ�ʲ����Ƶ�0�е�0�У�
			sprite->move(offset.x-ts, offset.y-ts);  // to do
			window->draw(*sprite);
		}
	}
}

void initGrid() {
	for (int i = 1; i <= GAME_ROWS_COUNT; i++) {
		for (int j = 1; j <= GAME_COLS_COUNT; j++) {
			grid[i][j].kind = rand() % 3; 
			grid[i][j].col = j;
			grid[i][j].row = i;
			grid[i][j].x = j * ts;
			grid[i][j].y = i * ts;
		}
	}
}

int main(void) {
	srand(time(0));

	RenderWindow window(VideoMode(485, 917), "Rock-xiaoxiaole");
	// ����ˢ�µ����֡��
	window.setFramerateLimit(60);

	Texture t1, t2;
	t1.loadFromFile("images/bg2.png");
	if (! t2.loadFromFile("images/t4.png")) {
		return -1;
	}
	Sprite spriteBg(t1);
	Sprite spriteBlock(t2);

	initGrid();

	while (window.isOpen()) {
		// �����û��ĵ���¼�
		doEvent(&window);

		// ���ƥ�����
		check();

		// �ƶ�����
		doMoving();

		// ����
		if (!isMoving) {
			xiaochu();
		}

		// ��ԭ����
		huanYuan();

		if (!isMoving) {
			updateGrid();
		}

		// ��Ⱦ��Ϸ����
		window.draw(spriteBg);
		// ��Ⱦ���е�С����
		drawBlocks(&spriteBlock, &window);
		// ��ʾ
		window.display();
	}

	return 0;
}