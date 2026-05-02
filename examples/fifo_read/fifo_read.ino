#include <7Semi_AS7343.h>

AS7343_7Semi sensor;

uint8_t fifoBuffer[64];

void setup()
{
    Serial.begin(115200);

    if (!sensor.begin(0x39, Wire, 400000))
    {
        Serial.println("Init failed");
        while (1);
    }

    Serial.println("AS7343 FIFO Example");

    sensor.setAutoSMUX(AS7343_Auto_SMUX::AUTO_18CH);
    sensor.setGain(Gain::GAIN_16X);
    sensor.setIntegrationTime(100, 999);
    sensor.setSpectralEnable(true);

    // FIFO config (7 channels → 14 bytes/frame)
    sensor.setFIFOMap(0x7F);
    sensor.setFifoFlicker(false);
    sensor.setFIFOLevelThreshold(FIFO_Level::FIFO_TH_16);

    sensor.setInterruptEnable(false, false, true, false);
}
void loop()
{
    AS7343_Status status;

    if (!sensor.readStatus(status))
        return;

    if (!status.fifo_interrupt)
        return;

    uint8_t level = 0;

    if (!sensor.getFifoLevel(level))
        return;

    Serial.print("FIFO Level: ");
    Serial.println(level);

    const uint8_t frameSize = 14; // 7 channels × 2 bytes
    uint8_t frames = level / frameSize;

    if (frames == 0)
        return;

    uint8_t bytesToRead = frames * frameSize;

    if (!sensor.readFifo(fifoBuffer, bytesToRead))
    {
        Serial.println("FIFO read failed");
        return;
    }

    // Decode frames
    for (uint8_t f = 0; f < frames; f++)
    {
        uint8_t *buf = &fifoBuffer[f * frameSize];

        uint16_t ch[7];

        for (uint8_t i = 0; i < 7; i++)
        {
            ch[i] = (buf[i * 2 + 1] << 8) | buf[i * 2];
        }

        Serial.print("Frame: ");
        for (uint8_t i = 0; i < 7; i++)
        {
            Serial.print(ch[i]);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Clear interrupt state
    sensor.wakeFromInterrupt();
}