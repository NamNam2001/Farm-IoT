#include <SPI.h>
#include <time.h>

void convert_timestamp_to_datetime(uint32_t timestamp)
{
    time_t rawtime = (time_t)timestamp;
    struct tm *timeinfo;

    timeinfo = localtime(&rawtime);

    // In ra ngày và giờ sử dụng Serial
    Serial.print("Ngày: ");
    Serial.print(timeinfo->tm_mday);
    Serial.print("-");
    Serial.print(timeinfo->tm_mon + 1);
    Serial.print("-");
    Serial.println(timeinfo->tm_year + 1900);

    Serial.print("Giờ: ");
    Serial.print(timeinfo->tm_hour);
    Serial.print(":");
    Serial.print(timeinfo->tm_min);
    Serial.print(":");
    Serial.println(timeinfo->tm_sec);
}

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    uint32_t timestamp = 1727507228;
    convert_timestamp_to_datetime(timestamp);
    delay(5000);
}
