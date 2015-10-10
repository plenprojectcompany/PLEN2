/*!
	@file      PurserCombinator.h
	@brief     PLEN2におけるコマンドラインの解析クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__PURSER_COMBINATOR_H_
#define _PLEN2__PURSER_COMBINATOR_H_

namespace PLEN2
{
	class PurserCombinator;
}

namespace Utility
{
	class AbstractPurser;
}

/*!
	@brief コマンドラインの解析クラス

	@note
	本クラスはそのままではコマンドの解析しか行わないため、
	ユーザは自前でイベントハンドラを定義する必要があります。
	<br><br>
	そのためには、継承を行いイベントハンドラをオーバーライドする必要があります。
	詳しくは各種virtual methodをご参照ください。
*/
class PLEN2::PurserCombinator
{
protected:
	typedef enum
	{
		READY, // same as HEADER_INCOMING
		COMMAND_INCOMING,
		ARGUMENTS_INCOMING,
		STATE_EOE
	} State;

	class Buffer
	{
	// macro:
		/*!
			@note
			バッファサイズは最低限、BLEのペイロード長(20byte)以上必要です。
			また、高速に処理を行うため2^Nを要求します。
		*/
		#define _PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH 128

	public:
		//! @brief バッファ長
		inline static const int LENGTH()
		{
			return _PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH;
		}

		char data[_PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH];
		unsigned char position;

		Buffer()
			: position(0)
		{
			for (int index = 0; index < LENGTH(); index++)
			{
				data[index] = '\0';
			}
		}
	};

	Buffer m_buffer;
	State m_state;
	unsigned char m_store_length;
	bool m_installing;
	Utility::AbstractPurser* m_purser[STATE_EOE];

	/*!
		@brief パース処理を中断するメソッド
	*/
	void m_abort();

public:
	PurserCombinator();

	virtual ~PurserCombinator() {}

	/*!
		@brief 1文字を読み込み、リングバッファに格納するメソッド
	*/
	void readByte(char byte);

	/*!
		@brief パーサの内部状態も加味しながら、パースを行うメソッド

		@return 判定結果
	*/
	bool accept();

	/*!
		@brief パーサの内部状態遷移メソッド
	*/
	void transition();

	/*!
		@brief transition() 実行前に呼び出される、ユーザ定義フック
	*/
	virtual void beforeFook();

	/*!
		@brief transition() 実行後に呼び出される、ユーザ定義フック
	*/
	virtual void afterFook();
};

#endif // _PLEN2__PURSER_COMBINATOR_H_