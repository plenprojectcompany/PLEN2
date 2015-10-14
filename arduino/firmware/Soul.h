/*!
	@file      Soul.h
	@brief     自然な挙動をPLENに与えるクラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__SOUL_H_
#define _PLEN2__SOUL_H_

namespace PLEN2
{
	class AccelerationGyroSensor;
	class MotionController;

	class Soul;
}

/*!
	@brief 自然な挙動をPLENに与えるクラス
*/
class PLEN2::Soul
{
private:
	//! 仰向けからの起き上がりモーションの配置スロット
	inline static const int SLOT_GETUP_FACE_UP()     { return 88;    }

	//! うつ伏せからの起き上がりモーションの配置スロット
	inline static const int SLOT_GETUP_FACE_DOWN()   { return 89;    }

	//! ランダムに切り替えるモーションの開始スロット
	inline static const int MOTIONS_SLOT_BEGIN()     { return 83;    }

	//! ランダムに切り替えるモーションの終了スロット
	inline static const int MOTIONS_SLOT_END()       { return 88;    }

	//! 挙動をランダムに切り替える基本間隔
	inline static const int BASE_INTERVAL_MSEC()     { return 15000; }

	//! 挙動をランダムに切り替える基本間隔への外乱
	inline static const int RANDOM_INTERVAL_MSEC()   { return 10000; }

	//! 自動で起き上がるまでの待機時間
	inline static const int GETUP_WAIT_MSEC()        { return 2000;  }

	//! サンプリング間隔
	inline static const int SAMPLING_INTERVAL_MSEC() { return 100;   }

	//! 重力軸と判定するための閾値
	inline static const int GRAVITY_AXIS_THRESHOLD() { return 13000; }

	unsigned long m_before_user_action_msec;
	unsigned long m_next_sampling_msec;

	unsigned long m_action_interval;

	bool m_lying;

	AccelerationGyroSensor* m_sensor_ptr;
	MotionController*       m_motion_ctrl_ptr;

public:
	/*!
		@brief コンストラクタ

		@param [in, out] sensor      センサインスタンス
		@param [in, out] motion_ctrl モーションコントローラインスタンス
	*/
	Soul(AccelerationGyroSensor& sensor, MotionController& motion_ctrl);

	/*!
		@brief PLEN2の状態の観測メソッド
	*/
	void logging();

	/*!
		@brief ユーザ操作を観測するメソッド

		このメソッドを、ユーザ操作を観測した全ての地点で実行してください。
	*/
	void userActionInputed();

	/*!
		@brief 観測状態に基づいて、適切な挙動を適用するメソッド
	*/
	void action();
};

#endif // _PLEN2__SOUL_H_