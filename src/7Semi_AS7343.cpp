#include "7Semi_AS7343.h"

AS7343_7Semi::AS7343_7Semi() : bus(nullptr) {}

AS7343_7Semi::~AS7343_7Semi()
{
    delete bus;
}

bool AS7343_7Semi::begin(uint8_t i2cAddress,
                         TwoWire &i2cPort,
                         uint32_t i2cClock)
{
    if (bus)
    {
        delete bus;
        bus = nullptr;
    }

    i2c.beginI2C(i2cAddress, i2cPort, i2cClock);

    bus = new BusIO_7Semi<I2C_Interface>(i2c);
    if (!bus)
        return false;

    delay(200); // allow sensor power-up

    if (!softReset())
        return false;

    delay(200); // wait after reset

    if (!setPower(AS7343_PowerOn::POWER_ON))
        return false;

    if (!setSpectralEnable(true))
        return false;

    uint8_t id;
    if (!getDeviceID(id))
        return false;

    if (id != AS7343_DEVICE_ID)
        return false;

    if (!bus->writeBit(AS7343_REG_ENABLE, 6, (uint8_t)true)) // PON bit
        return false;

    return true;
}

bool AS7343_7Semi::setRegisterBank(bool bank1)
{
    if (!bus)
        return false;

    return bus->writeBit(AS7343_REG_CFG0, 4, (uint8_t)bank1); // select register bank
}

bool AS7343_7Semi::getRegisterBank(bool &bank1)
{
    if (!bus)
        return false;

    return getBit(AS7343_REG_CFG0, 4, bank1); // read active register bank
}

bool AS7343_7Semi::getDeviceID(uint8_t &id)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true)) // ID is in bank 1
        return false;

    if (!bus->read(AS7343_REG_ID, id))
        return false;

    if (!setRegisterBank(false)) // restore default bank
        return false;

    return true;
}

bool AS7343_7Semi::setPower(AS7343_PowerOn powerOn)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!bus->writeBit(AS7343_REG_ENABLE, 0, (uint8_t)powerOn)) // PON bit
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::setSpectralEnable(bool enableSpectral)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!bus->writeBit(AS7343_REG_ENABLE, 1, (uint8_t)enableSpectral)) // SP_EN bit
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::getPower(bool &powerOn)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!getBit(AS7343_REG_ENABLE, 0, powerOn)) // read PON bit
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::getSpectralEnable(bool &enableSpectral)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!getBit(AS7343_REG_ENABLE, 1, enableSpectral)) // read SP_EN bit
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::setSMUX(bool enableSMUX)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!bus->writeBit(AS7343_REG_ENABLE, 4, (uint8_t)enableSMUX)) // SMUX enable
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::setLED(bool ledOn, uint16_t brightness)
{
    if (!bus)
        return false;

    if (brightness < 4)
        brightness = 4;
    if (brightness > 258)
        brightness = 258;

    uint8_t regVal = (brightness - 4) / 2; // convert mA → register

    if (!setRegisterBank(false))
        return false;

    uint8_t v = 0;
    v |= (regVal & 0x7F); // brightness
    if (ledOn)
        v |= (1 << 7); // LED enable

    if (!bus->write(AS7343_REG_LED, v))
        return false;

    return true;
}

bool AS7343_7Semi::getLED(bool &ledOn, uint16_t &brightness)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t v = 0;

    if (!bus->read(AS7343_REG_LED, v))
        return false;

    ledOn = (v >> 7) & 0x01;

    uint8_t regVal = v & 0x7F;
    brightness = 4 + (regVal * 2); // convert register → mA

    return true;
}


