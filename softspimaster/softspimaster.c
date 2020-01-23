/*
 *  softspi.c
 *
 *  Created on: 17 de nov de 2018
 *  Author: Helder Sales
 */

#include <softspimaster/softspimaster.h>

// Emulating TI's driverlib for MSP430 (G2 series doesn't have support)
// to make it portable to other MSP430s that have driverlib
void GPIO_setAsOutputPin(uint16_t pinBase, uint16_t pin)
{
    switch(pinBase)
    {
    case GPIO_PORT_P1:
        P1DIR |= pin;
        break;

    case GPIO_PORT_P2:
        P2DIR |= pin;
        break;

    default:
        __never_executed();
    }
}

void GPIO_setAsInputPin(uint16_t pinBase, uint16_t pin)
{
    switch(pinBase)
    {
    case GPIO_PORT_P1:
        P1DIR &= ~pin;
        break;

    case GPIO_PORT_P2:
        P2DIR &= ~pin;
        break;

    default:
        __never_executed();
    }
}

void GPIO_setOutputHighOnPin(uint16_t pinBase, uint16_t pin)
{
    switch(pinBase)
    {
    case GPIO_PORT_P1:
        P1OUT |= pin;
        break;

    case GPIO_PORT_P2:
        P2OUT |= pin;
        break;

    default:
        __never_executed();
    }
}

void GPIO_setOutputLowOnPin(uint16_t pinBase, uint16_t pin)
{
    switch(pinBase)
    {
    case GPIO_PORT_P1:
        P1OUT &= ~pin;
        break;

    case GPIO_PORT_P2:
        P2OUT &= ~pin;
        break;

    default:
        __never_executed();
    }
}

void GPIO_toggleOutputOnPin(uint16_t pinBase, uint16_t pin)
{
    switch(pinBase)
    {
    case GPIO_PORT_P1:
        P1OUT ^= pin;
        break;

    case GPIO_PORT_P2:
        P2OUT ^= pin;
        break;

    default:
        __never_executed();
    }
}

uint16_t GPIO_getInputPinValue(uint16_t pinBase, uint16_t pin)
{
    switch(pinBase)
    {
    case GPIO_PORT_P1:
        return(pin & P1IN);

    case GPIO_PORT_P2:
        return(pin & P2IN);

    default:
        __never_executed();
    }

    return 0;
}

void configSPI(SoftSPIConfig_t *config)
{
    // Config SPI pins
    GPIO_setAsOutputPin(config->ssPinBase, config->ssPin);
    GPIO_setAsOutputPin(config->sckPinBase, config->sckPin);
    GPIO_setAsOutputPin(config->mosiPinBase, config->mosiPin);
    GPIO_setAsInputPin(config->misoPinBase, config->misoPin);

    // Leave SS inactive
    if(config->spiSSPolarity == ACTIVE_LOW)
        GPIO_setOutputHighOnPin(config->ssPinBase, config->ssPin);

    else
        GPIO_setOutputLowOnPin(config->ssPinBase, config->ssPin);

    // Leave SCK active
    // Because of toggle functions used later on, the polarity is defined
    // only here
    if(config->spiPolarity == CPOL_0)
        GPIO_setOutputLowOnPin(config->sckPinBase, config->sckPin);

    else
        GPIO_setOutputHighOnPin(config->sckPinBase, config->sckPin);
}

