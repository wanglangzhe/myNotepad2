
#include "dataStruct.cpp"

#ifndef OIJEFLDOIJWERLKJ
#define OIJEFLDOIJWERLKJ

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define KEY_UP(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000)?0:1)  

#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <conio.h>

class animated{
#define MAXN 30
#define MAXM 100

	public:
		animated(): curX(0), curY(0) {
			system("mode con cols=100 lines=32");
		}
		void clear() const {
			system("cls");
		}

		void saveScreen() {
			for (int i = 0; i <= MAXN; ++i) {
				for (int j = 0; j <= MAXM; ++j) buffer[i][j] = screen[i][j];
			}
		}
		void loadToScreen(int &pt) {
			for (int i = 0; i < MAXN && pt < bc->size(); ++i) {
				for (int j = 0; j < MAXM && pt < bc->size(); ++j) {
					label[i][j] = pt;
					screen[i][j] = bc->getChar(pt++);

					if (screen[i][j] == '\n') 
						for (++j; j < MAXM; ++j) screen[i][j] = 0;
				}
			}
		}
		void init(blockChain &t) {
			bc = &t;
			line = pt = 0;
			memset(screen, ' ', sizeof(screen));
			memset(label, -1, sizeof(label));
		
			loadToScreen(pt);

			showScreen();
		}
		// about change
		void delPreChar() {
			--curX;
			if (curX < 0) { --curY; curX = MAXM - 1; }
			if (curY < 0) { rollOneLineUp(); curY = 0; }
			gotoxy();
			
			delChar();
		}
		void delChar() {
			saveScreen();

			bc->delChar(label[curY][curX]);
			pt = label[0][0];
			if (pt < 0) pt = 0;
			loadToScreen(pt);

			showScreen();
		}
		void addChar(char ch) { // in cur!
			saveScreen();	
			
			bc->addChar(label[curY][curX], ch);
			
			pt = label[0][0];
			if (pt < 0) pt = 0;
			loadToScreen(pt);

			showScreen();
		}

		void rollOneLineDown() {
			saveScreen();
			for (int i = 0; i + 1 < MAXN; ++i) {
				for (int j = 0; j < MAXM; ++j) {
					screen[i][j] = screen[i+1][j];
					label[i][j] = label[i+1][j];
				}
			}
			memset(screen[MAXN-1], 0, sizeof(screen[MAXN-1]));
			memset(label[MAXN-1], -1, sizeof(label[MAXN-1]));
			for (int j = 0, i = MAXN - 1; j < MAXM && pt < bc->size(); ++j) {
				label[i][j] = pt;
				screen[i][j] = bc->getChar(pt++);
				if (screen[i][j] == '\n') break;
			}
			showScreen();
		}

		void rollOneLineUp() {
			string tmp = bc->getPreEnterStr(label[0][0] - 1, MAXM);


			if (tmp.size() == 0) return ;

			saveScreen();
			for (int i = MAXN - 1; i; --i) {
				for (int j = 0; j < MAXM; ++j) {
					screen[i][j] = screen[i-1][j];
					label[i][j] = label[i-1][j];
					if (i == MAXN - 1) {
						if (label[i][j] != -1) pt = label[i][j] + 1;
					}
				}
			}

			memset(screen[0], 0, sizeof(screen[0]));
			memset(label[0], -1, sizeof(label[0]));


			for (int i = ((int)tmp.size()) - 1, j = label[1][0] - 1; i >= 0; --i, --j) {
				screen[0][i] = tmp[i]; label[0][i] = j;
			}

			showScreen();
		}

		void delAline(int l) {
			saveScreen();

			for (int i = l, j = 0; j < MAXM; ++j) screen[i][j] = 0;

			showScreen();
		}
		// about show /////////////////////////////////////////////////////
		void showScreen(int type = 0) {
			//cout << 
			hideCur();
			for (int i = 0; i <= MAXN; ++i) {
				for (int j = 0; j < MAXM; ++j) {
					if (screen[i][j] == buffer[i][j]) continue;
					showChar(screen[i][j] < 33 ? ' ': screen[i][j], j, i);
				}
			}
			showCur();
			gotoxy();
		}
	
		void showChar(char ch, int x, int y) const {
			gotoxy(x, y);
			putchar(ch);
		}
		
		void showChar(char ch = ' ') const {
			gotoxy(curX, curY);
			putchar(ch);
		}

		void setAndShowChar(char ch, int x, int y) {
			screen[y][x] = ch;
			setCur(x, y);
			gotoxy();
			putchar(ch);
		}
		// about show end /////////////////////////////////////////////////////

		//about cur /////////////////////////////////////////////////////////////
		void setCur(int x, int y) {
			curX = x; curY = y;
		}

		void moveLeftCur(int lim = 0) {
			curX = std::max(curX - 1, lim);
			gotoxy();
		}
		void moveRightCur(int lim = MAXM - 1) {
			if (curX != lim && screen[curY][curX+1] == 0) return ;
			if (curX == lim) {
				curX = 0;
				gotoxy();
				moveDownCur();
				return ;
			}
			curX = std::min(curX + 1, lim);
			gotoxy();
		}
		void moveUpCur(int lim = 0) {
			if (curY == lim) rollOneLineUp();
			curY = std::max(curY - 1, lim);
			gotoxy();
		}
		void moveDownCur(int lim = MAXN - 1) {
			if (curY == lim) rollOneLineDown();
			curY = std::min(curY + 1, lim);
			gotoxy();
		}	
		void saveCur() {
			saveCurX = curX;
			saveCurY = curY;
		}
		void restoreCur() {
			curX = saveCurX;
			curY = saveCurY;
			gotoxy();
		}
		void setMode(int type) {
			switch (type) {
				case EDIT_MODE: 
					setCurHeight(100);
					break;
				case WRITE_MODE:
					setCurHeight(1);
					break;
				case CON_MODE:
					setCurHeight(1);
					break;
				default:
					break;
			}
		}
		// about cur end /////////////////////////////////////////////////////////
	private:
		char key;
		int curX, curY, saveCurX, saveCurY;
		int line, pt;
		char screen[MAXN+10][MAXM+10], buffer[MAXN+10][MAXM+10];
		int label[MAXN+10][MAXM+10];
		blockChain *bc;

		void gotoxy(int x, int y) const {                //c++版gotoxy函数    移动光标至x， y，横x竖y
			COORD c;
			c.X=x;c.Y=y;
			SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE),c);
		}
		void gotoxy() {
			COORD c;
			while (curX && screen[curY][curX] == 0) --curX;
			c.X = curX; c.Y = curY;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);

		}
		void hideCur() const {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(hOut,&cci);
			cci.bVisible=false;
			SetConsoleCursorInfo(hOut,&cci);	
		}
		void showCur() const {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(hOut,&cci);
			cci.bVisible=true;
			SetConsoleCursorInfo(hOut,&cci);	
		}
		void setCurHeight(const int h) const {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(hOut,&cci);
			cci.dwSize = h;
			SetConsoleCursorInfo(hOut,&cci);	
		}
		//#undef MAXN
		//#undef MAXM
};



#endif


#ifdef DEBUG

using std::cout;
using std::endl;

int main() {
	animated a;
	a.clear();

	while (1) {
		for (int i = 0; i < 5 ; ++i)
			for (int j = 0; j < 5 ; ++j)
				a.showChar('a', i, j);	
		Sleep(100);
		for (int i = 0; i < 5 ; ++i)
			for (int j = 0; j < 5 ; ++j)
				a.showChar('b', i, j);
		Sleep(100);
	}
	while (1);

	return 0;
}

#endif
