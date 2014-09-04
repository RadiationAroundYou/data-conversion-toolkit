/// @file Frames.h
/// @brief Header file for the frame container classes.

#ifndef Frames_h
#define Frames_h 1

// Standard include statements.
#include <fstream>
#include <istream>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>

// ROOT include statements.
#include "TROOT.h"
#include "TObject.h"
#include "TH2.h"
#include "TVector3.h"
#include "TString.h"

// Local include statements.
#include "WriteToNtuple.h"
#include "FramesConsts.h"

using namespace std;

/// @brief Container class for the frame pixel information.
///
/// @author J. Idarraga (Principal Author - idarraga@cern.ch).
/// @author T. Whyntie (editor for CERN\@school - t.whyntie@qmul.ac.uk).
/// @date August 2006 (ed. February 2014).
class FrameContainer {

 private:

  // The payload
  //-------------
  /// @brief Map of pixel X to ToT counts.
  std::map<int, int> m_frameXC;

  /// @brief Map of the pixel triggers.
  std::map<int, int> m_lvl1;   

  // Frame counters
  //----------------

  /// @brief The number of hit pixels in the frame.
  Int_t m_nEntriesPad;

  /// @brief The number of hits in the frame.
  ///
  /// Note that the number of hits in the frame may not necessarily
  /// be the same, for example if exposure time is long enough.
  Int_t m_nHitsInPad;

  /// @brief The total Time-over-Threshold counts in the frame.
  Int_t m_nChargeInPad;

  /// @brief Is this a simulated frame?
  Bool_t m_isMCData;

  /// @brief The Monte Carlo (truth) energy (from sim. hits) [keV].
  std::map<int, double> m_frameXC_TruthE;

  /// @brief Corrected MC charge [keV].
  ///
  /// Note that detector effects included at the digitization step.
  std::map<int, double> m_frameXC_E;

 public:

  /// @brief Constructor.
  FrameContainer();

  /// @brief Destructor.
  virtual ~FrameContainer() {};

  /// @brief Fill a pixel. 
  ///
  /// @param [in] col The pixel x (column) [pixels].
  /// @param [in] row The pixel y (row) [pixels].
  /// @param [in] width The frame width [pixels].
  /// @param [in] counts The number of ToT counts recorded.
  /// @param [in] truthE The Monte Carlo energy in the pixel [keV].
  /// @param [in] E The energy measured by the pixel [keV].
  void FillOneElement(
  Int_t col,
  Int_t row,
  Int_t width,
  Int_t counts,
  Double_t truthE,
  Double_t E);

  /// @brief Adds a pixel entry to the frame.
  ///
  /// @param [in] x The pixel x coordinate.
  /// @param [in] y The pixel y coordinate.
  /// @param [in] w The width of the frame.
  /// @param [in] C The counts recorded by the pixel.
  void FillOneElement(Int_t x, Int_t y, Int_t w, Int_t C);

  /// @brief Adds a pixel entry to the frame.
  ///
  /// @param [in] X The combined (x,y) pixel coordinate, X = x+wy.
  /// @param [in] C The counts recorded by the pixel.
  void FillOneElement(Int_t X, Int_t C);

  /// @brief Set the pixel level 1 trigger.
  ///
  /// @param [in] x The pixel x coordinate.
  /// @param [in] y The pixel y coordinate.
  /// @param [in] w The width of the frame.
  /// @param [in] lvl1 The pixel level 1 trigger.
  void SetLVL1(Int_t x, Int_t y, Int_t w, Int_t lvl1);

  /// @brief Clear the level 1 pixel trigger map.
  inline void ClearLVL1() { m_lvl1.clear(); }

  /// @brief Get the pixel level 1 mask.
  ///
  /// @return The level 1 pixel mask.
  inline map<int,int> const & GetLVL1() { return m_lvl1; }

  /// @brief Get the pixel ToT counts map.
  ///
  /// @return The map of pixel X to ToT counts.
  inline map<int,int> const & GetPixelCounts() { return m_frameXC; }

  /// @brief Get the pixel energies map.
  ///
  /// @return The map of pixel X to pixel energy E (keV).
  inline map<int,double> const & GetPixelEnergies() { return m_frameXC_E; }

  /// @brief Reset the frame's pixel counters.
  void ResetCountersPad();

  /// @brief Empty the pixel data from the frame.
  void CleanUpMatrix();

  /// @brief Set the frame as a simulated frame (Monte Carlo, MC).
  void SetFrameAsMCData() { m_isMCData = true;  };

