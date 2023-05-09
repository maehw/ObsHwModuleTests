#include "GpsSoftwareSerial.h"

GpsSoftwareSerial::GpsSoftwareSerial(uint8_t receivePin, uint8_t transmitPin) :
    SoftwareSerial(receivePin, transmitPin),
    mSeenUbx(false),
    mSeenNmea(false),
    mRxState(IDLE)
{
}

GpsSoftwareSerial::~GpsSoftwareSerial()
{
  ~SoftwareSerial();
}

void GpsSoftwareSerial::begin(long speed)
{
  SoftwareSerial::begin(speed);
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
        mSeenUbx = true;
      }
      else
      {
        mRxState = IDLE;
      }
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
        mRxState = NMEA_MAGIC_MATCH;
        mSeenNmea = true;
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
  inspect(character);
  return character;
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
