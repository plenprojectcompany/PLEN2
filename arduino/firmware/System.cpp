/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/


// 独自ライブラリ
#include "Pin.h"
#include "System.h"


// マクロ定義
#define _DEBUG false

#define _PLEN2__SYSTEM__USBSERIAL Serial
#define _PLEN2__SYSTEM__BLESERIAL Serial1


PLEN2::System::System()
{
	_PLEN2__SYSTEM__BLESERIAL.begin(BLESERIAL_BAUDRATE());
	_PLEN2__SYSTEM__USBSERIAL.begin(USBSERIAL_BAUDRATE());

	pinMode(Pin::RS485_TXD(), OUTPUT);
	pinMode(Pin::LED_OUT(),   OUTPUT);

	digitalWrite(Pin::RS485_TXD(), LOW);
	digitalWrite(Pin::LED_OUT(),   LOW);
}


Stream& PLEN2::System::USBSerial()
{
	return _PLEN2__SYSTEM__USBSERIAL;
}


Stream& PLEN2::System::BLESerial()
{
	return _PLEN2__SYSTEM__BLESERIAL;
}


Stream& PLEN2::System::inputSerial()
{
	return _PLEN2__SYSTEM__BLESERIAL;
}


Stream& PLEN2::System::outputSerial()
{
	return _PLEN2__SYSTEM__USBSERIAL;
}


void PLEN2::System::timer1Attach()
{
	#if _DEBUG
		outputSerial().println(F("=== running in function : System::timer1Attach()"));
	#endif

	TIMSK1 = _BV(TOIE1);
}


void PLEN2::System::timer1Detach()
{
	#if _DEBUG
		outputSerial().println(F("=== running in function : System::timer1Detach()"));
	#endif

	TIMSK1 &= ~_BV(TOIE1);
}


void PLEN2::System::dump()
{
	#if _DEBUG
		outputSerial().println(F("=== running in function : System::dump()"));
	#endif

	outputSerial().println(F("{"));
		outputSerial().print(F("\t\"device\": \""));
		outputSerial().print(DEVICE());
		outputSerial().println(F("\","));

		outputSerial().print(F("\t\"codename\": \""));
		outputSerial().print(CODENAME());
		outputSerial().println(F("\","));

		outputSerial().print(F("\t\"version\": \""));
		outputSerial().print(VERSION());
		outputSerial().println(F("\""));
	outputSerial().println(F("}"));
}