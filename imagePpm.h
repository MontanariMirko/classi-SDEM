#ifndef IMAGEPPM_H
#define IMAGEPPM_H
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <math.h>
#include <algorithm>
using namespace std;

#define PI 3.14159265

template <typename T>
class imagePpm{
private:
	string _linea;
	int _width;
	int _height;
	int _numDati;
	int _maxVal;
	vector<T> _r, _g, _b;

public:
	imagePpm() : _width(0), _height(0) {}

	imagePpm(int width, int height) : _width(width), _height(height){
		_numDati = _width * _height;
		_r.resize(_numDati);
		_g.resize(_numDati);
		_b.resize(_numDati);
	}

	//carica immagine dei pixel nel vettore
	bool loadImage(string nome){
		fstream _file(nome.c_str(), fstream::in | fstream::binary);
		getline(_file, _linea);
		if(_linea != "P6")
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
		unsigned char c = ' ';
		while(true){
			//inserimento rosso
			_file.read((char *)&c, 1);
			_r.push_back((T)c);
			/*++i;*/
			if(_file.eof())
				break;
			//inserimento verde
			_file.read((char *)&c, 1);
			_g.push_back((T)c);
			/*++i;*/
			if(_file.eof())
				break;
			//inserimento blu
			_file.read((char *)&c, 1);
			_b.push_back((T)c);
			/*++i;*/
			if(_file.eof())
				break;
		}	
		return true;
	}

	//calcolo il valore massimo di grigio
	void maxVal(){
		int tmp_r = *max_element(_r.begin(), _r.end());
		int tmp_g = *max_element(_g.begin(), _g.end());
		int tmp_b = *max_element(_b.begin(), _b.end());
		if(tmp_r > tmp_g)
			_maxVal = tmp_r;
		else
			_maxVal = tmp_g;
		if(tmp_b > _maxVal)
			_maxVal = tmp_b;
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

	T& operator() (int col, int x, int y){
		int i = y*_width+x;
		if(col == 0)
			return _r[i];
		if(col == 1)
			return _g[i];
		if(col == 2)
			return _b[i];
	}

	T& operator() (int col, int i){
		if(col == 0)
			return _r[i];
		if(col == 1)
			return _g[i];
		if(col == 2)
			return _b[i];
	}

	void savePpm(string nome){
		fstream file(nome.c_str(), fstream::out | fstream::binary);
		file << "P6" << endl;
		file << "# Creato da Montanari Mirko" << endl;
		file << _width << " " << _height << endl;
		maxVal();
		file << _maxVal << endl;
		for(int i=0; i<getDim(); ++i){
			file.write((const char *)&_r[i], 1);
			file.write((const char *)&_g[i], 1);
			file.write((const char *)&_b[i], 1);
		}
		file.close();
	}

	void resize(int n){
		int dx, dy;
		if((_width % n) == 0)
			dx = 0;
		else
			dx = n - (_width % n);
		if((_height % n) == 0)
			dy = 0;
		else
			dy = n - (_height % n);
		imagePpm<T> img2(_width+dx, _height+dy);
		//copio valori già esistenti
		for(int i=0; i<_height; ++i){
			for(int k=0; k<_width; ++k){
				img2(0,k,i) = _r[i*_width+k];
				img2(1,k,i) = _g[i*_width+k];
				img2(2,k,i) = _b[i*_width+k];
			}
		}
		//aggiungo pixel alla larghezza
		for(int i=0; i<_height; ++i){
			for(int k=_width; k<_width+dx; ++k){
				img2(0,k,i) = img2(0,_width-1,i);
				img2(1,k,i) = img2(1,_width-1,i);
				img2(2,k,i) = img2(2,_width-1,i);
			}
		}
		//aggiungo pixel alla lunghezza
		for(int i=_height; i<_height+dy; ++i){
			for(int k=0; k<_width+dx; ++k){
				img2(0,k,i) = img2(0,k,_height-1);
				img2(1,k,i) = img2(1,k,_height-1);
				img2(2,k,i) = img2(2,k,_height-1);
			}
		}
		_r = img2._r;
		_g = img2._g;
		_b = img2._b;
		_width = img2.getWidth();
		_height = img2.getHeight();
		_numDati = img2.getDim();
	}

	
	void ruota(double angle){
		unsigned w, h;
		h = (unsigned)floor(_height*cos(angle*PI/180)+_width*sin(angle*PI/180));
		w = (unsigned)floor(_height*sin(angle*PI/180)+_width*cos(angle*PI/180));
		imagePpm<T> tmp(w,h);
		for(int y=0; y<tmp.getHeight(); ++y){
			for(int x=0; x<tmp.getWidth(); ++x){
				tmp(0, x, y) = tmp(1, x, y) = tmp(2, x, y) = 255;
			}
		}
		int qty = (floor(_height*sin(angle*PI/180)));

		for(unsigned y=0; y<_height; ++y){
			for(unsigned x=0; x<_width; ++x){
				    int x1 = floor(x*cos(angle*PI/180)-y*sin(angle*PI/180))+qty;
					int y1 = floor(x*sin(angle*PI/180)+y*cos(angle*PI/180));
					if(x1>0 && y1>0 && y1<h && x1<w){
						tmp(0, x1, y1) = _r[y*_width+x];
						tmp(1, x1, y1) = _g[y*_width+x];
						tmp(2, x1, y1) = _b[y*_width+x];
					}
			}
		}

		for(int y=1; y<tmp.getHeight()-1; ++y){
			for(int x=1; x<tmp.getWidth()-1; ++x){
				if(tmp(0,x,y)==255 && tmp(1,x,y)==255 && tmp(2,x,y)==255){
					double media = (tmp(0,x-1,y)+tmp(0,x,y-1)+tmp(0,x+1,y)+tmp(0,x,y+1))/4;
					tmp(0,x,y) = (byte)media;
					media = (tmp(1,x-1,y)+tmp(1,x,y-1)+tmp(1,x+1,y)+tmp(1,x,y+1))/4;
					tmp(1,x,y) = (byte)media;
					media = (tmp(2,x-1,y)+tmp(2,x,y-1)+tmp(2,x+1,y)+tmp(2,x,y+1))/4;
					tmp(2,x,y) = (byte)media;
				}
			}
		}
		tmp.savePpm("prova.ppm");
	}
};

#endif //IMAGEPPM_H