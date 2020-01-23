#include <msp430.h>
#include <softspimaster/softspimaster.h>

uint16_t g_currentTemp;
uint8_t data[2];

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    if (CALBC1_16MHZ == 0xFF)
        while(1);

    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    SoftSPIConfig_t spiConfig =
    {
     .spiBitOrder = MSB,
     .spiSSPolarity = ACTIVE_LOW,
     .spiPolarity = CPOL_0,
     .spiPhase = CPHA_0,
     .ssPin = GPIO_PIN2,
     .sckPin = GPIO_PIN3,
     .mosiPin = GPIO_PIN4,
     .misoPin = GPIO_PIN5,
     .ssPinBase = GPIO_PORT_P1,
     .sckPinBase = GPIO_PORT_P1,
     .mosiPinBase = GPIO_PORT_P1,
     .misoPinBase = GPIO_PORT_P1
    };

    configSPI(&spiConfig);

    data[0] = 0xDE;
    data[1] = 0xAD;

    for(;;)
    {
        softSPI(&spiConfig, data, sizeof data, WRITE);

        __delay_cycles(800000);
    }
}
