#include "GpsSoftwareSerial.h"

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

bool GpsSoftwareSerial::recordUbxMessage(UbxMessageClass msgClass, uint8_t msgId)
{
  bool success = true;

  if(msgClass == UBX_MSG_CLASS_INVALID)
  {
    return false;
  }

  switch(msgClass)
  {
    case UBX_MSG_CLASS_ACK:
      switch((UbxMessageClass)msgId)
      {
        case UBX_MSG_ID_ACK_NAK: // intentional fall-through
        case UBX_MSG_ID_ACK_ACK:
          // TODO: continue and also do the recording!
          success = true;
          break;
        default:
          success = false;
          break;
      }
      break;
    case UBX_MSG_CLASS_CFG:
      switch((UbxMessageClass)msgId)
      {
        case UBX_MSG_ID_CFG_PRT:
          // TODO: continue and also do the recording!
          success = true;
          break;
        default:
          success = false;
          break;
      }
      break;
    default:
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
      Serial.print("Message class: ");
      Serial.print(mLastMsgClass, HEX);
      Serial.print(", ID: ");
      Serial.println(c, HEX);
      if(recordUbxMessage(mLastMsgClass, c))
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
