/// @file Utils.cc
/// @brief Implementation of the utility classes.

#include "Utils.h"

namespace Utils {

  // MyWid - number formatting utility implementation
  MyWid::MyWid(const double num, const unsigned width, const unsigned precision) {
    oss_ << std::showpoint << std::fixed << std::setw(width - 1) << std::setprecision(precision) << num;
  }

  MyHex::MyHex(const UChar_t num, const unsigned width) {
    oss_ << "0x" << std::fixed << std::setw(width) << std::setfill('0') << hex << Int_t(num); //Int_t(num);
  }

  //MyHex::MyHex(const uint8_t num, const unsigned width) {
  //  oss_ << "0x" << std::fixed << std::setw(width) << std::setfill('0') << hex << num;
  //}


  //
  // TimeHandler constructor
  //
  TimeHandler::TimeHandler(double time)
  :
    m_time_s(time),
    m_time(time)
  {

    //cout << "DEBUG TMP: " << Utils::MyWid(time,30,8) << endl;

    string ts  = static_cast<string>(ctime(&m_time));

    string m_day = ts.substr( 0,3);
    string m_mon = ts.substr( 4,3);
    int    m_dat = atoi(ts.substr( 8,2).c_str());
    int    m_hou = atoi(ts.substr(11,2).c_str());
    int    m_min = atoi(ts.substr(14,2).c_str());
    int    m_sec = atoi(ts.substr(17,2).c_str());
    string m_yea = ts.substr(20,4);

    double secfrac = m_time_s - double(int(m_time_s));

    //Tue Feb 05 09:42:49.423306 2013
    m_px_time
     << m_day << " "
     << m_mon << " "
     << setfill('0') << setw(2) << m_dat << " "
     << setfill('0') << setw(2) << m_hou << ":"
     << setfill('0') << setw(2) << m_min << ":"
     << showpoint << fixed << setfill('0') << setw(9)
     << setprecision(6) << double(m_sec) + secfrac << " "
     << m_yea;

  }//end of TimeHandler constructor.

  //
  // ByteHandler constructor
  //
  ByteHandler::ByteHandler(char * b, bool debug = false)
  :
    dbg(debug),
    byte(*b),
    bits(byte)
  {

    if (dbg) {
      cout
        << "* DEBUG: constructed byte '" << bits << "' -> '" << MyHex(byte, 2) << "' -> " << int(byte) << endl;
    }

  }//end of ByteHandler constructor.


}//end of Utils namespace.