// Transmit data with 8 bits size
void sendData(SoftSPIConfig_t *config, uint8_t data, bool stop)
{
    static const uint8_t byteSize = 8;

    uint8_t bitOrder = 0x00;

    // Set bit order (MSB or LSB)
    if(config->spiBitOrder == MSB)
        bitOrder = 0x80;

    else
        bitOrder = 0x01;

    // Init SPI comm with SS active
    if(config->spiSSPolarity == ACTIVE_LOW)
        GPIO_setOutputLowOnPin(config->ssPinBase, config->ssPin);

    else
        GPIO_setOutputHighOnPin(config->ssPinBase, config->ssPin);

    // Send data with selected phase
    if(config->spiPhase == CPHA_0)
    {
        __delay_cycles(CLOCK / (SPISPEED * 2));

        for(uint8_t j = 0; j < byteSize; j++)
        {
            if(data & bitOrder)
                GPIO_setOutputHighOnPin(config->mosiPinBase, config->mosiPin);

            else
                GPIO_setOutputLowOnPin(config->mosiPinBase, config->mosiPin);

            __delay_cycles(CLOCK / (SPISPEED * 2));

            GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);

            __delay_cycles(CLOCK / (SPISPEED * 2));

            GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);

            if(config->spiBitOrder == MSB)
                data <<= 1;

            else
                data >>= 1;
        }

        if(stop)
        {
            // SS inactive terminating comm and MOSI low
            GPIO_setOutputLowOnPin(config->mosiPinBase, config->mosiPin);

            __delay_cycles(CLOCK / (SPISPEED * 2));

            GPIO_toggleOutputOnPin(config->ssPinBase, config->ssPin);
        }
    }

    else
    {
        __delay_cycles(CLOCK / (SPISPEED * 2));

        for(uint8_t j = 0; j < byteSize; j++)
        {
            __delay_cycles(CLOCK / (SPISPEED * 2));

            GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);

            if(data & bitOrder)
                GPIO_setOutputHighOnPin(config->mosiPinBase, config->mosiPin);

            else
                GPIO_setOutputLowOnPin(config->mosiPinBase, config->mosiPin);

            __delay_cycles(CLOCK / (SPISPEED * 2));

            GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);

            if(config->spiBitOrder == MSB)
                data <<= 1;

            else
                data >>= 1;
        }

        if(stop)
        {
            // SS inactive terminating comm and MOSI low
            GPIO_setOutputLowOnPin(config->mosiPinBase, config->mosiPin);

            __delay_cycles(CLOCK / (SPISPEED * 2));

            GPIO_toggleOutputOnPin(config->ssPinBase, config->ssPin);
        }
    }
}

// Read data with 8 bits size
void readData(SoftSPIConfig_t *config, uint8_t *data, uint8_t count)
{
    static const uint8_t byteSize = 8;

    // Clear buffer
    memset(data, 0x00, count);

    // Init SPI comm with SS active
    if(config->spiSSPolarity == ACTIVE_LOW)
        GPIO_setOutputLowOnPin(config->ssPinBase, config->ssPin);

    else
        GPIO_setOutputHighOnPin(config->ssPinBase, config->ssPin);

    // Receive data with selected phase
    if(config->spiPhase == CPHA_0)
    {
        for(uint8_t i = 0; i < count; i++)
        {
            __delay_cycles(CLOCK / (SPISPEED * 2));

            for(uint8_t j = 0; j < byteSize; j++)
            {
                // Receive data according to the bit order choosed
                if(config->spiBitOrder == MSB)
                    data[i] |= GPIO_getInputPinValue(config->misoPinBase,
                                                     config->misoPin) <<
                                                     ((byteSize - 1) - j);

                else
                    data[i] |= GPIO_getInputPinValue(config->misoPinBase,
                                                     config->misoPin) << j;

                __delay_cycles(CLOCK / (SPISPEED * 2));

                GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);

                __delay_cycles(CLOCK / (SPISPEED * 2));

                GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);
            }
        }
    }

    else
    {
        for(uint8_t i = 0; i < count; i++)
        {
            __delay_cycles(CLOCK / (SPISPEED * 2));

            for(uint8_t j = 0; j < byteSize; j++)
            {
                __delay_cycles(CLOCK / (SPISPEED * 2));

                GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);

                // Receive data with selected bit order
                if(config->spiBitOrder == MSB)
                    data[i] |= GPIO_getInputPinValue(config->misoPinBase,
                                                     config->misoPin) <<
                                                     ((byteSize - 1) - j);

                else
                    data[i] |= GPIO_getInputPinValue(config->misoPinBase,
                                                     config->misoPin) << j;

                __delay_cycles(CLOCK / (SPISPEED * 2));

                GPIO_toggleOutputOnPin(config->sckPinBase, config->sckPin);
            }
        }
    }

    //SS inactive terminating comm
    __delay_cycles(CLOCK / (SPISPEED * 2));

    GPIO_toggleOutputOnPin(config->ssPinBase, config->ssPin);
}

void softSPI(SoftSPIConfig_t *config, uint8_t *data, uint8_t count, bool rw)
{
    if(!rw)
    {
        for(uint8_t i = 0; i < count - 1; i++)
        {
            sendData(config, *data, false);
            data++;
        }

        sendData(config, *data, true);
    }

    else
        readData(config, data, count);
}
