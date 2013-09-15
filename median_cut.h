#ifndef MEDIAN_CUT_H_
#define MEDIAN_CUT_H_
#include "imagePpm.h"
#include <list>
#include <limits>
#include <queue>
#include <algorithm>

const int NUM_DIMENSIONS = 3;

typedef unsigned char byte;
//--------------------------------------------------------------------------------------------------------
struct Point{
    unsigned char x[NUM_DIMENSIONS];
};
//--------------------------------------------------------------------------------------------------------
template <int index>
class CoordinatePointComparator{
 public:
    bool operator()(Point left, Point right){
        return left.x[index] < right.x[index];
    }
};
//-------------------------------------------------------------------------------------------------------
class Block{
 Point minCorner, maxCorner;
 Point* points;
 int pointsLength;
 public:
 Block::Block(Point* points, int pointsLength){
  this->points = points;
  this->pointsLength = pointsLength;
  for(int i=0; i < NUM_DIMENSIONS; i++){
        minCorner.x[i] = std::numeric_limits<unsigned char>::min();
        maxCorner.x[i] = std::numeric_limits<unsigned char>::max();
  }
}
//------------------------------------------------------------------------------------------------------
Point * Block::getPoints(){
    return points;
}
//-------------------------------------------------------------------------------------------------------
int Block::numPoints() const{
    return pointsLength;
}
//--------------------------------------------------------------------------------------------------------
int Block::longestSideIndex() const{
 int m = maxCorner.x[0] - minCorner.x[0];
 int maxIndex = 0;
 for(int i=1; i < NUM_DIMENSIONS; i++){
        int diff = maxCorner.x[i] - minCorner.x[i];
        if (diff > m)        {
            m = diff;
            maxIndex = i;
        }
 }
 return maxIndex;
}
//-------------------------------------------------------------------------------------------------------
int Block::longestSideLength() const{
    int i = longestSideIndex();
    return maxCorner.x[i] - minCorner.x[i];
}
//-------------------------------------------------------------------------------------------------------------
bool Block::operator<(const Block& rhs) const
{
    return this->longestSideLength() < rhs.longestSideLength();
}
//-----------------------------------------------------------------------------------------------------------
void Block::shrink(){
 int i,j;
 for(j=0; j<NUM_DIMENSIONS; j++){
    minCorner.x[j] = maxCorner.x[j] = points[0].x[j];
 }
 for(i=1; i < pointsLength; i++){
	for(j=0; j<NUM_DIMENSIONS; j++){
            minCorner.x[j] = min(minCorner.x[j], points[i].x[j]);
            maxCorner.x[j] = max(maxCorner.x[j], points[i].x[j]);
    }
 }
}
//---------------------------------------------------------------------------------------------------------------
private:
 template <typename T>
 static T min(const T a, const T b){
        if (a < b)
            return a;
        else
            return b;
 }
 template <typename T>
 static T max(const T a, const T b){
  if (a > b)
     return a;
  else
     return b;
 }
};
//Fine class Block
//--------------------------------------------------------------------------------

