/**
 * PLEN2 - Firmware for Arduino (Atmega32u4)
 * =============================================================================
 *
 * Code name "Cytisus" (version 1.00).
 *
 * Copyright (c) 2015.
 * ---
 * - Kazuyuki TAKASE - https://github.com/Guvalif
 * - PLEN Project Company Ltd. - http://plen.jp/
 * 
 * Build enviroment.
 * ---
 * - Windows 8.1 Professional edition
 * - Windows 7 Home Premium
 * - Arduino IDE ver.1.6.0
 * - Sublime Text 2 ver.2.0.2
 * - [Stino](https://github.com/Robot-Will/Stino)
 * - [Arduino Unit](https://github.com/mmurdoch/arduinounit)
 * 
 * License.
 * ---
 * This software is released under the MIT License.
 * (See also : http://opensource.org/licenses/mit-license.php)
 */


// 標準Cライブラリ関連
#include <string.h>

// Arduinoライブラリ関連
#include <EEPROM.h>
#include <Wire.h>


// #define _DEBUG           // デバッグプリントを行います。
// #define _DEBUG_CODE      // コードインタプリタ回りのデバッグを行います。
// #define _DEBUG_MOTION    // モーション再生回りのデバッグを行います。
// #define _DEBUG_INSTALL   // モーションインストール回りのデバッグを行います。
// #define _DEBUG_EXTEEPROM // EEPROMの読み書き回りのデバッグを行います。
// #define _DEBUG_HARD      // 割り込み回りがシビアなメソッドについてもデバッグプリントを行います。


/**
 * ピンマッピング
 * =============================================================================
 * NOTE:
 * ---
 * PLEN2の回路図も併せてご参照ください。
 * Arduino Microのピンマッピング → http://arduino.cc/en/Hacking/PinMapping32u4
 */
namespace Pin
{
	inline static const int MULTIPLEXER_SELECT0() { return 5;  }
	inline static const int MULTIPLEXER_SELECT1() { return 6;  }
	inline static const int MULTIPLEXER_SELECT2() { return 12; }
	inline static const int PWM_OUT_00_07()       { return 11; }
	inline static const int PWM_OUT_08_15()       { return 10; }
	inline static const int PWM_OUT_16_23()       { return 9;  }
	inline static const int RS485_TXD()           { return 4;  }
	inline static const int LED_OUT()             { return 13; }
}


/**
 * 前方参照・定数定義
 * =============================================================================
 * CAUTION!:
 * ---
 * コンパイルの都合上、配列の要素数はマクロで参照しないとエラーとなります。
 * C++11のconstexprを使用すれば、静的に決定可能な変数はinline展開され、
 * マクロと同様に扱ってもエラーとなりません。
 *
 * Arduino IDEでもソースコードをC++11とみなしてコンパイルを行うことが可能です。
 * 参考 → http://www.cloverfield.jp/2015/03/18/786
 */
namespace System
{
	#define         _SYSTEM__USBSERIAL                       Serial
	#define         _SYSTEM__BLESERIAL                       Serial1
	#define         _SYSTEM__PWM_OUT_00_07_REGISTER          OCR1C
	#define         _SYSTEM__PWM_OUT_08_15_REGISTER          OCR1B
	#define         _SYSTEM__PWM_OUT_16_23_REGISTER          OCR1A
	inline static const long USBSERIAL_BAUDRATE()   { return 2000000; }
	inline static const long BLESERIAL_BAUDRATE()   { return 2000000; }
	inline static const int  EEPROM_SIZE()          { return 1024;    }
}

namespace ExternalEEPROM
{
	inline static const long CLOCK()      { return 400000;  }
	inline static const long SIZE()       { return 0x20000; }
	inline static const int  BLOCK()      { return 32;      }
	inline static const int  ADDRESS()    { return 0x50;    }
	inline static const int  SELECT_BIT() { return 2;       }
}

namespace Purser
{
	namespace Buffer
	{
		// NOTE:
		// ---
		// バッファサイズはBLEのペイロード(20byte)以上ならばOK
		inline static const int LENGTH()       { return 32; }
		#define        _BUFFER__LENGTH                  32
		inline static const int TO_NUM__BASE() { return 16; } // 数値に変換する際の基数
	}
}

namespace Multiplexer
{
	inline static const int SUM()            { return 3; } // 実装個数
	inline static const int SELECTABLE_NUM() { return 8; } // 制御可能数
}

namespace Joint
{
	struct JointSetting
	{
		unsigned int MIN;  // 関節可動域最小値
		unsigned int MAX;  // 関節可動域最大値
		unsigned int HOME; // 関節初期位置
	};

	inline static const int SUM()                  { return 24;   } // 実装個数
	#define         _JOINT__SUM                             24
	inline static const int ANGLE_MIN()            { return 300;  }
	inline static const int ANGLE_MAX()            { return 1500; }
	inline static const int CONFIG_FLAG_ADDRESS()  { return 0;    }
	inline static const int CONFIG_FLAG_VALUE()    { return 0;    }
	inline static const int CONFIG_BEGIN_ADDRESS() { return 1;    }

	namespace PWM
	{
		inline static const int NEUTRAL() { return 652; } // 機械的中間PWM
		inline static const int MIN()     { return 493; } // 機械的可動最大PWM
		inline static const int MAX()     { return 810; } // 機械的可動最小PWM
	}
}

namespace Motion
{
	struct HeaderDefinition
	{
		char          name[20];
		unsigned char extra[3];
		unsigned char frame_num;
		unsigned char slot;
	};

	inline static const int SLOT_MIN()     { return 0;  }
	inline static const int SLOT_MAX()     { return 99; }
	inline static const int FLAMENUM_MIN() { return 1;  }
	inline static const int FLAMENUM_MAX() { return 20; }
	#define        _MOTION__FLAMENUM_MAX            20

	namespace Frame
	{
		struct FrameDefinition
		{
			unsigned int  transition_delay_msec;
			int           joint_angle[_JOINT__SUM];
			unsigned char number;
		};

		// NOTE:
		// ---
		// 更新間隔は、本ファームウェアのタイマ割り込みの設定では約32[msec]です。
		inline static const int UPDATE_MSEC() { return 32; }
	}

	void setHeader(HeaderDefinition*);
	void getHeader(unsigned char, HeaderDefinition*);
	void setFrame(unsigned char, Frame::FrameDefinition*);
	void getFrame(unsigned char, unsigned char, Frame::FrameDefinition*);
}


namespace Config
{
	extern Motion::HeaderDefinition       header;
	extern Motion::Frame::FrameDefinition frame;
}


/**
 * システム管理メソッド・変数
 * =============================================================================
 */
namespace System
{
	Stream* input_serial  = &_SYSTEM__BLESERIAL;
	Stream* output_serial = &_SYSTEM__USBSERIAL;

	// 入力シリアルの切り替えを行います。
	void toggleInputSerial()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : System::toggleInputSerial()"));
		#endif

		if (input_serial == &_SYSTEM__BLESERIAL)
		{
			input_serial = &_SYSTEM__USBSERIAL;
		}
		else
		{
			input_serial = &_SYSTEM__BLESERIAL;
		}
	}

	// Timer1割り込みを許可します。
	void timer1Start()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : System::timer1Start()"));
		#endif


		TIMSK1 = _BV(TOIE1);
	}

	// Timer1割り込みを禁止します。
	void timer1Stop()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : System::timer1Stop()"));
		#endif

		TIMSK1 &= ~_BV(TOIE1);
	}
}


/**
 * 外部EEPROM管理メソッド
 * =============================================================================
 * NOTE:
 * ---
 * 使用しているEEPROM，24FC1025は、一度にアクセス可能な領域サイズが
 * 128byteですが、ArduinoのWireライブラリのバッファサイズが32byteのため、
 * 性能を最大限に引き出すことはできません。
 *
 * また注意点として、書き込みの場合アドレス指定の2byteもこの32byteに含まれます。
 * つまり、1度に書き込み可能な正味の領域サイズは30byteとなります。
 *
 *
 * TASK:
 * ---
 * I2Cのバッファサイズ拡張。
 */
