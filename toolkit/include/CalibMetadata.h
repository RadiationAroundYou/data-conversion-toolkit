/// @file CalibMetadata.h
/// @brief Header file for the calibration data metadata container class.

#ifndef CalibMetadata_h
#define CalibMetadata_h 1

// Standard include statements.
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>

// ROOT include statements.
#include "TString.h"
#include "TXMLEngine.h"

#include "Utils.h"

using namespace std;

/// @brief A container class for the calibration metadata.
///
/// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
/// @date February 2014
class CalibMetadata {

 public:
  /// @brief Constructor.
  ///
  /// @param [in] xmlPath The full path to the XML metadata file.
  explicit CalibMetadata(TString xmlPath);

  /// @brief Destructor.
  ~CalibMetadata(); 

 protected:
  /// @brief Copy constructor (protected).
  ///
  /// @param [in] cmd The CalibMetadata object to copy.
  CalibMetadata(const CalibMetadata & cmd);

  /// @brief Copy assignment operator (protected).
  ///
  /// @param [in] cmd The CalibMetadata object to copy.
  CalibMetadata & operator=(const CalibMetadata & cmd); 

 public:

  /// @brief Get the path to the XML configuration file.
  ///
  /// @return The path to the XML configuration file.
  TString const & GetXmlPath() { return m_xmlPath; }

  // Payload information - accessor methods
  //----------------------------------------

  /// @brief Get the frame width.
  ///
  /// @return The frame width [pixels].
  Int_t const & GetFrameWidth()    { return fWidth;  }

  /// @brief Get the frame height.
  ///
  /// @return The frame height [pixels].
  Int_t const & GetFrameHeight()   { return fHeight; }

  /// @brief Get the payload format.
  ///
  /// @return The payload format.
  Int_t const & GetPayloadFormat() { return fFormat; }

  /// @brief Get the dataset ID.
  ///
  /// @return the dataset ID.
  TString const & GetMPXDataSetNumber() { return fMPXDataSetNumber; }

  // Acquisition information
  //-------------------------

  /// @brief Get the acquisition mode (code).
  ///
  /// @return The acquisition mode (integer code).
  inline Int_t GetAcqMode() { return fAcq_mode; }

  /// @brief Get the acquisition counters.
  ///
  /// @return A vector of the acquisition counters.
  inline vector<Int_t> GetCounters() { return fCounters; }

  /// @brief Get the hardware timer mode..
  ///
  /// @return The hardware timer mode.
  inline Int_t GetHwTimerMode() { return fHw_timer; }

  /// @brief Get the auto-erase interval [s].
  ///
  /// @return The auto-erase interval.
  inline Double_t GetAutoEraseInterval() { return fAuto_erase_interval; }

  /// @brief Get the auto-erase interval counter.
  ///
  /// @return The auto-erase interval counter.
  inline Int_t GetAutoEraseIntervalCounter()
    { return fAuto_erase_interval_counter; }

  /// @brief Get the time since the last trigger [s].
  ///
  /// @return The time since the last trigger [s].
  inline Double_t GetLastTriggerTime() { return fTrigger_time; }

  /// @brief Get the coincidence mode.
  ///
  /// @return The coincidence mode code.
  inline UChar_t GetCoincidenceMode() { return fCoincidence_mode; }

  /// @brief Get the coincidence delay.
  ///
  /// @return The coincidence delay time [64us].
  inline UChar_t GetCoincidenceDelayTime() { return fCoincidence_delay; }

  /// @brief Get the coincidence live time.
  ///
  /// @return The coincidence live time [s].
  inline Double_t GetCoincidenceLiveTime() { return fCoinc_live_time; }

  /// @brief Get the Pixelman version.
  ///
  /// @return The Pixelman version.
  inline TString GetPixelmanVersion() { return fPixelman_version; }

  // Geospatial information - accessor methods
  //-------------------------------------------
  /// @brief Get the latitude.
  ///
  /// @return The latitude [deg.].
  inline Double_t GetLatitude() { return fLatitude; }

  /// @brief Get the longitude.
  ///
  /// @return The longitude [deg.].
  inline Double_t GetLongitude() { return fLongitude; }
  /// @brief Get the altitude.
  ///
  /// @return The altitude [km].
  inline Double_t GetAltitude() { return fAltitude; }

  // Temporal information - accessor methods
  //-----------------------------------------
  // [Not available from the calibration information.]

  // Detector settings - accessor methods
  //--------------------------------------

