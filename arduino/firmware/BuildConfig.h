/*!
    @file      BuildConfig.h
    @brief     Config file for each build environment
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_BUILD_CONFIG_H
#define PLEN2_BUILD_CONFIG_H


/*!
    @brief Configuration macro to build the firmware for PLEN1.4

    @attention
    This macro and TARGET_PLEN20 macro are incompatible.
    You need to enable only one configuration.
*/
#define TARGET_PLEN14 false

/*!
    @brief Configuration macro to build the firmware for PLEN2

    @attention
    This macro and TARGET_PLEN14 macro are incompatible.
    You need to enable only one configuration.
*/
#define TARGET_PLEN20 true

/*!
    @brief Configuration macro to build the firmware for developer edition

    If you enable this macro, it changes following feature(s):
    - Serial communication speed
*/
#define TARGET_DEVELOPER_EDITION false

/*!
    @brief Configuration macro to build the firmware for mirror edition

    If you enable this macro, it changes following feature(s):
    - Output serial
    - Joint controll methods
    - Protocol
*/
#define TARGET_MIRROR_EDITION false


#if TARGET_PLEN14 == TARGET_PLEN20
    #error "TARGET_PLEN14" and "TARGET_PLEN20" macros are incompatible! (You need to enable only one configuration.)
#endif

#endif // PLEN2_BUILD_CONFIG_H
