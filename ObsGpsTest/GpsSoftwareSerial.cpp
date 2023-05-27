#include "GpsSoftwareSerial.h"

#define DEBUG_PRINTLN(msg)  Serial.println(msg);
#define DEBUG_PRINT(msg)  Serial.print(msg);

GpsSoftwareSerial::GpsSoftwareSerial(uint8_t receivePin, uint8_t transmitPin) :
    SoftwareSerial(receivePin, transmitPin),
    mSeenUbx(false),
    mSeenNmea(false),
    mRxState(IDLE),
    mRxCount(0),
    mLastMsgClass(UBX_MSG_CLASS_INVALID),
    mUbxMsgStatus{0}
{
  memset(mRxStartupMem, 0, RX_STARTUP_MEM_LEN);
}

GpsSoftwareSerial::~GpsSoftwareSerial()
{
  ~SoftwareSerial();
}

void GpsSoftwareSerial::begin(long speed)
{
  SoftwareSerial::begin(speed);
}

bool GpsSoftwareSerial::isValidMessageClass(uint8_t c)
{
  return (c == UBX_MSG_CLASS_NAV || c == UBX_MSG_CLASS_RXM || c == UBX_MSG_CLASS_INF || c == UBX_MSG_CLASS_ACK || 
          c == UBX_MSG_CLASS_CFG || c == UBX_MSG_CLASS_MON || c == UBX_MSG_CLASS_AID || c == UBX_MSG_CLASS_TIM || c == UBX_MSG_CLASS_ESF);
}

bool GpsSoftwareSerial::recordUbxTxMessage(UbxMessageClass msgClass, UbxMessageId msgId)
{
  return recordUbxMessage(msgClass, msgId, false);
}

bool GpsSoftwareSerial::recordUbxRxMessage(UbxMessageClass msgClass, UbxMessageId msgId)
{
  return recordUbxMessage(msgClass, msgId, true);
}

bool GpsSoftwareSerial::rxedMessage(UbxMessage msg)
{
  return (mUbxMsgStatus[msg] & UBX_MSG_STATUS_RX_FLAG) ? true : false;
}

bool GpsSoftwareSerial::polledMessage(UbxMessage msg)
{
  return (mUbxMsgStatus[msg] & UBX_MSG_STATUS_POLL_FLAG) ? true : false;
}

