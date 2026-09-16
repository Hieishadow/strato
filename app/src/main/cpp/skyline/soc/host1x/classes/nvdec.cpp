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
        if (method == 0x80) {
            LOGI("NVDEC Snap865 decoding 0x%X", argument);
            if (codec) {
                ssize_t inIdx = AMediaCodec_dequeueInputBuffer(codec, 10000);
                if (inIdx >= 0) {
                    size_t sz;
                    uint8_t* buf = AMediaCodec_getInputBuffer(codec, inIdx, &sz);
                    if (buf) {
                        AMediaCodec_queueInputBuffer(codec, inIdx, 0, sz, 0, 0);
                    }
                }
                AMediaCodecBufferInfo info;
                ssize_t outIdx = AMediaCodec_dequeueOutputBuffer(codec, &info, 10000);
                if (outIdx >= 0) {
                    AMediaCodec_releaseOutputBuffer(codec, outIdx, true);
                }
            }
        }
        if (opDoneCallback)
            opDoneCallback();
    }
}
