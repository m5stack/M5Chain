/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_HPP_
#define _CHAIN_HPP_

#include "ChainCommon.hpp"
#include "ChainJoystick.hpp"
#include "ChainKey.hpp"

class Chain : virtual public ChainCommon,
              virtual public ChainJoystick,
              virtual public ChainKey {
public:
private:
};

#endif  // _CHAIN_HPP_