namespace ExternalEEPROM
{
	// 領域境界を1つのスロットとして、1スロットごとに読み込みを行います。
	int readBlock(unsigned int slot, char* data, unsigned int read_size = BLOCK())
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : ExternalEEPROM::readBlock()"));
		#endif

		if (read_size > BLOCK())
		{
			return -1;
		}

		int  slave_address = ADDRESS();
		long data_address  = (long)slot * BLOCK();

		if (data_address >= (SIZE() / 2))
		{
			slave_address |= _BV(SELECT_BIT()); // B0 = 1のメモリブロックを選択
			data_address  -= (SIZE() / 2);
		}

		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print("slave_address : ");
			System::output_serial->println(slave_address, HEX);
			System::output_serial->print("data_address : ");
			System::output_serial->println(data_address, HEX);
		#endif

		Wire.beginTransmission(slave_address);
		Wire.write((byte)(data_address >> 8));     // High側アドレスを指定する
		Wire.write((byte)(data_address & 0x00ff)); // Low側アドレスを指定する
		int ret = Wire.endTransmission();

		if (ret == 0)
		{
			ret = Wire.requestFrom(slave_address, read_size);
			if (ret == read_size)
			{
				for (int index = 0; index < read_size; index++)
				{
					data[index] = Wire.read();
				}
			}
			else
			{
				ret = -1;
			}
		}

		return ret;
	}

	// 領域境界を1つのスロットとして、1スロットごとに書き込みを行います。
	// CAUTION!:
	// ---
	// EEPROMへの書き込みには時間がかかるので、このメソッドの使用後に
	// delay()を5[msec]程度、ハードコーディングしてください。
	int writeBlock(unsigned int slot, char* data, unsigned int write_size = BLOCK())
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : ExternalEEPROM::writeBlock()"));
		#endif

		if (write_size > BLOCK())
		{
			return -1;
		}

		int  slave_address = ADDRESS();
		long data_address  = (long)slot * BLOCK();

		if (data_address >= (SIZE() / 2))
		{
			slave_address |= _BV(SELECT_BIT()); // B0 = 1のメモリブロックを選択
			data_address  -= (SIZE() / 2);
		}

		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print("slave_address : ");
			System::output_serial->println(slave_address, HEX);
			System::output_serial->print("data_address : ");
			System::output_serial->println(data_address, HEX);
		#endif

		Wire.beginTransmission(slave_address);
		Wire.write((byte)(data_address >> 8));     // High側アドレスを指定する
		Wire.write((byte)(data_address & 0x00ff)); // Low側アドレスを指定する

		for (int index = 0; index < write_size; index++)
		{
			Wire.write(*data++);
		}

		return Wire.endTransmission();
	}
}


/**
 * 関節管理メソッド・変数
 * =============================================================================
 * NOTE:
 * ---
 * PWM信号の出力ピンが3本なので、8出力のマルチプレクサと組み合わせることで
 * 計24個までのサーボモータを制御します。
 *
 * サーボモータの許容最小パルス幅は0.78[msec]，許容最大パルス幅は2.18[msec]と、
 * 実測値として確認されています。その際の可動範囲は130[deg]となります。
 * ただし、本ファームウェアでは個体差を考慮して少なめに見積もっています。
 */
namespace Joint
{
	JointSetting SETTINGS[_JOINT__SUM] =
	{
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT00 ([01] 左：肩ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1150 }, // JOINT01 ([02] 左：腿ヨー)
		{ ANGLE_MIN(), ANGLE_MAX(), 1200 }, // JOINT02 ([03] 左：肩ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 800  }, // JOINT03 ([04] 左：肘ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 800  }, // JOINT04 ([05] 左：腿ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 850  }, // JOINT05 ([06] 左：腿ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1400 }, // JOINT06 ([07] 左：膝ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1200 }, // JOINT07 ([08] 左：足首ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 850  }, // JOINT08 ([09] 左：足首ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT09 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT10 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT11 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT12 ([10] 右：肩ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 950  }, // JOINT13 ([11] 右：腿ヨー)
		{ ANGLE_MIN(), ANGLE_MAX(), 550  }, // JOINT14 ([12] 右：肩ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 1100 }, // JOINT15 ([13] 右：肘ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 1000 }, // JOINT16 ([14] 右：腿ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 1100 }, // JOINT17 ([15] 右：腿ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 400  }, // JOINT18 ([16] 右：膝ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 580  }, // JOINT19 ([17] 右：足首ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1000 }, // JOINT20 ([18] 右：足首ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT21 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT22 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }  // JOINT23 (未使用)
	};

	const JointSetting SETTINGS_INITIAL[_JOINT__SUM] =
	{
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT00 ([01] 左：肩ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1150 }, // JOINT01 ([02] 左：腿ヨー)
		{ ANGLE_MIN(), ANGLE_MAX(), 1200 }, // JOINT02 ([03] 左：肩ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 800  }, // JOINT03 ([04] 左：肘ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 800  }, // JOINT04 ([05] 左：腿ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 850  }, // JOINT05 ([06] 左：腿ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1400 }, // JOINT06 ([07] 左：膝ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1200 }, // JOINT07 ([08] 左：足首ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 850  }, // JOINT08 ([09] 左：足首ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT09 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT10 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT11 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT12 ([10] 右：肩ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 950  }, // JOINT13 ([11] 右：腿ヨー)
		{ ANGLE_MIN(), ANGLE_MAX(), 550  }, // JOINT14 ([12] 右：肩ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 1100 }, // JOINT15 ([13] 右：肘ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 1000 }, // JOINT16 ([14] 右：腿ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 1100 }, // JOINT17 ([15] 右：腿ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 400  }, // JOINT18 ([16] 右：膝ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 580  }, // JOINT19 ([17] 右：足首ピッチ)
		{ ANGLE_MIN(), ANGLE_MAX(), 1000 }, // JOINT20 ([18] 右：足首ロール)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT21 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }, // JOINT22 (未使用)
		{ ANGLE_MIN(), ANGLE_MAX(), 900  }  // JOINT23 (未使用)
	};

	// Atmege32u4のEEPROMから、関節の設定情報を読み出します。
	// EEPROM内に値が存在しない場合は、デフォルトの値を書き込みます。
	void init()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Joint::init()"));
		#endif

		unsigned char* filler = (unsigned char*)SETTINGS;
		
