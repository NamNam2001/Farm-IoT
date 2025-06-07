#include <Arduino.h>
#include <SPI.h>
#define ADC1_BASE 0x40012400 // Địa chỉ base của ADC1
#define ADC1 ((adc_reg_map *)ADC1_BASE)
#define RCC_BASE 0x40021000 // Địa chỉ base của RCC
#define RCC ((rcc_reg_map *)RCC_BASE)
void init_ADC_TemperatureSensor()
{
    // Bật cảm biến nhiệt độ (TSVREFE = 1)
    ADC1->CR2 |= (1 << 23);

    // Đặt thời gian lấy mẫu cho kênh 16 (ADC_IN16)
    ADC1->SMPR1 &= ~(7 << 18); // Xóa các bit cũ
    ADC1->SMPR1 |= (5 << 18);  // Chọn thời gian lấy mẫu 17.1µs

    // Bật ADC
    ADC1->CR2 |= (1 << 0);

    // Đợi ADC ổn định
    delay(10);
}
float readTemperature()
{
    ADC1->SQR3 = 16;        // Chọn kênh 16 (Cảm biến nhiệt độ)
    ADC1->CR2 |= (1 << 22); // Bắt đầu chuyển đổi ADC

    while (!(ADC1->SR & (1 << 1)))
        ; // Chờ kết quả ADC (EOC = 1)

    uint16_t raw = ADC1->DR; // Đọc giá trị ADC

    // Chuyển đổi ADC sang điện áp
    float VSENSE = (raw * 3.3) / 4095.0;

    // Tính toán nhiệt độ
    float temperature = (1.43 - VSENSE) / 0.0043 + 25;

    return temperature;
}

void setup()
{
    Serial.begin(115200);
    init_ADC_TemperatureSensor();
    // check_ADC_Clock();
}

void loop()
{
    float temp = readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    // check_ADC_Clock();
    delay(1000);
}