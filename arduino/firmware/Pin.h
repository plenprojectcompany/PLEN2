/*!
	@file      Pin.h
	@brief     各デバイスの機能をArduinoのピンマッピングに即して、別名として提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__PIN_H_
#define _PLEN2__PIN_H_

namespace PLEN2
{
	/*!
		@brief ピンマッピングの管理
		
		これらの関数の戻り値を、Arduino標準ライブラリのピン指定に渡して下さい。
		なお、これらの関数は全てコンパイル時に静的に評価されるため、
		実行時のオーバーヘッドは存在しません。

		@note
		PLEN2の回路図も併せてご参照ください。
		→ https://github.com/plenproject/plen__baseboard
		
		@sa
		Arduino Microのピンマッピング → http://arduino.cc/en/Hacking/PinMapping32u4
	 */
	namespace Pin
	{
		//! @brief マルチプレクサの出力選択，0ビット目
		inline static const int MULTIPLEXER_SELECT0() { return 5;  }

		//! @brief マルチプレクサの出力選択，1ビット目
		inline static const int MULTIPLEXER_SELECT1() { return 6;  }

		//! @brief マルチプレクサの出力選択，2ビット目
		inline static const int MULTIPLEXER_SELECT2() { return 12; }

		//! @brief PWMの出力先，サーボモータ00～07番用
		inline static const int PWM_OUT_00_07()       { return 11; }

		//! @brief PWMの出力先，サーボモータ08～15番用
		inline static const int PWM_OUT_08_15()       { return 10; }

		//! @brief PWMの出力先，サーボモータ16～23番用
		inline static const int PWM_OUT_16_23()       { return 9;  }

		//! @brief シリアルの出力方向選択
		inline static const int RS485_TXD()           { return 4;  }

		//! @brief LEDの出力先
		inline static const int LED_OUT()             { return 13; }

		//! @brief ランダムデバイスの入力先
		inline static const int RANDOM_DEVCIE_IN()    { return 6;  }
	}
}

#endif // _PLEN2__PIN_H_