  /// @brief Set the frame as real data.
  void SetFrameAsData()   { m_isMCData = false; };

  /// @brief Get the number of hit pixels.
  Int_t GetEntriesPad()  { return m_nEntriesPad; };

  /// @brief Get the number of hits in the frame.
  Int_t GetHitsInPad()   { return m_nHitsInPad;   };

  /// @brief Get the total ToT counts in the frame.
  Int_t GetChargeInPad() { return m_nChargeInPad; };

  // Class definition macro for the ROOT dictionary.
  ClassDef(FrameContainer,3)

};//end of FrameContainer class definition.


/// @brief Container class for the frame data and metadata.
///
/// @author J. Idarraga (principal author - idarraga@cern.ch)
/// @author T. Whyntie (editor for CERN\@school - t.whyntie@qmul.ac.uk)
/// @date August 2006 (ed. January 2014)
///
/// This class contains the metadata and the frame data (payload).
/// It inherits the methods to populate the payload from the
/// FrameContainer class..
class FrameStruct : public FrameContainer {

 public:

  /// @brief Constructor.
  ///
  /// @param [in] datasetid The dataset ID.
  FrameStruct(TString datasetid);

  inline FrameStruct() {};

  /// @brief Destructor.
  ~FrameStruct() {};

  /// @brief Populate a metadata item in the frame.
  ///
  /// @param [in] METAString String extracted from the DSC file.
  /// @param [in] metaCode The code of the metadata item.
  ///
  /// See FramesConsts.h for the metadata codes.
  void FillMetaData(TString METAString, Int_t metaCode);

  /// @brief Increases the frame ID.
  void IncreaseId() { fFrameId++; }

  /// @brief Reset the frame metadata values.
  void RewindMetaDataValues();

  // Payload information access methods
  //------------------------------------

  /// @brief Set the frame width.
  void SetnX(int x) {fWidth  = x;};

  /// @brief Get the frame width.
  Int_t const & GetFrameWidth()    { return fWidth;  }

  /// @brief Set the frame height.
  void SetnY(int y) {fHeight = y;};

  /// @brief Get the frame height.
  Int_t const & GetFrameHeight()   { return fHeight; }

  /// @brief Set the payload format.
  ///
  /// @param [in] format The payload format code.
  ///
  /// 0: [X, C] (default).
  void SetPayloadFormat(Int_t format) { fFormat = format; }

  /// @brief Get the payload format.
  Int_t GetPayloadFormat() { return fFormat; }

  /// @brief Set the frame ID.
  ///
  /// @param [in] id The frame ID to assign to the frame.
  void SetId (Int_t id) { fFrameId=id; }

  /// @brief Get the frame ID.
  Int_t GetFrameId() { return fFrameId; };

  /// @brief Set the dataset ID.
  ///
  /// @param [in] id The dataset id to assign to the frame.
  inline void SetDataSet(TString id) { fMPXDataSetNumber = id; }

  /// @brief Get the dataset ID.
  TString GetDataSet() { return fMPXDataSetNumber; };

  /// @brief Updates the frame occupancy from the hit pixel information.
  void UpdateOccupancy();

  /// @brief Get the frame occupancy.
  ///
  /// @return The frame occupancy (number of hit pixels).
  Int_t GetOccupancy(); 

  /// @brief Get the frame occupancy (percentage).
  ///
  /// @return The frame occupancy (% of pixels hit).
  Double_t GetOccupancyPc(); 

  /// @brief Calculate the dose and equivalent dose rates.
  ///
  /// @todo Implement the dose rate calculations.
  void CalculateDoseRates();


  // Acquisition information - access methods
  //------------------------------------------

  /// @brief Get the acquisition mode (code).
  ///
  /// @return The acquisition mode (integer code).
  inline Int_t GetAcqMode() { return fAcq_mode; }

  /// @brief Set the acquisition mode (integer code).
  ///
  /// @param [in] acqm The acquisition mode (integer code).
  inline void SetAcqMode(Int_t acqm) { fAcq_mode = acqm; }

  /// @brief Get the acquisition counters.
  ///
  /// @return A vector of the acquisition counters.
  inline vector<Int_t> GetCounters() { return fCounters; }

  /// @brief Set the acquisition counters.
  ///
  /// @param [in] cs The acquisition counters.
  inline void SetCounters(vector<Int_t> cs) { fCounters = cs; }

  /// @brief Get the hardware timer mode..
  ///
  /// @return The hardware timer mode.
  inline Int_t GetHwTimerMode() { return fHw_timer; }