bool GpsSoftwareSerial::recordUbxMessage(UbxMessageClass msgClass, UbxMessageId msgId, bool rxedNotPoll)
{
  bool success = true;
  // TODO: realize as critical section (multiple contexts may want to write flags!)
  uint8_t UBX_MSG_STATUS_FLAG = rxedNotPoll ? UBX_MSG_STATUS_RX_FLAG : UBX_MSG_STATUS_POLL_FLAG;

  if(msgClass == UBX_MSG_CLASS_INVALID)
  {
    DEBUG_PRINTLN("UBX_MSG_CLASS_INVALID: ");
    DEBUG_PRINT("  ID: "); DEBUG_PRINTLN(msgId);
    return false;
  }

  switch(msgClass)
  {
    case UBX_MSG_CLASS_NAV:
      switch(msgId)
      {
        case UBX_MSG_ID_NAV_POSECEF:
          mUbxMsgStatus[UBX_MSG_NAV_POSECEF] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_POSLLH:
          mUbxMsgStatus[UBX_MSG_NAV_POSLLH] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_STATUS:
          mUbxMsgStatus[UBX_MSG_NAV_STATUS] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_DOP:
          mUbxMsgStatus[UBX_MSG_NAV_DOP] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_SOL:
          mUbxMsgStatus[UBX_MSG_NAV_SOL] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_VELECEF:
          mUbxMsgStatus[UBX_MSG_NAV_VELECEF] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_VELNED:
          mUbxMsgStatus[UBX_MSG_NAV_VELNED] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_TIMEGPS:
          mUbxMsgStatus[UBX_MSG_NAV_TIMEGPS] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_TIMEUTC:
          mUbxMsgStatus[UBX_MSG_NAV_TIMEUTC] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_CLOCK:
          mUbxMsgStatus[UBX_MSG_NAV_CLOCK] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_SVINFO:
          mUbxMsgStatus[UBX_MSG_NAV_SVINFO] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_DGPS:
          mUbxMsgStatus[UBX_MSG_NAV_DGPS] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_SBAS:
          mUbxMsgStatus[UBX_MSG_NAV_SBAS] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_EFKSTATUS:
          mUbxMsgStatus[UBX_MSG_NAV_EFKSTATUS] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_NAV_AOPSTATUS:
          mUbxMsgStatus[UBX_MSG_NAV_AOPSTATUS] |= UBX_MSG_STATUS_FLAG;
          break;
        default:
          DEBUG_PRINTLN("UBX_MSG_CLASS_NAV: ");
          DEBUG_PRINT("  CLASS: "); DEBUG_PRINTLN(msgClass);
          DEBUG_PRINT("  ID: "); DEBUG_PRINTLN(msgId);
          success = false;
          break;
      }
      break;
    case UBX_MSG_CLASS_ACK:
      switch(msgId)
      {
        case UBX_MSG_ID_ACK_NAK:
          mUbxMsgStatus[UBX_MSG_ACK_NAK] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_ACK_ACK:
          mUbxMsgStatus[UBX_MSG_ACK_ACK] |= UBX_MSG_STATUS_FLAG;
          break;
        default:
          DEBUG_PRINTLN("UBX_MSG_CLASS_ACK: ");
          DEBUG_PRINT("  CLASS: "); DEBUG_PRINTLN(msgClass);
          DEBUG_PRINT("  ID: "); DEBUG_PRINTLN(msgId);
          success = false;
          break;
      }
      break;
    case UBX_MSG_CLASS_CFG:
      switch(msgId)
      {
        case UBX_MSG_ID_CFG_PRT:
          mUbxMsgStatus[UBX_MSG_CFG_PRT] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_MSG:
          mUbxMsgStatus[UBX_MSG_CFG_MSG] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_INF:
          mUbxMsgStatus[UBX_MSG_CFG_INF] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_DAT:
          mUbxMsgStatus[UBX_MSG_CFG_DAT] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_TP:
          mUbxMsgStatus[UBX_MSG_CFG_TP] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_RATE:
          mUbxMsgStatus[UBX_MSG_CFG_RATE] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_FXN:
          mUbxMsgStatus[UBX_MSG_CFG_FXN] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_RXM:
          mUbxMsgStatus[UBX_MSG_CFG_RXM] |= UBX_MSG_STATUS_FLAG;
          break;
        case UBX_MSG_ID_CFG_EKF:
          mUbxMsgStatus[UBX_MSG_CFG_EKF] |= UBX_MSG_STATUS_FLAG;
          break;
        default:
          DEBUG_PRINTLN("UBX_MSG_CLASS_CFG: ");
          DEBUG_PRINT("  CLASS: "); DEBUG_PRINTLN(msgClass);
          DEBUG_PRINT("  ID: "); DEBUG_PRINTLN(msgId);
          success = false;
          break;
      }
      break;
    default:
      DEBUG_PRINTLN("UBX_MSG_CLASS_INVALID: ");
      DEBUG_PRINT("  CLASS: "); DEBUG_PRINTLN(msgClass);
      DEBUG_PRINT("  ID: "); DEBUG_PRINTLN(msgId);
      success = false;
      break;
  }

  return success;
}

