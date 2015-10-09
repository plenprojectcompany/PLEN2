/*!
	@file      Interpreter.h
	@brief     コードインタプリタの管理クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__INTERPRETER_H_
#define _PLEN2__INTERPRETER_H_


namespace PLEN2
{
	class Interpreter;
	class MotionController;
}

/*!
	@brief コードインタプリタの管理クラス

	@attention
	このクラスはMotionControllerのprivateメンバにも影響を与えるため、
	シーケンス構造によっては、各種メソッドの実行によって思わぬ動作をすることがあります。
*/
class PLEN2::Interpreter
{
// macro:
	/*!
		@brief コードキューバッファの大きさ

		@attention
		高速に処理をするために、値として2^Nが定義される必要があります。
	*/
	#define _PLEN2__INTERPRETER__QUEUE_SIZE 32

public:
	/*!
		@brief コード構造体
	*/
	class Code
	{
	public:
		unsigned char slot;       //!< モーションスロットの指定
		unsigned char loop_count; //!< ループ回数の指定
	};

	//! @brief キューサイズ
	inline static const int QUEUE_SIZE() { return _PLEN2__INTERPRETER__QUEUE_SIZE; }


	/*!
		@brief コンストラクタ

		@param [in, out] motion_ctrl モーションコントローラインスタンス
	*/
	Interpreter(MotionController& motion_ctrl);

	/*!
		@brief コードの実行をキューに予約するメソッド

		@param [in] code コードインスタンス

		@return 実行結果
		@retval true  コードキューへのプッシュに成功
		@retval false コードキューオーバーフロー
	*/
	bool pushCode(const Code& code);

	/*!
		@brief キューの先頭コードを実行するメソッド

		@return 実行結果
		@retval true  コードキューのポップに成功(※実行にも成功しているとは限りません。)
		@retval false コードキューが空

		@attention
		内部の処理の流れとして、"スロットの再生"→"ヘッダの書き換え"を行います。
		通常のモーション再生と並行する場合、シーケンス構造によっては思わぬ動作をすることがあります。
	*/
	bool popCode();

	/*!
		@brief 実行予約済みのコードが存在するか判定するメソッド

		@return 判定結果
	*/
	bool ready();

	/*!
		@brief インタプリタの初期化メソッド
	*/
	void reset();


private:
	Code m_code_queue[_PLEN2__INTERPRETER__QUEUE_SIZE];
	unsigned char m_queue_begin;
	unsigned char m_queue_end;
	MotionController* m_motion_ctrl_ptr;
};

#endif // _PLEN2__INTERPRETER_H_