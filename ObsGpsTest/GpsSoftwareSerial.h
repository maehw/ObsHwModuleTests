#ifndef GpsSoftwareSerial_h
#define GpsSoftwareSerial_h

#include <SoftwareSerial.h>


class GpsSoftwareSerial : SoftwareSerial {
  public:
    static const unsigned int RX_STARTUP_MEM_LEN = 64;

    enum GpsRxState {
      IDLE,
      UBX_CANDIDATE, // 1st sync byte rx'ed (0xB5)
      UBX_MAGIC_MATCH, // 2nd sync byte rx'ed (0x62)
      NMEA_CANDIDATE_1, // 1st byte is '$' (start of sequence)
      NMEA_CANDIDATE_2, // 2nd byte is 'G'
      NMEA_MAGIC_MATCH // 3rd byte is 'N' or 'P' (i.e. received "$GP"/"$GN")
    };

    GpsSoftwareSerial(uint8_t receivePin, uint8_t transmitPin);
    ~GpsSoftwareSerial();
    void begin(long speed);
    int read();
    int available();
    size_t write(uint8_t b);

    bool hasSeenUbx();
    bool hasSeenNmea();
    unsigned int getRxCount();
    unsigned int getRxStartupMemLen() { return RX_STARTUP_MEM_LEN; }
    int* getRxStartupMem() { return mRxStartupMem; }
  private:
    void inspect(int c);

    bool mSeenUbx;
    bool mSeenNmea;
    GpsRxState mRxState;
    unsigned int mRxCount;
    int mRxStartupMem[RX_STARTUP_MEM_LEN];
};

#endif