  /// @brief Set the hardware timer mode.
  ///
  /// @param [in] hwtm The hardware timer mode.
  inline void SetHwTimerMode(Int_t hwtm) { fHw_timer = hwtm; }

  /// @brief Get the auto-erase interval [s].
  ///
  /// @return The auto-erase interval.
  inline Double_t GetAutoEraseInterval() { return fAuto_erase_interval; }

  /// @brief Set the auto-erase interval.
  ///
  /// @param [in] aei The auto-erase interval [s].
  inline void SetAutoEraseInterval(Double_t aei) {fAuto_erase_interval = aei;}

  /// @brief Get the auto-erase interval counter.
  ///
  /// @return The auto-erase interval counter.
  inline Int_t GetAutoEraseIntervalCounter()
    { return fAuto_erase_interval_counter; }

  /// @brief Set the auto-erase interval counter.
  ///
  /// @param [in] aeic The auto-erase interval counter.
  inline void SetAutoEraseIntervalCounter(Int_t aeic)
    { fAuto_erase_interval_counter = aeic; }

  /// @brief Get the time since the last trigger [s].
  ///
  /// @return The time since the last trigger [s].
  inline Double_t GetLastTriggerTime() { return fTrigger_time; }

  /// @brief Set the time since the last trigger.
  ///
  /// @param [in] ltt The time since the last trigger [s].
  inline void SetLastTriggerTime(Double_t ltt) { fTrigger_time = ltt; }

  /// @brief Get the coincidence mode.
  ///
  /// @return The coincidence mode code.
  inline UChar_t GetCoincidenceMode() { return fCoincidence_mode; }

  /// @brief Set the coincidence mode.
  ///
  /// @param [in] cm The coincidence mode byte code.
  inline void SetCoincidenceMode(UChar_t cm) { fCoincidence_mode = cm; }

  /// @brief Get the coincidence delay.
  ///
  /// @return The coincidence delay time [64us].
  inline UChar_t GetCoincidenceDelayTime() { return fCoincidence_delay; }

  /// @brief Set the coincidence delay time..
  ///
  /// @param [in] cdt The coincidence delay time [64us].
  inline void SetCoincidenceDelayTime(UChar_t cdt) { fCoincidence_delay = cdt; }

  /// @brief Get the coincidence live time.
  ///
  /// @return The coincidence live time [s].
  inline Double_t GetCoincidenceLiveTime() { return fCoinc_live_time; }

  /// @brief Set the coincidence live time.
  ///
  /// @param [in] clt The coincidence live time [s].
  inline void SetCoincidenceLiveTime(Double_t clt) { fCoinc_live_time = clt; }

  /// @brief Get the Pixelman version.
  ///
  /// @return The Pixelman version.
  inline TString GetPixelmanVersion() { return fPixelman_version; }

  /// @brief Set the Pixelman version.
  ///
  /// @param [in] pxversion The Pixelman version.
  inline void SetPixelmanVersion(TString pxversion) { fPixelman_version = pxversion; }

  // Geospatial information - accessor methods
  //-------------------------------------------

  /// @brief Get the latitude.
  ///
  /// @return The latitude [deg.].
  inline Double_t GetLatitude() { return fLatitude; }

  /// @brief Set the latitude.
  ///
  /// @param [in] lat The latitude [deg.].
  inline void SetLatitude(Double_t lat) { fLatitude = lat; }

  /// @brief Get the longitude.
  ///
  /// @return The longitude [deg.].
  inline Double_t GetLongitude() { return fLongitude; }

  /// @brief Set the longitude.
  ///
  /// @param [in] lon The longitude [deg.].
  inline void SetLongitude(Double_t lon) { fLongitude = lon; }

  /// @brief Get the altitude.
  ///
  /// @return The altitude [km].
  inline Double_t GetAltitude() { return fAltitude; }

  /// @brief Set the altitude.
  ///
  /// @param [in] alt The altitude [km].
  inline void SetAltitude(Double_t alt) { fAltitude = alt; }

  /// @brief Get the lab frame rotational velocity Omega_x.
  ///
  /// @return The rotational velocity of the lab frame Omega_x [deg./s].
  inline Double_t GetOmega_x() { return m_omega_x; }

  /// @brief Set the lab frame rotational velocity Omega_x [deg./s].
  ///
  /// @param [in] omega_x The x rotational velocity of the lab frame [deg./s].
  inline void SetOmega_x(Double_t omega_x) { m_omega_x = omega_x; }

