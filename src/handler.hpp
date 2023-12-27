#ifndef HANDLER_HPP
#define HANDLER_HPP


#include <REncoder.h>

#include "global_var.hpp"
#include "pin_define.hpp"

extern REncoder encoder;

    void
    encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                   RENCODER_ROTATION_VALUE_TYPE encPos);

void switchHandler();

#endif