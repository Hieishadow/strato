// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <common.h>
#include <media/NdkMediaCodec.h>

namespace skyline::soc::host1x {
    class NvDecClass {
      private:
        std::function<void()> opDoneCallback;
        AMediaCodec* decoder = nullptr; // decoder do S20 FE
        bool isInitialized = false;

      public:
        NvDecClass(std::function<void()> opDoneCallback);
        void CallMethod(u32 method, u32 argument);
        void InitDecoder(); // inicia o MediaCodec
    };
}
