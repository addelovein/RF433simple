#ifndef RF433SIMPLE_H
#define RF433SIMPLE_H
#include <RF433send.h>
#include <RF433any.h>

struct txinfo
{
    RfSend *rfsender;
    const char *protocol_name;
};
typedef struct : public TimingsExt
{
    uint8_t nb_bits;
    uint8_t encoding;
    byte data[6];
} SignalPack_t;

class TX433
{
private:
    SignalPack_t all_Protocols[12];
    uint8_t PIN_RFOUT = 0;
public:
    TX433(uint8_t PIN_RFOUT = 0);
    void Send();
    uint32_t Decimal = 0;
    uint8_t Protocol = 1;
    uint16_t Bits = 0;
    uint8_t Repeat = 3;
};

class RX433
{
private:
    void Byte2Dec(int output_dec, byte bufferSize, byte *bytes);
    void Byte2Hex(char *output_hex, byte bufferSize, byte *buffer);
    void (*localPointerToCallback)(Decoder *object);
    uint8_t PIN_RFIN = 2;
    bool SYSTEM_RECEIVE = true;

public:
    RX433(uint8_t PIN_RFIN = 2){};
    void setCallback(void (*userDefinedCallback)(Decoder *object)) { localPointerToCallback = userDefinedCallback; }
    void Listen();
    void Stop()
    {
        SYSTEM_RECEIVE = false;
        ESP_LOGD("STOP", "Set Flag");
    };
};

#endif