		if (EEPROM.read(CONFIG_FLAG_ADDRESS()) != CONFIG_FLAG_VALUE())
		{
			EEPROM.write(CONFIG_FLAG_ADDRESS(), CONFIG_FLAG_VALUE());
			delay(5);

			for (int index = 0; index < sizeof(SETTINGS); index++)
			{
				EEPROM.write(CONFIG_BEGIN_ADDRESS() + index, filler[index]);
				delay(5);
			}
		}
		else
		{
			for (int index = 0; index < sizeof(SETTINGS); index++)
			{
				filler[index] = EEPROM.read(CONFIG_BEGIN_ADDRESS() + index);
			}
		}
	}

	// 指定したサーボモータの関節可動域最小値を、指定した値に設定します。
	void setMinAngle(unsigned char joint_id, unsigned int angle)
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Joint::setMinAngle()"));
		#endif

		if (joint_id >= SUM())
		{
			#ifdef _DEBUG
				System::output_serial->print(F(">>> joint_id : bad argment (value : "));
				System::output_serial->print((int)joint_id);
				System::output_serial->println(F(")"));
			#endif

			return;
		}

		SETTINGS[joint_id].MIN = angle;

		unsigned char* filler = (unsigned char*)&(SETTINGS[joint_id].MIN);
		int address_offset = (int)(filler) - (int)SETTINGS;

		for (int index = 0; index < sizeof(SETTINGS[joint_id].MIN); index++)
		{
			EEPROM.write(CONFIG_BEGIN_ADDRESS() + address_offset + index, filler[index]);
			delay(5);
		}

		#ifdef _DEBUG
			System::output_serial->print(F(">>> address_offset : "));
			System::output_serial->println(address_offset);
		#endif
	}

	// 指定したサーボモータの関節可動域最大値を、指定した値に設定します。
	void setMaxAngle(unsigned char joint_id, unsigned int angle)
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Joint::setMaxAngle()"));
		#endif

		if (joint_id >= SUM())
		{
			#ifdef _DEBUG
				System::output_serial->print(F(">>> joint_id : bad argment (value : "));
				System::output_serial->print((int)joint_id);
				System::output_serial->println(F(")"));
			#endif

			return;
		}

		SETTINGS[joint_id].MAX = angle;

		unsigned char* filler = (unsigned char*)&(SETTINGS[joint_id].MAX);
		int address_offset = (int)(filler) - (int)SETTINGS;

		for (int index = 0; index < sizeof(SETTINGS[joint_id].MAX); index++)
		{
			EEPROM.write(CONFIG_BEGIN_ADDRESS() + address_offset + index, filler[index]);
			delay(5);
		}

		#ifdef _DEBUG
			System::output_serial->print(F(">>> address_offset : "));
			System::output_serial->println(address_offset);
		#endif
	}

	// 指定したサーボモータの関節初期位置を、指定した値に設定します。
	void setHomeAngle(unsigned char joint_id, unsigned int angle)
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Joint::setHomeAngle()"));
		#endif

		if (joint_id >= SUM())
		{
			#ifdef _DEBUG
				System::output_serial->print(F(">>> joint_id : bad argment (value : "));
				System::output_serial->print((int)joint_id);
				System::output_serial->println(F(")"));
			#endif

			return;
		}

		SETTINGS[joint_id].HOME = angle;

		unsigned char* filler = (unsigned char*)&(SETTINGS[joint_id].HOME);
		int address_offset = (int)(filler) - (int)SETTINGS;

		for (int index = 0; index < sizeof(SETTINGS[joint_id].HOME); index++)
		{
			EEPROM.write(CONFIG_BEGIN_ADDRESS() + address_offset + index, filler[index]);
			delay(5);
		}

		#ifdef _DEBUG
			System::output_serial->print(F(">>> address_offset : "));
			System::output_serial->println(address_offset);
		#endif
	}

	// 指定したサーボモータの角度を、指定した値に設定します。
	void setAngle(unsigned char joint_id, unsigned int angle)
	{
		#ifdef _DEBUG_HARD
			System::output_serial->println(F("in fuction : Joint::setAngle()"));
		#endif

		if (joint_id >= SUM())
		{
			#ifdef _DEBUG_HARD
				System::output_serial->print(F(">>> joint_id : bad argment (value : "));
				System::output_serial->print((int)joint_id);
				System::output_serial->println(F(")"));
			#endif

			return;
		}

		Config::frame.joint_angle[joint_id] = angle;
	}
}


/**
 * コンフィグメソッド・変数
 * =============================================================================
 */
namespace Config
{
	bool          enable   = false;

	unsigned char joint_id = 0;
	unsigned int  angle    = 0;

	Motion::HeaderDefinition       header = { 0 };
	Motion::Frame::FrameDefinition frame  = { 0 };

	bool disable()
	{
		return !enable;
	}

	// 保持している関節情報をダンプします。
	void dumpJointSettings()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Config::dumpJointSettings()"));
		#endif

		for (int index = 0; index < Joint::SUM(); index++)
		{
			System::output_serial->print(F("joint : "));
			System::output_serial->println(index);
			System::output_serial->print(F(">>> MAX  : "));
			System::output_serial->println(Joint::SETTINGS[index].MAX);
			System::output_serial->print(F(">>> MIN  : "));
			System::output_serial->println(Joint::SETTINGS[index].MIN);
			System::output_serial->print(F(">>> HOME : "));
			System::output_serial->println(Joint::SETTINGS[index].HOME);
		}
	}

	// 指定したスロットのモーション情報をダンプします。
	void dumpMotion(unsigned char slot)
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Config::dumpMotion()"));
		#endif

		long begin_time = micros();
		Motion::getHeader(slot, &header);
		long end_time = micros();

		char name[21];
		name[20] = '\0';

		System::output_serial->print(F("arg. slot : "));
		System::output_serial->println((int)slot);
		System::output_serial->print(F("exec time [us] : "));
		System::output_serial->println(end_time - begin_time);
		memcpy(name, header.name, 20);
		System::output_serial->print(F("name : "));
		System::output_serial->println(name);
		System::output_serial->print(F("extra[0] : "));
		System::output_serial->println((int)header.extra[0]);
		System::output_serial->print(F("extra[1] : "));
		System::output_serial->println((int)header.extra[1]);
		System::output_serial->print(F("extra[2] : "));
		System::output_serial->println((int)header.extra[2]);
		System::output_serial->print(F("frame_num : "));
		System::output_serial->println((int)header.frame_num);
		System::output_serial->print(F("int. slot : "));
		System::output_serial->println((int)header.slot);

		for (int index = 0; index < header.frame_num; index++)
		{
			begin_time = micros();
			Motion::getFrame(slot, index, &frame);
			end_time = micros();

			System::output_serial->print(F("exec time [us] : "));
			System::output_serial->println(end_time - begin_time);
			System::output_serial->print(F("transition_delay_msec : "));
			System::output_serial->println(frame.transition_delay_msec);
			for (int joint = 0; joint < Joint::SUM(); joint++)
			{
				System::output_serial->print(F("joint ["));
				System::output_serial->print(joint);
				System::output_serial->print(F("] : "));
				System::output_serial->println(frame.joint_angle[joint]);
			}
			System::output_serial->print(F("number : "));
			System::output_serial->println((int)frame.number);
		}
	}

	// 関節情報を初期化します。
	void resetJointSettings()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Config::resetJointSettings()"));
		#endif

		unsigned char* filler = (unsigned char*)Joint::SETTINGS_INITIAL;
		
		EEPROM.write(Joint::CONFIG_FLAG_ADDRESS(), Joint::CONFIG_FLAG_VALUE());
		delay(5);

		for (int index = 0; index < sizeof(Joint::SETTINGS_INITIAL); index++)
		{
			EEPROM.write(Joint::CONFIG_BEGIN_ADDRESS() + index, filler[index]);
			delay(5);
		}

		for (int index = 0; index < Joint::SUM(); index++)
		{
			Joint::SETTINGS[index] = Joint::SETTINGS_INITIAL[index];
		}
	}
}


/**
 * モーション管理メソッド・変数
 * =============================================================================
 * NOTE:
 * ---
 */
namespace Motion
{
	bool _playing = false;
	HeaderDefinition header = { 0 };

	namespace Frame
	{
		unsigned int  transition_count = 0;
		volatile bool _updatable       = false;

		FrameDefinition  buffer[3] = { 0 };
		FrameDefinition* now  = buffer;
		FrameDefinition* next = buffer + 1;
		FrameDefinition* back = buffer + 2;

		long now_fixed_point[_JOINT__SUM]  = { 0 };
		long diff_fixed_point[_JOINT__SUM] = { 0 };

		bool updatable()
		{
			return _updatable;
		}

		bool updateFinished()
		{
			return (transition_count == 0);
		}

		bool nextFrameLoadable()
		{
			if (header.extra[0] != 0) return true;

			return ((Frame::next->number + 1) < header.frame_num);
		}

		void buffering()
		{
			FrameDefinition* temp = now;
			now  = next;
			next = back;
			back = temp;
		}

		void update()
		{
			_updatable = false;
			transition_count--;

			for (int index = 0; index < Joint::SUM(); index++)
			{
				now_fixed_point[index] += diff_fixed_point[index];
				now->joint_angle[index] = now_fixed_point[index] >> 16;
			}
		}
	}

	// モーション関連の設定の初期関数です。
	// Arduinoのsetup()内で読みだしてください。
	void init()
	{
		// タイマ1の設定
		// =========================================================================
		// CAUTION:
		// ---
		// 比較一致の出力はLOWレベルの方が直感的でわかりやすいかと思いますが、
		// それだとマルチプレクサで出力先を切り替える処理の最中にもPWM信号が
		// 出力されているため、出力先切り替え時にインパルスノイズが乗ります。
		cli();

		TCCR1A =
			_BV(WGM11)  | _BV(WGM10)  | // 10bit高速PWM動作に設定
			_BV(COM1A1) | _BV(COM1A0) | // 比較一致でOC1AをHIGHレベルに出力
			_BV(COM1B1) | _BV(COM1B0) | // 比較一致でOC1BをHIGHレベルに出力
			_BV(COM1C1) | _BV(COM1C0);  // 比較一致でOC1CをHIGHレベルに出力

		TCCR1B = 
			_BV(WGM12) |                // 10bit高速PWM動作に設定
			_BV(CS11)  | _BV(CS10);     // 前置分周64に設定

		TIFR1 = _BV(OCF1A) | _BV(OCF1B) | _BV(OCF1C) | _BV(TOV1); // 割り込みフラグをクリア

		for (int index = 0; index < Joint::SUM(); index++)
		{
			Frame::now->joint_angle[index] = Joint::SETTINGS[index].HOME;
		}

		sei();
		TIMSK1 = _BV(TOIE1); // タイマ1、割り込み開始
	}