bool AS7343_7Semi::readSpectralData(AS7343_SpectralData &data)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t buffer[36]; // 18 channels × 2 bytes

    if (!bus->read(AS7343_REG_DATA_0_L, buffer, 36))
        return false;

    // combine LSB + MSB
    data.BLUE_450    = (uint16_t)buffer[1]  << 8 | buffer[0];
    data.GREN_555    = (uint16_t)buffer[3]  << 8 | buffer[2];
    data.YELLOW_600  = (uint16_t)buffer[5]  << 8 | buffer[4];
    data.NIR_855     = (uint16_t)buffer[7]  << 8 | buffer[6];
    data.BRITNESS_1  = (uint16_t)buffer[9]  << 8 | buffer[8];
    data.FLICKER_1   = (uint16_t)buffer[11] << 8 | buffer[10];
 
    data.BLUE_425    = (uint16_t)buffer[13] << 8 | buffer[12];
    data.BLUE_475    = (uint16_t)buffer[15] << 8 | buffer[14];
    data.GREEN_515   = (uint16_t)buffer[17] << 8 | buffer[16];
    data.RED_640     = (uint16_t)buffer[19] << 8 | buffer[18];
    data.BRITNESS_2  = (uint16_t)buffer[21] << 8 | buffer[20];
    data.FLICKER_2   = (uint16_t)buffer[23] << 8 | buffer[22];

    data.VIOLET_405  = (uint16_t)buffer[25] << 8 | buffer[24];
    data.RED_690     = (uint16_t)buffer[27] << 8 | buffer[26];
    data.RED_745     = (uint16_t)buffer[29] << 8 | buffer[28];
    data.GREEN_550   = (uint16_t)buffer[31] << 8 | buffer[30];
    data.BRITNESS_3  = (uint16_t)buffer[33] << 8 | buffer[32];
    data.FLICKER_3   = (uint16_t)buffer[35] << 8 | buffer[34];

    return true;
}

bool AS7343_7Semi::setGain(Gain gain)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!bus->writeBits(AS7343_REG_CFG1, 0, 5, (uint8_t)gain)) // AGAIN bits
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::getGain(uint8_t &gain)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!bus->readBits(AS7343_REG_CFG1, 0, 5, gain))
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::setIntegrationTime(uint8_t atime, uint16_t astep)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if (!bus->write(AS7343_REG_ATIME, atime))
        return false;

    if (!bus->write(AS7343_REG_ASTEP_L, (uint8_t)(astep & 0xFF)))
        return false;

    if (!bus->write(AS7343_REG_ASTEP_H, (uint8_t)((astep >> 8) & 0xFF)))
        return false;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::getIntegrationTime(uint8_t &atime, uint16_t &astep)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    uint8_t low = 0, high = 0;

    if (!bus->read(AS7343_REG_ATIME, atime))
        return false;

    if (!bus->read(AS7343_REG_ASTEP_L, low))
        return false;

    if (!bus->read(AS7343_REG_ASTEP_H, high))
        return false;

    astep = ((uint16_t)high << 8) | low;

    if (!setRegisterBank(false))
        return false;

    return true;
}

bool AS7343_7Semi::setInterruptEnable(bool asien, bool sp_ien, bool f_ien, bool sien)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t reg = 0;

    if (!bus->read(AS7343_REG_INTENAB, reg)) // preserve reserved bits
        return false;

    if (asien) reg |= (1 << 7); else reg &= ~(1 << 7);
    if (sp_ien) reg |= (1 << 3); else reg &= ~(1 << 3);
    if (f_ien)  reg |= (1 << 2); else reg &= ~(1 << 2);
    if (sien)   reg |= (1 << 0); else reg &= ~(1 << 0);

    if (!bus->write(AS7343_REG_INTENAB, reg))
        return false;

    return true;
}

bool AS7343_7Semi::getInterruptEnable(bool &asien, bool &sp_ien, bool &f_ien, bool &sien)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t reg = 0;

    if (!bus->read(AS7343_REG_INTENAB, reg))
        return false;

    asien = (reg >> 7) & 0x01;
    sp_ien = (reg >> 3) & 0x01;
    f_ien  = (reg >> 2) & 0x01;
    sien   = (reg >> 0) & 0x01;

    return true;
}

bool AS7343_7Semi::softReset()
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    if (!bus->writeBit(AS7343_REG_CONTROL, 3, (uint8_t)1)) // SW_RESET bit
        return false;

    delay(10); // allow reset

    return true;
}


