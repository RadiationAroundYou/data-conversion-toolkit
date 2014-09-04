/// @file BlobFinder.cpp
/// @brief Implementation of the BlobFinder class (and related classes).

#include "BlobFinder.h"

//
// Pixel constructor.
//
Pixel::Pixel(Int_t X, Int_t Y, Double_t V, Int_t M) {
  x  = X;
  y  = Y;
  v  = V;
  mask    =  M;
  level   = -1;
  cluster = -1;
}//end of Pixel constructor.

//
// Pixel::getNeighbor
//
Pixel * Pixel::getNeighbor(Int_t dir) {
  std::map<Int_t,Pixel*>::iterator iter=Neighbor.find(dir);
  if (iter!=Neighbor.end()) return iter->second;
  return 0;
}//end of Pixel getNeighbor method.

//
// Pixel::setNeighbor
//
void Pixel::setNeighbor(Int_t dir, Pixel * p) {
  if (p) {
    Neighbor[dir] = p;
  }
}


//
// Blob constructor.
//
Blob::Blob() {
  totalEnergy = 0;
}//end of Blob constructor.

//
// Blob destructor.
//
Blob::~Blob() {

}//end of Blob destructor.

//
// Blob::insert
//
void Blob::insert(Pixel* pixel){
  listPixel.push_back(pixel);
  setPixel.insert(pixel);
  totalEnergy += pixel->getV();
}//end of Blob::insert (Pixel) method.

//
// Blob::isNotContain
//
bool Blob::isNotContain(Pixel* pixel) {
  return setPixel.find(pixel)==setPixel.end();
}


//
// Calculate the properties of the Blob.
//
void Blob::getBlobProperties() {

}//end of Blob getBlobProperties method.

//
// Blob::freeBlob
//
void Blob::freeBlob() 
{

  std::list<Pixel*>::iterator iter;

  for(iter=listPixel.begin();iter != listPixel.end(); iter++){
    delete (*iter);
  }

}

// BlobFinder methods
//--------------------

//
// BlobFinder constructor
//
BlobFinder::BlobFinder(std::map<int,int> const & data,
                       Int_t R,
                       Int_t C)
{

  listBlob.clear();

  bool dbg=false;

  if (dbg) {
    cout
      << "DEBUG: * Data supplied to the BlobFinder has "<<data.size()<<" pixels."<<endl;
  }

  // An arbitrary check on number of pixels in the frame.
  //if (data.size()>=10000) return;

  Int_t dirX[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
  Int_t dirY[8] = { 0,  1,  1,  1,  0, -1, -1, -1};

  std::map<int, Pixel*> mapPixel;          // Map of the pixels.


  // Loop over the data supplied to the BlobFinder.
  // * Puts all of the data into the pixel map;
  // * Assigns neighbouring pixels where it find them.
  //std::map<int,double>::iterator mapIter; // Iterator for the pixel map.
  std::map<int,int>::const_iterator mapIter; // Iterator for the pixel map.
  //
  for(mapIter=data.begin(); mapIter!=data.end(); mapIter++) {
//            int xplace = r*C + c; //xplace is increasing
    int xplace=mapIter->first; // The current pixel X.
    Int_t r=xplace/C;          // The current row.
    Int_t c=xplace%C;          // The current column.
    //if(dbg) cout<<"DEBUG: xplace,r,c="<<xplace<<", "<<r<<", "<<c<<endl;

    // Create (a pointer to) a new pixel.
    double counts = mapIter->second;
    //Pixel* p = new Pixel(c, r, double(data[xplace]), -1);
    Pixel* p = new Pixel(c, r, counts, -1);
    //Pixel* p = new Pixel(c, r, data[xplace], -1);
    mapPixel[xplace] = p; //put the pixel on the map.

    // Loop over the eight possible directions.
    for(Int_t dir = 0; dir < 8; dir++) {
      Int_t nr = r + dirY[dir]; // Next row.    // SH be careful
      Int_t nc = c + dirX[dir]; // Next column.

      // If the next row or column is on an edge, skip it.
      if(nr<0 || nr>=R || nc<0 || nc>=C) continue;

      // Find the next X pixel value.
      Int_t nplace = nr * C + nc;    // The next X value.
      std::map<Int_t,Pixel*>::iterator pIter; // Iterator to search the map.
      pIter = mapPixel.find(nplace); // Search the map.
      if( pIter!=mapPixel.end()) {   // We have a pixel at the next X! // SH - hit
        Pixel * n = pIter->second;   // The neighbouring pixel.
        p->setNeighbor(dir,n);       // Set the found pixel as a neighbour.
        n->setNeighbor((dir+4)%8,p); // And return the favour.
      }//end of hit pixel check.
    }//end over the loop of the possible directions.
  }//end of loop over the pixel map.

  //if (dbg) cout<<"DEBUG: * Size of Pixel map = "<<mapPixel.size()<<endl;

  // Now loop over the pixels
  std::map<int, Pixel*>::iterator iter;
  for(iter=mapPixel.begin(); iter!=mapPixel.end(); iter++) {
    Pixel * p = iter->second;

    // So, the mask determines if the pixel has been clustered???
    if(p->getMask()==-1) { //start a new blob
      Blob aBlob; // The new blob
      p->setMask(0);
      aBlob.insert(p);

      // Loop over the list of pixels in the blob.
      std::list<Pixel*>::iterator listIter=aBlob.listPixel.begin();
      while(listIter!=aBlob.listPixel.end()) {
        Pixel* px = (*listIter);
        for(Int_t dir = 0; dir < 8; dir++){
          Pixel * n = px->getNeighbor(dir);
          if (n) {
            px->setMask(px->getMask() + pow(2.0,1.0*dir));
            // If the Pixel isn't already in the Blob, add it.
            if(aBlob.isNotContain(n)) {
              n->setMask(0);
              aBlob.insert(n);
            }//end of Pixel presence check.
          }//end of Pixel existence check.
        }//end of loop over the directions.
        listIter++;
      }//end of while loop over the Blob contents.
      aBlob.getBlobProperties();
      insert(aBlob);   
    }//end of check on the Pixel mask.
  }//end of loop over the Pixels.
  //getBlobProperties();

}//end of BlobFinder constructor (doubles).

//
// BlobFinder destructor.
//
BlobFinder::~BlobFinder(){

}//end of BlobFinder destructor.
