#include <fstream>
using namespace std;

class bitwriter{
	unsigned char _val;
	fstream& _fout;
	unsigned _cont;

public:
	// costruttore
	bitwriter(fstream& f) : _cont(0), _fout(f) {}

	// distruttore
	~bitwriter(){
		while(_cont < 8){
			_val <<= 1;
			_cont++;
		}
		_fout.write((char *)&_val, 1);
	}

	// ritorna il numero massimo di bit utilizzati per la rappresentazione dei valori
	unsigned numBit(const vector<unsigned>& vett){
		unsigned max = *max_element(vett.begin(), vett.end());
		int i = 0;
		while(max > 1){
			max = max / 2;
			++i;
		}
		++i;
		return i;
	}

	// converte e scrive il numero covnertito
	void bwrite(unsigned elem, unsigned n){
		while (n > 0)
		{
			_val = _val << 1;
			_val += ((elem>>(n-1)) & 1);
			++_cont;
			--n;
			if(_cont == 8){
				_fout.write((char *)&_val, 1); 
				_cont = 0;
			}
		}
	}
};

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