bool AS7343_7Semi::setAutoZero(bool enableAutoZero, uint8_t autoZeroFrequency)
{
    if (!bus)
        return false;

    if (!setSpectralEnable(false)) // must disable spectral engine
        return false;

    if (!setRegisterBank(false))
        return false;

    if (!bus->write(AS7343_REG_AZ_CONFIG, autoZeroFrequency))
        return false;

    if (enableAutoZero)
    {
        if (!bus->writeBit(AS7343_REG_CONTROL, 2, (uint8_t)1)) // trigger autozero
            return false;

        delay(5); // wait for completion
    }

    if (!setSpectralEnable(true)) // restore spectral engine
        return false;

    return true;
}

bool AS7343_7Semi::flushFIFO()
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_CONTROL, 1, (uint8_t)1); // FIFO_CLR
}

bool AS7343_7Semi::wakeFromInterrupt()
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_CONTROL, 0, (uint8_t)1); // CLEAR_SAI
}

bool AS7343_7Semi::setWaitTime(uint8_t wtime)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->write(AS7343_REG_WTIME, wtime);
}

bool AS7343_7Semi::getWaitTime(uint8_t &wtime)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->read(AS7343_REG_WTIME, wtime);
}

bool AS7343_7Semi::setWaitTimeMs(uint16_t time_ms)
{
    if (!bus)
        return false;

    uint16_t wtime = (time_ms / 2.78f) - 1; // ms → register

    if (wtime > 255)
        wtime = 255;

    return setWaitTime((uint8_t)wtime);
}

float AS7343_7Semi::getWaitTimeMs(uint8_t wtime)
{
    return (wtime + 1) * 2.78f;
}

bool AS7343_7Semi::setGPIOMode(bool inputEnable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_GPIO, 2, (uint8_t)inputEnable);
}

bool AS7343_7Semi::getGPIOInput(bool &state)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return getBit(AS7343_REG_GPIO, 2, state);
}

bool AS7343_7Semi::setGPIOInvert(bool invert)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_GPIO, 3, (uint8_t)invert);
}

bool AS7343_7Semi::setFIFOLevelThreshold(FIFO_Level th)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBits(AS7343_REG_CFG8, 6, 2, (uint8_t)th); // FIFO_TH bits
}

bool AS7343_7Semi::getFIFOLevelThreshold(uint8_t &th)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->readBits(AS7343_REG_CFG8, 6, 2, th);
}

bool AS7343_7Semi::setLowAlert(uint16_t lowTH)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t data[2];
    data[0] = lowTH & 0xFF;         // LSB
    data[1] = (lowTH >> 8) & 0xFF;  // MSB

    return bus->write(AS7343_REG_SP_TH_L_LSB, data, 2); // write LSB+MSB together
}

bool AS7343_7Semi::getLowAlert(uint16_t &lowTH)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t data[2];

    if (!bus->read(AS7343_REG_SP_TH_L_LSB, data, 2))
        return false;

    lowTH = ((uint16_t)data[1] << 8) | data[0];

    return true;
}

bool AS7343_7Semi::setHighAlert(uint16_t highTH)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t data[2];
    data[0] = highTH & 0xFF;
    data[1] = (highTH >> 8) & 0xFF;

    return bus->write(AS7343_REG_SP_TH_H_LSB, data, 2);
}

bool AS7343_7Semi::getHighAlert(uint16_t &highTH)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t data[2];

    if (!bus->read(AS7343_REG_SP_TH_H_LSB, data, 2))
        return false;

    highTH = ((uint16_t)data[1] << 8) | data[0];

    return true;
}

bool AS7343_7Semi::setAlertChannel(Alert_Channel ch)
{
    if (!bus)
        return false;

    if (ch > Alert_Channel::CH5)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBits(AS7343_REG_CFG12, 0, 3, (uint8_t)ch);
}

bool AS7343_7Semi::getAlertChannel(uint8_t &ch)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->readBits(AS7343_REG_CFG12, 0, 3, ch);
}

