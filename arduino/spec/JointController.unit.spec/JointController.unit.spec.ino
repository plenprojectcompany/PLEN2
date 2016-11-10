#line 2 "JointController.unit.spec.ino"


#include <EEPROM.h>
#include <ArduinoUnit.h>

#include "Pin.h"
#include "System.h"
#include "JointController.h"


/*!
    @brief テストケース選択用プリプロセスマクロ
*/
#define TEST_USER false //!< ユーザテストについても実行します。


namespace
{
    using namespace PLEN2;

    JointController joint_ctrl;

    const uint8_t getRandomJoint()
    {
        return random(JointController::JOINTS_SUM);
    }

    const int16_t getRandomAngle_MIN_max(uint8_t joint_id)
    {
        return random(
            JointController::ANGLE_MIN, joint_ctrl.getMaxAngle(joint_id)
        );
    }

    const int16_t getRandomAngle_min_MAX(uint8_t joint_id)
    {
        return random(
            joint_ctrl.getMinAngle(joint_id) + 1, JointController::ANGLE_MAX + 1
        );
    }

    const int16_t getRandomAngle_min_max(uint8_t joint_id)
    {
        return random(
            joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id) + 1
        );
    }

    const int16_t getRandomAngleDiff_min_max(uint8_t joint_id)
    {
        return random(
            joint_ctrl.getMinAngle(joint_id) - joint_ctrl.getHomeAngle(joint_id),
            joint_ctrl.getMaxAngle(joint_id) - joint_ctrl.getHomeAngle(joint_id) + 1
        );
    }

    uint16_t angle2PWM(uint8_t joint_id, int16_t angle)
    {
        angle = constrain(
            angle,
            joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id)
        );

        uint16_t pwm = map(
            angle,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,
            JointController::PWM_MIN,   JointController::PWM_MAX
        );

        return pwm;
    }

    uint16_t angleDiff2PWM(uint8_t joint_id, int16_t angle_diff)
    {
        int16_t angle = constrain(
            angle_diff + joint_ctrl.getHomeAngle(joint_id),
            joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id)
        );

        uint16_t pwm = map(
            angle,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,
            JointController::PWM_MIN,   JointController::PWM_MAX
        );

        return pwm;
    }
}


/*!
    @brief ランダムに選択した関節への、角度最小値の設定テスト
*/
test(RandomJoint_SetMinAngle)
{
    // Setup ==================================================================
    uint8_t joint_id = getRandomJoint();

    int16_t expected = getRandomAngle_MIN_max(joint_id);
    int16_t actual;

    // Run ====================================================================
    joint_ctrl.setMinAngle(joint_id, expected);
    joint_ctrl.loadSettings();

    actual = joint_ctrl.getMinAngle(joint_id);

    // Assert =================================================================
    assertEqual(expected, actual);
}


