/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_HPP_
#define _CHAIN_HPP_

#include "ChainCommon/ChainCommon.hpp"
#include "ChainJoystick/ChainJoystick.hpp"
#include "ChainKey/ChainKey.hpp"
#include "ChainAngle/ChainAngle.hpp"
#include "ChainEncoder/ChainEncoder.hpp"
#include "ChainToF/ChainToF.hpp"
#include "ChainSwitch/ChainSwitch.hpp"
#include "ChainPIR/ChainPIR.hpp"
#include "ChainMIC/ChainMIC.hpp"
#include "UnitChainBus/UnitChainBus.hpp"
#include "Unit8Servos2Chain/Unit8Servos2Chain.hpp"
#include "ChainBuzzer/ChainBuzzer.hpp"
#include "ChainPedal/ChainPedal.hpp"
#include "ChainMono/ChainMono.hpp"
#include "ChainRGB/ChainRGB.hpp"
#include "ChainDLight/ChainDLight.hpp"
#include "ChainENV/ChainENV.hpp"
#include "ChainIMU/ChainIMU.hpp"
#include "Chain4Relay/Chain4Relay.hpp"

class Chain : virtual public ChainCommon,
              virtual public ChainJoystick,
              virtual public ChainKey,
              virtual public ChainAngle,
              virtual public ChainEncoder,
              virtual public ChainToF,
              virtual public ChainSwitch,
              virtual public ChainPIR,
              virtual public ChainMIC,
              virtual public UnitChainBus,
              virtual public Unit8Servos2Chain,
              virtual public ChainBuzzer,
              virtual public ChainMono,
              virtual public ChainRGB,
              virtual public ChainPedal,
              virtual public ChainDLight,
              virtual public ChainENV,
              virtual public ChainIMU,
              virtual public Chain4Relay {
public:
private:
};

#endif  // _CHAIN_HPP_
