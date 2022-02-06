#ifndef SD_CARD_LOGGER_HPP
#define SD_CARD_LOGGER_HPP

#include <SD.h>

class SDCardLogger
{
private:
    File logFile;
    File jsonFile;
    bool isSDPresent;
    bool isSDInitialized;
    bool isSDLoggingEnabled;
    uint8_t CS_PIN;
public:
    SDCardLogger(uint8_t CS_PIN);
    void enableSDLogging();
    void disableSDLogging();
    bool getSDLoggingEnabled();
    void SDWriteLog(String message);
    void SDWriteLog(String message, String filename);
    void SDWriteLog(String message, String filename, String mode);
    void SDWriteLog(String TimeStamp, String Temperature, String Pressure, String Altitude, String Humidity);
    void writeHeader();
    ~SDCardLogger();
};

SDCardLogger::SDCardLogger(uint8_t CS_PIN)
{
    this->CS_PIN = CS_PIN;
    isSDPresent = SD.begin(CS_PIN);
}

SDCardLogger::~SDCardLogger()
{
}

void SDCardLogger::enableSDLogging()
{
    if (isSDPresent)
    {
        if (!isSDInitialized)
        {
            logFile = SD.open("log.cvs", FILE_WRITE);
            isSDInitialized = true;
        }
        isSDLoggingEnabled = true;
        this->writeHeader();
    }
}

void SDCardLogger::disableSDLogging()
{
    if (isSDPresent)
    {
        if (isSDInitialized)
        {
            logFile.close();
            isSDInitialized = false;
        }
        isSDLoggingEnabled = false;
    }
}

bool SDCardLogger::getSDLoggingEnabled()
{
    return isSDLoggingEnabled;
}

void SDCardLogger::SDWriteLog(String message)
{
    if (isSDPresent && isSDInitialized && isSDLoggingEnabled)
    {
        logFile.print(message);
    }
}

void SDCardLogger::SDWriteLog(String message, String filename)
{
    if (isSDPresent && isSDInitialized && isSDLoggingEnabled)
    {
        logFile = SD.open(filename, FILE_WRITE);
        logFile.print(message);
        logFile.close();
    }
}

void SDCardLogger::SDWriteLog(String message, String filename, String mode)
{
    if (isSDPresent && isSDInitialized && isSDLoggingEnabled)
    {
        logFile = SD.open(filename, mode.c_str());
        logFile.print(message);
        logFile.close();
    }
}

void SDCardLogger::writeHeader()
{
    if (isSDPresent && isSDInitialized && isSDLoggingEnabled && !SD.exists("log.cvs"))
    {
        logFile = SD.open("log.cvs", FILE_WRITE);
        logFile.print("TimeStamp,Temperature,Pressure,Altitude,Humidity\n");
    }
}

void SDCardLogger::SDWriteLog(String TimeStamp, String Temperature, String Pressure, String Altitude, String Humidity)
{
    if (isSDPresent && isSDInitialized && isSDLoggingEnabled)
    {
        logFile.print(TimeStamp);
        logFile.print(",");
        logFile.print(Temperature);
        logFile.print(",");
        logFile.print(Pressure);
        logFile.print(",");
        logFile.print(Altitude);
        logFile.print(",");
        logFile.print(Humidity);
        logFile.print("\n");
    }
}

#endif