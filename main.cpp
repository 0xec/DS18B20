#include <iostream>
#include <bitset>
#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

#define PIN RPI_GPIO_P1_11

class DS18B20 {
public:
    DS18B20() {
        if (!bcm2835_init()) {
            cout << "Init DS18B20 fail" << endl;
        }

        bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
    }
    void ReadTemp() {
        reset();
        writeCommand(0xcc);
        writeCommand(0x44);
        bcm2835_delay(1000);
        reset();
        writeCommand(0xcc);
        writeCommand(0xbe);
        int t1 = readCommand();
        int t2 = readCommand();
        reset();

        int value = t2 << 8 | t1;
        //cout << "value=" << (float)value * 6.25f << endl;
        printf("value=%f\n", (float)value);
    }

    virtual ~DS18B20() {
        bcm2835_close();
    }

protected:
    void reset() {
        bcm2835_gpio_write(PIN, LOW);
        bcm2835_delayMicroseconds(600);
        bcm2835_gpio_write(PIN, HIGH);
        bcm2835_delayMicroseconds(60);
        int value = bcm2835_gpio_lev(PIN);
        bcm2835_delayMicroseconds(5000);
        //cout << "reset=" << value << endl;
    }
    void writeCommand(uint8_t cmd) {
        std::bitset<8> bs(cmd);

        for (int i = 0; i < 8; i++) {
            bcm2835_gpio_write(PIN, LOW);
            bcm2835_delayMicroseconds(10);
            bcm2835_gpio_write(PIN, bs[i]);
            bcm2835_delayMicroseconds(40);
            bcm2835_gpio_write(PIN, HIGH);
        }
    }

    uint8_t readCommand() {
        std::bitset<8> bs;

        for (int i = 0; i < 8; i++) {
            bcm2835_gpio_write(PIN, LOW);
            bcm2835_gpio_write(PIN, HIGH);
           // bcm2835_delayMicroseconds(10);
            int value = bcm2835_gpio_lev(PIN);
            bcm2835_delayMicroseconds(45);
            bs.set(i, value);
        }

        cout << "test:" << bs << endl;

        return bs.to_ulong();
    }
};

int main()
{
    DS18B20 sensor;

    sensor.ReadTemp();

    return 0;
}
