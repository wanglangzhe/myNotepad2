#ifndef QAZWSXEDCRFV
#define QAZWSXEDCRFV

#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
#include <queue>
#include <fstream>

#define PB push_back

using std::vector;
using std::string;
using std::queue;

namespace wmz{

	class node{
		public:
			//build need fix !
			node() {
				data.clear();
			}
			node(int l, int r): lLink(l), rLink(r) {
				data.clear();
			}
			node(const char *ch, int l, int r): lLink(l), rLink(r) {
				data = string(ch);
			}
			node(const string &str, int l, int r): lLink(l), rLink(r) {
				data = str;
			}
			//add
			void addChar(int t = 0, char ch = ' ') {
				data.insert(t, 1, ch);
			}
			void addString(int t = 0, const char *a = "") {
				data.insert(t, a);
			}
			void addString(int t = 0, const string &a = string("")) {
				data.insert(t, a);
			}
			void setLlink(int t) { lLink = t; }
			void setRlink(int t) { rLink = t; }

			// get
			string getStr() const {
				return data;
			}
			string getStr(int l, int r, int type = 0) const {
				if (type == 0) return data.substr(l, r - l + 1);
				if (type == 1) return data.substr(l, r);
			}
			char getChar(int t) const {
				return data[t];
			}
			int getLlink() const { return lLink; }
			int getRlink() const { return rLink; }
			int size() const { return data.size(); }

			// del
			void delStr(int st, int num, int type = 0) {
				if (type == 0) data.erase(st, num - st + 1);
				if (type == 1) data.erase(st, num);	
			}
		private:
			string data;
			int lLink, rLink;
	};
};


class blockChain{

#define BC blockChain
#define HEAD 0
#define TAIL 1
#define INSERT 0
#define FIND 1
//#define INITLEN 1024
#define INITLEN 8

	public:
		BC(): totLen(0), blockSize(INITLEN), blockNum(0) {
			data.clear();
			while (pool.size()) pool.pop();
			data.PB(wmz::node(HEAD, TAIL));
			data.PB(wmz::node(HEAD, TAIL));
		}
		
		void init(string file) {

			std::ifstream is; is.open(file.c_str());
			
			char ch;
			for (int i = 0; is.get(ch) ; ++i) {
				addChar(i, ch);
			}

			is.close();
		}

		void outToFile(string file) {
			std::ofstream os;
			os.open(file.c_str());

			os << this->getAllStr() << std::flush;

			os.close();
		}

		string getAllStr() const {
			string res = "";
			for (int i = data[HEAD].getRlink(); i != TAIL; i = data[i].getRlink()) res += data[i].getStr();
			return res;
		}

		void addChar(int t = 0, char ch = ' ') {
			// fix input data
				if (t < 0) t = 0;
				if (t > totLen) t = totLen;
			// fix end
			int p, savet = t;
			p = getBlock(t); 	

			if (p == TAIL) { //	往末尾增加字符
				int nt = newBlock(string("") + ch, data[TAIL].getLlink(), TAIL);
				data[data[TAIL].getLlink()].setRlink(nt);
				data[TAIL].setLlink(nt);
			} else { //往中间某个块增加字符
				data[p].addChar(t, ch);

				//维护单个块大小
				fixBlock(p);

				//维护全局块大小
				if (blockNum > blockSize * 4) {
					blockSize *= 2;
				} else 
				if (blockSize > 1000 && blockSize > blockNum * 4) {
					blockSize /= 2;
				}

			}
			++totLen;
		}

		char getChar(int t) const {
			int p = getBlock(t, 1);
			return data[p].getChar(t);
		}
		
		string getPreEnterStr(int t, int lim) { //从起始点开始，找到下一个回车，并把期间的字符串（包含起始点）返回（选取最后lim个字符）
			//这是一个偷懒版本，复杂度O(n)，对每个链节打标记能优化为O(sqrt(n))
			string res = "";
			int p = getBlock(t, FIND);
			if (p < 0) return "";

			res = data[p].getChar(t) + res;

			for (; p != HEAD && res.size() < lim; ) {
				--t; if (t < 0) {
					p = data[p].getLlink(); 
					if (p != HEAD) {
						t = data[p].size() - 1;
					}
				}
				if (p != HEAD) {
					if (data[p].getChar(t) == '\n' || data[p].getChar(t) == '\r') break;
					res = data[p].getChar(t) + res;
				}
			}

			return res.substr(res.size() / lim * lim, res.size() % lim);
		}

		void delChar(int t = 0) {
			// fix input data
			if (t < 0) t = 0;
			if (t >= totLen) t = totLen - 1;
			// fix end		
			int p = getBlock(t, FIND);
			data[p].delStr(t, t);
			if (data[p].size() == 0) delBlock(p);
			--totLen;
		}

		int size() const { return totLen; }

	private:
		vector <wmz::node >	data;
		queue <int > pool;
		int totLen, blockSize, blockNum;
		//fix function
		int getBlock(int &t, int type = 0) const { //type 0 用于插入，因为是找到一个开区间，type 1 用于查询，因为查询是闭区间上的
			//fix input
			if (t < 0) return -1;
			//fix end
			int p = data[HEAD].getRlink();
			if (type == 0) {
				while (t > data[p].size()) {
					t -= data[p].size();
					p = data[p].getRlink();
				}
			} else 
			if (type == 1) {
				while (t >= data[p].size()) {
					t -= data[p].size();
					p = data[p].getRlink();
				}
			}
			return p;
		}
		int newBlock(const string &str = "", int l = 0, int r = 0) {
			++blockNum;

			if (pool.size()) {
				int res = pool.front(); pool.pop();
				return res;
			}
			int res = data.size();
			data.PB(wmz::node(str, l, r));

			return res;
		}
		bool delBlock(int t = -1) {
			--blockNum;
			//fix data
			if (t == HEAD || t == TAIL) {
				puts("error in delBlock! The head or tail can't be deleted.");
				return false;
			} else if (t >= data.size()) {
				puts("error in delBlock! The t not in range");
				return false;
			}
			//fix end

			pool.push(t);
			data[data[t].getLlink()].setRlink(data[t].getRlink());
			data[data[t].getRlink()].setLlink(data[t].getLlink());

			return true;
		}
		bool fixBlock(int p) {
			if (data[p].size() > blockSize) {
				int lim = data[p].size() / 2, sz = data[p].size(), nt;

				nt = newBlock(data[p].getStr(lim, sz - 1), p, data[p].getRlink());

				data[p].delStr(lim, sz - 1);
				data[data[p].getRlink()].setLlink(nt);
				data[p].setRlink(nt);
			}
		}

#undef BC
#undef HEAD
#undef TAIL
#undef INITLEN
#undef FIND
#undef INSERT

};

#undef PB

#endif 


#ifdef DEBUG

using std::cout;
using std::endl;

int main() {
	blockChain a;
	for (int i = 0; i < 10; ++i) a.addChar(0, 'a');
	for (int i = 0; i < 50; i += 2) a.addChar(i, '@');
	
	cout << a.size() << endl << a.getAllStr() << endl;

	int l = a.size();
	for (int i = 0; i < l; ++i) {
		a.delChar(0);
		cout << a.getAllStr() <<  endl;
	}

	cout << a.size() << ' ' << a.getAllStr() << endl;

	for (int i = 0; i < a.size(); ++i) a.delChar(0);

	
	cout << a.size() << ' ' << a.getAllStr() << endl;
}

#endif
