/*
   Sketch for FemtoF3 Evo brushless board with SBUS receiver

   Copyright (c) 2018 Simon D. Levy

   This file is part of Hackflight.

   Hackflight is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Hackflight is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with Hackflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <hackflight.hpp>
#include <mixers/quadx.hpp>
#include <receivers/sbus.hpp>
#include <receivers/dummy.hpp>

#include "femtof3.h"

constexpr uint8_t CHANNEL_MAP[6] = {0, 1, 2, 3, 4, 5};

static hf::Hackflight h;

extern "C" {

#include "io/serial.h"
#include "drivers/serial_uart.h"

    static uint8_t byte;

    static void sbusDataReceive(uint16_t c, void *data)
    {
        (void)data;

        byte = (uint8_t)c;
    }

    static serialPort_t * sbusSerialPort;

    static uint8_t _sbusSerialAvailable(void)
    {
        return serialRxBytesWaiting(sbusSerialPort);
    }

    static uint8_t _sbusSerialRead(void)
    {
        return serialRead(sbusSerialPort);
    }

    void setup(void)
    {
        hf::Stabilizer * stabilizer = new hf::Stabilizer(
                0.10f,      // Level P
                0.125f,     // Gyro cyclic P
                0.001875f,  // Gyro cyclic I
                0.175f,     // Gyro cyclic D
                0.625f,    // Gyro yaw P
                0.005625f); // Gyro yaw I

        //hf::SBUS_Receiver * rc = new hf::SBUS_Receiver(CHANNEL_MAP);

        hf::Dummy_Receiver * rc = new hf::Dummy_Receiver();

        // Initialize Hackflight firmware
        h.init(new FemtoF3(), rc, new hf::MixerQuadX(), stabilizer);

        // Set up UART
        uartPinConfigure(serialPinConfig());

        // Open serial connection to receiver
        sbusSerialPort = uartOpen(UARTDEV_3, sbusDataReceive, NULL, 100000, MODE_RX, 
                (portOptions_e)((uint8_t)SERIAL_STOPBITS_2|(uint8_t)SERIAL_PARITY_EVEN|(uint8_t)SERIAL_INVERTED));

        // Start the receiver
        //rc->begin();
    }

    void loop(void)
    {
        h.update();

        uint32_t time = micros();
        static uint32_t _time;
        if (time - _time > 1000) { 
            hf::Debug::printf("%02X\n", byte);
            _time = time;
        }
    }

} // extern "C"

uint8_t sbusSerialAvailable(void)
{
    return 0;//_sbusSerialAvailable();
}

uint8_t sbusSerialRead(void)
{
    return 0;//_sbusSerialRead();
}

