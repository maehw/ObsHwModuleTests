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
      UBX_VALID_MSG_CLASS, // 3rd UBX byte rx'ed and found within the expected set
      NMEA_CANDIDATE_1, // 1st byte is '$' (start of sequence)
      NMEA_CANDIDATE_2, // 2nd byte is 'G'
      NMEA_MAGIC_MATCH // 3rd byte is 'N' or 'P' (i.e. received "$GP"/"$GN")
    };

    // UBX message classes (ignoring "proprietary" ones)
    enum UbxMessageClass {
      UBX_MSG_CLASS_INVALID = 0x00,
      UBX_MSG_CLASS_NAV = 0x01,
      UBX_MSG_CLASS_RXM = 0x02,
      UBX_MSG_CLASS_INF = 0x04,
      UBX_MSG_CLASS_ACK = 0x05,
      UBX_MSG_CLASS_CFG = 0x06,
      UBX_MSG_CLASS_MON = 0x0A,
      UBX_MSG_CLASS_AID = 0x0B,
      UBX_MSG_CLASS_TIM = 0x0D,
      UBX_MSG_CLASS_ESF = 0x10
    };

    // UBX message classes (ignoring "proprietary" ones)
    enum UbxMessageId {
      UBX_MSG_ID_ACK_NAK = 0x00,
      UBX_MSG_ID_CFG_PRT = 0x00,

      UBX_MSG_ID_NAV_POSECEF = 0x01,
      UBX_MSG_ID_ACK_ACK = 0x01,

      UBX_MSG_ID_NAV_POSLLH = 0x02,
      UBX_MSG_ID_NAV_STATUS = 0x03,
      UBX_MSG_ID_NAV_DOP = 0x04,
      UBX_MSG_ID_NAV_SOL = 0x06,
      UBX_MSG_ID_NAV_VELECEF = 0x11,
      UBX_MSG_ID_NAV_VELNED = 0x12,
      UBX_MSG_ID_NAV_TIMEGPS = 0x20,
      UBX_MSG_ID_NAV_TIMEUTC = 0x21,
      UBX_MSG_ID_NAV_NAVCLOCK = 0x22,
      UBX_MSG_ID_NAV_SVINFO = 0x30,
      UBX_MSG_ID_NAV_DGPS = 0x31,
      UBX_MSG_ID_NAV_SBAS = 0x32,
      UBX_MSG_ID_NAV_EFKSTATUS = 0x40,
      UBX_MSG_ID_NAV_AOPSTATUS = 0x60
    };

    // UBX message classes (w/o UBX related IDs, continuous 0-based integer range)
    enum UbxMessage {
      UBX_MSG_NAV_POSECEF = 0,
      UBX_MSG_NAV_POSLLH,
      UBX_MSG_NAV_STATUS,
      UBX_MSG_NAV_DOP,
      UBX_MSG_NAV_SOL,
      UBX_MSG_NAV_VELECEF,
      UBX_MSG_NAV_VELNED,
      UBX_MSG_NAV_TIMEGPS,
      UBX_MSG_NAV_TIMEUTC,
      UBX_MSG_NAV_NAVCLOCK,
      UBX_MSG_NAV_SVINFO,
      UBX_MSG_NAV_DGPS,
      UBX_MSG_NAV_SBAS,
      UBX_MSG_NAV_EFKSTATUS,
      UBX_MSG_NAV_AOPSTATUS,
      UBX_MSG_NUM
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
    bool isValidMessageClass(uint8_t c);
  private:
    void inspect(int c);
    bool recordUbxMessage(UbxMessageClass msgClass, uint8_t msgId);

    bool mSeenUbx;
    bool mSeenNmea;
    GpsRxState mRxState;
    unsigned int mRxCount;
    int mRxStartupMem[RX_STARTUP_MEM_LEN];
    UbxMessageClass mLastMsgClass;
    uint8_t mUbxMsgStatus[UBX_MSG_NUM];
};

#endif
