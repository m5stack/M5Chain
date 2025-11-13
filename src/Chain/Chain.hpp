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

class Chain : virtual public ChainCommon,
              virtual public ChainJoystick,
              virtual public ChainKey,
              virtual public ChainAngle,
              virtual public ChainEncoder,
              virtual public ChainToF {
public:
private:
};

#endif  // _CHAIN_HPP_
