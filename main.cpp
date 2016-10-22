#include "my.h"

#define DEBUGG
#ifdef DEBUGG

using std::cin;
using std::cout;
using std::endl;

#endif

class text{

	public:
		text(): mode(EDIT_MODE) {
			a.setMode(mode);
		}

		void run() {
			bc.init("test.txt");
			a.init(bc);

			for (;;) { // have opened a file
				if (KEY_DOWN(VK_ESCAPE)) {
					a.setMode(mode = EDIT_MODE);
				}
				int ch = getch();

				if (mode == EDIT_MODE) {
					
					switch (ch) {
						case 'h': a.moveLeftCur(); break;
						case 'j': a.moveDownCur(); break;
						case 'k': a.moveUpCur(); break;
						case 'l': a.moveRightCur(); break;
						case 224:
							switch(getch()) {
								case 72: a.moveUpCur(); break;
								case 75: a.moveLeftCur(); break;
								case 77: a.moveRightCur(); break;
								case 80: a.moveDownCur(); break;
								default: break;
							}
							break;
						case 'i':							 
							a.setMode(mode = WRITE_MODE);
							break;
						case ':': /* important! */ 
							a.setMode(mode = CON_MODE);
							break;
						default: break;
					}
				} else 
				if (mode == WRITE_MODE) {
					switch (ch) {
						case 224: {
							char tmpch = getch();
							switch (tmpch) {
								case 72: a.moveUpCur(); break;
								case 75: a.moveLeftCur(); break;
								case 77: a.moveRightCur(); break;
								case 80: a.moveDownCur(); break;
						
								case CHAR_DELETE: a.delChar(); break;

								default: 
									 break;
							}
						}
							break;
						case CHAR_BACKSPACE:
							a.delPreChar();
							break;	

						default: 
							if (ch >= 32 && ch < 128) { // is char
								a.addChar(ch);
								a.moveRightCur();
							}
							break;
					}
				}
				if (mode == CON_MODE) {
					a.saveCur();

					a.setAndShowChar(':', 0, MAXN); 

					string cmd = "";
					for (; (ch = getch()) != '\n' && ch != '\r';) {
						if (ch == CHAR_BACKSPACE) {
							a.setAndShowChar(' ', cmd.size(), MAXN);
							cmd.pop_back();

							continue;
						}

						if (KEY_DOWN(VK_ESCAPE)) {
							cmd = "";
							break;
						}
						cmd += ch; a.setAndShowChar(ch, cmd.size(), MAXN);
					}

					////////////////// decode the cmd
					if (cmd == "q") {
						a.clear();
						puts("The programe exited");
						return ;
					}
					if (cmd == "w") {
						std::ofstream os;
						os.open("test3.txt");
						os << bc.getAllStr() << std::flush;
						os.close();
					}

					//////////////////// decode end
					a.delAline(MAXN);

					a.restoreCur();
					mode = EDIT_MODE;
				} 
			}
		}

	private:
		animated a;
		blockChain bc;
		int mode;

#undef WRITE_MODE
#undef EDIT_MODE
#undef CON_MODE
};

using std::cout;
using std::endl;
using std::cin;

int main() {
	text t;
	t.run();

	return 0;
}
