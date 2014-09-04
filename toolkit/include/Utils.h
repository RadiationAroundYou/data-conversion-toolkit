/// @file Utils.h
/// @brief Header file for the utility classes.

#ifndef UTILS_HH
#define UTILS_HH

// Standard include statements.
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <bitset>
#include <stdint.h>

// ROOT include statements.
#include "TROOT.h"

using namespace std;

/// @brief Namespace for utilities, helper functions, etc.
namespace Utils {

  /// @brief Convenience class for formatting numbers in output streams.
  ///
  /// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
  /// @date January 2014
  class MyWid {

    public:

     /// @brief Constructor.
     ///
     /// @param [in] num The number to be output.
     /// @param [in] width The desired width to output the number with.
     /// @param [in] precision The desired precision for the output number.
     MyWid(const double num, const unsigned width = 10, const unsigned precision = 4);

     /// @brief The get-from operator.
     friend inline std::ostream & operator << (std::ostream &os, const MyWid & h) {
       return os << h.oss_.str();
     }

    private:

     /// @brief The output stream.
     std::ostringstream oss_;

  };//end of class MyWid

  /// @brief Convenience class for outputting hex numbers to output streams.
  ///
  /// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
  /// @date January 2014
  /// @todo Complete this class!
  class MyHex {

    public:

     /// @brief Constructor.
     ///
     /// @param [in] num The number to be output.
     /// @param [in] width The desired width to output the number with.
     MyHex(const UChar_t num, const unsigned width = 2);

     /// @brief Constructor (unsigned short).
     ///
     /// @param [in] num The number to be output.
     /// @param [in] width The desired width to output the number with.
     //MyHex(const uint8_t num, const unsigned width = 2);

     /// @brief The get-from operator.
     friend inline std::ostream & operator << (std::ostream &os, const MyHex & h) {
       return os << h.oss_.str();
     }

    private:

     /// @brief The output stream.
     std::ostringstream oss_;

  };//end of class MyHex


  /// @brief Container class for time information.
  ///
  /// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
  /// @author J. Cook (CERN\@school - Queen Mary, University of London)
  class TimeHandler {

   public:

    /// @brief Constructor.
    ///
    /// @param [in] time The time, including fraction of a second [s].
    TimeHandler(double time);

    /// @brief Destructor.
    ~TimeHandler() {};

   protected:
    /// @brief The copy constructor.
    ///
    /// @param [in] th The TimeHandler to copy.
    TimeHandler(const TimeHandler & th);

    /// @brief The copy assignment operator.
    ///
    /// @param [in] th The TimeHandler to copy.
    TimeHandler & operator=(const TimeHandler & th); 

   public:

    /// @brief Returns a time stamp string in the Pixelman format.
    ///
    /// @return The time stamp string in Pixelman format.
    inline string GetPixelmanTime() { return m_px_time.str(); }

   private:
    /// @brief The time, in seconds, since The Beginning (including decimal).
    double m_time_s;

    /// @brief The time (time_t format).
    time_t m_time;

    /// @brief The day.
    string m_day;

    /// @brief The date.
    string m_dat;

    /// @brief The month.
    string m_mon;

    /// @brief The year.
    string m_yea;

    /// @brief The timestamp (ctime format).
    string m_tim;

    /// @brief The timestamp (Pixelman format).
    stringstream m_px_time;

  };//end of TimeHandler class definition.


  /// @brief Container class for two-byte words.
  ///
  /// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
  class ByteHandler {

   public:

    /// @brief Constructor.
    ///
    /// @param [in] b The input byte.
    /// @param [in] debug Are we in debug mode?
    ByteHandler(char * b, bool debug);

    /// @brief Destructor.
    ~ByteHandler() {};

   protected:
    /// @brief The copy constructor.
    ///
    /// @param [in] bh The ByteHandler to copy.
    ByteHandler(const ByteHandler & bh);

    /// @brief The copy assignment operator.
    ///
    /// @param [in] bh The TimeHandler to copy.
    ByteHandler & operator=(const ByteHandler & bh); 

   public:

    /// @brief Is the bit at position pos 1 or 0?
    ///
    /// @param [in] pos The bit position to test.
    /// @return The result.
    inline bool bit(uint8_t pos) { return bits.test(pos); };

   private:

    /// @brief Are we in debug mode?
    bool dbg;

   protected:

    // The 8-bit byte.
    uint8_t byte;

    // The bits of the byte.
    bitset<8> bits;

  };//end of ByteHandler class definition.

}//end of Utils namespace

#endif
