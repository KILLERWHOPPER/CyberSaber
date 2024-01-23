#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <REncoder.h>

#include "global_var.hpp"
#include "pin_define.hpp"
#include "features/evil_apple_juice/evil_apple_juice.hpp"
#include "features/RC522/RC522.hpp"

extern REncoder encoder;

void encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                    RENCODER_ROTATION_VALUE_TYPE encPos);

void switchHandler();

#endif