bool AS7343_7Semi::readStatus(AS7343_Status &status)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t v = 0;

    if (!bus->read(AS7343_REG_STATUS, v))
        return false;

    status.spectral_saturation = (v >> 7) & 0x01;
    status.spectral_int         = (v >> 3) & 0x01;
    status.fifo_interrupt       = (v >> 2) & 0x01;
    status.system               = (v >> 0) & 0x01;

    if (!bus->read(AS7343_REG_STATUS2, v))
        return false;

    status.valid_data                    = (v >> 6) & 0x01;
    status.saturation.digital           = (v >> 4) & 0x01;
    status.saturation.analog            = (v >> 3) & 0x01;
    status.saturation.flicker_analog    = (v >> 2) & 0x01;
    status.saturation.flicker_digital   = (v >> 1) & 0x01;

    if (!bus->read(AS7343_REG_STATUS3, v))
        return false;

    status.interrupt.high_alert = (v >> 5) & 0x01;
    status.interrupt.low_alert  = (v >> 4) & 0x01;

    if (!bus->read(AS7343_REG_STATUS4, v))
        return false;

    status.error.fifo_overflow           = (v >> 7) & 0x01;
    status.error.over_temp               = (v >> 5) & 0x01;
    status.error.flicker_trigger_error   = (v >> 4) & 0x01;
    status.error.spectral_trigger_error  = (v >> 2) & 0x01;
    status.error.sleep_active            = (v >> 1) & 0x01;
    status.init_busy                     = (v >> 0) & 0x01;

    if (!bus->read(AS7343_REG_STATUS5, v))
        return false;

    status.interrupt.flicker_detect = (v >> 3) & 0x01;
    status.interrupt.smux_done      = (v >> 2) & 0x01;

    if (!bus->read(AS7343_REG_FD_STATUS, v))
        return false;

    status.flicker.valid_100Hz    = (v >> 2) & 0x01;
    status.flicker.valid_120Hz    = (v >> 3) & 0x01;
    status.flicker.detected_100Hz = (v >> 0) & 0x01;
    status.flicker.detected_120Hz = (v >> 1) & 0x01;
    status.flicker.saturate       = (v >> 4) & 0x01;
    status.flicker.valid          = (v >> 5) & 0x01;

    if (!bus->read(AS7343_REG_ASTATUS, v))
        return false;

    status.spectral_saturation = (v >> 7) & 0x01;
    status.gain = v & 0x0F;

    return true;
}

bool AS7343_7Semi::setLowPowerMode(bool enable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_CFG0, 5, (uint8_t)enable);
}

bool AS7343_7Semi::setWLong(bool enable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_CFG0, 2, (uint8_t)enable);
}

bool AS7343_7Semi::setSleepAfterInterrupt(bool enable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_CFG3, 4, (uint8_t)enable);
}

bool AS7343_7Semi::setSMUXCommand(AS7343_SMUX_Command cmd)
{
    if (!bus)
        return false;

    if (!setSMUX(true)) // enable SMUX before command
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBits(AS7343_REG_CFG6, 3, 2, (uint8_t)cmd);
}

bool AS7343_7Semi::enableInterrupt(bool flickerDetect, bool smuxDone)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t v = 0;
    v |= (flickerDetect << 6);
    v |= (smuxDone << 4);

    return bus->write(AS7343_REG_CFG9, v);
}

bool AS7343_7Semi::setAutoSMUX(AS7343_Auto_SMUX mode)
{
    if (!bus)
        return false;

    if (mode > AS7343_Auto_SMUX::AUTO_18CH)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBits(AS7343_REG_CFG20, 5, 2, (uint8_t)mode);
}

bool AS7343_7Semi::getAutoSMUX(uint8_t &mode)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->readBits(AS7343_REG_CFG20, 5, 2, mode);
}

bool AS7343_7Semi::setFlickerFIFOMode(AS7343_Fifo_8_Bit_Flicker_Mode enable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_CFG20, 7, (uint8_t)enable);
}