	void play(unsigned char motion_slot)
	{
		_playing = true;

		getHeader(motion_slot, &header);
		getFrame(motion_slot, 0, Frame::next);

		if (header.frame_num > FLAMENUM_MIN())
		{
			getFrame(motion_slot, 1, Frame::back);

			for (int index = 0; index < Joint::SUM(); index++)
			{
				Frame::back->joint_angle[index] += Joint::SETTINGS[index].HOME;
			}
		}

		Frame::transition_count = Frame::next->transition_delay_msec / Frame::UPDATE_MSEC();

		for (int index = 0; index < Joint::SUM(); index++)
		{
			Frame::next->joint_angle[index] += Joint::SETTINGS[index].HOME;

			Frame::now_fixed_point[index]  = (long)(Frame::now->joint_angle[index]) << 16;
			Frame::diff_fixed_point[index] = (((long)(Frame::next->joint_angle[index]) << 16) - Frame::now_fixed_point[index]);
			Frame::diff_fixed_point[index] /= Frame::transition_count;
		}
	}

	void stop()
	{
		_playing = false;
	}

	bool playing()
	{
		return _playing;
	}

	// モーション定義情報を外部EEPROMへ書き込みます。
	// TASK: 定数定義の分離
	// CAUTION: ヘッダーサイズの変化に弱い実装
	void setHeader(HeaderDefinition* p_header)
	{
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::setHeader()"));
		#endif

		char* filler = (char*)p_header;

		int ret = ExternalEEPROM::writeBlock((int)p_header->slot * 41, filler, sizeof(*p_header) /* = 25byte */);
		delay(10);

		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print(F("return : "));
			System::output_serial->println(ret);
		#endif
	}

	// モーション定義情報を外部EEPROMから読み込みます。
	// TASK: 定数定義の分離
	// CAUTION: ヘッダーサイズの変化に弱い実装
	void getHeader(unsigned char motion_slot, HeaderDefinition* p_header)
	{
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::getHeader()"));
		#endif

		char* filler = (char*)p_header;

		int ret = ExternalEEPROM::readBlock((int)motion_slot * 41, filler, sizeof(*p_header) /* = 25byte */);

		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print(F("return : "));
			System::output_serial->println(ret);
		#endif

		#ifdef _DEBUG_MOTION
			System::output_serial->print("slot(arg) : ");
			System::output_serial->println(motion_slot);
			System::output_serial->print("slot(get) : ");
			System::output_serial->println(p_header->slot);
		#endif
	}

	// フレームを外部EEPROMへ書き込みます。
	// TASK: 定数定義の分離
	// CAUTION: フレームサイズの変化に弱い実装
	void setFrame(unsigned char motion_slot, Frame::FrameDefinition* p_frame)
	{
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::setFrame()"));
		#endif

		char* filler = (char*)p_frame;

		int ret = ExternalEEPROM::writeBlock((int)motion_slot * 41 + 1 + (int)p_frame->number * 2, filler, 30);
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [0] : "));
			System::output_serial->println(ret);
		#endif
		delay(10);

		ret = ExternalEEPROM::writeBlock((int)motion_slot * 41 + 1 + (int)p_frame->number * 2 + 1, filler + 30, 21);
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [1] : "));
			System::output_serial->println(ret);
		#endif
		delay(10);
	}

	// フレームを外部EEPROMから読み込みます。
	// TASK: 定数定義の分離
	// CAUTION: フレームサイズの変化に弱い実装
	void getFrame(unsigned char motion_slot, unsigned char frame_num, Frame::FrameDefinition* p_frame)
	{
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::getFrame()"));
		#endif

		char* filler = (char*)p_frame;

		int ret = ExternalEEPROM::readBlock((int)motion_slot * 41 + 1 + (int)frame_num * 2, filler, 30);
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [0] : "));
			System::output_serial->println(ret);
		#endif

		ret = ExternalEEPROM::readBlock((int)motion_slot * 41 + 1 + (int)frame_num * 2 + 1, filler + 30, 21);
		#ifdef _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [1] : "));
			System::output_serial->println(ret);
		#endif
	}
}


/**
 * コードインタプリタの定義
 * =============================================================================
 */
namespace Code
{
	inline static const int CODE_SIZE() { return 2;  }
	inline static const int STACK_MAX() { return 64; }

	class CodeModel {
	public:
		unsigned char slot;
		unsigned char loop;
	};

	CodeModel codes[64] = { 0 };

	char stack = 0;
	char stack_index = 0;
	bool _running = false;

	unsigned char _loop;

	void init()
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::init()"));
		#endif

		stack = 0;
		stack_index = 0;
		_running = false;
	}

	bool running()
	{
		return _running;
	}

	bool setCode(unsigned char slot, unsigned char loop)
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::setCode()"));
		#endif

		if (stack == STACK_MAX())
		{
			return false;
		}

		stack++;

		#ifdef _DEBUG_CODE
			System::output_serial->print(F(">>> stack : "));
			System::output_serial->println((int)stack);

			System::output_serial->print(F(">>> slot : "));
			System::output_serial->println((int)slot);

			System::output_serial->print(F(">>> loop : "));
			System::output_serial->println((int)loop);
		#endif

		codes[stack - 1].slot = slot;
		codes[stack - 1].loop = loop;

		return true;
	}

	bool getCode(unsigned char& slot, unsigned char& loop)
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::getCode()"));
		#endif

		if (stack == 0)
		{
			return false;
		}

		slot = codes[stack_index].slot;
		loop = codes[stack_index].loop;
		stack_index++;

		#ifdef _DEBUG_CODE
			System::output_serial->print(F(">>> stack : "));
			System::output_serial->println((int)stack);

			System::output_serial->print(F(">>> slot : "));
			System::output_serial->println((int)slot);

			System::output_serial->print(F(">>> loop : "));
			System::output_serial->println((int)loop);
		#endif

		stack--;

		return true;
	}

	void run()
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::run()"));
		#endif

		if (!Motion::playing())
		{
			stack_index = 0;
			unsigned char slot;

			if (getCode(slot, _loop))
			{
				#ifdef _DEBUG_CODE
					System::output_serial->println(F(">>> getCode() == true"));
				#endif

				Motion::play(slot);
				if (Motion::header.extra[0] != 1)
				{
					Motion::header.extra[0] = 1;
					Motion::header.extra[1] = 0;
					Motion::header.extra[2] = Motion::header.frame_num - 1;
				}

				_running = true;
			}
		}
	}
}

namespace Utility {
	bool stop = false;

	void motionStopHelper()
	{
		stop = true;
	}

	void motionPlayHelper()
	{
		stop = false;
	}
}

/**
 * コマンドパーサの定義
 * =============================================================================
 * NOTE:
 * ---
 * 本ファームウェアにおけるパーサは、Buffer, Token, State, Packet, および
 * それらを操作するためのメソッド群から構成されます。
 *
 * 一般的なオートマトンにおけるトークンの実態が2種類存在し、1つはToken::STR, 
 * もう1つはPacket::DEFINITIONSです。これらの違いは、文字列が実態か，
 * それとも任意のバイト列が実態か、ということです。
 *
 * 状態遷移表はState::TRANSITIONにディスプレイとして定義されています。
 * ここで、メモリサイズを節約するため、バイト列が実態のトークン識別子を0とし、
 * 各遷移テーブルのサイズを最小限に抑えています。
 *
 * メインループでの処理を簡便にするため、字句解析機にあたるlexAccept()メソッドが
 * 内部状態に依存する挙動を行っています。
 *
 * ユーザが編集する必要があるのは、基本的にexecEventHandler()メソッドだけです。
 */
