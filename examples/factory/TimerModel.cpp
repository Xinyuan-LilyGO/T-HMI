#include "TimerModel.h"

#include "esp_sntp.h"
#include <time.h>
#include <Arduino.h>

#define NTP_SERVER1                  "pool.ntp.org"
#define NTP_SERVER2                  "time.nist.gov"
#define GMT_OFFSET_SEC               (3600 * 8)
#define DAY_LIGHT_OFFSET_SEC         0

TimerModel::TimerModel() {
}

TimerModel::~TimerModel() {
}


void TimerModel::init() {
    sntp_servermode_dhcp(1);
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
}

int32_t TimerModel::getHour() {
    struct tm timeinfo;
    getLocalTime(&timeinfo, 100);
    return timeinfo.tm_hour;
}


int32_t TimerModel::getMin() {
    struct tm timeinfo;
    getLocalTime(&timeinfo, 100);
    return timeinfo.tm_min;
}