bool AS7343_7Semi::getFlickerFIFOMode(bool &enable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return getBit(AS7343_REG_CFG20, 7, enable);
}

bool AS7343_7Semi::setInterruptPersistence(AS7343_Persistence pers)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBits(AS7343_REG_PERS, 0, 4, (uint8_t)pers);
}

bool AS7343_7Semi::getInterruptPersistence(uint8_t &pers)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->readBits(AS7343_REG_PERS, 0, 4, pers);
}

bool AS7343_7Semi::setFIFOMap(uint8_t mapMask)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    mapMask &= 0x7F; // valid bits only
    return bus->write(AS7343_REG_FIFO_MAP, mapMask);
}

bool AS7343_7Semi::getFIFOMap(uint8_t &mapMask)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    if (!bus->read(AS7343_REG_FIFO_MAP, mapMask))
        return false;

    mapMask &= 0x7F;
    return true;
}

bool AS7343_7Semi::setFifoFlicker(bool EnableFifoFlicker)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->writeBit(AS7343_REG_FIFO_CFG0, 7, (uint8_t)EnableFifoFlicker);
}

bool AS7343_7Semi::getFifoFlicker(bool &EnableFifoFlicker)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return getBit(AS7343_REG_FIFO_CFG0, 7, EnableFifoFlicker);
}

bool AS7343_7Semi::getFifoLevel(uint8_t &fifoLevel)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return bus->read(AS7343_REG_FIFO_LVL, fifoLevel);
}

bool AS7343_7Semi::readFifo(uint8_t *fifoData, uint8_t len)
{
    if (!bus || !fifoData)
        return false;

    uint8_t fifo_level = 0;

    if (!getFifoLevel(fifo_level))
        return false;

    if (fifo_level < len)
        len = fifo_level; // limit to available data

    return bus->read(AS7343_REG_FDATA_L, fifoData, len);
}

bool AS7343_7Semi::setFlickerEnable(bool enable)
{
    if (!bus)
        return false;

    if (!setRegisterBank(true))
        return false;

    if(!bus->writeBit(AS7343_REG_ENABLE, 6, (uint8_t)enable)) // FDEN bit
        return false;

    if (!setRegisterBank(false))
        return false;


    return true;
}

bool AS7343_7Semi::readFlickerFrequency(uint16_t &freq)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t v = 0;

    if (!bus->read(AS7343_REG_FD_STATUS, v))
        return false;

    if ((v >> 0) & 0x01)
        freq = 100; // detected 100Hz
    else if ((v >> 1) & 0x01)
        freq = 120; // detected 120Hz
    else
        freq = 0;   // no flicker

    return true;
}

bool AS7343_7Semi::isFlickerValid(bool &valid)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    return getBit(AS7343_REG_FD_STATUS, 5, valid); // VALID bit
}

bool AS7343_7Semi::getFlickerInfo(bool &detected, uint16_t &freq)
{
    if (!bus)
        return false;

    if (!setRegisterBank(false))
        return false;

    uint8_t v = 0;

    if (!bus->read(AS7343_REG_FD_STATUS, v))
        return false;

    Serial.println(v,HEX);

    detected = ((v >> 0) & 0x01) || ((v >> 1) & 0x01);

    if ((v >> 0) & 0x01)
        freq = 100;
    else if ((v >> 1) & 0x01)
        freq = 120;
    else
        freq = 0;

    if (!bus->write(AS7343_REG_FD_STATUS, (uint8_t)0x7F))
        return false;

    return true;
}


bool AS7343_7Semi::getRGB(uint16_t &r, uint16_t &g, uint16_t &b)
{
    if (!bus)
        return false;

    AS7343_SpectralData data;

    if (!readSpectralData(data))
        return false;

    r = data.RED_640;
    g = data.GREEN_515;
    b = data.BLUE_475;

    return true;
    
}

bool AS7343_7Semi::getBit(uint8_t reg, uint8_t position, bool &value)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    if (!bus->readBit(reg, position, v))
        return false;

    value = (v & 0x01);

    return true;
}