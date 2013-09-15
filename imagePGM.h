#ifndef IMAGE_H
#define IMAGE_H
#define _USE_MATH_DEFINES
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;

template <typename T>
class imagePgm{
private:
	string _linea;
	int _width;
	int _height;
	int _numDati;
	int _maxGray;
	vector<T> _img;

	//funzione per il calcolo del DCT
	double C(int num){
		if(num == 0)
			return (1/sqrt(2.0));
		else
			return 1;
	}

public:
	imagePgm() : _width(0), _height(0) {}

	imagePgm(int width, int height) : _width(width), _height(height){
		_numDati = _width * _height;
		_img.resize(_numDati);
	}

	//carica immagine dei pixel nel vettore
	bool loadImage(string nome){
		fstream _file(nome.c_str(), fstream::in | fstream::binary);
		getline(_file, _linea);
		if(_linea != "P5")
			return false;
		do{
			getline(_file, _linea);
		}while(_linea[0] == '#');
		//leggo e converto i valori della dimensione dell'immagine
		string w, h;
		int i = 0;
		while(_linea[i] != ' '){
			w += _linea[i];
			++i;
		}
		++i;
		while(i != _linea.size()){
			h += _linea[i];
			++i;
		}
		_width = atoi(w.c_str());
		_height = atoi(h.c_str());
		_numDati = _width * _height;

		//leggo il valore della scala di grigi
		getline(_file, _linea);

		//carico i dati sul vettore
		i = 0;
		unsigned char c = ' ';
		while(i != _numDati){
			_file.read((char *)&c, 1);
			_img.push_back((T)c);
			++i;
		}	
		return true;
	}

	//calcolo il valore massimo di grigio
	void maxGray(){
		_maxGray = *max_element(_img.begin(), _img.end());
	}


	//ritorna la larghezza dell'immagine
	int getWidth(){
		return _width;
	}

	//ritorna l'altezza dell'immagine
	int getHeight(){
		return _height;
	}

	//ritorna il numero di pixel dell'immagine
	int getDim(){
		_numDati = _width * _height;
		return _numDati;
	}

	T& operator() (int x, int y){
		int i = y*_width+x;
		return _img[i];
	}

	T& operator() (int i){
		return _img[i];
	}

	void savePgm(string nome){
		fstream file(nome.c_str(), fstream::out | fstream::binary);
		file << "P5" << endl;
		file << "# Creato da Montanari Mirko" << endl;
		file << _width << " " << _height << endl;
		file << "255" << endl;
		for(int i=0; i<getDim(); ++i)
			file.write((const char *)&_img[i], 1);
		file.close();
	}

	void resize(int n){
		int dx = n - (_width % 8);
		int dy = n - (_height % 8);
		imagePgm<T> img2(_width+dx, _height+dy);
		//copio valori già esistenti
		for(int i=0; i<_height; ++i){
			for(int k=0; k<_width; ++k){
				img2(k,i) = _img[i*_width+k];
			}
		}
		//aggiungo pixel alla larghezza
		for(int i=0; i<_height; ++i){
			for(int k=_width; k<_width+dx; ++k){
				img2(k,i) = img2(_width-1,i);
			}
		}
		//aggiungo pixel alla lunghezza
		for(int i=_height; i<_height+dy; ++i){
			for(int k=0; k<_width+dx; ++k){
				img2(k,i) = img2(k,_height-1);
			}
		}
		_img = img2._img;
		_width = img2.getWidth();
		_height = img2.getHeight();
		_numDati = img2.getDim();
	}

	void DCT(){
		imagePgm<int> img_dct(_width,_height);
		for(int blocky=0; blocky<_height; blocky+=8){
			for(int blockx=0; blockx<_width; blockx+=8){
				for(int v=0; v<8; ++v){
					for(int u=0; u<8; ++u){
						double S = 0;
						for(int y=0; y<8; ++y){
							for(int x=0; x<8; ++x){
								S += (_img[(blocky+y)*_width+(blockx+x)]-127) * cos(((2*x+1)*u*M_PI/16)) * cos(((2*y+1)*v*M_PI)/16);
							}
						}
						S = S*(1/(double)4)*C(u)*C(v);
						img_dct(blockx+u, blocky+v) = int((S/8)+127);
					}
				}
			}
		}
		_img = img_dct._img;
	}

	void riordina_dct(){
		imagePgm<T> img2(_width, _height);
		int j = _width/8;
		int i = _height/8;
	
		for(int blocky=0; blocky<_height; blocky+=8){
			for(int blockx=0; blockx<_width; blockx+=8){
				for(int y=0; y<8; ++y){
					for(int x=0; x<8; ++x){
						img2(j*x+(blockx/8),i*y+(blocky/8)) = _img[(blocky+y)*_width+(blockx+x)];
					}
				}
			}
		}
		_img = img2._img;
	}
};

#endif //IMAGE_H