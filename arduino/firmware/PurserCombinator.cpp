/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define _DEBUG false

// 独自ライブラリ
#if _DEBUG
	#include "System.h"
#endif
#include "Purser.h"
#include "PurserCombinator.h"


namespace
{
	/*!
		@note
		CharGroupPurserは内部でリニアサーチを行うため、
		優先度の高いヘッダほど先頭に配置する。

		$ : Controller  [Priority 3]
		# : Interpreter [Priority 2]
		> : Setter      [Priority 1]
		< : Getter      [Priority 0]
	*/
	Utility::CharGroupPurser header_purser("$#><");


	/*!
		@attention
		コマンドを増やす場合、必ずソートした状態の配列を与える。
		(StringGroupPurserは内部でバイナリサーチを行うため。)
	*/
	const char* CONTROLLER_SYMBOL[] = {
		"AD", // ANGLE DIFF
		"AN", // ANGLE
		"HP", // HOME POSITION
		"MP", // PLAY MOTION, @todo "PM"に変更し、[[deprecated]]にする。
		"MS", // STOP MOTION, @todo "SM"に変更し、[[deprecated]]にする。
		"PM", // PLAY MOTION
		"SM"  // STOP MOTION
	};
	const unsigned char CONTROLLER_ARGS_STORE_LENGTH[] = {
		5, 5, 0, 2, 0, 2, 0
	};

	const unsigned char CONTROLLER_SYMBOL_LENGTH = sizeof(CONTROLLER_SYMBOL) / sizeof(CONTROLLER_SYMBOL[0]);
	Utility::StringGroupPurser controller_purser(CONTROLLER_SYMBOL, CONTROLLER_SYMBOL_LENGTH);


	const char* INTERPRETER_SYMBOL[] = {
		"PO", // POP CODE
		"PU", // PUSH CODE
		"RI"  // RESET INTERPRETER
	};
	const unsigned char INTERPRETER_ARGS_STORE_LENGTH[] = {
		0, 4, 0
	};

	const unsigned char INTERPRETER_SYMBOL_LENGTH = sizeof(INTERPRETER_SYMBOL) / sizeof(INTERPRETER_SYMBOL[0]);
	Utility::StringGroupPurser interpreter_purser(INTERPRETER_SYMBOL, INTERPRETER_SYMBOL_LENGTH);


	const char* SETTER_SYMBOL[] = {
		"HO", // HOME ANGLE
		"IN", // INSTALL MOTION, @todo "MF", "MH"に分割し、[[deprecated]]にする。
		"JS", // JOINT SETTINGS
		"MA", // MAX ANGLE
		"MF", // MOTION FRAME
		"MH", // MOTION HEADER
		"MI"  // MIN ANGLE
	};
	const unsigned char SETTER_ARGS_STORE_LENGTH[] = {
		5, 0, 0, 5, 104, 30, 5
	};

	const unsigned char SETTER_SYMBOL_LENGTH = sizeof(SETTER_SYMBOL) / sizeof(SETTER_SYMBOL[0]);
	Utility::StringGroupPurser setter_purser(SETTER_SYMBOL, SETTER_SYMBOL_LENGTH);


	const char* GETTER_SYMBOL[] = {
		"JS", // JOINT SETTINGS
		"MO", // MOTION
		"VI"  // VERSION INFORMATION
	};
	const unsigned char GETTER_ARGS_STORE_LENGTH[] = {
		0, 2, 0
	};

	const unsigned char GETTER_SYMBOL_LENGTH = sizeof(GETTER_SYMBOL) / sizeof(GETTER_SYMBOL[0]);
	Utility::StringGroupPurser getter_purser(GETTER_SYMBOL, GETTER_SYMBOL_LENGTH);


	const unsigned char* ARGS_STORE_LENGTH[] = {
		CONTROLLER_ARGS_STORE_LENGTH,
		INTERPRETER_ARGS_STORE_LENGTH,
		SETTER_ARGS_STORE_LENGTH,
		GETTER_ARGS_STORE_LENGTH
	};

