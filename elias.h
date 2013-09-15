#include <fstream>
using namespace std;

class bitreader{
	unsigned char _val;
	fstream& _fin;
	unsigned _nbit;

	void carica(){
		_fin.read((char *)&_val, 1);
		_nbit = 8;
	}

public:
	bitreader(fstream& f) : _fin(f), _nbit(0) {}

	void bread(unsigned& num){
		if(_nbit == 0){
			carica();
		}
		_nbit--;
		num = (_val >> _nbit) & 1;
	}

	void bread(unsigned& num, unsigned n){
		num = 0;
		while(n > 0){
			unsigned a;
			bread(a);
			num |= a << --n;		
		}
	}
};

class elias{
	unsigned char _val;
	unsigned _cont;
	fstream& _file;

public:
	elias(fstream& f) : _file(f), _cont(0) {}

	~elias(){
		while(_cont != 8){
			_val <<= 1;
			++_cont;
		}
		_file.write((char *)&_val, 1);
	}

	unsigned numbit(unsigned elem){
		int i = 0;
		while(elem > 0){
			elem = elem /2;
			++i;
		}
		return i;
	}

	void write(unsigned elem){
		unsigned nbit = numbit(elem);
		
		if(nbit != 0){
			for(int i=0; i<nbit-1; ++i){
				_val <<= 1;
				++_cont;
				if(_cont == 8){
					_file.write((char *)&_val, 1);
					_cont = 0;
				}
			}

			while (nbit > 0){
				_val = _val << 1;
				_val += ((elem>>(nbit-1)) & 1);
				++_cont;
				--nbit;
				if(_cont == 8){
					_file.write((char *)&_val, 1); 
					_cont = 0;
				}
			}
		}
	}

	void decodifica(vector<unsigned>& vet){
		unsigned n = 0;
		unsigned nbit = 1;
		bitreader br(_file);
		while(_file.good()){
			while(n == 0){
				br.bread(n, 1);
				if(n == 0)
					++nbit;
			}
			--nbit;
			n = 1;
			n <<= nbit;
			unsigned n2;
			br.bread(n2,nbit);
			n +=n2;
			vet.push_back(n);
			n = 0;
			nbit = 1;
		}
	}
};