  /// @brief Get the lab frame rotational velocity Omega_y.
  ///
  /// @return The rotational velocity of the lab frame Omega_y [deg./s].
  inline Double_t GetOmega_y() { return m_omega_y; }

  /// @brief Set the lab frame rotational velocity Omega_y [deg./s].
  ///
  /// @param [in] omega_y The y rotational velocity of the lab frame [deg./s].
  inline void SetOmega_y(Double_t omega_y) { m_omega_y = omega_y; }

  /// @brief Get the lab frame rotational velocity Omega_z.
  ///
  /// @return The rotational velocity of the lab frame Omega_z [deg./s].
  inline Double_t GetOmega_z() { return m_omega_z; }

  /// @brief Set the lab frame rotational velocity Omega_z [deg./s].
  ///
  /// @param [in] omega_z The z rotational velocity of the lab frame [deg./s].
  inline void SetOmega_z(Double_t omega_z) { m_omega_z = omega_z; }

  /// @brief Get the lab frame roll.
  ///
  /// @return The roll angle of the laboratory frame [deg.].
  inline Double_t GetRoll() { return m_roll_orf; }

  /// @brief Set the lab frame roll.
  ///
  /// @param [in] roll The roll angle of the laboratory frame [deg.].
  inline void SetRoll(Double_t roll) { m_roll_orf = roll; }

  /// @brief Get the lab frame pitch angle.
  ///
  /// @return The pitch angle of the laboratory frame [deg.].
  inline Double_t GetPitch() { return m_pitch_orf; }

  /// @brief Set the pitch angle of the laboratory frame.
  ///
  /// @param [in] pitch The pitch angle of the laboratory frame [deg.].
  inline void SetPitch(Double_t pitch) { m_pitch_orf = pitch; }

  /// @brief Get the yaw angle of the laboratory frame.
  ///
  /// @return The yaw angle of the laboratory frame.
  inline Double_t GetYaw() { return m_yaw_orf; }

  /// @brief Set the yaw angle of the laboratory frame.
  ///
  /// @param [in] yaw The yaw angle of the laboratory frame [deg.].
  inline void SetYaw(Double_t yaw) { m_yaw_orf = yaw; }


  // Temporal information - accessor methods
  //-----------------------------------------

  /// @brief Get the frame start time [s].
  ///
  /// @return The frame start time [s].
  inline Double_t GetStartTime() { return fStart_time; }

  /// @brief Set the frame start time [s].
  ///
  /// @param [in] starttime The frame start time.
  inline void SetStartTime(Double_t starttime) { fStart_time = starttime; }

  /// @brief Get the frame start time (string, Pixelman format).
  ///
  /// @return The frame start time string in Pixelman format.
  inline TString GetStartTimeS() { return fStart_timeS; }

  /// @brief Set the frame start time (string).
  ///
  /// @param [in] sts The frame start time string in Pixelman format.
  inline void SetStartTimeS(string sts) { fStart_timeS = sts; }

  /// @brief Get the acquisition time [s].
  ///
  /// @return The acquisition time [s].
  inline Double_t GetAcqTime() { return fAcq_time; }

  /// @brief Set the acquisition time [s].
  ///
  /// @param [in] acqt The acquisition time [s].
  inline void SetAcqTime(Double_t acqt) { fAcq_time = acqt; }


  // Detector settings - accessor methods
  //--------------------------------------

  /// @brief Get the detector polarity.
  ///
  /// @return The polarity (0 = negative, 1 = postive).
  inline Int_t GetPolarity() { return fPolarity; }

  /// @brief Set the detector polarity.
  ///
  /// @param [in] polarity The polarity (0 = negative, 1 = positive).
  inline void SetPolarity(Int_t polarity) { fPolarity = polarity; }

  /// @brief Get the bias voltage (HV).
  ///
  /// @return The bias voltage (HV) [V].
  inline Double_t GetHV() { return fHV; }

  /// @brief Set the bias voltage (HV).
  ///
  /// @param [in] hv The bias voltage (HV) [V].
  inline void SetHV(Double_t hv) { fHV = hv; }

  /// @brief Get the device DACs.
  ///
  /// @return A vector of the 14 device DACs.
  inline vector<Int_t> GetDACs() { return fDACs; }

  /// @brief Set the device DACS.
  ///
  /// @param [in] dacs A vector of the device DACs.
  /// @todo Implement methods with specific DAC values.
  Bool_t SetDACs(vector<Int_t> dacs);

