#include <list>
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
using namespace std;

struct dati{
	unsigned val;
	unsigned codice;
	unsigned lunghezza;
	double probab;
	dati *sx;
	dati *dx;
};

bool ordina(dati *a, dati *b){
		return a->probab > b->probab;
	}

class huffman{
	list<dati*> _lista;
	map<int,dati>& _mappa;
	int i;

public:
	huffman(map<int,dati>& mappa) : _mappa(mappa) {}

	void calcolaHuffman(const vector<unsigned>& val){
		//calcolo il valore massimo
		int uMax = *max_element(val.begin(), val.end());
		//calcolo le frequenze dei valori
		vector<unsigned> freq(uMax+1);
		for(int i=0; i<uMax+1; ++i){
			freq[i] = count(val.begin(), val.end(), i);
		}
		//calcolo le probabilità dei valori
		vector<double> prob(uMax+1);
		transform(freq.begin(), freq.end(), prob.begin(), bind2nd(divides<double>(), val.size()));
		//richiamo il metodo creaAlbero che andrà a creare la struttura
		creaAlbero(prob);
		creaMappa();
	}

	void creaAlbero(const vector<double>& prob){
		//creazione della lista
		for(unsigned i=0; i<prob.size(); ++i){
			if(prob[i] > 0){
				dati *tmp = new dati();
				tmp->val = i;
				tmp->probab = prob[i];
				tmp->lunghezza = 0;
				tmp->dx = NULL;
				tmp->sx = NULL;
				_lista.push_back(tmp);
			}
		}
		//oridnamento della lista
		_lista.sort(ordina);
		//finchè ci sono due nodi continua a ciclare
		while(_lista.size() > 1){
			//coipa il primo nodo
			dati *tmp1 = _lista.back();
			_lista.pop_back();
			//copia il secondo nodo
			dati *tmp2 = _lista.back();
			_lista.pop_back();
			//crea un nuovo nodo
			dati *tmp = new dati();
			tmp->probab = tmp1->probab + tmp2->probab;
			tmp->sx=tmp1;
			tmp->dx = tmp2;
			tmp->lunghezza = 0;
			_lista.push_back(tmp);
			_lista.sort(ordina);
		}
	}

	void leggiSx(int num, int l, dati *nodo){
		if(nodo->sx == NULL && nodo->dx == NULL){
			dati tmp;
			tmp.codice = num;
			tmp.lunghezza = l;
			_mappa.insert(pair<int,dati>(nodo->val,tmp));
		}
		else{
			leggiSx((num<<1),l+1,nodo->sx);
			leggiDx((num<<1)+1,l+1,nodo->dx);
		}
	}

	void leggiDx(int num, int l, dati *nodo){
		if(nodo->sx == NULL && nodo->dx == NULL){
			dati tmp;
			tmp.codice = num;
			tmp.lunghezza = l;
			_mappa.insert(pair<int,dati>(nodo->val,tmp));
		}
		else{
			leggiSx((num<<1),l+1,nodo->sx);
			leggiDx((num<<1)+1,l+1,nodo->dx);
		}
	}

	void creaMappa(){
		int n = 0;
		leggiSx((n<<1),1,_lista.front()->sx);
		leggiDx((n<<1)+1,1,_lista.front()->dx);
	}
};
