#ifndef NTPDATETIME_HPP
#define NTPDATETIME_HPP

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <DS3231.h>

WiFiUDP ntpUDP;

class NTPTimeDate
{
private:
    NTPClient * timeClient;
    DS3231 clock;
    bool century = false;
    bool h12Flag = false;
    bool pmFlag = false;
    const char* _poolServerName = "pool.ntp.org";
    int currentNTPYear;
    int currentNTPMonth;
    int currentNTPDay;
    int currentNTPHour;
    int currentNTPMinute;
    int currentNTPSecond;
    int currentRTCYear;
    int currentRTCMonth;
    int currentRTCDay;
    int currentRTCHour;
    int currentRTCMinute;
    int currentRTCSecond;
    bool isUpdateSuccessful;
    void checkRTCTime();
public:
    NTPTimeDate(const char* _poolServerName);
    NTPTimeDate();
    String getNTPDate();
    String getNTPTime();
    void changeServer(const char* poolServerName);
    void NTPUpdate();
    void setRTCTime(int year, int month, int day, int hour, int minute, int second);
    int getCurrentNTPYear();
    int getCurrentNTPMonth();
    int getCurrentNTPDay();
    int getCurrentNTPHour();
    int getCurrentNTPMinute();
    int getCurrentNTPSecond();
    int getCurrentRTCYear();
    int getCurrentRTCMonth();
    int getCurrentRTCDay();
    int getCurrentRTCHour();
    int getCurrentRTCMinute();
    int getCurrentRTCSecond();
    String Now();
    ~NTPTimeDate();
};

NTPTimeDate::NTPTimeDate(const char* _poolServerName)
{
    Wire.begin();
    timeClient = new NTPClient(ntpUDP, _poolServerName, 0, 60000);
}

NTPTimeDate::NTPTimeDate()
{
    Wire.begin();
    timeClient = new NTPClient(ntpUDP, this->_poolServerName, 0, 60000);
}

NTPTimeDate::~NTPTimeDate()
{
}

void NTPTimeDate::changeServer(const char* poolServerName)
{
    _poolServerName = poolServerName;
}

void NTPTimeDate::NTPUpdate()
{
    this->isUpdateSuccessful = timeClient->update();
    if (this->isUpdateSuccessful)
    {
        Serial.println("NTP update successful");
        unsigned long epochTime = timeClient->getEpochTime();
        struct tm *ptm = gmtime ((time_t *)&epochTime);
        this->currentNTPYear = ptm->tm_year+1900;
        this->currentNTPMonth = ptm->tm_mon+1;
        this->currentNTPDay = ptm->tm_mday;
        this->currentNTPHour = ptm->tm_hour;
        this->currentNTPMinute = ptm->tm_min;
        this->currentNTPSecond = ptm->tm_sec;
        Serial.println(this->Now());
        this->checkRTCTime();
    }

}

String NTPTimeDate::getNTPDate()
{
    if (this->isUpdateSuccessful)
    {
    unsigned long epochTime = timeClient->getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    this->currentNTPYear = ptm->tm_year+1900;
    this->currentNTPMonth = ptm->tm_mon+1;
    this->currentNTPDay = ptm->tm_mday;
    String currentDate = String(this->currentNTPYear) + "-" + String(this->currentNTPMonth) + "-" + String(this->currentNTPDay);
    return currentDate;
    }
    else
    {
        return "";
    }
}

String NTPTimeDate::getNTPTime()
{
    if (this->isUpdateSuccessful)
    {
    unsigned long epochTime = timeClient->getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    this->currentNTPHour = ptm->tm_hour;
    this->currentNTPMinute = ptm->tm_min;
    this->currentNTPSecond = ptm->tm_sec;
    String currentTime = String(this->currentNTPHour) + ":" + String(this->currentNTPMinute) + ":" + String(this->currentNTPSecond);
    return currentTime;
    }
    else
    {
        return "";
    }
}

void NTPTimeDate::setRTCTime(int year, int month, int day, int hour, int minute, int second)
{
    this->clock.setHour(hour);
    this->clock.setMinute(minute);
    this->clock.setSecond(second);
    this->clock.setDate(day);
    this->clock.setMonth(month);
    this->clock.setYear(year);
}

void NTPTimeDate::checkRTCTime(){
    if (this->clock.getSecond() != this->currentNTPSecond)
    {
        this->clock.setSecond(this->currentNTPSecond);
    }
    if (this->clock.getMinute() != this->currentNTPMinute)
    {
        this->clock.setMinute(this->currentNTPMinute);
    }
    if (this->clock.getHour(this->h12Flag, this->pmFlag) != this->currentNTPHour)
    {
        this->clock.setHour(this->currentNTPHour);
    }
    if (this->clock.getDate() != this->currentNTPDay)
    {
        this->clock.setDate(this->currentNTPDay);
    }
    if (this->clock.getMonth(this->century) != this->currentNTPMonth)
    {
        this->clock.setMonth(this->currentNTPMonth);
    }
    if (this->clock.getYear() != this->currentNTPYear)
    {
        this->clock.setYear(this->currentNTPYear);
    }
}

int NTPTimeDate::getCurrentNTPYear()
{
    return this->currentNTPYear;
}

int NTPTimeDate::getCurrentNTPMonth()
{
    return this->currentNTPMonth;
}

int NTPTimeDate::getCurrentNTPDay()
{
    return this->currentNTPDay;
}

int NTPTimeDate::getCurrentNTPHour()
{
    return this->currentNTPHour;
}

int NTPTimeDate::getCurrentNTPMinute()
{
    return this->currentNTPMinute;
}

int NTPTimeDate::getCurrentNTPSecond()
{
    return this->currentNTPSecond;
}

int NTPTimeDate::getCurrentRTCYear()
{
    this->currentRTCYear = this->clock.getYear();
    return this->currentRTCYear;
}

int NTPTimeDate::getCurrentRTCMonth()
{
    this->currentRTCMonth = this->clock.getMonth(this->century);
    return this->currentRTCMonth;
}

int NTPTimeDate::getCurrentRTCDay()
{
    this->currentRTCDay = this->clock.getDate();
    return this->currentRTCDay;
}

int NTPTimeDate::getCurrentRTCHour()
{
    this->currentRTCHour = this->clock.getHour(this->h12Flag, this->pmFlag);
    return this->currentRTCHour;
}

int NTPTimeDate::getCurrentRTCMinute()
{
    this->currentRTCMinute = this->clock.getMinute();
    return this->currentRTCMinute;
}

int NTPTimeDate::getCurrentRTCSecond()
{
    this->currentRTCSecond = this->clock.getSecond();
    return this->currentRTCSecond;
}

String NTPTimeDate::Now()
{
 //   1990-06-20 08:03:00
    return String(this->getCurrentNTPYear()) + '-' + String(this->getCurrentNTPMonth()) + '-' + String(this->getCurrentNTPDay()) + ' ' + String(this->getCurrentNTPHour()) + ':' + String(this->getCurrentNTPMinute()) + ':' + String(this->getCurrentNTPSecond());
}

#endif