  /// @brief Get the detector polarity.
  ///
  /// @return The polarity (0 = negative, 1 = postive).
  inline Int_t GetPolarity() { return fPolarity; }

  /// @brief Get the bias voltage (HV).
  ///
  /// @return The bias voltage (HV) [V].
  inline Double_t GetHV() { return fHV; }

  /// @brief Get the device DACs.
  ///
  /// @return A vector of the 14 device DACs.
  inline vector<Int_t> GetDACs() { return fDACs; }

  /// @brief Get the Medipix clock frequency.
  ///
  /// @return The Medipix clock frequency [MHz].
  inline Double_t GetMpxClock() { return fMpx_clock; }

  /// @brief Get the Timepix clock frequency setting.
  ///
  /// @return The Timepix clock frequency setting (1-4).
  ///
  /// * 0: 10 MHz;
  /// * 1: 20 MHz;
  /// * 2: 40 MHz;
  /// * 3: 80 MHz.
  inline Byte_t GetTpxClock() { return fTimepix_clock; }

  /// @brief Get the back side preamp. enabled flag value.
  ///
  /// @return Is the back side preamp. enabled?.
  inline Bool_t GetBsActive() { return fBS_active; }


  // Detector information - accessor methods
  //-----------------------------------------

  /// @brief Get the chipboard ID.
  ///
  /// @return The chip board ID.
  inline TString GetChipboardID() { return fChipboardID; }

  /// @brief Get the detector's custom name.
  ///
  /// @return The the detector's custom name.
  inline TString GetCustomName() { return fCustom_name; }

  /// @brief Get the firmware version.
  ///
  /// @return The firmware version.
  inline TString GetFirmware() { return fFirmware; }

  /// @brief Get the Medipix interface type.
  ///
  /// @return The Medipix interfac type.
  inline TString GetInterface() { return fInterface; }

  /// @brief Get the Medipix detector type.
  ///
  /// @return The Medipix detector type.
  inline Int_t GetMpxType() { return fMpx_type; }

  /// @brief Get the filters applied to the readout.
  ///
  /// @return The filters applied.
  inline TString GetAppFilters() { return fApplied_filters; }

  /// @brief Get the detector x coordinate (lab frame).
  ///
  /// @return The detector x coordinate (lab frame) [mm].
  inline Double_t GetDet_x() { return m_x; }

  /// @brief Get the detector y coordinate (lab frame).
  ///
  /// @return The detector y coordinate (lab frame) [mm].
  inline Double_t GetDet_y() { return m_y; }

  /// @brief Get the detector z coordinate (lab frame).
  ///
  /// @return The detector z coordinate (lab frame) [mm].
  inline Double_t GetDet_z() { return m_z; }

  /// @brief Get the detector rotation angle Omega_x.
  ///
  /// @return The detector rotation angle Omega_x [deg.].
  inline Double_t GetOmega_x() { return m_omega_x; }

  /// @brief Get the detector rotation angle Omega_y.
  ///
  /// @return The detector rotation angle Omega_y [deg.].
  inline Double_t GetOmega_y() { return m_omega_y; }

  /// @brief Get the detector rotation angle Omega_z.
  ///
  /// @return The detector rotation angle Omega_z [deg.].
  inline Double_t GetOmega_z() { return m_omega_z; }

  /// @brief Get the lab frame roll.
  ///
  /// @return The roll angle of the laboratory frame [deg.].
  inline Double_t GetRoll() { return m_roll_orf; }

  /// @brief Get the lab frame pitch angle.
  ///
  /// @return The pitch angle of the laboratory frame [deg.].
  inline Double_t GetPitch() { return m_pitch_orf; }

  /// @brief Get the yaw angle of the laboratory frame.
  ///
  /// @return The yaw angle of the laboratory frame.
  inline Double_t GetYaw() { return m_yaw_orf; }


  // Source information - accessor methods
  //---------------------------------------

  /// @brief Get the source ID.
  ///
  /// @return The source ID.
  inline TString GetSourceId() { return fSourceId; }

  /// @brief Prints the contents of the calibration XML file.
  void PrintContents();

 private:
  /// @brief The Parses the XML metadata file.
  Bool_t ParseXmlFile(); 

 private:
  /// @brief The full path to the XML metadata file.
  TString m_xmlPath; 

  // Frame metadata
  //================
  //
  // Payload information
  //---------------------
  
  /// @brief The Frame width [pixels].
  Int_t fWidth;            
  