  /// @brief Get the Medipix clock frequency.
  ///
  /// @return The Medipix clock frequency [MHz].
  inline Double_t GetMpxClock() { return fMpx_clock; }

  /// @brief Set the Medipix clock frequency.
  ///
  /// @param [in] mpxclock The Medipix clock frequency [MHz].
  inline void SetMpxClock(Double_t mpxclock) {fMpx_clock = mpxclock; }

  /// @brief Get the Timepix clock frequency setting.
  ///
  /// @return The Timepix clock frequency setting (1-4).
  ///
  /// * 0: 10 MHz;
  /// * 1: 20 MHz;
  /// * 2: 40 MHz;
  /// * 3: 80 MHz.
  inline Double_t GetTpxClock() { return fTimepix_clock; }
  //inline Byte_t GetTpxClock() { return fTimepix_clock; }

  /// @brief Set the Timepix clock frequency byte code.
  ///
  /// @param [in] tpxclock The Timepix clock frequency byte code.
  inline void SetTpxClock(Double_t tpxclock) { fTimepix_clock = tpxclock; }
  //inline void SetTpxClock(Byte_t tpxclock) { fTimepix_clock = tpxclock; }

  /// @brief Get the back side preamp. enabled flag value.
  ///
  /// @return Is the back side preamp. enabled?.
  inline Bool_t GetBsActive() { return fBS_active; }

  /// @brief Set the back side preamp. enabled flag.
  ///
  /// @param [in] bsactive Is the back side preamp. flag enabled?
  inline void SetBsActive(Bool_t bsactive) { fBS_active = bsactive; }


  // Detector information - accessor methods
  //-----------------------------------------

  /// @brief Get the chipboard ID.
  ///
  /// @return The chip board ID.
  inline TString GetChipboardID() { return fChipboardID; }

  /// @brief Set the chipboard ID.
  ///
  /// @param [in] cbid The chipboard ID.
  inline void SetChipboardID(TString cbid) { fChipboardID = cbid; }

  /// @brief Get the detector's custom name.
  ///
  /// @return The the detector's custom name.
  inline TString GetCustomName() { return fCustom_name; }

  /// @brief Set the detector's custom name.
  ///
  /// @param [in] cname The detector's custom name.
  inline void SetCustomName(TString cname) { fCustom_name = cname; }

  /// @brief Get the firmware version.
  ///
  /// @return The firmware version.
  inline TString GetFirmware() { return fFirmware; }

  /// @brief Set the firmware version.
  ///
  /// @param [in] fwversion The firmware version.
  inline void SetFirmware(TString fwversion) { fFirmware = fwversion; }

  /// @brief Get the Medipix interface type.
  ///
  /// @return The Medipix interfac type.
  inline TString GetInterface() { return fInterface; }

  /// @brief Set the Medipix interface type.
  ///
  /// @param [in] interface The Medipix interface type.
  inline void SetInterface(TString interface) { fInterface = interface; }

  /// @brief Get the Medipix detector type.
  ///
  /// @return The Medipix detector type.
  inline Int_t GetMpxType() { return fMpx_type; }

  /// @brief Set the Medipix detector type.
  ///
  /// @param [in] mpxtype The Medipix detector type.
  ///
  /// * 1: Medipix 2.1
  /// * 2: MXR
  /// * 3: Timepix 
  ///
  inline void SetMpxType(Int_t mpxtype) { fMpx_type = mpxtype; }

  /// @brief Get the filters applied to the readout.
  ///
  /// @return The filters applied.
  inline TString GetAppFilters() { return fApplied_filters; }

  /// @brief Set the filters applied to the readout.
  ///
  /// @param [in] af The applied filters.
  inline void SetAppFilters(TString af) { fApplied_filters = af; }

  /// @brief Get the detector x coordinate (lab frame).
  ///
  /// @return The detector x coordinate (lab frame) [mm].
  inline Double_t GetDet_x() { return m_x; }

  /// @brief Set the detector x coordinate (lab frame) [mm].
  ///
  /// @param [in] x The detector x coordinate (lab frame) [mm].
  inline void SetDet_x(Double_t x) { m_x = x; }

  /// @brief Get the detector y coordinate (lab frame).
  ///
  /// @return The detector y coordinate (lab frame) [mm].
  inline Double_t GetDet_y() { return m_y; }

  /// @brief Set the detector z coordinate (lab frame) [mm].
  ///
  /// @param [in] y The detector y coordinate (lab frame) [mm].
  inline void SetDet_y(Double_t y) { m_y = y; }