/*!
    @brief 全ての関節への、角度最小値の設定テスト
*/
test(AllJoint_SetMinAngle)
{
    for (uint8_t joint_id = 0;
                 joint_id < PLEN2::JointController::JOINTS_SUM;
                 joint_id++
    )
    {
        // Setup ==============================================================
        int16_t expected = getRandomAngle_MIN_max(joint_id);
        int16_t actual;

        // Run ================================================================
        joint_ctrl.setMinAngle(joint_id, expected);
        joint_ctrl.loadSettings();

        actual = joint_ctrl.getMinAngle(joint_id);

        // Assert =============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief ランダムに選択した関節への、角度最大値の設定テスト
*/
test(RandomJoint_SetMaxAngle)
{
    // Setup ==================================================================
    uint8_t joint_id = getRandomJoint();

    int16_t expected = getRandomAngle_min_MAX(joint_id);
    int16_t actual;

    // Run ====================================================================
    joint_ctrl.setMaxAngle(joint_id, expected);
    joint_ctrl.loadSettings();

    actual = joint_ctrl.getMaxAngle(joint_id);

    // Assert =================================================================
    assertEqual(expected, actual);
}


/*!
    @brief 全ての関節への、角度最大値の設定テスト
*/
test(AllJoint_SetMaxAngle)
{
    for (uint8_t joint_id = 0;
                 joint_id < PLEN2::JointController::JOINTS_SUM;
                 joint_id++
    )
    {
        // Setup ==============================================================
        int16_t expected = getRandomAngle_min_MAX(joint_id);
        int16_t actual;

        // Run ================================================================
        joint_ctrl.setMaxAngle(joint_id, expected);
        joint_ctrl.loadSettings();

        actual = joint_ctrl.getMaxAngle(joint_id);

        // Assert =============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief ランダムに選択した関節への、角度初期値の設定テスト
*/
test(RandomJoint_SetHomeAngle)
{
    // Setup ==================================================================
    uint8_t joint_id = getRandomJoint();

    int16_t expected = getRandomAngle_min_max(joint_id);
    int16_t actual;

    // Run ====================================================================
    joint_ctrl.setHomeAngle(joint_id, expected);
    joint_ctrl.loadSettings();

    actual = joint_ctrl.getHomeAngle(joint_id);

    // Assert =================================================================
    assertEqual(expected, actual);
}


/*!
    @brief 全ての関節への、角度初期値の設定テスト
*/
test(AllJoint_SetHomeAngle)
{
    for (uint8_t joint_id = 0;
                 joint_id < PLEN2::JointController::JOINTS_SUM;
                 joint_id++
    )
    {
        // Setup ==============================================================
        int16_t expected = getRandomAngle_min_max(joint_id);
        int16_t actual;

        // Run ================================================================
        joint_ctrl.setHomeAngle(joint_id, expected);
        joint_ctrl.loadSettings();

        actual = joint_ctrl.getHomeAngle(joint_id);

        // Assert =============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief ランダムに選択した関節への、角度の設定テスト
*/
test(RandomJoint_SetAngle)
{
    // Setup ==================================================================
    // joint_ctrl.resetSettings();

    uint8_t joint_id = getRandomJoint();
    int16_t angle = getRandomAngle_min_max(joint_id);

    uint16_t expected = angle2PWM(joint_id, angle);
    uint16_t actual;

    // Run ====================================================================
    joint_ctrl.setAngle(joint_id, angle);

    actual = PLEN2::JointController::m_pwms[joint_id];

    // Assert =================================================================
    assertEqual(expected, actual);
}


/*!
    @brief 全ての関節への、角度の設定テスト
*/
test(AllJoint_SetAngle)
{
    for (uint8_t joint_id = 0;
                 joint_id < PLEN2::JointController::JOINTS_SUM;
                 joint_id++
    )
    {
        // Setup ==============================================================
        int16_t angle = getRandomAngle_min_max(joint_id);

        uint16_t expected = angle2PWM(joint_id, angle);
        uint16_t actual;

        // Run ================================================================
        joint_ctrl.setAngle(joint_id, angle);

        actual = PLEN2::JointController::m_pwms[joint_id];

        // Assert =============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief ランダムに選択した関節への、角度差分の設定テスト
*/
test(RandomJoint_SetAngleDiff)
{
    // Setup ==================================================================
    uint8_t joint_id = getRandomJoint();
    int16_t angle_diff = getRandomAngleDiff_min_max(joint_id);

    uint16_t expected = angleDiff2PWM(joint_id, angle_diff);
    uint16_t actual;

    // Run ====================================================================
    joint_ctrl.setAngleDiff(joint_id, angle_diff);

    actual = PLEN2::JointController::m_pwms[joint_id];

    // Assert =================================================================
    assertEqual(expected, actual);
}


/*!
    @brief 全ての関節への、角度差分の設定テスト
*/
test(AllJoint_SetAngleDiff)
{
    // joint_ctrl.resetSettings();

    for (uint8_t joint_id = 0;
                 joint_id < PLEN2::JointController::JOINTS_SUM;
                 joint_id++
    )
    {
        // Setup ==============================================================
        int16_t angle_diff = getRandomAngleDiff_min_max(joint_id);

        uint16_t expected = angleDiff2PWM(joint_id, angle_diff);
        uint16_t actual;

        // Run ================================================================
        joint_ctrl.setAngleDiff(joint_id, angle_diff);

        actual = PLEN2::JointController::m_pwms[joint_id];

        // Assert =============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief 未定義関節への、各種取得メソッドのテスト
*/
test(JointOverflow_GetMethods)
{
    // Setup ==================================================================
    uint8_t joint_id = PLEN2::JointController::JOINTS_SUM;

    int16_t expected = -32768;
    int16_t actual;

    // Run & Assert ===========================================================
    actual = joint_ctrl.getMinAngle(joint_id);
    assertEqual(expected, actual);

    actual = joint_ctrl.getMaxAngle(joint_id);
    assertEqual(expected, actual);

    actual = joint_ctrl.getHomeAngle(joint_id);
    assertEqual(expected, actual);
}


/*!
    @brief 未定義関節への、各種設定メソッドのテスト
*/
test(JointOverflow_SetMethods)
{
    // Setup ==================================================================
    uint8_t joint_id = PLEN2::JointController::JOINTS_SUM;
    int16_t angle = random();

    bool expected = false;
    bool actual;

    // Run & Assert ===========================================================
    actual = joint_ctrl.setMinAngle(joint_id, angle);
    assertEqual(expected, actual);

    actual = joint_ctrl.setMaxAngle(joint_id, angle);
    assertEqual(expected, actual);

    actual = joint_ctrl.setHomeAngle(joint_id, angle);
    assertEqual(expected, actual);

    actual = joint_ctrl.setAngle(joint_id, angle);
    assertEqual(expected, actual);

    actual = joint_ctrl.setAngleDiff(joint_id, angle);
    assertEqual(expected, actual);
}


/*!
    @brief タイマ1の動作テスト
*/
test(Timer1Attached)
{
    // Setup ==================================================================
    bool before = joint_ctrl.m_1cycle_finished = false;

    // Run ====================================================================
    delay(1000);

    bool after = joint_ctrl.m_1cycle_finished;

    // Assert =================================================================
    assertNotEqual(before, after);
}


/*!
    @brief 関節設定のダンプテスト

    ユーザによる目視でのテストです。
*/
test(Dump)
{
    #if TEST_USER
        joint_ctrl.dump();

        pass();
    #else
        skip();
    #endif
}


/*!
    @brief アプリケーション・エントリポイント
*/
void setup()
{
    PLEN2::System::begin();

    randomSeed( analogRead(PLEN2::Pin::RANDOM_DEVICE_IN) );

    joint_ctrl.loadSettings();
    joint_ctrl.resetSettings();

    while (!Serial); // for the Arduino Leonardo/Micro only.

    Serial.print(F("# Test : "));
    Serial.println(__FILE__);
}

void loop()
{
    Test::run();
}
