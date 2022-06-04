#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
using namespace sf;

#define GAME_ROWS_COUNT  8
#define GAME_COLS_COUNT  8

int ts = 57;  // 每一个游戏小方块区域的大小

bool isMoving = false;
bool isSwap = false;

// 相邻位置的第几次单击，第2次单击才交换方块
int click = 0;

Vector2i pos; //鼠标单击时的位置
Vector2i offset(15, 273);

int posX1, posY1; //第一次单击的位置(记录行和列的序号）
int posX2, posY2; //第二次单击的位置(记录行和列的序号）

struct Block {
	int x, y; //坐标值     x ==  col * ts   y == row * ts;
	int row, col;  //第几行，第几列
	int kind; //表示第几种小方块
	bool match; //表示是否成三
	int alpha; //透明度

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

		// 是相邻方块就交换位置
		if (abs(posX2 - posX1) + abs(posY2 - posY1) == 1) {
			// 交换相邻的两个小方块
			// 消消乐的方块，怎么表示？
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
			Block& p = grid[i][j]; // 引用p, 就是grid[i][j]的别名
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
		// 如果此时没有产生匹配效果，就要还原
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
			// 设置透明度
			sprite->setColor(Color(255, 255, 255, p.alpha));
			sprite->setPosition(p.x, p.y);
			// 因为数组gird中的Block, 每个Block的行标，列标是从1计算的，
			// 并根据行标和列表来计算的x,y坐标
			// 所以坐标的偏移，需要少便宜一些，也就是相当于在正方形区域的左上角的左上角方向偏移一个单位
			// 在这个位置开发存放第0行第0列（实际不绘制第0行第0列）
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
	// 设置刷新的最大帧率
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
		// 处理用户的点击事件
		doEvent(&window);

		// 检查匹配情况
		check();

		// 移动处理
		doMoving();

		// 消除
		if (!isMoving) {
			xiaochu();
		}

		// 还原处理
		huanYuan();

		if (!isMoving) {
			updateGrid();
		}

		// 渲染游戏画面
		window.draw(spriteBg);
		// 渲染所有的小方块
		drawBlocks(&spriteBlock, &window);
		// 显示
		window.display();
	}

	return 0;
}