	Utility::AbstractPurser* command_purser[] = {
		&controller_purser,
		&interpreter_purser,
		&setter_purser,
		&getter_purser
	};


	/*!
		@note
		文字列長の制限は別途 m_store_length に設定することで行う。
		(与えられた文字長のみ受理するパーサクラスを実装したほうが効果的？)
	*/
	Utility::HexStringPurser args_purser;


	/*!
		@note
		<string>を許容するプロトコル用に、一時的に使用する。
	*/
	Utility::NilPurser nil_purser;


	#if _DEBUG
		PLEN2::System system;
	#endif
}


void PLEN2::PurserCombinator::m_abort()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : PurserCombinator::m_abort()"));
	#endif

	m_store_length    = 1;
	m_state           = READY;
	m_installing      = 0;
	m_buffer.position = 0;
}


PLEN2::PurserCombinator::PurserCombinator()
	: m_store_length(1)
	, m_state(READY)
	, m_installing(false)
{
	m_purser[READY]              = &header_purser;
	m_purser[COMMAND_INCOMING]   = command_purser[0];
	m_purser[ARGUMENTS_INCOMING] = &args_purser;
}


void PLEN2::PurserCombinator::readByte(char byte)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : PurserCombinator::readByte()"));
	#endif

	m_buffer.data[m_buffer.position] = byte;
	(++m_buffer.position) &= (Buffer::LENGTH() - 1);

	m_buffer.data[m_buffer.position] = '\0';
}


bool PLEN2::PurserCombinator::accept()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : PurserCombinator::accept()"));
	#endif

	if (m_buffer.position < m_store_length)
	{
		return false;
	}

	if (m_purser[m_state]->purse(m_buffer.data) == false)
	{
		m_abort();

		return false;
	}

	return true;
}


void PLEN2::PurserCombinator::transition()
{
	beforeFook();

	#if _DEBUG
		system.outputSerial().println(F("=== running in function : PurserCombinator::transition()"));
	#endif

	switch (m_state)
	{
		case READY:
		{
			m_state = COMMAND_INCOMING;
			m_purser[COMMAND_INCOMING] = command_purser[m_purser[READY]->index()];
			m_store_length = 2;

			break;
		}

		case COMMAND_INCOMING:
		{
			m_state = ARGUMENTS_INCOMING;

			// コマンドラインに関する部分特殊化
			if (m_purser[READY]->index() == 2 /* := Setter */)
			{
				// インストールコマンドを受理した場合、モーションヘッダ書き込みコマンドへ書き換える。
				if (m_purser[COMMAND_INCOMING]->index() == 1 /* := INSTALL MOTION */)
				{
					m_purser[COMMAND_INCOMING]->purse("MH");
					m_installing = true;
				}

				// モーションヘッダ書き込みコマンドを受理した場合、コマンドラインの制約を解除する。
				if (m_purser[COMMAND_INCOMING]->index() == 5 /* := MOTION HEADER */)
				{
					m_purser[ARGUMENTS_INCOMING] = &nil_purser;
				}
			}

			unsigned char header_id = m_purser[READY]->index();
			unsigned char cmd_id    = m_purser[COMMAND_INCOMING]->index();

			m_store_length = ARGS_STORE_LENGTH[header_id][cmd_id];

			// 以下の条件が満たされる場合、コマンドラインの引数部は存在しないためREADY状態へ遷移する。
			if (m_store_length == 0)
			{
				m_state = READY;
				m_store_length = 1;
			}

			break;
		}

		case ARGUMENTS_INCOMING:
		{
			m_state = READY;
			m_purser[ARGUMENTS_INCOMING] = &args_purser;
			m_store_length = 1;

			break;
		}

		default:
		{
			#if _DEBUG
				system.outputSerial().println(">>> error : Undefined state was inputed.");
			#endif

			m_abort();
		}
	}

	m_buffer.position = 0;

	afterFook();
}


void PLEN2::PurserCombinator::beforeFook()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : PurserCombinator::beforeFook()"));
	#endif
}


void PLEN2::PurserCombinator::afterFook()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : PurserCombinator::afterFook()"));
	#endif
}
