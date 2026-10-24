#pragma once

#include <Arduino.h>
#include <SPI.h>

class XPT2046 {
public:
    XPT2046(SPIClass &spi, uint8_t cs, uint8_t irq);

    void begin(uint16_t xres, uint16_t yres);
    bool pressed(void);

    uint16_t RawX(void) const;
    uint16_t RawY(void) const;
    uint16_t X(void) const;
    uint16_t Y(void) const;

    void setCal(uint16_t xmin, uint16_t xmax, uint16_t ymin, uint16_t ymax,
                uint16_t xres, uint16_t yres);
    void setRotation(uint8_t rotation);

private:
    SPIClass *_spi = nullptr;
    uint8_t _cs;
    uint8_t _irq;
    int16_t _xraw = 0;
    int16_t _yraw = 0;
    int16_t _xcoord = 0;
    int16_t _ycoord = 0;
    uint16_t _hmin = 0;
    uint16_t _hmax = 4095;
    uint16_t _vmin = 0;
    uint16_t _vmax = 4095;
    uint16_t _hres;
    uint16_t _vres;
    uint16_t _hmin_logic;
    uint16_t _hmax_logic;
    uint16_t _vmin_logic;
    uint16_t _vmax_logic;
    uint16_t _hres_logic;
    uint16_t _vres_logic;
    uint8_t _rotation = 0;
};
