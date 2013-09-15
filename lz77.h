#ifndef LZ77_H
#define LZ77_H

#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

struct terna{
	int posizione;
	int lunghezza;
	unsigned char c;

	terna() : posizione(0), lunghezza(0) {}
};

template <typename T>
class lz77{
	vector<T> _diz;
	vector<T> _buff;
	unsigned _dimMaxDiz;

public:
	lz77() : _dimMaxDiz(6) {}

	lz77(unsigned dim) : _dimMaxDiz(dim) {}

	void setFinestra(unsigned dim){
		_dimMaxDiz = dim;
	}

	void setDimDizionario(unsigned dim){
		_diz.resize(dim);
	}

	void caricaDiz(unsigned i){
		_diz.clear();
		vector<T>::iterator it;
		while(i >= 1 && _diz.size() < _dimMaxDiz){
			--i;
			it = _diz.begin();
			_diz.insert(it, _buff[i]);
		}
	}
	void codifica(fstream& f, string nome){
		//leggo il file e creo il buffer
		while(true){
			T tmp;
			f.read((char*)&tmp,1);
			if(f.eof())
				break;
			_buff.push_back(tmp);
		}

		//creo il file codificato
		fstream fout(nome.c_str(), fstream::out | fstream::binary);

		//-------------------CODIFICA LZ77-------------------------------
		unsigned i = 0;
		unsigned i2 = 0;
		while(i < _buff.size()){
			//variabili utilizzate nel calcolo ciclico nel dizionario
			terna var;
			terna tmp;
			//creo il dizionario
			caricaDiz(i);
			//variabili utilizzate come indici nella ricerca ciclica del dizionario
			unsigned k = _diz.size();
			unsigned k2;
			//variabili per il conteggio dei caratteri assoluti trovati nella finestra
			int cont = 0;
			int cont2 = 0;
			//finchè c'è una modifica della variabile della terna del risultato ciclo
			while(k>0){
				bool trovato = false;
				bool indietro = false;
				//il carattere è stato trovato nel dizionario
				if(_buff[i] == _diz[k-1]){
					trovato = true;
					if(i==_buff.size()-1){
						break;
					}
					//aumento lunghezza e posizione della terna risultante
					var.lunghezza++;
					++cont;
					i2 = i;
					k2 = k;
				}
				var.posizione++;
				//controllo che i valori successivi al dizionario siano uguali a quelli del buffer
				while(trovato == true){
					trovato = false;
					if((i2+1) < _buff.size() && (i+var.lunghezza+1)<_buff.size())
						++i2;
					else
						break;
					
					if(k2 >= _diz.size()){
						k2 = k;
						indietro = true;
					}
					else
						++k2;
					if(_buff[i2] == _diz[k2-1]){
						trovato = true;
						var.lunghezza++;
						if(!indietro)
							++cont;
					}
				}
				//se ho trovato una sequenza migliore la sostituisco a quella risultante
				if(cont > cont2){
					tmp = var;
					cont2 = cont;
				}
				--k;
				cont = 0;
				var.lunghezza = 0;
			}

			if(tmp.lunghezza > 0)
				i = i+ tmp.lunghezza + 1;
			else
				++i;

			tmp.c = _buff[i - 1];
			cout << tmp.posizione << "\t" << tmp.lunghezza << "\t" << tmp.c << endl;
			fout.write((const char*)&tmp.posizione,1);
			fout.write((const char*)&tmp.lunghezza,1);
			fout.write((const char*)&tmp.c,1);
		}
		fout.close();
	}

	void decodifica(fstream& f){
		//vettore utilizzato come buffer/finestra scorrevole
		vector<T> vet;
		terna var;
		int cursore = 0;
		while(true){
			//leggo una terna di valori
			f.read((char*)&var.posizione,1);
			f.read((char*)&var.lunghezza,1);
			f.read((char*)&var.c,1);
			//se il file è arrivato alal fine esco dal ciclo while
			if(f.eof())
				break;
			//leggo n volte il buffer con n uguale alla lunghezza della terna
			for(int i=0; i<var.lunghezza; ++i){
				vet.push_back(vet[cursore-var.posizione+i]);
				cout << vet[cursore-var.posizione+i];
			}
			//incremento il cursore della posizione corrente
			cursore += var.lunghezza;
			//inserisco il carattere della terna
			vet.push_back(var.c);
			cout << var.c;
			++cursore;
		}
	}
};

#endif // LZ77_H