  /// @brief Get the detector z coordinate (lab frame).
  ///
  /// @return The detector z coordinate (lab frame) [mm].
  inline Double_t GetDet_z() { return m_z; }

  /// @brief Set the detector z coordinate (lab frame) [mm].
  ///
  /// @param [in] z The detector z coordinate (lab frame) [mm].
  inline void SetDet_z(Double_t z) { m_z = z; }

  /// @brief Get the detector rotation angle alpha.
  ///
  /// @return The detector rotation angle alpha [deg.].
  inline Double_t GetEulerA() { return m_euler_a; }

  /// @brief Set the detector rotation angle alpha [deg.].
  ///
  /// @param [in] alpha The detector rotation angle alpha [deg.].
  inline void SetEulerA(Double_t alpha) { m_euler_a = alpha; }

  /// @brief Get the detector rotation angle beta.
  ///
  /// @return The detector rotation angle beta [deg.].
  inline Double_t GetEulerB() { return m_euler_b; }

  /// @brief Set the detector rotation angle beta [deg.].
  ///
  /// @param [in] beta The detector rotation angle beta [deg.].
  inline void SetEulerB(Double_t beta) { m_euler_b = beta; }

  /// @brief Get the detector rotation angle gamma.
  ///
  /// @return The detector rotation angle gamma [deg.].
  inline Double_t GetEulerC() { return m_euler_c; }

  /// @brief Set the detector rotation angle gamma [deg.].
  ///
  /// @param [in] gamma The detector rotation angle gamma [deg.].
  inline void SetEulerC(Double_t gamma) { m_euler_c = gamma; }


  // Source information
  //--------------------

  /// @brief Get the source ID.
  ///
  /// @return The source ID.
  inline TString GetSourceId() { return fSourceId; }

  /// @brief Set the source ID.
  ///
  /// @param [in] sid The source ID.
  inline void SetSourceId(TString sid) { fSourceId = sid; }

  /// @brief Get the x coordinates of the primary vertices [mm].
  ///
  /// @return A vector of the primary vertex x coordinates [mm].
  inline vector<Double_t> GetPVxs() { return m_primaryVertex_x; }

  /// @brief Get the y coordinates of the primary vertices [mm].
  ///
  /// @return A vector of the primary vertex y coordinates [mm].
  inline vector<Double_t> GetPVys() { return m_primaryVertex_y; }

  /// @brief Get the z coordinates of the primary vertices [mm].
  ///
  /// @return A vector of the primary vertex z coordinates [mm].
  inline vector<Double_t> GetPVzs() { return m_primaryVertex_z; }

  /// @brief Add a primary vertex to the frame (MC).
  ///
  /// @param [in] x The primary vertex x coordinate [mm].
  /// @param [in] y The primary vertex y coordinate [mm].
  /// @param [in] z The primary vertex z coordinate [mm].
  void SetPrimaryVertex(Double_t x, Double_t y, Double_t z);


 private:

  // Payload information
  //---------------------

  /// @brief The frame width.
  Int_t fWidth;

  /// @brief The frame height.
  Int_t fHeight;

  /// @brief The payload format.
  Int_t fFormat;

  /// @brief The frame ID.
  Long_t fFrameId;

  /// @brief The dataset ID. 
  TString fMPXDataSetNumber;

  /// @brief The frame occupancy (number of hit pixels).
  Int_t m_occupancy;

  /// @brief The frame occupancy as a percentage of the total pixels.
  Double_t m_occupancyPc;

  /// @brief The dose rate of the frame (uGy/min.).
  Double_t m_DoseRate;

  /// @brief The equivalent dose rate of the frame (uSv/min.).
  Double_t m_DoseEquivalentRate;

  // Acquisition information
  //-------------------------
  
  /// @brief The acquisition mode.
  Int_t fAcq_mode;
  
  /// @brief The acquisition counters.
  std::vector<Int_t> fCounters;

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
  //
  /// @brief The start time [s].
  Double_t fStart_time;

  /// @brief The start time (string, Pixelman format).
  TString fStart_timeS;

  /// @brief The acquisition time [s].
  Double_t fAcq_time;

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
  Double_t fTimepix_clock;
  //Byte_t fTimepix_clock;

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


  // Source information
  //--------------------
  /// @brief The source ID.
  TString fSourceId;

  /// @brief Vector of primary vertex x coordinates [mm].
  vector<Double_t> m_primaryVertex_x;
  
  /// @brief Vector of primary vertex y coordinates [mm].
  vector<Double_t> m_primaryVertex_y;

