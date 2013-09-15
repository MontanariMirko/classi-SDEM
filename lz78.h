#ifndef LZ78_H
#define LZ78_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

template <typename T>
class lz78{
	map<int,string> _diz;
	vector<T> _buff;

public:
	void codifica(fstream& file){
		T tmp;
		//carico il contenuto del file nel vettore _buff
		while(true){
			file.read((char*)&tmp,1);
			if(file.eof())
				break;
			_buff.push_back(tmp);
		}
		//creo il file contenente il vettore codificato
		fstream fout("codice.bin", fstream::out | fstream::binary);
		//creo l'iteratore per scprrere il vettore e lo posiziono all'inizio di esso
		vector<T>::iterator it;
		it = _buff.begin();
		//scorro il vettore fino alla fine
		while(it != _buff.end()){
			bool trovato = false;
			string ric;
			unsigned index;
			//concateno il valore corrente alla stringa da ricercare
			ric += *it;
			map<int,string>::iterator mit(_diz.begin());
			while(mit != _diz.end()){
				//se trovo la stringa memorizzo il valore dell'indice della mappa
				//concateno il carattere successico alla stringa di ricerca
				//e riparto con la ricerca dall'inizio della mappa
				if((*mit).second == ric){
					trovato = true;
					index = (*mit).first;
					++it;
					ric += *it;
					mit = _diz.begin();
				}
				else{
					++mit;
				}
			}
			//se ho trovato la stringa nella mappa
			if(trovato){
				_diz.insert(pair<int,string>(_diz.size()+1,ric));
				cout << "(" << index << ", " << *it << ")" << endl;
				fout.write((const char*)&index,1);
				fout.write((const char*)&(*it),1);
			}
			else{
				_diz.insert(pair<int,string>(_diz.size()+1,ric));
				cout << "(0, " << *it << ")" << endl;
				unsigned n = 0;
				fout.write((const char*)&n,1);
				fout.write((const char*)&(*it),1);
			}
			++it;
		}
		fout.close();
	}

	void decodifica(fstream& fin){
		//cancello il contenuto del dizionario
		_diz.clear();
		unsigned index = 0;
		char c;
		//leggo tutto il file
		while(true){
			fin.read((char*)&index,1);
			fin.read((char*)&c,1);
			if(fin.eof())
				break;
			//se l'indice è uguale a zero inserisco nel dizionario il carattere letto
			if(index == 0){
				cout << c;
				string tmp;
				tmp += c;
				_diz.insert(pair<int,string>(_diz.size()+1,tmp));
			}
			//atrimenti leggo la stringa contenuta nella mappa all'indirizzo letto
			//ci concateno il carattere letto e lo inserisco in un nuovo campo della mappa
			else{
				string tmp;
				tmp += _diz[index];
				tmp += c;
				_diz.insert(pair<int,string>(_diz.size()+1,tmp));
				cout << tmp;
			}

		}
	}
};

#endif //LZ78_h