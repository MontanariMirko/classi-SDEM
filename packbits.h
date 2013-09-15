#ifndef PACKBITS_H
#define PACKBITS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

template <typename T>
class packbits{
	vector<T> _buff;

public:
	void codifica(fstream& fin){
		while(true){
			T tmp;
			fin.read((char*)&tmp,1);
			if(fin.eof())
				break;
			_buff.push_back(tmp);
		}

		vector<T>::iterator it(_buff.begin());
		fstream fout("codice.bin", fstream::out | fstream::binary);
		string tmp;
		unsigned char cont,num;
		cont = 0;
		num = 0;
		T temp;
		while(it != _buff.end()){
			temp = *it;
			++cont;
			++it;
			while(it != _buff.end() && temp == *it){
				++cont;
				++it;
				if(it == _buff.end())
					break;
			}
			if(cont == 1){
				tmp += temp;
				++num;
				cont = 0;
			}
			else{
				if(num != 0){
					cout << (int)num << "\t" <<tmp << endl;
					fout.write((char*)&num,1);
					for(int i=0; i<tmp.size(); ++i)
						fout.write((char*)&tmp[i],1);
					num = 0;
					tmp.clear();
				}
				cout << int(256-cont) << "\t" << temp << endl;
				cont = 256 - cont;
				fout << cont;
				fout << temp;
				cont = 0;
			}
		}
		if(num != 0){
			cout << (int)num << "\t" <<tmp << endl;
			fout.write((char*)&num,1);
			for(int i=0; i<tmp.size(); ++i)
						fout.write((char*)&tmp[i],1);
		}
		if(cont != 0){
			cout << int(256-cont) << "\t" << temp << endl;
			cont = 256 - cont;
			fout << cont;
			fout << temp;
		}
	}

	void decodifica(fstream& fin){
		T tmp;
		unsigned char cont;
		fstream fout("deco.bin", fstream::out | fstream::binary);
		while(true){
			fin.read((char*)&cont,1);
			if(fin.eof())
				break;
			if(cont < 128){
				for(int i=0; i<cont; ++i){
					fin.read((char*)&tmp,1);
					cout << tmp;
					fout.write((char*)&tmp,1);
				}
			}
			else{
				fin.read((char*)&tmp,1);
				for(int i=0; i<(256-cont); ++i){
					cout << tmp;
					fout.write((char*)&tmp,1);
				}
			}
		}
		fout.close();
	}

};

#endif //PACKBITS_H