  /// @brief Vector of primary vertex z coordinates [mm].
  vector<Double_t> m_primaryVertex_z;


  // Macro for the ROOT dictionary.
  ClassDef(FrameStruct,4)

};//end of the FrameStruct class definition.


// Forward declaration.
class WriteToNtuple;

/// @brief A class for handling frame information.
///
/// @author J. Idarraga (principal author - idarraga@cern.ch).
/// @author T. Whyntie (editor for CERN\@school - t.whyntie@qmul.ac.uk).
/// @date August 2006 (ed. February 2014).
class FramesHandler {

 private:

  /// @brief The number of frames.
  Int_t m_nFrames;

  /// @brief The frame container.
  FrameStruct * m_aFrame;

  /// @brief The frame width [pixels].
  ///
  /// This information is replicated for speed purposes.
  Int_t m_width; 
 
  /// @brief The frame width [pixels].
  ///
  /// This information is replicated for speed purposes.
  Int_t m_height;

  /// @brief Should a frame matrix be returned?
  Bool_t getAFrameMatrix_flag;

  /// @brief Should a frame histogram be returned?
  Bool_t getAFrameHist_flag;

  /// @brief Bit used in parsing the metadata.
  Int_t m_metaBit;

  /// @brief Code used to parse the metadata.
  Int_t m_metaCode;

  /// @brief Flag used in parsing the metadata.
  Bool_t m_ParseAndHold;

  /// @brief The number of frames in matrix format.
  Int_t nFrames256x256;
 
  /// @brief The number of frames in [X, Y, C] format.
  Int_t nFramesXYC;

  /// @brief The detector ID (Allpix).
  ///
  /// Detector ID - in case there is more than one detector in the
  /// Allpix simulation.
  Int_t m_detID;

 public:

  /// @brief Constructor.
  ///
  /// @param [in] dataset The path to the folder containing the dataset files.
  FramesHandler(TString dataset);

  /// @brief Destructor.
  ~FramesHandler();

  // DEPRECATED
  //Int_t IdentifyTypeOfInput(TString, Int_t &, Int_t &, int &);

  /// @brief Identify the payload format from the DSC file.
  ///
  /// @param [in] fullDSCFileName The path of the DSC file.
  /// @param [out] width The frame width [pixels]
  /// @param [out] height The frame height [pixels]. 
  /// @param [out] nFrames The number of frames.
  /// @return The type (format) of the payload. 0 is an error.
  Int_t IdentifyTypeOfInput2(
    TString fullDSCFileName,
    Int_t & width,
    Int_t & height,
    int & nFrames);

  // Frame filling methods
  //-----------------------

  /// @brief Read in and fill a single frame of data.
  ///
  /// @param [in] fullFileName File name of the payload data file.
  /// @param [in] fullDSCFileName File name of the DSC file.
  /// @param [out] ftype Type of frame file (XYC or matrix).
  /// @return The number of frames to read: != 1 is an error.
  int readOneFrame(
    TString fullFileName,
    TString fullDSCFileName,
    int * ftype);

  /// @brief Read in and fill multiple frames of data.
  ///
  /// @param [in] datafile The path of the payload file.
  /// @param [in] dscfile The path of the DSC file.
  /// @param [in] idxfile The path of the index file.
  /// @param [out] wte Pointer to the ntuple file container.
  /// @param [in] ftype The payload format (type) code.
  /// @return Did the frame processing work?
  Bool_t ProcessMultiframe(
    TString datafile,
    TString dscfile,
    TString idxfile,
    WriteToNtuple * wte,
    int ftype);

  /// @brief Fill a single pixel in the frame container (incl. energy).
  ///
  /// @param [in] col Pixel column (x).
  /// @param [in] row Pixel row (y).
  /// @param [in] counts The ToT counts recorded by the pixel.
  /// @param [in] truthE The Monte Carlo (truth) energy [keV].
  /// @param [in] E The energy measured by the pixel [keV].
  /// @return Was the pixel setting successful?
  Bool_t LoadFramePixel(Int_t col, Int_t row, Int_t counts,
                        Double_t truthE, Double_t E);

  /// @brief Fill a single pixel in the frame container.
  ///
  /// @param [in] col Pixel column (x).
  /// @param [in] row Pixel row (y).
  /// @param [in] counts The ToT counts recorded by the pixel.
  /// @return Was the pixel setting successful?
  Bool_t LoadFramePixel(Int_t col, Int_t row, Int_t counts);

