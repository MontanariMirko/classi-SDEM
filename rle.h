#ifndef RLE_H
#define RLE_H

#include <fstream>
using namespace std;

template <typename T>
class rle{
	fstream& _file;

public:
	rle(fstream& file) : _file(file) {}

	void codifica(string nome){
		fstream fout(nome.c_str(), fstream::out | fstream::binary);
		T c;
		int cont = 1;
		T c1;
		if(_file.good())
			_file.read((char*)&c,1);
		while(!_file.eof()){
			_file.read((char*)&c1,1);
			if(_file.eof())
				break;
			while(c == c1){
				++cont;
				_file.read((char*)&c1,1);
				if(_file.eof())
				break;
			}
			fout.write((const char*)&c,1);
			fout.write((const char*)&cont,1);
			c = c1;
			cont = 1;
		}
		fout.write((const char*)&c,1);
		fout.write((const char*)&cont,1);
		fout.close();
	}

	void decodifica(string nome){
		fstream fout(nome.c_str(), fstream::out | fstream::binary);
		char c;
		char cont;
		while(true){
			_file.read((char*)&c,1);
			if(_file.eof())
				break;
			_file.read((char*)&cont,1);
			if(_file.eof())
				break;
			for(int i=0; i<(int)cont; ++i)
				fout.write((char*)&c,1);
		}
		fout.close();
	}
};

#endif //RLE_H