namespace Purser
{
	namespace Buffer
	{
		char data[_BUFFER__LENGTH] = { 0 };
		char position = 0;
	}

	namespace Token
	{
		typedef enum
		{
			OTHER,
			MOTION_PLAY_COMMAND,         // [00] モーションの再生
			SET_CODE_COMMAND,            // [12] コードの設定コマンド
			CODE_RUN_COMMAND,            // [01] コードの実行
			STOP_COMMAND,                // [02] 停止
			MOTION_INSTALL_COMMAND,      // [03] モーションデータのインストール
			JOINT_MIN_SETTING_COMMAND,   // [04] 関節可動域最小値の設定
			JOINT_MAX_SETTING_COMMAND,   // [05] 関節可動域最大値の設定
			JOINT_HOME_SETTING_COMMAND,  // [06] 関節初期位置の設定
			JOINT_MOVE_COMMAND,          // [07] 関節可動命令
			SERIAL_TOGGLE_COMMAND,       // [08] 読み込みシリアルの切り替え
			DUMP_JOINT_SETTING_COMMAND,  // [09] 関節設定のダンプコマンド
			DUMP_MOTION_COMMAND,         // [10] モーションのダンプコマンド
			RESET_JOINT_SETTING_COMMAND, // [11] 関節設定のリセットコマンド
			PURSER_TOKEN_EOF,
			PURSER_ERROR
		} PurserToken;

		const char* STR[] =
		{
			"OTHER",
			"$MP", // [00] モーションの再生
			"#SC", // [12] コードの設定コマンド
			"$CR", // [01] コードの実行
			"$MS", // [02] 停止
			"#IN", // [03] モーションデータのインストール
			"#MI", // [04] 関節可動域最小値の設定
			"#MA", // [05] 関節可動域最大値の設定
			"#HO", // [06] 関節初期位置の設定
			"#SA", // [07] 関節可動命令
			"###", // [08] 読み込みシリアルの切り替え
			"#DJ", // [09] 関節設定のダンプコマンド
			"#DM", // [10] モーションのダンプコマンド
			"#RJ"  // [11] 関節設定のリセットコマンド
		};

		PurserToken initial = OTHER;
		PurserToken now     = OTHER;
	}

	namespace State
	{
		typedef enum
		{
			INIT,
			SLOT_ELEMENT_INCOMING,
			NAME_ELEMENT_INCOMING,
			EXTRA_ELEMENT_INCOMING,
			FRAMENUM_ELEMENT_INCOMING,
			FRAME_ELEMENT_INCOMING,
			JOINT_ID_ELEMENT_INCOMING,
			ANGLE_ELEMENT_INCOMING,
			MOTION_SLOT_ELEMENT_INCOMING,
			CODE_ELEMENT_INCOMING,
			PURSER_STATE_EOF
		} PurserState;

		#ifdef _DEBUG
			const char* STR[] =
			{
				"INIT",
				"SLOT_ELEMENT_INCOMING",
				"NAME_ELEMENT_INCOMING",
				"EXTRA_ELEMENT_INCOMING",
				"FRAMENUM_ELEMENT_INCOMING",
				"FRAME_ELEMENT_INCOMING",
				"JOINT_ID_ELEMENT_INCOMING",
				"ANGLE_ELEMENT_INCOMING",
				"MOTION_SLOT_ELEMENT_INCOMING",
				"CODE_ELEMENT_INCOMING"
			};
		#endif

		const PurserState TRANSITION_INIT[] =
		{
			INIT,                         // トークン"OTHER"入力時の状態遷移先
			MOTION_SLOT_ELEMENT_INCOMING, // トークン"MOTION_PLAY_COMMAND"入力時の状態遷移先
			CODE_ELEMENT_INCOMING,        // トークン"SET_CODE_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"CODE_RUN_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"STOP_COMMAND"入力時の状態遷移先
			SLOT_ELEMENT_INCOMING,        // トークン"MOTION_INSTALL_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_MIN_SETTING_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_MAX_SETTING_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_HOME_SETTING_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_MOVE_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"SERIAL_TOGGLE_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"DUMP_JOINT_SETTING_COMMAND"入力時の状態遷移先
			MOTION_SLOT_ELEMENT_INCOMING, // トークン"DUMP_MOTION_COMMAND"入力時の状態遷移先
			INIT                          // トークン"RESET_JOINT_SETTING_COMMAND"入力時の状態遷移先
		};

		const PurserState TRANSITION_SLOT_ELEMENT_INCOMING[] =
		{
			NAME_ELEMENT_INCOMING          // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_NAME_ELEMENT_INCOMING[] =
		{
			EXTRA_ELEMENT_INCOMING         // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_EXTRA_ELEMENT_INCOMING[] =
		{
			FRAMENUM_ELEMENT_INCOMING      // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_FRAMENUM_ELEMENT_INCOMING[] =
		{
			FRAME_ELEMENT_INCOMING         // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_FRAME_ELEMENT_INCOMING[] =
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_JOINT_ID_ELEMENT_INCOMING[] =
		{
			ANGLE_ELEMENT_INCOMING         // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_ANGLE_ELEMENT_INCOMING[] =
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_MOTION_SLOT_ELEMENT_INCOMING[] =
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_CODE_ELEMENT_INCOMING[] = 
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState* TRANSITION[] =
		{
			TRANSITION_INIT,
			TRANSITION_SLOT_ELEMENT_INCOMING,
			TRANSITION_NAME_ELEMENT_INCOMING,
			TRANSITION_EXTRA_ELEMENT_INCOMING,
			TRANSITION_FRAMENUM_ELEMENT_INCOMING,
			TRANSITION_FRAME_ELEMENT_INCOMING,
			TRANSITION_JOINT_ID_ELEMENT_INCOMING,
			TRANSITION_ANGLE_ELEMENT_INCOMING,
			TRANSITION_MOTION_SLOT_ELEMENT_INCOMING,
			TRANSITION_CODE_ELEMENT_INCOMING
		};

		PurserState now = INIT;
	}

	namespace Packet
	{
		struct PacketDefinition
		{
			const char bytes; // パケットサイズ
			int count;        // パケット受信予定数 (-1を定義した場合、プレースホルダと見なす)
		};

		PacketDefinition DEFINITIONS[] =
		{
			{ 3,  1  }, // 状態"INIT"において到着するパケット
			{ 2,  1  }, // 状態"SLOT_ELEMENT_INCOMING"において到着するパケット
			{ 20, 1  }, // 状態"NAME_ELEMENT_INCOMING"において到着するパケット
			{ 2,  3  }, // 状態"EXTRA_ELEMENT_INCOMING"において到着するパケット
			{ 2,  1  }, // 状態"FRAME_NUM_ELEMENT_INCOMING"において到着するパケット
			{ 4,  -1 }, // 状態"FRAME_ELEMENT_INCOMING"において到着するパケット
			{ 2,  1  }, // 状態"JOINT_ID_ELEMENT_INCOMING"において到着するパケット
			{ 3,  1  }, // 状態"ANGLE_ELEMENT_INCOMING"において到着するパケット
			{ 2,  1  }, // 状態"MOTION_SLOT_ELEMENT_INCOMING"において到着するパケット
			{ 2,  2  }  // 状態"CODE_ELEMENT_INCOMING"において到着するパケット
		};
	}

	// パーサーの初期化を行います。
	// arduinoのsetup()内で呼ぶ必要はありません。
	void init()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::init()"));
		#endif

		memset(Buffer::data, '\0', Buffer::LENGTH()); // Sanity check.
		Buffer::position = 0;
		
		State::now = State::INIT;
		Token::now = Token::OTHER;

		Packet::DEFINITIONS[State::INIT].count                          = 1;
		Packet::DEFINITIONS[State::SLOT_ELEMENT_INCOMING].count         = 1;
		Packet::DEFINITIONS[State::NAME_ELEMENT_INCOMING].count         = 1;
		Packet::DEFINITIONS[State::EXTRA_ELEMENT_INCOMING].count        = 3;
		Packet::DEFINITIONS[State::FRAMENUM_ELEMENT_INCOMING].count     = 1;
		Packet::DEFINITIONS[State::FRAME_ELEMENT_INCOMING].count        = -1;
		Packet::DEFINITIONS[State::JOINT_ID_ELEMENT_INCOMING].count     = 1;
		Packet::DEFINITIONS[State::ANGLE_ELEMENT_INCOMING].count        = 1;
		Packet::DEFINITIONS[State::MOTION_SLOT_ELEMENT_INCOMING].count  = 1;
		Packet::DEFINITIONS[State::CODE_ELEMENT_INCOMING].count         = 2;
	}

	// 1バイトをリングバッファ(もどき)に格納します。
	// 本来リングバッファにする必要はないので、オーバーヘッドが気になる場合は
	// 内部のif文を削除してください。
	void readByte(char byte)
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::readByte()"));
		#endif

		if ((State::now == State::INIT) && (Buffer::position == 0))
		{
			if ((byte != '#') && (byte != '$')) return;
		}

		if (Buffer::position == Buffer::LENGTH()) Buffer::position = 0; // Sanity check.

		Buffer::data[Buffer::position++] = byte;
	}

	// 字句解析を行い、入力された文字列を受理するか判定します。
	// ユーザが内部を編集する必要は基本的にありません。
	bool lexAccept()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::lexAccept()"));
		#endif

		bool bytes_accepted = (Buffer::position == Packet::DEFINITIONS[State::now].bytes);

		if (bytes_accepted && (State::now == State::INIT))
		{
			bool token_accepted = false;

			for (
				int token = Token::MOTION_PLAY_COMMAND;
				token != Token::PURSER_TOKEN_EOF;
				token++
			)
			{
				if (strncmp(Buffer::data, Token::STR[token], Packet::DEFINITIONS[State::INIT].bytes) == 0)
				{
					Token::now = (Token::PurserToken)token;
					token_accepted = true;
				}
			}

			Buffer::position = 0;

			return token_accepted;
		}

		Token::now = Token::OTHER;

		return bytes_accepted;
	}

	// トークン履歴を作成します。
	// 先に到着したトークンに依存する処理を実現するために必要です。
	void makeTokenLog()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::makeTokenLog()"));
		#endif

		// コマンド受理時のトークンを履歴に保持
		if (State::now == State::INIT)
		{
			Token::initial = Token::now;
		}
	}

