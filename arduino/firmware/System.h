/*!
	@file      System.h
	@brief     AVR MCUにおける基本機能の管理クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__SYSTEM_H_
#define _PLEN2__SYSTEM_H_

#include "Arduino.h"

namespace PLEN2
{
	class System;
}

/*!
	@brief AVR MCUにおける基本機能の管理クラス

	具体的には以下の機能が提供されます。
	- 割り込みのattach，detach
	- 入出力シリアルの管理・提供
*/
class PLEN2::System
{
private:
	//! @brief 動作デバイス
	inline static const char* DEVICE()             { return "PLEN2";   }

	//! @brief コードネーム
	inline static const char* CODENAME()           { return "Cytisus"; }

	//! @brief バージョン番号
	inline static const char* VERSION()            { return "1.2.1";   }

	//! @brief USBシリアルインタフェースの通信速度
	inline static const long  USBSERIAL_BAUDRATE() { return 2000000L;  }
	
	//! @brief BLEシリアルインタフェースの通信速度
	inline static const long  BLESERIAL_BAUDRATE() { return 2000000L;  }

public:
	//! @brief MCU内EEPROMのサイズ
	inline static const int INTERNAL_EEPROMSIZE() { return 1024; }

	/*!
		@brief コンストラクタ
	*/
	System();

	/*!
		@brief  USBシリアルインスタンスの取得メソッド

		@return USBシリアルインスタンスの参照
	*/
	Stream& USBSerial();

	/*!
		@brief  BLEシリアルインスタンスの取得メソッド

		@return BLEシリアルインスタンスの参照
	*/
	Stream& BLESerial();

	/*!
		@brief  入力シリアルインスタンスの取得メソッド

		@return 入力シリアルインスタンスの参照
	*/
	Stream& inputSerial();

	/*!
		@brief  出力シリアルインスタンスの取得メソッド
		
		@return 出力シリアルインスタンスの参照
	*/
	Stream& outputSerial();

	/*!
		@brief タイマ1割り込みの許可メソッド
	*/
	void timer1Attach();

	/*!
		@brief タイマ1割り込みの禁止メソッド

		@attention
		サーボモータへのPWM信号が突然遮断されると、多くの場合意図しない角度に回転してしまいます。
		そのため、単純にサーボモータの動きを止めるためにこのメソッドを使うことは推奨されません。
	*/
	void timer1Detach();

	/*!
		@brief システム構成のダンプメソッド

		@note
		以下のような書式のJSON文字列を出力します。
		@code
		{
			"device": <string>,
			"codename": <string>,
			"version": <string>
		}
		@endcode
	*/
	void dump();
};

#endif // _PLEN2__SYSTEM_H_