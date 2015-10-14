/*!
	@file      AccelerationGyroSensor.h
	@brief     加速度・ジャイロセンサの管理クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__ACCELERATION_GYRO_SENSOR_H_
#define _PLEN2__ACCELERATION_GYRO_SENSOR_H_

namespace PLEN2
{
	class AccelerationGyroSensor;
}

/*!
	@brief 加速度・ジャイロセンサの管理クラス

	@note
	加速度・ジャイロセンサの値は、頭基板からバスを介して取得する必要があります。
*/
class PLEN2::AccelerationGyroSensor
{
// macro:
	#define _PLEN2__ACCELERATION_GYRO_SENSOR__SUM 6

private:
	//! @brief センサの総数
	inline static const int SUM()
	{
		return _PLEN2__ACCELERATION_GYRO_SENSOR__SUM;
	}

	int m_values[_PLEN2__ACCELERATION_GYRO_SENSOR__SUM];

public:
	/*!
		@brief センサ値のサンプリングを行うメソッド

		@note
		loop() から定期的に呼び出すことを想定しています。

		@attention
		バスを介してデータの送受信を行うため、内部で割り込みと通信待ちのロックが発生します。
		そのため、コンストラクタ内や割り込みベクタ内での使用は推奨されません。
		<br><br>
		電力供給のタイミングやファームウェアの立上げタイミングが"サーボ基盤"→"頭基板"なため、
		電源投入の早い段階で本メソッドを実行すると、通信タイミングがずれるためフリーズします。
		(おおむね、3000[msec]の遅延をいれると大丈夫なようです。)
	*/
	void sampling();

	/*!
		@brief X軸の加速度を取得するメソッド

		@return X軸の加速度

		@attention
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	const int& getAccX();

	/*!
		@brief Y軸の加速度を取得するメソッド

		@return Y軸の加速度

		@attention
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	const int& getAccY();

	/*!
		@brief Z軸の加速度を取得するメソッド

		@return Z軸の加速度

		@attention
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	const int& getAccZ();

	/*!
		@brief ロール軸(X軸に関する回転軸)の角速度を取得するメソッド

		@return ロール軸の角速度

		@attention
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	const int& getGyroRoll();

	/*!
		@brief ピッチ軸(Y軸に関する回転軸)の角速度を取得するメソッド

		@return ピッチ軸の角速度

		@attention
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	const int& getGyroPitch();

	/*!
		@brief ヨー軸(Z軸に関する回転軸)の角速度を取得するメソッド

		@return ヨー軸の角速度

		@attention
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	const int& getGyroYaw();

	/*!
		@brief サンプリングを行った後、各種センサ値をダンプするメソッド

		@note
		以下のような書式のJSON文字列を出力します。
		@code
		{
			"Acc X": <integer>,
			"Acc Y": <integer>,
			"Acc Z": <integer>,
			"Gyro Roll": <integer>,
			"Gyro Pitch": <integer>,
			"Gyro Yaw": <integer>
		}
		@endcode
	*/
	void dump();
};

#endif // _PLEN2__ACCELERATION_GYRO_SENSOR_H_