	// パーサーのエラー時処理を委譲します。
	void errorAbort()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::errorAbort()"));
		#endif

		Token::now = Token::PURSER_ERROR;
	}

	// 状態ごとのイベントハンドラを実行します。
	// 各状態に対応して処理をcase文内部に記述してください。
	void execEventHandler()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::execEventHandle()"));
		#endif

		// 到着バイト列の末尾にnull文字を入れることで文字列化
		Buffer::data[Packet::DEFINITIONS[State::now].bytes] = '\0';

		switch (State::now)
		{
			case State::INIT:
			{
				switch (Token::initial)
				{
					// これらのコマンドはコンフィグフレームを描画するために、共通のswitch文に通す。
					case Token::JOINT_MIN_SETTING_COMMAND:
					case Token::JOINT_MAX_SETTING_COMMAND:
					case Token::JOINT_HOME_SETTING_COMMAND:
					case Token::JOINT_MOVE_COMMAND:
					{
						// 非コンフィグモードからコンフィグモードに移ったときのみ、コンフィグフレームを初期化
						if (Config::enable == false)
						{
							Utility::motionPlayHelper();

							for (int index = 0; index < Joint::SUM(); index++)
							{
								Config::frame.joint_angle[index] = Joint::SETTINGS[index].HOME;
							}

							Config::enable = true;
						}

						break;
					}

					case Token::MOTION_INSTALL_COMMAND:
					{
						// 割り込みの優先順序を考慮し、モーションの再生を中断
						Utility::motionStopHelper();

						break;
					}

					case Token::SERIAL_TOGGLE_COMMAND:
					{
						System::toggleInputSerial();

						break;
					}

					case Token::DUMP_JOINT_SETTING_COMMAND:
					{
						Config::dumpJointSettings();

						break;
					}

					case Token::DUMP_MOTION_COMMAND:
					{
						// 割り込みの優先順位を考慮し、モーションの再生を中断
						Utility::motionStopHelper();

						break;
					}

					case Token::RESET_JOINT_SETTING_COMMAND:
					{
						Config::resetJointSettings();

						break;
					}

					case Token::CODE_RUN_COMMAND:
					{
						if (!Code::running())
						{
							Code::run();
						}

						break;
					}

					case Token::STOP_COMMAND:
					{
						if (Motion::playing())
						{
							Motion::header.extra[0] = 0;
						}

						break;
					}

					default:
					{
						Config::enable = false;

						break;
					}
				}

				break;
			}

			case State::SLOT_ELEMENT_INCOMING:
			{
				unsigned char slot = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (slot > Motion::SLOT_MAX())
				{
					#ifdef _DEBUG_INSTALL
						System::output_serial->println(F(">>> slot : bad argment (value : "));
						System::output_serial->print((int)slot);
						System::output_serial->println(F(")"));
					#endif

					Config::header.slot = Motion::SLOT_MAX();
				}
				else
				{
					Config::header.slot = slot;
				}

				break;
			}

			case State::NAME_ELEMENT_INCOMING:
			{
				memcpy(Config::header.name, Buffer::data, 20); // TASK: 定数定義の分離

				break;
			}

			case State::EXTRA_ELEMENT_INCOMING:
			{
				unsigned char extra = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());
				Config::header.extra[3 - Packet::DEFINITIONS[State::now].count] = extra; // TASK: 定数定義の分離

				break;
			}

			case State::FRAMENUM_ELEMENT_INCOMING:
			{
				unsigned char frame_num = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (frame_num > Motion::FLAMENUM_MAX())
				{
					#ifdef _DEBUG_INSTALL
						System::output_serial->println(F(">>> frame_num : bad argment (value : "));
						System::output_serial->print((int)frame_num);
						System::output_serial->println(F(")"));
					#endif

					Config::header.frame_num = Motion::FLAMENUM_MAX();
				}
				else if (frame_num < Motion::FLAMENUM_MIN())
				{
					#ifdef _DEBUG_INSTALL
						System::output_serial->println(F(">>> frame_num : bad argment (value : "));
						System::output_serial->print((int)frame_num);
						System::output_serial->println(F(")"));
					#endif

					Config::header.frame_num = Motion::FLAMENUM_MIN();

					for (int index = 0; index < Joint::SUM(); index++)
					{
						Config::frame.joint_angle[index] = Joint::SETTINGS[index].HOME;
					}

					Motion::setHeader(&Config::header);
					Motion::setFrame(Config::header.slot, &Config::frame);

					errorAbort();

					break;
				}
				else 
				{
					Config::header.frame_num = frame_num;
				}

				Motion::setHeader(&Config::header);

				#ifdef _DEBUG_INSTALL
					char name[22];
					name[20] = '|';
					name[21] = '\0';
					memcpy(name, Config::header.name, 20);

					System::output_serial->print(F("name : "));
					System::output_serial->println(name);
					System::output_serial->print(F("extra[0] : "));
					System::output_serial->println((int)Config::header.extra[0]);
					System::output_serial->print(F("extra[1] : "));
					System::output_serial->println((int)Config::header.extra[1]);
					System::output_serial->print(F("extra[2] : "));
					System::output_serial->println((int)Config::header.extra[2]);
					System::output_serial->print(F("frame_num : "));
					System::output_serial->println((int)Config::header.frame_num);
					System::output_serial->print(F("int. slot : "));
					System::output_serial->println((int)Config::header.slot);
				#endif

				Config::frame.number = 0;

				// FRAME_ELEMENTの到着数を適切な数値に置換
				Packet::DEFINITIONS[State::FRAME_ELEMENT_INCOMING].count = 25 * Config::header.frame_num; // TASK: 定数定義の置換

				break;
			}

