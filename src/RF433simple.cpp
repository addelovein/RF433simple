#include "RF433simple.h"

TX433::TX433(uint8_t PIN)
{
    PIN_RFOUT = PIN;
    all_Protocols[0].encoding = (uint8_t)RfSendEncoding::TRIBIT;
    all_Protocols->first_high = 0;
    all_Protocols->first_low = 0;
    all_Protocols->first_low_ignored = 0;
}

int getmin(int64_t i)
{
    if (i == (int8_t)(i & 0xFF))
        return 1;
    if (i == (int16_t)(i & 0xFFFF))
        return 2;
    if (i == (int32_t)(i & 0xFFFFFFFF))
        return 4;
    return 8;
}

void TX433::Send()
{
    uint8_t SEND_BYTES = 0;
    uint16_t SEND_BITS = 0;

    // CHECK IF USER SET FIXED BIT TO SEND AND SET BYTE ARRAY ACCORDINGLY
    if (Bits > 0)
    {
        SEND_BYTES = (Bits - 1) / 8 + 1;
        SEND_BITS = Bits;
        ESP_LOGD("TX", "CUSTOM BIT Set to: %dBIT - Setting Bytes to: %dbytes", Bits, SEND_BYTES);
    }
    else
    {
        SEND_BYTES = getmin(Decimal);
        SEND_BITS = SEND_BYTES * 8;
        ESP_LOGD("TX", "NO BIT Set - Setting Bytes to: %dbytes", SEND_BYTES);
    }
    uint32_t x = (Decimal);

    ESP_LOGD("TX", "SEND_BYTES=%dbytes AND SENDBITS=%dBIT", SEND_BYTES, SEND_BITS);
    // CONVERT INTEGER TO BIT ARRAY WITh REQUESTED SIZE
    ESP_LOGD("TX", "Creating byte array with size: %d", SEND_BYTES);
    uint8_t *bytes;
    bytes = (uint8_t *)(&x);
    ESP_LOGD("BITHANDLER", "-------------- DEBUG -------------------");
    for (int i = 0; i < sizeof(bytes); i++)
        ESP_LOGD("ORIGINAL BYTE", "Byte[%d]=0x%d\n", i, bytes[i]);
    // REVERSE BIT ORDER
    *(uint32_t *)bytes = __builtin_bswap32(*(uint32_t *)bytes);
    ESP_LOGD("BITHANDLER", "\n-------------- AFTER -------------------");
    for (int i = sizeof(bytes) - SEND_BYTES; i < sizeof(bytes); i++)
        ESP_LOGD("AFTER SWAP BYTE", "Byte[%d]=0x%d\n", i, bytes[i]);

    uint8_t result[SEND_BYTES];
    uint8_t ti = 0;
    ESP_LOGD("BITHANDLER", "\n-------------- NEW ARRAY ---------------");
    for (int i = sizeof(bytes) - SEND_BYTES; i < sizeof(bytes); i++)
    {
        result[ti++] = bytes[i];
    }
    for (int i = 0; i < sizeof(result); i++)
        ESP_LOGD("AFTER SHIFT", "Byte[%d]=0x%d", i, result[i]);

    ESP_LOGD("BITHANDLER", "\n----------- END DEBUG -------------------");
    ESP_LOGD("TX", "Create HEX String");
    char output_hex[SEND_BYTES];
    for (int ti = SEND_BYTES - 1; ti >= 0; ti--)
        sprintf(output_hex + (SEND_BYTES - 1 - ti) * 2, "%02X", result[ti]);

    // SEND
    ESP_LOGD("TX", "Prepairing to send %d bytes of data...", SEND_BYTES);
    txinfo tx = {rfsend_builder(RfSendEncoding::TRIBIT_INVERTED, PIN_RFOUT,
                                RFSEND_DEFAULT_CONVENTION, Repeat, NULL, 9720, 0, 0,
                                270, 270, 540, 0, 0, 0, 9720, SEND_BITS),
                 "protocol 11"};
    byte n = tx.rfsender->send(SEND_BYTES, result);
    ESP_LOGD("TX", "Sent Packet: 0x%s(%d bytes) %dx times (Protocol: %d)", output_hex, SEND_BYTES, n, Protocol);
}

//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
///// RX - Receiver
RX433::RX433(uint8_t PIN)
{
    PIN_RFIN = PIN;
}

void RX433::Listen()
{
    ESP_LOGD("Listen", "Start");
    SYSTEM_RECEIVE = true;

    Track track(PIN_RFIN);
    do
    {
        track.treset();
        while (!track.do_events())
        {
            delay(1);
            if (!SYSTEM_RECEIVE)
                break;
        }
        if (!SYSTEM_RECEIVE)
            break;

        Decoder *pdec0 = track.get_data(RF433ANY_FD_ALL);
        localPointerToCallback(pdec0);
        delete pdec0;

    } while (SYSTEM_RECEIVE);
    ESP_LOGD("Listen", "Shutdown");
}

void RX433::Byte2Hex(char *output_hex, byte bufferSize, byte *buffer)
{
    for (int ti = sizeof(buffer) + 1; ti > (sizeof(buffer) + 1) - bufferSize; ti--)
        sprintf(output_hex + strlen(output_hex), "%02X", buffer[ti]);
}

void RX433::Byte2Dec(int output_dec, byte bufferSize, byte *bytes)
{
    /*
    uint8_t start_arr[8] = {0,};
    int ti=5;
    for (int i = 0; i<6; i++){
        memcpy(start_arr[i],bytes[ti--],1);
    }
        for (int i = 0; i<8; i++){
            printf("B[%d]=%d");
        }
        Serial.println();

    output_dec = (uint64_t)(*(uint64_t*)&start_arr[0]);
    */
}
