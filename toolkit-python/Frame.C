/// @file Frame.C
/// @brief Skeleton implementation of the Frame classes.

// Standard include statements.
#include <map>
#include <vector>
#include "TString.h"

#include "TROOT.h"

/// @brief Skeleton implementation of the FrameContainer class.
class FrameContainer {

 public:

  // Payload accessor methods
  //--------------------------

  /// @brief Get the frame payload.
  std::map<int,int> GetFrameXC() { return m_frameXC; }

  /// @brief Get the number of hit pixels.
  Int_t GetEntriesPad()  { return m_nEntriesPad; };

  /// @brief Get the number of hits in the frame.
  Int_t GetHitsInPad()   { return m_nHitsInPad;   };

  /// @brief Get the total ToT counts in the frame.
  Int_t GetChargeInPad() { return m_nChargeInPad; };

  Bool_t IsMCData() { return m_isMCData; };

  /// @brief Get the pixel level 1 mask.
  ///
  /// @return The level 1 pixel mask.
  inline map<int,int> const & GetLVL1() { return m_lvl1; }

  /// @brief Clear the level 1 pixel trigger map.
  inline void ClearLVL1() { m_lvl1.clear(); }

 private:

  // The payload
  //-------------
  /// @brief Map of pixel X to ToT counts.
  std::map<int, int> m_frameXC;

  /// @brief Map of the pixel triggers.
  std::map<int, int> m_lvl1;   

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

};

/// @brief Skeleton implementation of the FrameStruct class.
class FrameStruct : public FrameContainer {

 public:

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

  /// @brief Set the dataset ID.
  ///
  /// @param [in] id The dataset id to assign to the frame.
  inline void SetDataSet(TString id) { fMPXDataSetNumber = id; }

  /// @brief Get the dataset ID.
  TString GetDataSet() { return fMPXDataSetNumber; };

  /// @brief Set the frame ID.
  ///
  /// @param [in] id The frame ID to assign to the frame.
  void SetId (Int_t id) { fFrameId=id; }

  /// @brief Get the frame ID.
  Int_t GetFrameId() { return fFrameId; };

  /// @brief Get the frame occupancy.
  ///
  /// @return The frame occupancy (number of hit pixels).
  ///
  /// Note that the full version of this method calculates the
  /// occupancy from the pixel map; here we just return the value
  /// stored in the ntuple.
  inline Int_t GetOccupancy() { return m_occupancy; }

  /// @brief Get the frame occupancy (percentage).
  ///
  /// @return The frame occupancy (% of pixels hit).
  inline Double_t GetOccupancyPc() { return m_occupancyPc; }

  // Acquisition information - accessor methods
  //--------------------------------------------

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

  // Geospatial information
  //------------------------

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

  // Detector information accessor methods
  //---------------------------------------

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
  ///
  /// Note: This is modified from the canonical SetDACs method,
  /// which checks the number of DACs supplied in the vector
  /// and returns a Bool_t.
  inline void SetDACs(vector<Int_t> dacs) { fDACs = dacs; }

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

  // Source information accessor methods
  //-------------------------------------

  /// @brief Get the source ID.
  ///
  /// @return The source ID.
  inline TString GetSourceId() { return fSourceId; }

  /// @brief Set the source ID.
  ///
  /// @param [in] sid The source ID.
  inline void SetSourceId(TString sid) { fSourceId = sid; }

  inline vector<Double_t> GetSourcePos_xs() { return m_pri_x; }
  inline vector<Double_t> GetSourcePos_ys() { return m_pri_y; }
  inline vector<Double_t> GetSourcePos_zs() { return m_pri_z; }

  inline vector<Double_t> GetSource_kinEs() { return m_pri_kinE; }

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

  // Source information
  //--------------------
  /// @brief The source ID.
  TString fSourceId;

  vector<Double_t> m_pri_x;
  vector<Double_t> m_pri_y;
  vector<Double_t> m_pri_z;
  //
  vector<Double_t> m_pri_px;
  vector<Double_t> m_pri_py;
  vector<Double_t> m_pri_pz;
  //
  vector<Double_t> m_pri_kinE;
  vector<Double_t> m_pri_m;

  /// @brief Vector of the source particle PDG IDs.
  vector<Int_t> m_pri_id;

};
