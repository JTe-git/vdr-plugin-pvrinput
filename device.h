#ifndef _PVR_DEVICE_H_
#define _PVR_DEVICE_H_

#include "common.h"

typedef enum { /* enumeration with encoder states */
  eStop,
  eStart,
} eEncState;

typedef enum {
  eTelevision,
  eRadio,
  eSVideo0,
  eSVideo1,
  eSVideo2,
  eSVideo3,
  eComposite0,
  eComposite1,
  eComposite2,
  eComposite3,
  eComposite4,
  eComponent,
  eExternalInput
} eVideoInputs;

typedef enum {
  undef,
  ivtv,
  cx18,
  pvrusb2,
  cx88_blackbird,
  hdpvr,
} eV4l2Driver;

typedef enum {
  UNDEF,
  PVR150,
  PVR250,
  PVR350,
  PVR500_1,
  PVR500_2,
  HVR1300,
  HVR1600,
  HVR1900,
  HVR1950,
  PVRUSB2,
  HDPVR,
} eV4l2CardName;

class cPvrReadThread;

class cPvrDevice :
public cDevice
{
friend class cPvrReadThread;
private:
  static bool Probe(int DeviceNumber);

  static cString externChannelSwitchScript;

public:
  static bool Initialize(void);
  static void Stop(void);
  static void ReInitAll(void);
  static int Count();
  static cPvrDevice * Get(int index);

private:
  int number;
  int v4l2_fd;
  int mpeg_fd;
  int radio_fd;
  int v4l2_dev;
  int mpeg_dev;
  int radio_dev;
  int inputs[12];
  int numInputs;
  int frequency;
  int vpid;
  int apid;
  int tpid;
  uint64_t CurrentNorm;
  int CurrentLinesPerFrame;
  int CurrentInput;
  int newFrequency;
  int newInput;
  uint64_t newNorm;
  int newLinesPerFrame;
  eVideoInputs newEncoderInput;
  cString BusID;
  eEncState    EncoderState;
  eVideoInputs EncoderInput;
  int driver_apiversion;
  bool SupportsSlicedVBI;
  bool hasDecoder;
  bool hasTuner;
  int  streamType;
  bool dvrOpen;
  bool delivered;
  bool isClosing;
  bool readThreadRunning;
  bool ChannelSettingsDone;
  bool FirstChannelSwitch;
  bool pvrusb2_ready;
  eV4l2Driver driver;
  eV4l2CardName cardname;
  cRingBufferLinear * tsBuffer;
  int tsBufferPrefill;
  cPvrReadThread    * readThread;
  cChannel currentChannel;
  cPvrSectionHandler  sectionHandler;

protected:
  virtual bool SetChannelDevice(const cChannel *Channel, bool LiveView);
  virtual bool SetPid(cPidHandle *Handle, int Type, bool On);
  virtual int  OpenFilter(u_short Pid, u_char Tid, u_char Mask);
#if VDRVERSNUM >= 10600
  virtual void CloseFilter(int Handle);
#endif
  virtual bool OpenDvr(void);
  virtual void CloseDvr(void);
  void         ResetBuffering();
  bool         IsBuffering();
  virtual bool GetTSPacket(uchar *&Data);
public:
  cPvrDevice(int DeviceNumber);
  virtual ~cPvrDevice(void);
  virtual bool ProvidesSource(int Source) const;
  virtual bool ProvidesTransponder(const cChannel *Channel) const;
  virtual bool ProvidesChannel(const cChannel *Channel, int Priority = -1,
                               bool *NeedsDetachReceivers = NULL) const;
  virtual int NumProvidedSystems(void) const;
  bool ParseChannel(const cChannel *Channel, int *input, uint64_t *norm, int *LinesPerFrame, int *card,
                    eVideoInputs *encoderInput, int *apid, int *vpid, int *tpid) const;
  void ReInit(void);
  void StopReadThread(void);
  void GetStandard(void);
  void TurnOffSlicedVBI(void);
  bool Tune(eVideoInputs encoderInput, int frequency);
  bool SetInput(int input);
  bool SetAudioInput(int input);
  bool SetVideoNorm(uint64_t norm);
  bool SetVideoSize(int width, int height);
  void SetTunerAudioMode(int tuneraudiomode);
  void SetAudioVolumeTV(void);
  void SetEncoderState(eEncState state);
  bool SetVBImode(int vbiLinesPerFrame, int vbistatus);
  bool ControlIdIsValid(__u32 ctrlid);
  // overloaded function SetControlValue
  int  SetControlValue(__u32 control, __s32 Val);
  int  SetControlValue(struct valSet * vs, __s32 Val);
  int  SetControlValue(__u32 control_class, __u32 control,
                       __s32 Val, struct v4l2_queryctrl queryctrl);
  int  QueryControl(struct valSet * vs);
  bool QueryAllControls(void);
  int  PutData(const unsigned char * Data, int Count);
};

#endif