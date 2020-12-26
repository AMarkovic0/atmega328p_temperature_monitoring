# temperature-on-mail-request-Atmega328p
Monitor temperature in your room using gmail and Atmega328p.

Using: Python 3.6.8 and avr-gcc
OS: Windows Subsystem for Linux (Ubuntu)
Sensor: ATMega328p builtin temperature sensor

main.c:

Program for atmega 328p.
Measuring temperature using builtin temperature sensor.
Communicates with PC using serial communication.
On request sends information about temperature to PC.

serialPY.py:

Program for PC to run in background. 
Communicates with Atmega 328p using serial communication.
Cheking for mail with activation code -> On activation mail asksing information about temperature from controller -> sending mail with information.

Before running:

Substitute mails and passwords in the code.
Plug Atmega328p to your PC
If you are running different OS, change COM port path. 

Files:

├── driver

│   └── serialPY.py

├── inc

│   └── uart.h

├── main.hex

├── makefile

├── README.md

└── src

    ├── main.c
    
    └── uart.c

3 directories, 7 files

AT commands:
"AT+LED" -> toggle led on PB13
"AT+TER" -> returns temperature value ( suitable for serial plotters )

Max period for commands = 100ms

Run:

make
make flash
python3 serialPY.py


