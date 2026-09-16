// SPDX-License-Identifier: MPL-2.0
#include "nvdec.h"
#include "logger.h"

namespace skyline::soc::host1x {
    NvDecClass::NvDecClass(std::function<void()> opDoneCallback)
        : opDoneCallback(std::move(opDoneCallback)) {}

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        // S20 FE 865 - pula cutscene preta
        if (opDoneCallback)
            opDoneCallback();
    }
}
