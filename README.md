# temperature-on-mail-request-Atmega328p

## Overview

This software has been made just for fun, while waiting for ESP2866.
This software measures temperature in Atmega328p.
On email request, this software will send you an email with measured temperatue.

The project has two components:

	- Software for the temperature measurment and signal processing
	- SMTP Client

## Atmega328p

Even the Atmega328p processor is on the Arduino board, I decided to not use the Arduino libraries but to make my own.
Temperature is measured using builtin temperature sensor, this sensor is usually used to monitor temperature of the MCU and
the data will not provide correct information about inroom temperatue.

## SMTP Client

Has been made without any knowledge about the module.