std::list<Point> Palette(Point* image, int numPoints, unsigned int desiredSize)
{
    std::priority_queue<Block> blockQueue;

    Block initialBlock(image, numPoints); //blocco iniziale di cui ho la dimensione
    initialBlock.shrink();

    blockQueue.push(initialBlock);
    while (blockQueue.size() < desiredSize && blockQueue.top().numPoints() > 1){
        Block longestBlock = blockQueue.top();
        blockQueue.pop();
        Point * begin  = longestBlock.getPoints();
		Point * median = longestBlock.getPoints() + (longestBlock.numPoints()+1)/2;
		Point * end    = longestBlock.getPoints() + longestBlock.numPoints();
		switch(longestBlock.longestSideIndex())	{
			case 0: std::nth_element(begin, median, end, CoordinatePointComparator<0>()); break;
			case 1: std::nth_element(begin, median, end, CoordinatePointComparator<1>()); break;
			case 2: std::nth_element(begin, median, end, CoordinatePointComparator<2>()); break;
		}
		Block block1(begin, median-begin), block2(median, end-median);
		block1.shrink();
		block2.shrink();
		blockQueue.push(block1);
        blockQueue.push(block2);
    }
	std::list<Point> result;
    while(!blockQueue.empty()){
        Block block = blockQueue.top();
        blockQueue.pop();
        Point * points = block.getPoints();
		int sum[NUM_DIMENSIONS] = {0};
        for(int i=0; i < block.numPoints(); i++){
            for(int j=0; j < NUM_DIMENSIONS; j++){
                sum[j] += points[i].x[j];
            }
        }
		Point averagePoint;
        for(int j=0; j < NUM_DIMENSIONS; j++)
            averagePoint.x[j] = sum[j] / block.numPoints();
        result.push_back(averagePoint);
    }
    return result;
}
//------------------------------------------------------------------------------------------------------
byte getbyte(unsigned long val, short b){
//---- Restituisce il byte numero b partendo da destra
 byte by=0;
 byte max= (b-1)*8;
 for(short j=8*b-1; j>=max; j--){  
	 if(val &(1<<j))
		 by=(by<<1)+1;
     else
         by=(by<<1);
 }
 return by;
}
//------------------------------------------------------------------------------------------------------
imagePpm<byte> MedianCut(imagePpm<byte>& imm,unsigned numcol){
 //numPoints=dim dell'immagine
 int numPoints = imm.getWidth()*imm.getHeight();
 //devo prendere i tre byte meno significativi di ogni imm(x,y) e assegnarli a Point
 Point* points = (Point*)malloc(sizeof(Point) * numPoints);
 unsigned int X=3;
 if(imm.colors()>255)
	X=6;

 //in point metto un vettore di dim 6 e se i byte sono 3 uso solo i primi tre
 //for(int i = 0; i < numPoints; i++){  
	//tmp=imm(i%imm.getWidth(),i/imm.getWidth());//nerdata i%imm.width() da il numero della colonna
 //   for(short j=X; j>0; j--)
	//	points[i].x[X-j]= getbyte(tmp, j);//primo byte buono di imm e poi il resto 
	   // crea un vettore di point in cui indica i singoli colori
 unsigned i = 0;
 for(unsigned y=0; y<imm.getHeight(); ++y){
	 for(unsigned x=0; x<imm.getWidth(); ++x){
		 points[i].x[0]= imm(0,x,y);
		 points[i].x[1]= imm(1,x,y);
		 points[i].x[2]= imm(2,x,y);
		 ++i;
	 }
 }
 std::list<Point> palette =Palette(points, numPoints,numcol);
 free(points);
 std::list<Point>::iterator iter;
 unsigned o=0;
 vector<unsigned long> val_unsig(numcol);
 fill(val_unsig.begin(),val_unsig.end(),0);
 for (iter = palette.begin() ; iter != palette.end(); iter++){
	//stampa la palette
    for(unsigned i=0; i<3; i++){
		val_unsig[o]=val_unsig[o]<<8;
        val_unsig[o]=(val_unsig[o])+((byte)iter->x[i]);           
        //cout<<val_unsig[o]<<endl;
		//praticamente questo for mette in un unico unsigned long i 3 byte di rgb che prima erano separati
    }
	o++;
 }
 //bisogna adattare i pixel dell'immagine in modo che siano arrotondati ai valori trovati
 unsigned long dim=imm.getWidth()*imm.getHeight();
 //considero tutti i pixel dell'immagine e vedo la diff da
 vector<short> diff(numcol);
 short by;
 short min;

 for(unsigned long j=0; j<dim; j++){
	//tmp=imm(j%imm.getWidth(),j/imm.getWidth()); 

	unsigned int sa=0;
    for (iter = palette.begin() ; iter != palette.end(); iter++){
		diff[sa]=0;
        by=0;
		// metto in diff il colore dei tre che ha la diff maggiore dall'originale
        /*for(short i=X; i>0; i--){
			by=getbyte(tmp, i)-iter->x[X-i]; 
            if(by<0) 
				by*=-1;
            if(by>diff[sa])
			    diff[sa]=by;
        }*/

		// metto in diff il colore dei tre che ha la diff maggiore dall'originale
				for(short i=X; i>0; i--){
					//by=getbyte(tmp, i)-iter->x[X-i]; 
					by=imm(i-1,j)-iter->x[X-i];
					if(by<0) 
						by*=-1;
					if(by>diff[sa])
						diff[sa]=by;
				}
				sa++;
    }
    //fra tutte le differenze peggiori, prendo la migliore
    min=diff[0];
	unsigned minore=0;
    for(unsigned m=1; m<diff.size(); m++){
		if(diff[m]<min){
		   min=diff[m];
		   minore=m;
	    }
	}
	/*for(unsigned i=0; i<3; ++i){
		unsigned long col = getbyte(val_unsig[minore],i+1);
		imm(i,j) = col;
	}*/

	imm(0,j)=getbyte(val_unsig[minore],1);
	imm(1,j)=getbyte(val_unsig[minore],2);
	imm(2,j)=getbyte(val_unsig[minore],3);

	//imm(0,j) = imm(1,j) = imm(2,j) = val_unsig[minore];
	//scrivo in imm la differenza migliore fra quelle selezionate precedentemente
	//imm(j%imm.getWidth(),j/imm.getWidth())=val_unsig[minore];
    //cout<<j%imm.width()<<" "<<j/imm.width()<<" "<<imm(j%imm.width(),j/imm.width())<<" "<<val_unsig[minore]<< endl;
    //cout<<j<<endl;
 }
 return imm;
}
//--------------------------------------------------------------------------------------------
/*
#include "median_cut.h"
#include "ppm.h"

int main(int argc,char *argv[]){
	image<unsigned long> im;
    read_ppm("rana.ppm", im);
    MedianCut(im,16);
	save_ppm("ranacut.ppm",im);			
}
*/
#endif /* #ifndef MEDIAN_CUT_H_ */