			case State::FRAME_ELEMENT_INCOMING:
			{
				static int frame_count = 0;

				if (frame_count == 0)
				{
					unsigned int transition_delay_msec = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());
					Config::frame.transition_delay_msec = transition_delay_msec;
				}
				else
				{
					int joint = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());
					Config::frame.joint_angle[frame_count - 1] = joint;
				}

				frame_count++;

				if (frame_count == 25)
				{
					Motion::setFrame(Config::header.slot, &Config::frame);

					#ifdef _DEBUG_INSTALL
						System::output_serial->print(F("transition_delay_msec : "));
						System::output_serial->println(Config::frame.transition_delay_msec);
						for (int joint = 0; joint < Joint::SUM(); joint++)
						{
							System::output_serial->print(F("joint ["));
							System::output_serial->print(joint);
							System::output_serial->print(F("] : "));
							System::output_serial->println(Config::frame.joint_angle[joint]);
						}
						System::output_serial->print(F("number : "));
						System::output_serial->println((int)Config::frame.number);
					#endif

					frame_count = 0;
					Config::frame.number++;
				}

				break;
			}

			case State::JOINT_ID_ELEMENT_INCOMING:
			{
				Config::joint_id = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (Config::joint_id > Joint::SUM())
				{
					Config::joint_id = Joint::SUM();
				}

				#ifdef _DEBUG
					System::output_serial->print(F(">>> joint id : "));
					System::output_serial->println((int)Config::joint_id);
				#endif

				break;
			}

			case State::ANGLE_ELEMENT_INCOMING:
			{
				Config::angle = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				switch (Token::initial)
				{
					case Token::JOINT_MIN_SETTING_COMMAND:
					{
						Joint::setMinAngle(Config::joint_id, Config::angle);

						break;
					}

					case Token::JOINT_MAX_SETTING_COMMAND:
					{
						Joint::setMaxAngle(Config::joint_id, Config::angle);

						break;
					}

					case Token::JOINT_HOME_SETTING_COMMAND:
					{
						Joint::setHomeAngle(Config::joint_id, Config::angle);

						break;
					}

					case Token::JOINT_MOVE_COMMAND:
					{
						Joint::setAngle(Config::joint_id, Config::angle);

						break;
					}

					default:
					{
						break;
					}
				}

				#ifdef _DEBUG
					System::output_serial->print(F(">>> angle : "));
					System::output_serial->println(Config::angle);
				#endif

				break;
			}

			case State::MOTION_SLOT_ELEMENT_INCOMING:
			{
				unsigned char motion_slot = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (motion_slot > Motion::SLOT_MAX())
				{
					motion_slot = Motion::SLOT_MAX();
				}

				#ifdef _DEBUG
					System::output_serial->print(F(">>> motion num : "));
					System::output_serial->println(motion_slot);
				#endif

				switch (Token::initial)
				{
					case Token::DUMP_MOTION_COMMAND:
					{
						Config::dumpMotion(motion_slot);

						break;
					}

					case Token::MOTION_PLAY_COMMAND:
					{
						if (!Motion::playing())
						{
							Utility::motionPlayHelper();
							Motion::play(motion_slot);
						}

						break;
					}

					default:
					{
						break;
					}
				}

				break;
			}

			case State::CODE_ELEMENT_INCOMING:
			{
				if (Code::running())
				{
					return;
				}

				static unsigned char call_count = 0;
				static unsigned char motion_slot;
				static unsigned char loop_count;

				if (call_count == 0)
				{
					motion_slot = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

					if (motion_slot > Motion::SLOT_MAX())
					{
						motion_slot = Motion::SLOT_MAX();
					}

					#ifdef _DEBUG
						System::output_serial->print(F(">>> motion slot : "));
						System::output_serial->println((int)motion_slot);
					#endif

					call_count++;
				}
				else if (call_count == 1)
				{
					loop_count = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

					#ifdef _DEBUG
						System::output_serial->print(F(">>> loop count : "));
						System::output_serial->println((int)loop_count);
					#endif

					Code::setCode(motion_slot, loop_count);
					call_count = 0;
				}

				break;
			}

			default:
			{
				#ifdef _DEBUG
					System::output_serial->print(F(">>> token now : "));
					System::output_serial->println(Token::STR[Token::now]);
				#endif

				break;
			}
		}
	}

	// 遷移テーブルを参照して状態遷移する関数です。
	// 内部を編集する必要は基本的にありません。
	void transition()
	{
		#ifdef _DEBUG
			System::output_serial->println(F("in fuction : Purser::transition()"));
			System::output_serial->print(F(">>> state now : "));
			System::output_serial->println(State::STR[State::now]);
		#endif

		if (Token::now == Token::PURSER_ERROR)
		{
			init();

			return;
		}

		Buffer::position = 0;

		if (--Packet::DEFINITIONS[State::now].count != 0)
		{
			#ifdef _DEBUG
				System::output_serial->print(F(">>> packet count : "));
				System::output_serial->println(Packet::DEFINITIONS[State::now].count);
			#endif

			return;
		}

		State::now = State::TRANSITION[State::now][Token::now];

		if (State::now == State::INIT)
		{
			init();
		}
	}
}


/**
 * 初期化メソッド
 * =============================================================================
 * NOTE:
 * ---
 * 必要な初期化処理はこちらに記述してください。
 *
 *
 * CAUTION!:
 * ---
 * デジタルピンの出力は初期レベルを与えない場合不定となります。
 * モード設定を行うようなピンの初期化は必ず行ってください。
 */
void setup()
{
	_SYSTEM__USBSERIAL.begin(System::USBSERIAL_BAUDRATE());
	_SYSTEM__BLESERIAL.begin(System::BLESERIAL_BAUDRATE());
	Wire.begin();
	Wire.setClock(ExternalEEPROM::CLOCK());

	pinMode(Pin::MULTIPLEXER_SELECT0(), OUTPUT);
	pinMode(Pin::MULTIPLEXER_SELECT1(), OUTPUT);
	pinMode(Pin::MULTIPLEXER_SELECT2(), OUTPUT);
	pinMode(Pin::PWM_OUT_00_07(),       OUTPUT);
	pinMode(Pin::PWM_OUT_08_15(),       OUTPUT);
	pinMode(Pin::PWM_OUT_16_23(),       OUTPUT);
	pinMode(Pin::RS485_TXD(),           OUTPUT);
	pinMode(Pin::LED_OUT(),             OUTPUT);

	digitalWrite(Pin::RS485_TXD(), LOW);
	digitalWrite(Pin::LED_OUT(),   LOW);

	Joint::init();
	Motion::init();
	Code::init();
}


/**
 * メインポーリングループ
 * =============================================================================
 * NOTE:
 * ---
 * 繰り返し実行したい処理はこちらに記述してください。
 *
 *
 * CAUTION!:
 * ---
 * 動作を強制終了したい場合、exit()命令を使用する必要があります。
 * return命令では、再度ループが実行されてしまいます。
 */
