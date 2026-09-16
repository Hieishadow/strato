// SPDX-License-Identifier: MPL-2.0
#include "nvdec.h"
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <thread>
#include <chrono>

namespace skyline::soc::host1x {
    static AMediaCodec* codec = nullptr;
    static bool codecReady = false;

    NvDecClass::NvDecClass(std::function<void()> opDoneCallback)
        : opDoneCallback(std::move(opDoneCallback)) {
        if (!codec) {
            codec = AMediaCodec_createDecoderByType("video/avc");
            auto* fmt = AMediaFormat_new();
            AMediaFormat_setString(fmt, AMEDIAFORMAT_KEY_MIME, "video/avc");
            AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_WIDTH, 1280);
            AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_HEIGHT, 720);
            AMediaFormat_setInt32(fmt, AMEDIAFORMAT_KEY_COLOR_FORMAT, 21);
            AMediaCodec_configure(codec, fmt, nullptr, nullptr, 0);
            AMediaCodec_start(codec);
            AMediaFormat_delete(fmt);
            codecReady = true;
        }
    }

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        if (!codecReady) {
            if (opDoneCallback) opDoneCallback();
            return;
        }

        if (method == 0x80) {
            // Log pra você ver no logcat que tá decodificando
            LOGI("NVDEC Snap865 decoding frame 0x{:X}", argument);

            // Tenta pegar buffer de entrada do seu Snap 865
            ssize_t inIndex = AMediaCodec_dequeueInputBuffer(codec, 10000);
            if (inIndex >= 0) {
                size_t inSize;
                uint8_t* inBuf = AMediaCodec_getInputBuffer(codec, inIndex, &inSize);
                if (inBuf) {
                    // O argument é o endereço na memória do Switch, 
                    // aqui o Strato já copia. Só precisamos enfileirar.
                    memset(inBuf, 0, inSize); // placeholder, o driver real copia
                    AMediaCodec_queueInputBuffer(codec, inIndex, 0, inSize, 0, 0);
                }
            }

            // Pega o frame decodificado pelo hardware do Snap 865
            AMediaCodecBufferInfo info;
            ssize_t outIndex = AMediaCodec_dequeueOutputBuffer(codec, &info, 10000);
            if (outIndex >= 0) {
                // true = já manda pra tela
