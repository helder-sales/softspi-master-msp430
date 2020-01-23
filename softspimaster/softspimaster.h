/*
 * softspi.h
 *
 *  Created on: 17 de nov de 2018
 *      Author: helde
 */

#ifndef SOFTSPIMASTER_SOFTSPIMASTER_H_
#define SOFTSPIMASTER_SOFTSPIMASTER_H_

#include <msp430.h>
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define GPIO_PORT_P1                                                          1
#define GPIO_PORT_P2                                                          2
#define GPIO_PORT_P3                                                          3
#define GPIO_PORT_P4                                                          4
#define GPIO_PORT_P5                                                          5
#define GPIO_PORT_P6                                                          6
#define GPIO_PORT_P7                                                          7
#define GPIO_PORT_P8                                                          8
#define GPIO_PORT_P9                                                          9
#define GPIO_PORT_P10                                                        10
#define GPIO_PORT_P11                                                        11
#define GPIO_PORT_PA                                                          1
#define GPIO_PORT_PB                                                          3
#define GPIO_PORT_PC                                                          5
#define GPIO_PORT_PD                                                          7
#define GPIO_PORT_PE                                                          9
#define GPIO_PORT_PF                                                         11
#define GPIO_PORT_PJ                                                         13

#define GPIO_PIN0                                                      (0x0001)
#define GPIO_PIN1                                                      (0x0002)
#define GPIO_PIN2                                                      (0x0004)
#define GPIO_PIN3                                                      (0x0008)
#define GPIO_PIN4                                                      (0x0010)
#define GPIO_PIN5                                                      (0x0020)
#define GPIO_PIN6                                                      (0x0040)
#define GPIO_PIN7                                                      (0x0080)
#define GPIO_PIN8                                                      (0x0100)
#define GPIO_PIN9                                                      (0x0200)
#define GPIO_PIN10                                                     (0x0400)
#define GPIO_PIN11                                                     (0x0800)
#define GPIO_PIN12                                                     (0x1000)
#define GPIO_PIN13                                                     (0x2000)
#define GPIO_PIN14                                                     (0x4000)
#define GPIO_PIN15                                                     (0x8000)
#define GPIO_PIN_ALL8                                                    (0xFF)
#define GPIO_PIN_ALL16                                                 (0xFFFF)

#define CLOCK           16000000
#define SPISPEED        1000000
#define WRITE           false
#define READ            true
#define ACTIVE_LOW      false
#define ACTIVE_HIGH     true
#define CPOL_0          false
#define CPOL_1          true
#define CPHA_0          false
#define CPHA_1          true
#define MSB             false
#define LSB             true

typedef struct
{
    bool spiPhase;
    bool spiPolarity;
    bool spiSSPolarity;
    bool spiBitOrder;
    uint8_t ssPinBase;
    uint8_t sckPinBase;
    uint8_t mosiPinBase;
    uint8_t misoPinBase;
    uint16_t ssPin;
    uint16_t sckPin;
    uint16_t mosiPin;
    uint16_t misoPin;
}SoftSPIConfig_t;

void configSPI(SoftSPIConfig_t *config);
void sendData(SoftSPIConfig_t *config, uint8_t data, bool stop);
void readData(SoftSPIConfig_t *config, uint8_t *data, uint8_t count);
void softSPI(SoftSPIConfig_t *config, uint8_t *data, uint8_t count, bool rw);

#endif /* SOFTSPIMASTER_SOFTSPIMASTER_H_ */
