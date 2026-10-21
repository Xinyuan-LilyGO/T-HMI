#include "xpt2046.h"

namespace {
const SPISettings SPI_SETTING(2000000, MSBFIRST, SPI_MODE0);

int16_t best_two_average(int16_t x, int16_t y, int16_t z) {
    const int16_t da = static_cast<int16_t>(abs(x - y));
    const int16_t db = static_cast<int16_t>(abs(x - z));
    const int16_t dc = static_cast<int16_t>(abs(z - y));

    if (da <= db && da <= dc) return static_cast<int16_t>((x + y) >> 1);
    if (db <= da && db <= dc) return static_cast<int16_t>((x + z) >> 1);
    return static_cast<int16_t>((y + z) >> 1);
}

// Map a raw axis value onto 0..resolution-1. A screen coordinate equal to the
// resolution is outside the panel, and arithmetic on equal calibration limits
// would divide by zero.
int16_t map_axis(int16_t value, uint16_t in_min, uint16_t in_max, uint16_t resolution) {
    if (resolution == 0 || in_min == in_max) return 0;
    const long mapped = map(value, in_min, in_max, 0, resolution - 1);
    return static_cast<int16_t>(constrain(mapped, 0L, static_cast<long>(resolution - 1)));
}
} // namespace

XPT2046::XPT2046(SPIClass &spi, uint8_t cs, uint8_t irq)
    : _spi(&spi), _cs(cs), _irq(irq), _hres(320), _vres(240), _hmin_logic(_hmin),
      _hmax_logic(_hmax), _vmin_logic(_vmin), _vmax_logic(_vmax), _hres_logic(_hres),
      _vres_logic(_vres) {}

bool XPT2046::pressed(void) {
    if (digitalRead(_irq) == HIGH) return false;
    if (_spi == nullptr) return false;

    _spi->beginTransaction(SPI_SETTING);
    digitalWrite(_cs, LOW);

    // Pen-down measurement also powers the panel between conversions.
    _spi->transfer16(0xB0);
    _spi->transfer16(0xC0);

    int16_t data[6];
    data[0] = _spi->transfer16(0x90) >> 4; // X
    data[1] = _spi->transfer16(0xD0) >> 4; // Y
    data[2] = _spi->transfer16(0x90) >> 4; // X
    data[3] = _spi->transfer16(0xD0) >> 4; // Y
    data[4] = _spi->transfer16(0x90) >> 4; // X
    data[5] = _spi->transfer16(0xD0) >> 4; // Y (power down)

    _spi->transfer16(0);
    digitalWrite(_cs, HIGH);
    _spi->endTransaction();

    _xraw = best_two_average(data[0], data[2], data[4]);
    _yraw = best_two_average(data[1], data[3], data[5]);

    if (_rotation == 0 || _rotation == 2) {
        _xcoord = map_axis(_xraw, _hmin_logic, _hmax_logic, _hres_logic);
        _ycoord = map_axis(_yraw, _vmin_logic, _vmax_logic, _vres_logic);
    } else {
        _xcoord = map_axis(_yraw, _hmin_logic, _hmax_logic, _hres_logic);
        _ycoord = map_axis(_xraw, _vmin_logic, _vmax_logic, _vres_logic);
    }

    return true;
}

uint16_t XPT2046::RawX(void) const { return static_cast<uint16_t>(_xraw); }

uint16_t XPT2046::RawY(void) const { return static_cast<uint16_t>(_yraw); }

uint16_t XPT2046::X(void) const { return static_cast<uint16_t>(_xcoord); }

uint16_t XPT2046::Y(void) const { return static_cast<uint16_t>(_ycoord); }

void XPT2046::begin(uint16_t hres, uint16_t vres) {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    pinMode(_irq, INPUT_PULLUP);

    _hres = hres;
    _vres = vres;
    _hres_logic = _hres;
    _vres_logic = _vres;
}

void XPT2046::setCal(uint16_t hmin, uint16_t hmax, uint16_t vmin, uint16_t vmax,
                     uint16_t hres, uint16_t vres) {
    _hmin = hmin;
    _hmax = hmax;
    _vmin = vmin;
    _vmax = vmax;
    _hres = hres;
    _vres = vres;
    _hmin_logic = _hmin;
    _hmax_logic = _hmax;
    _vmin_logic = _vmin;
    _vmax_logic = _vmax;
    _hres_logic = _hres;
    _vres_logic = _vres;
}

void XPT2046::setRotation(uint8_t rotation) {
    switch (rotation % 4) {
    case 0:
        _hmin_logic = _hmin;
        _hmax_logic = _hmax;
        _vmin_logic = _vmin;
        _vmax_logic = _vmax;
        _hres_logic = _hres;
        _vres_logic = _vres;
        break;
    case 1:
        _hmin_logic = _vmin;
        _hmax_logic = _vmax;
        _vmin_logic = _hmax;
        _vmax_logic = _hmin;
        _hres_logic = _vres;
        _vres_logic = _hres;
        break;
    case 2:
        // 180 degrees: keep the axes, reverse both calibration ranges.
        _hmin_logic = _hmax;
        _hmax_logic = _hmin;
        _vmin_logic = _vmax;
        _vmax_logic = _vmin;
        _hres_logic = _hres;
        _vres_logic = _vres;
        break;
    default:
        _hmin_logic = _vmax;
        _hmax_logic = _vmin;
        _vmin_logic = _hmin;
        _vmax_logic = _hmax;
        _hres_logic = _vres;
        _vres_logic = _hres;
        break;
    }
    _rotation = rotation % 4;
}