void loop()
{
	if (Config::disable() && Motion::playing() && !Code::running())
	{
		if (Motion::Frame::updatable())
		{
			Motion::Frame::update();
		}

		if (Motion::Frame::updateFinished())
		{
			if (Motion::Frame::nextFrameLoadable())
			{
				Motion::Frame::buffering();
				Motion::Frame::transition_count = Motion::Frame::next->transition_delay_msec / Motion::Frame::UPDATE_MSEC();

				for (int index = 0; index < Joint::SUM(); index++)
				{
					Motion::Frame::now_fixed_point[index]  = (long)(Motion::Frame::now->joint_angle[index]) << 16;
					Motion::Frame::diff_fixed_point[index] = ((long)(Motion::Frame::next->joint_angle[index]) << 16) - Motion::Frame::now_fixed_point[index];
					Motion::Frame::diff_fixed_point[index] /= Motion::Frame::transition_count;
				}

				if (   (Motion::header.extra[0] == 1)
					&& (Motion::Frame::next->number == Motion::header.extra[2]))
				{
					Motion::getFrame(Motion::header.slot, Motion::header.extra[1], Motion::Frame::back);
				}
				else if (   (Motion::header.extra[0] == 2)
					     && (Motion::Frame::next->number == (Motion::header.frame_num - 1)))
				{
					Motion::getFrame(Motion::header.extra[1], 0, Motion::Frame::back);
				}
				else
				{
					Motion::getFrame(Motion::header.slot, Motion::Frame::next->number + 1, Motion::Frame::back);
				}

				for (int index = 0; index < Joint::SUM(); index++)
				{
					Motion::Frame::back->joint_angle[index] += Joint::SETTINGS[index].HOME;
				}
			}
			else
			{
				Motion::stop();
			}
		}
	}

	if (Config::disable() && Motion::playing() && Code::running())
	{
		if (Motion::Frame::updatable())
		{
			Motion::Frame::update();
		}

		if (Motion::Frame::updateFinished())
		{
			#ifdef _DEBUG_CODE
				System::output_serial->println(F(">>> Motion::Frame::updateFinished() == true"));
			#endif

			if (Motion::Frame::nextFrameLoadable())
			{
				Motion::Frame::buffering();
				Motion::Frame::transition_count = Motion::Frame::next->transition_delay_msec / Motion::Frame::UPDATE_MSEC();

				for (int index = 0; index < Joint::SUM(); index++)
				{
					Motion::Frame::now_fixed_point[index]  = (long)(Motion::Frame::now->joint_angle[index]) << 16;
					Motion::Frame::diff_fixed_point[index] = ((long)(Motion::Frame::next->joint_angle[index]) << 16) - Motion::Frame::now_fixed_point[index];
					Motion::Frame::diff_fixed_point[index] /= Motion::Frame::transition_count;
				}
				
				if (   (Motion::header.extra[0] == 1)
					&& (Motion::Frame::next->number == Motion::header.extra[2])
					&& (Code::_loop >= 1))
				{
					Code::_loop--;

					if (Code::_loop == 0)
					{
						Motion::header.extra[0] = 0;
					}

					Motion::getFrame(Motion::header.slot, Motion::header.extra[1], Motion::Frame::back);
				}
				else
				{
					Motion::getFrame(Motion::header.slot, Motion::Frame::next->number + 1, Motion::Frame::back);
				}

				for (int index = 0; index < Joint::SUM(); index++)
				{
					Motion::Frame::back->joint_angle[index] += Joint::SETTINGS[index].HOME;
				}
			}
			else
			{
				unsigned char slot;

				if (Code::getCode(slot, Code::_loop))
				{
					#ifdef _DEBUG_CODE
						System::output_serial->println(F(">>> next code."));
					#endif

					Motion::play(slot);
					if (Motion::header.extra[0] != 1)
					{
						Motion::header.extra[0] = 1;
						Motion::header.extra[1] = 0;
						Motion::header.extra[2] = Motion::header.frame_num - 1;
					}
				}
				else
				{
					#ifdef _DEBUG_CODE
						System::output_serial->println(F(">>> code end."));
					#endif

					Motion::stop();
					Code::_running = false;
				}
			}
		}
	}

	if (_SYSTEM__USBSERIAL.available())
	{
		Purser::readByte(_SYSTEM__USBSERIAL.read());

		if (Purser::lexAccept())
		{
			Purser::makeTokenLog();
			Purser::execEventHandler();
			Purser::transition();
		}
	}

	if (_SYSTEM__BLESERIAL.available())
	{
		Purser::readByte(_SYSTEM__BLESERIAL.read());

		if (Purser::lexAccept())
		{
			Purser::makeTokenLog();
			Purser::execEventHandler();
			Purser::transition();
		}
	}
}


/**
 * タイマ1 オーバーフロー割り込みベクタ
 * =============================================================================
 * NOTE:
 * ---
 * 実行タイミングはTCNT1がオーバーフローしたときです。
 * PLEN2の場合では、16[MHz]に64分周をかけ、かつ10bitのカウンタを使用するので、
 * (16,000,000 / (64 * 1,024))^-1 * 1,000 = 4.096[msec]ごとに割り込まれます。
 *
 * この4.096[msec]という値は、サーボモータのPWM信号許容入力間隔に対して
 * 十分に小さな値です。そこで、各サーボモータに対して割り込み8回に1回の割合で
 * PWM信号を入力し、割り込み毎に出力先のサーボモータを切り替えることで、
 * 複数のサーボモータの制御を実現しています。
 *
 *
 * CAUTION!:
 * ---
 * 内部で変更される変数は、基本的にvolatile修飾子をつけるのが無難です。
 * コンパイラでの最適化による、わかりづらいバグを防ぐことができます。
 *
 * AVRマイコンではPWM信号の出力がダブルバッファリングによって制御されるため、
 * PWM信号の出力値を関節値の参照先より1つ先読みする必要があります。
 * この操作を怠った場合、複数のサーボモータ制御は意図しない挙動をするので、
 * 十分に注意してください。
 *
 *
 * TASK:
 * ---
 * モーションクラスへの統合，描画フレームの固定
 */
ISR(TIMER1_OVF_vect)
{
	#ifdef _DEBUG_HARD
		System::output_serial->println(F("in vector : TIMER1_OVF_vect"));
	#endif

	volatile static unsigned char output_select = 0;
	volatile static unsigned char joint_select  = 1; // ダブルバッファリングを考慮し1つ先読み

	if (output_select == Multiplexer::SELECTABLE_NUM())
	{
		output_select = 0;
	}

	if (joint_select == Multiplexer::SELECTABLE_NUM())
	{
		joint_select = 0;
		Motion::Frame::_updatable = true;
	}

	// PWM信号が出力される前に出力先を切り替える必要があるので、
	// タイマー割り込みのなるべく早い段階で切り替え処理を行う。
	digitalWrite(Pin::MULTIPLEXER_SELECT0(), bitRead(output_select, 0));
	digitalWrite(Pin::MULTIPLEXER_SELECT1(), bitRead(output_select, 1));
	digitalWrite(Pin::MULTIPLEXER_SELECT2(), bitRead(output_select, 2));

	// 調整コマンドの実行中は、調整用フレームを書きだす
	if (Config::enable)
	{
		_SYSTEM__PWM_OUT_00_07_REGISTER = map(
			Config::frame.joint_angle[joint_select + 0 * Multiplexer::SELECTABLE_NUM()],
			Joint::ANGLE_MIN(), Joint::ANGLE_MAX(), Joint::PWM::MIN(), Joint::PWM::MAX()
		);

		_SYSTEM__PWM_OUT_08_15_REGISTER = map(
			Config::frame.joint_angle[joint_select + 1 * Multiplexer::SELECTABLE_NUM()],
			Joint::ANGLE_MIN(), Joint::ANGLE_MAX(), Joint::PWM::MIN(), Joint::PWM::MAX()
		);

		_SYSTEM__PWM_OUT_16_23_REGISTER = map(
			Config::frame.joint_angle[joint_select + 2 * Multiplexer::SELECTABLE_NUM()],
			Joint::ANGLE_MIN(), Joint::ANGLE_MAX(), Joint::PWM::MIN(), Joint::PWM::MAX()
		);
	}
	else if (!Utility::stop)
	{
		_SYSTEM__PWM_OUT_00_07_REGISTER = constrain(
			map(
				Motion::Frame::now->joint_angle[joint_select + 0 * Multiplexer::SELECTABLE_NUM()],
				Joint::ANGLE_MIN(), Joint::ANGLE_MAX(), Joint::PWM::MIN(), Joint::PWM::MAX()
			),
			Joint::PWM::MIN(), Joint::PWM::MAX()
		);

		_SYSTEM__PWM_OUT_08_15_REGISTER = constrain(
			map(
				Motion::Frame::now->joint_angle[joint_select + 1 * Multiplexer::SELECTABLE_NUM()],
				Joint::ANGLE_MIN(), Joint::ANGLE_MAX(), Joint::PWM::MIN(), Joint::PWM::MAX()
			),
			Joint::PWM::MIN(), Joint::PWM::MAX()
		);

		_SYSTEM__PWM_OUT_16_23_REGISTER = constrain(
			map(
				Motion::Frame::now->joint_angle[joint_select + 2 * Multiplexer::SELECTABLE_NUM()],
				Joint::ANGLE_MIN(), Joint::ANGLE_MAX(), Joint::PWM::MIN(), Joint::PWM::MAX()
			),
			Joint::PWM::MIN(), Joint::PWM::MAX()
		);
	}
	else
	{
		_SYSTEM__PWM_OUT_00_07_REGISTER = 1023;
		_SYSTEM__PWM_OUT_08_15_REGISTER = 1023;
		_SYSTEM__PWM_OUT_16_23_REGISTER = 1023;
	}

	output_select++;
	joint_select++;
}