#ifndef LZW_H
#define LZW_H

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "bit.h"
using namespace std;

template <typename T>
class lzw{
	vector<T> _buff;
	map<int,string> _diz;

	void caricaMappa(){
		_diz.clear();
		for(unsigned i=0; i<256; ++i){
			string s;
			s += i;
			_diz.insert(pair<int,string>(i,s));
		}
	}
public:
	lzw(){
		caricaMappa();
	}

	void codifica(fstream& fin, unsigned& nbit){
		//leggo il file da codificare
		while(true){
			T tmp;
			fin.read((char*)&tmp,1);
			if(fin.eof())
				break;
			_buff.push_back(tmp);
		}
		unsigned i = 1;
		string corrente;
		//leggo il primo valore del buffer
		corrente += _buff[0];
		bool trovato = false;
		//creo il vettore di output
		vector<unsigned> dati;
		while(i < _buff.size()){
			//aggiungo un carattere alla stringa da ricercare
			corrente += _buff[i];
			trovato = false;
			//ricerco la stringa nel dizionario
			for(unsigned k=0; k<_diz.size(); ++k){
				if(_diz[k] == corrente){
					trovato = true;
					break;
				}
			}
			//se la stringa è stata trovata aggiungerò un ulteriore carattere
			if(trovato){
				++i;
			}
			else{
				//altrimenti inserisco la stringa nel dizionario
				_diz.insert(pair<int,string>(_diz.size(),corrente));
				corrente = corrente.substr(0,corrente.length()-1);
				unsigned k;
				for(k=0; k<_diz.size(); ++k){
					if(_diz[k] == corrente){
						break;
					}
				}
				//salvo in output la stringa meno l'ultimo carattere
				cout << k << "\t" << _diz.size()-1 << "=" <<_diz[_diz.size()-1] << endl;
				dati.push_back(k);
				//la stringa da ricercare diventerà l'ultimo carattere della stringa precedente
				corrente = _buff[i];
				++i;
			}
		}
		fstream fout("codice.bin", fstream::out | fstream::binary);
		bitwriter bw(fout);
		nbit = bw.numBit(dati);
		vector<unsigned>::iterator it(dati.begin());
		while(it != dati.end()){
			bw.bwrite(*it, nbit);
			++it;
		}
		bw.~bitwriter();
		fout.close();
	}

	void decodifica(fstream& fin, unsigned& nbit){
		caricaMappa();
		bitreader br(fin);
		vector<unsigned> vet;
		while(true){
			if(fin.eof())
				break;
			unsigned n;
			br.bread(n, nbit);
			vet.push_back(n);
		}

		vector<unsigned>::iterator it(vet.begin());
		cout << _diz[*it];
		unsigned tmp = *it;
		++it;
		while(it != vet.end()){
			string s;
			s += _diz[tmp];
			s += _diz[*it].substr(0,1);
			_diz.insert(pair<int,string>(_diz.size(),s));
			cout << _diz[*it];
			tmp = *it;
			++it;
		}
	}
};

#endif //LZW_H