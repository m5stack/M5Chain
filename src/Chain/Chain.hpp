/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
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
#include "UnitChainBus/UnitChainBus.hpp"
#include "ChainMono/ChainMono.hpp"
#include "ChainRGB/ChainRGB.hpp"
#include "ChainPIR/ChainPIR.hpp"
#include "ChainSwitch/ChainSwitch.hpp"
#include "ChainMIC/ChainMIC.hpp"
#include "ChainBuzzer/ChainBuzzer.hpp"
#include "Unit8Servos2Chain/Unit8Servos2Chain.hpp"
#include "ChainDLight/ChainDLight.hpp"
#include "ChainENV/ChainENV.hpp"
#include "ChainIMU/ChainIMU.hpp"
#include "ChainPedal/ChainPedal.hpp"

class Chain : virtual public ChainCommon,
              virtual public ChainJoystick,
              virtual public ChainKey,
              virtual public ChainAngle,
              virtual public ChainEncoder,
              virtual public ChainToF,
              virtual public UnitChainBus,
              virtual public ChainMono,
              virtual public ChainRGB,
              virtual public ChainPIR,
              virtual public ChainSwitch,
              virtual public ChainMIC,
              virtual public ChainBuzzer,
              virtual public ChainDLight,
              virtual public ChainENV,
              virtual public ChainIMU,
              virtual public ChainPedal,
              virtual public Unit8Servos2Chain {
public:
private:
};

#endif  // _CHAIN_HPP_