void GpsSoftwareSerial::inspect(int c)
{
  // There's no guarantee to identify UBX and NMEA protocols by checking two or three consecutive bytes, but it's a start...
  switch(mRxState)
  {
    case IDLE:
      if(c == 0xB5)
      {
        mRxState = UBX_CANDIDATE;
      }
      else if(c == '$')
      {
        mRxState = NMEA_CANDIDATE_1;
      }
      break;
    case UBX_CANDIDATE:
      if(c == 0x62)
      {
        mRxState = UBX_MAGIC_MATCH;
      }
      else
      {
        mRxState = IDLE;
      }
      break;
    case UBX_MAGIC_MATCH:
      // check message class
      if(isValidMessageClass(c))
      {
        mLastMsgClass = (UbxMessageClass)c;
        mRxState = UBX_VALID_MSG_CLASS;
      }
      else
      {
        mLastMsgClass = UBX_MSG_CLASS_INVALID;
        mRxState = IDLE;
      }
      break;
    case UBX_VALID_MSG_CLASS:
      // already have message class, now check message ID
//      Serial.print("Message class: ");
//      Serial.print(mLastMsgClass, HEX);
//      Serial.print(", ID: ");
//      Serial.println(c, HEX);
      if(recordUbxRxMessage(mLastMsgClass, (UbxMessageId)c))
      {
        mSeenUbx = true; // if we ever get here, asue that UBX is alive
      }
      mRxState = IDLE; // always return to IDLE
      break;
    case NMEA_CANDIDATE_1:
      if(c == 'G')
      {
        mRxState = NMEA_CANDIDATE_2;
      }
      else
      {
        mRxState = IDLE;
      }
      break;
    case NMEA_CANDIDATE_2:
      if(c == 'P' || c == 'N')
      {
        mSeenNmea = true;
        mRxState = NMEA_MAGIC_MATCH;
      }
      else
      {
        mRxState = IDLE;
      }
      break;
    default:
      mRxState = IDLE;
      break;
  }
}

bool GpsSoftwareSerial::pollUbxMessage(UbxMessageClass msgClass, UbxMessageId msgId)
{
  bool txSuccess = true;
  // send an UBX (binar protocol) message
  // (and implicitely expect an ACK-* message back; please note that the RX side is not covered within this function)
  uint8_t ubxTxMsg[8] = {
    0xB5, // 1st sync char
    0x62, // 2nd sync char
    (uint8_t)msgClass, // message class
    (uint8_t)msgId, // message ID
    0x00, // 1st length byte
    0x00, // 2nd length byte
    0xDE, // 1st checksum byte 'CK_A' (to be calculated)
    0xAD, // 2nd checksum byte 'CK_B' (to be calculated)
  };

  uint16_t checksum = calcFletcherChecksum(&ubxTxMsg[2], /*len=*/4); // calculate checksum (start at the class field, i.e. at offset +2)
  ubxTxMsg[6] = (checksum >> 8) & 0x00FF; // update CK_A
  ubxTxMsg[7] = checksum & 0x00FF; // update CK_B

  // send those message bytes!
  for(size_t i=0; i<sizeof(ubxTxMsg)/sizeof(ubxTxMsg[0]); i++)
  {
    write(ubxTxMsg[i]);
  }

  txSuccess = recordUbxTxMessage(msgClass, msgId);

  return txSuccess;
}

int GpsSoftwareSerial::read()
{
  int character = SoftwareSerial::read();
  if(character != -1)
  {
    inspect(character);

    if(mRxCount < RX_STARTUP_MEM_LEN)
    {
      mRxStartupMem[mRxCount] = character; // memorize for later analysis
    }
    
    if(mRxCount < UINT_MAX) // saturate to prevent roll-over
    {
      ++mRxCount;
    }
  }
  return character;
}

size_t GpsSoftwareSerial::write(uint8_t b)
{
  return SoftwareSerial::write(b);
}

int GpsSoftwareSerial::available()
{
  return SoftwareSerial::available();
}

bool GpsSoftwareSerial::hasSeenUbx()
{
  return mSeenUbx;
};

bool GpsSoftwareSerial::hasSeenNmea()
{
  return mSeenNmea;
};

unsigned int GpsSoftwareSerial::getRxCount()
{
  return mRxCount;
}

uint16_t GpsSoftwareSerial::calcFletcherChecksum(uint8_t* pData, size_t len)
{
  // calculate checksum using 8 bit Fletcher algorithm
  uint8_t ckA = 0;
  uint8_t ckB = 0;
  for(size_t i=0; i<len; i++)
  {
    ckA += *pData;
    ckB += ckA;
    pData++;
  }
  return ((uint16_t)ckA << 8) | (uint16_t)ckB;
}