  /// @brief Set a level 1 trigger.
  ///
  /// @param [in] col Pixel column (x).
  /// @param [in] row Pixel row (y).
  /// @param [in] lvl1 Trigger value.
  void SetLVL1(Int_t, Int_t, Int_t);

  /// @brief Load a single frame's metadata entry .
  ///
  /// @param [in] METAString
  /// @param [in] metaCode
  Bool_t LoadFrameMetaData(TString METAString, Int_t metaCode);

  /// @brief Add a primary vertex to the frame.
  ///
  /// @param [in] vx The primary vertex x coordinate [mm].
  /// @param [in] vy The primary vertex y coordinate [mm].
  /// @param [in] vz The primary vertex z coordinate [mm].
  inline Bool_t LoadPrimaryVertexInfo(Double_t vx, Double_t vy, Double_t vz) {
    m_aFrame->SetPrimaryVertex(vx, vy, vz);
    return true;
  };

  /// @brief Increase the current frame ID.
  void IncreaseCurrentFrameId();
  
  /// @brief Set the current frame ID.
  inline void SetCurrentFrameId(Int_t id) { m_aFrame->SetId(id); };

  /// @brief Register the frame as Monte Carlo data.
  void SetAsMCData() { m_aFrame->SetFrameAsMCData(); };
 
  /// @brief Set the frame width.
  ///
  /// @param [in] x The frame width [pixels].
  void SetnX(int x) {m_width = x; m_aFrame->SetnX(x); };

  /// @brief Set the frame height.
  ///
  /// @param [in] y The frame height [pixels].
  void SetnY(int y) { m_height = y; m_aFrame->SetnY(y); };

  /// @brief Push a value back n bytes (32 bit).
  ///
  /// @param [in] val The value to push back.
  /// @param [in] bytes The bytes to push back.
  /// @param [in] nbytes The number of bytes to push back.
  void push_back_nbytes(unsigned int * val, char * bytes, int nbytes);

  /// @brief Push a value back n bytes (64 bits).
  ///
  /// @param [in] val The value to push back.
  /// @param [in] bytes The bytes to push back.
  /// @param [in] nbytes The number of bytes to push back.
  void push_back_nbytes(long long * val, char * bytes, int nbytes);
  
  /// @brief Get the data file index values.
  ///
  /// @param [in] filestr_idx The index file stream.
  /// @param [in] dscPos The position in the DSC file.
  /// @param [in] dataPos The position in the data file.
  /// @param [in] sfPos
  void GetIdxValues(
    fstream   * filestr_idx,
    long long * dscPos,
    long long * dataPos,
    long long * sfPos
  );

  /// @brief Convert a pixel (x, y) coordinate to a pixel X coordinate.
  ///
  /// @param [in] x The pixel x.
  /// @param [in] y The pixel y.
  /// @param [in] dimX The width of the frame [pixels].
  Int_t XYtoX(Int_t x, Int_t y, Int_t dimX);

  /// @brief Get the detector ID.
  ///
  /// @return The detector ID (used in Allpix).
  Int_t GetDetectorId() { return m_detID; };

  /// @brief Set the detector ID.
  ///
  /// Int_t [in] id The detector ID.
  void  SetDetectorId(Int_t id) { m_detID = id; };

  /// @brief Reset the frame data and metadata.
  ///
  /// @param [in] rewind_metadata Reset the metadata too?
  void RewindAll(bool rewind_metadata = true);

  //TH2I * getAFrameHist(TString, TString, TString);

  /// @brief Get a frame matrix.
  ///
  /// @param [in] fullFileName The frame payload file name.
  /// @param [in] fullDSCFileName The frame DSC file name.
  Int_t ** getAFrameMatrix(TString fullFileName, TString fullDSCFileName);

  //TH2I * getHistFrame(Int_t, Int_t *);

  /// @brief Get a pointer to the FrameStruct object.
  ///
  /// @return The pointer to the FrameStruct object.
  FrameStruct * getFrameStructObject() { return m_aFrame; };

  /// @brief Has the matrix for one frame been completely loaded?
  ///
  /// @return Has the matrix for one frame been completely loaded?
  Bool_t frameSupervisor();

  /// @brief Parse a line of the frame metadata from the DSC file.
  ///
  /// @param [in] aMetaLine The line of metadata.
  /// @param [in] currentLine The current line of the DSC file.
  void parseMetaLine(TString aMetaLine, Int_t currentLine);

  //void push_back_nbytes(unsigned int *, char *, Int_t);

};//end of FramesHandler class definition.

#endif
