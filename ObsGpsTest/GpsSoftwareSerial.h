#ifndef GpsSoftwareSerial_h
#define GpsSoftwareSerial_h

#include <SoftwareSerial.h>


class GpsSoftwareSerial {
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

    // UBX message classes (ignoring "proprietary" ones), i.e. same numbers with aliases grouped
    enum UbxMessageId {
      UBX_MSG_ID_ACK_NAK = 0x00,
      UBX_MSG_ID_CFG_PRT = 0x00,

      UBX_MSG_ID_NAV_POSECEF = 0x01,
      UBX_MSG_ID_ACK_ACK = 0x01,
      UBX_MSG_ID_CFG_MSG = 0x01,

      UBX_MSG_ID_NAV_POSLLH = 0x02,
      UBX_MSG_ID_CFG_INF = 0x02,

      UBX_MSG_ID_NAV_STATUS = 0x03,

      UBX_MSG_ID_NAV_DOP = 0x04,

      UBX_MSG_ID_NAV_SOL = 0x06,
      UBX_MSG_ID_CFG_DAT = 0x06,
      
      UBX_MSG_ID_CFG_TP = 0x07,

      UBX_MSG_ID_CFG_RATE = 0x08,

      UBX_MSG_ID_CFG_FXN = 0x0E,

      UBX_MSG_ID_NAV_VELECEF = 0x11,
      UBX_MSG_ID_CFG_RXM = 0x11,

      UBX_MSG_ID_NAV_VELNED = 0x12,
      UBX_MSG_ID_CFG_EKF = 0x12,

      UBX_MSG_ID_NAV_TIMEGPS = 0x20,
      UBX_MSG_ID_NAV_TIMEUTC = 0x21,
      UBX_MSG_ID_NAV_CLOCK = 0x22,
      UBX_MSG_ID_NAV_SVINFO = 0x30,
      UBX_MSG_ID_NAV_DGPS = 0x31,
      UBX_MSG_ID_NAV_SBAS = 0x32,
      UBX_MSG_ID_NAV_EFKSTATUS = 0x40,
      UBX_MSG_ID_NAV_AOPSTATUS = 0x60
    };

    const uint8_t UBX_MSG_STATUS_RX_FLAG = 0x10;
    const uint8_t UBX_MSG_STATUS_POLL_FLAG = 0x01;

    // UBX message classes (w/o UBX related IDs, continuous 0-based integer range)
    enum UbxMessage {
      UBX_MSG_NAV_POSECEF,
      UBX_MSG_NAV_POSLLH,
      UBX_MSG_NAV_STATUS,
      UBX_MSG_NAV_DOP,
      UBX_MSG_NAV_SOL,
      UBX_MSG_NAV_VELECEF,
      UBX_MSG_NAV_VELNED,
      UBX_MSG_NAV_TIMEGPS,
      UBX_MSG_NAV_TIMEUTC,
      UBX_MSG_NAV_CLOCK,
      UBX_MSG_NAV_SVINFO,
      UBX_MSG_NAV_DGPS,
      UBX_MSG_NAV_SBAS,
      UBX_MSG_NAV_EFKSTATUS,
      UBX_MSG_NAV_AOPSTATUS,

      UBX_MSG_ACK_NAK,
      UBX_MSG_ACK_ACK,

      UBX_MSG_CFG_PRT,
      UBX_MSG_CFG_MSG,
      UBX_MSG_CFG_INF,
      UBX_MSG_CFG_DAT,
      UBX_MSG_CFG_TP,
      UBX_MSG_CFG_RATE,
      UBX_MSG_CFG_FXN,
      UBX_MSG_CFG_RXM,
      UBX_MSG_CFG_EKF,

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
    bool pollUbxMessage(UbxMessageClass msgClass, UbxMessageId msgId);
    bool rxedMessage(UbxMessage msg);
    bool polledMessage(UbxMessage msg);
  private:
    void inspect(int c);
    bool recordUbxRxMessage(UbxMessageClass msgClass, UbxMessageId msgId);
    bool recordUbxTxMessage(UbxMessageClass msgClass, UbxMessageId msgId);
    bool recordUbxMessage(UbxMessageClass msgClass, UbxMessageId msgId, bool rxedNotPoll);
    uint16_t calcFletcherChecksum(uint8_t* pData, size_t len);

    bool mSeenUbx;
    bool mSeenNmea;
    GpsRxState mRxState;
    unsigned int mRxCount;
    int mRxStartupMem[RX_STARTUP_MEM_LEN];
    UbxMessageClass mLastMsgClass;
    uint8_t mUbxMsgStatus[UBX_MSG_NUM];
    int mRxPin;
    int mTxPin;
};

#endif
