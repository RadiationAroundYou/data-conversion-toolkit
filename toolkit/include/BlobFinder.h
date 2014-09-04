/// @file BlobFinder.h
/// @brief Header file for the BlobFinder class (and related classes).

#ifndef BlobFinder_h
#define	BlobFinder_h

// Standard include statements.
#include <iostream>
#include <map>
#include <list>
#include <set>
#include <math.h>

// ROOT include statements.
#include "TROOT.h"

using namespace std;


/// @brief A container class for Timepix pixels.
///
/// @author Son Hoang (principle author).
/// @author T. Whyntie (editor for CERN\@school).
class Pixel {

 public:

  /// @brief Constructor.
  ///
  /// @param [in] x The pixel x coordinate.
  /// @param [in] y The pixel y coordinate.
  /// @param [in] C The pixel ToT counts.
  /// @param [in] M The pixel mask value.
  Pixel(Int_t x, Int_t y, Double_t C, Int_t M);

 private:

  /// @brief The pixel x coordinate.
  Int_t x;

  /// @brief The pixel y coordinate.
  Int_t y;

  /// @brief The pixel ToT count.
  Double_t v;

  /// @brief Map of the neighbouring pixels.
  map<Int_t, Pixel *> Neighbor;

  /// @brief Set to -1 on creation... whether clustered or not?
  Int_t mask;    

  /// @brief ???
  Int_t level;

  /// @brief ???
  Int_t cluster;

 public:

  /// @brief Get the pixel x coordinate.
  ///
  /// @return The pixel x coordinate.
  inline Int_t getX() { return x; }

  /// @brief Get the pixel y coordinate.
  ///
  /// @return The pixel y coordinate.
  inline Int_t getY() { return y; }

  /// @brief Get the pixel ToT count.
  ///
  /// @return The pixel ToT count.
  inline Double_t getV() { return v; }

  /// @brief Get the pixel's mask.
  ///
  /// @return The pixel's mask value.
  inline Int_t getMask() { return mask; }

  /// @brief Set the Pixel's mask value.
  ///
  /// @param [in] M The pixel's mask value.
  inline void setMask(Int_t M) { mask = M;}

  /// @brief The the neighbouring pixel.
  ///
  /// @param [in] dir The direction to look in.
  /// @return The neighbouring pixel in the supplied direction.
  Pixel * getNeighbor(Int_t dir);

  /// @brief Set the neighbouring pixel.
  ///
  /// @param [in] dir The direction of the neighbouring pixel.
  /// @param [in] p The neighbouring pixel.
  void setNeighbor(Int_t dir, Pixel* p);

};//end of Pixel class definition.

/// @brief Container class for Blobs (clusters).
///
/// @author Son Hoang (principle author).
/// @author T. Whyntie (editor for CERN\@school).
class Blob {

 public:

  /// @brief Constructor.
  Blob();

  /// @brief Destructor.
  ~Blob();

 private:

  /// @brief Total ToT counts of the blob, set when clustering.
  Double_t totalEnergy; 

  /// @brief The list of pixels in the Blob.
  std::set<Pixel*> setPixel; 
    
 public:

  /// @brief Calculate the Blob properties.
  void getBlobProperties();

  /// @brief List of Pixels in the Blob.
  std::list<Pixel*> listPixel; 

  /// @brief Insert a pixel into the Blob.
  ///
  /// @param [in] pixel The pixel to insert.
  void insert(Pixel* pixel);

  /// @brief Checks if a pixel is in the Blob or not.
  ///
  /// @param [in] pixel The pixel to check for.
  /// @return Is the pixel in the Blob?
  bool isNotContain(Pixel* pixel);

  /// @brief Get the number of pixels in the Blob.
  ///
  /// @return The number of pixels in the Blob.
  inline Int_t getSize() { return listPixel.size(); }

  /// @brief Get the total number of ToT counts in the Blob.
  ///
  /// @return The total counts in the Blob.
  inline Double_t getTotalEnergy() { return totalEnergy; }

  /// @brief Free up the Blob.
  void freeBlob();

};//end of Blob class definition.

/// @brief Finds blobs (clusters) in Timepix frames.
///
/// @author Son Hoang (principle author).
/// @author T. Whyntie (editor for CERN\@school).
class BlobFinder {
  
 public:

  /// @brief Constructor.
  ///
  /// @param [in] data The pixel data for the frame.
  /// @param [in] R The number of rows (frame height).
  /// @param [in] C The number of columns (frame width).
  BlobFinder(std::map<int,int> const & data, Int_t R, Int_t C);

  /// @brief Destructor.
  ~BlobFinder();

  /// @brief Add a blob to the list of blobs.
  ///
  /// @param [in] blob The blob to add to the blob finder.
  inline void insert(Blob blob) { listBlob.push_back(blob); }

  /// @brief The list of Blobs found in the frame.
  std::list<Blob> listBlob; 

  /// @brief Get the number of blobs.
  ///
  /// @return The number of blobs (clusters) found in the frame.
  inline Int_t getSize() { return listBlob.size(); }

  /// @brief Get the list of clusters.
  ///
  /// @return The list of blobs.
  inline list<Blob> & getList() { return listBlob; }

};//end of BlobFinder class definition.

#endif	/* BlobFinder_h */