  /// @brief The Frame height [pixels].
  Int_t fHeight;

  /// @brief The Payload format.
  Int_t fFormat;
  
  /// @brief The Dataset ID.
  TString fMPXDataSetNumber; 

  
  // Acquisition information
  //-------------------------
  
  /// @brief The acquisition mode.
  Int_t fAcq_mode; 
 
  /// @brief The acquisition counters.
  vector<Int_t> fCounters;

  /// @brief The hardware timer mode.
  Int_t fHw_timer;

  /// @brief The auto erase interval [s].
  Double_t fAuto_erase_interval;

  /// @brief The auto erase interval counter.
  Int_t fAuto_erase_interval_counter; 
  
  /// @brief The time since the last trigger [s].
  Double_t fTrigger_time;
  
  /// @brief The coincidence mode.
  UChar_t fCoincidence_mode;
  
  /// @brief The coincidence delay [64us].
  UChar_t fCoincidence_delay;
  
  /// @brief The coincidence live time [s].
  Double_t fCoinc_live_time;
  
  /// @brief The Pixelman version.
  TString fPixelman_version;


  // Detector settings
  //-------------------

  /// @brief The detector polarity.
  Int_t fPolarity;

  /// @brief The bias voltage (HV) [V].
  Double_t fHV; 

  /// @brief The DAC settings.
  vector<Int_t> fDACs;

  /// @brief The Medipix clock [MHz].
  Double_t fMpx_clock;

  /// @brief The Timepix clock setting.
  Byte_t fTimepix_clock;

  /// @brief Is the back side preamp. enabled?
  Bool_t fBS_active;       

  // Detector information
  //----------------------

  /// @brief The Chipboard ID.
  TString fChipboardID;

  /// @brief The Custom detector name.
  TString fCustom_name;

  /// @brief The Firmware version.
  TString fFirmware;

  /// @brief The Interface type.
  TString fInterface;

  /// @brief The Medipix type (1:2.1; 2:MXR; 3:TPX)..
  Int_t fMpx_type;

  /// @brief The Applied filters.
  TString fApplied_filters;
  
  /// @brief The x position of the chip surface centre [mm].
  Double_t m_x;         

  /// @brief The y position of the chip surface centre [mm].
  Double_t m_y;         

  /// @brief The z position of the chip surface centre [mm].
  Double_t m_z;         

  /// @brief The Euler angle alpha of the detector rotation [deg.].
  Double_t m_euler_a;

  /// @brief The Euler angle beta of the detector rotation [deg.].
  Double_t m_euler_b;

  /// @brief The Euler angle gamma of the detector rotation [deg.].
  Double_t m_euler_c;


  //
  // Geospatial information
  //------------------------

  /// @brief The Latitude of the laboratory frame [deg.].
  Double_t fLatitude;
  
  /// @brief The longitude of the laboratory frame [deg.].
  Double_t fLongitude;

  /// @brief The altitude of the laboratory frame [km].
  Double_t fAltitude;

  /// @brief The rotational velocity of the lab frame [deg./s].
  Double_t m_omega_x;   

  /// @brief The rotational velocity of the lab frame [deg./s].
  Double_t m_omega_y;

  /// @brief The rotational velocity of the lab frame [deg./s].
  Double_t m_omega_z;

  /// @brief The roll angle of the laboratory frame [deg.].
  Double_t m_roll_orf;

  /// @brief The pitch angle of the laboratory frame [deg.].
  Double_t m_pitch_orf;

  /// @brief The yaw angle of the laboratory frame [deg.].
  Double_t m_yaw_orf;
 
  // Temporal information
  //----------------------
  // These are determined frame by frame, and so are not included
  // in the calibration meta data class.
  //Double_t fStart_time;  /// @brief The Start time of the frame [s].
  //TString  fStart_timeS; /// @brief The Start time of the frame (string).
  //Double_t fAcq_time;    /// @brief The Acquisition time of the frame [s].
  //

  // Source information
  //--------------------

  /// @brief The Label for the particle source.
  TString fSourceId; 

  // TODO: implement the vector of source particles (MC).
  //
  // Note that these will be left blank (empty) for the calibration
  // data, since it's real data.

  /// @brief The Vector of PV x positions.
  vector<Double_t> m_primaryVertex_x;

  /// @brief The Vector of PV y positions.
  vector<Double_t> m_primaryVertex_y;

  /// @brief The Vector of PV z positions.
  vector<Double_t> m_primaryVertex_z;

};//end of class definition.

#endif

