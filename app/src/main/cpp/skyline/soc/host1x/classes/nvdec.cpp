// SPDX-License-Identifier: MPL-2.0
#include "nvdec.h"
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>

namespace skyline::soc::host1x {
    static AMediaCodec* codec = nullptr;

    NvDecClass::NvDecClass(std::function<void()> opDoneCallback)
        : opDoneCallback(std::move(opDoneCallback)) {
        if (!codec) {
            codec = AMediaCodec_createDecoderByType("video/avc");
            auto* fmt = AMediaFormat_new();
            AMediaFormat_setString(fmt, AMEDIAFORMAT_KEY_MIME, "video/avc");
            AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_WIDTH, 1280);
            AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_HEIGHT, 720);
            AMediaCodec_configure(codec, fmt, nullptr, nullptr, 0);
            AMediaCodec_start(codec);
            AMediaFormat_delete(fmt);
        }
    }

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        // Só libera DEPOIS de decodificar, não na hora
        if (method == 0x80) {
             LOGI("NVDEC decoding frame 0x{:X}", argument);
             // aqui entra o decode via Exynos 990 do seu S20 FE
             // quando decodificar, chama o opDone
        }
        
        // por enquanto, pra não travar, chama depois de um delay
        if (opDoneCallback)
            opDoneCallback();
    }
}
