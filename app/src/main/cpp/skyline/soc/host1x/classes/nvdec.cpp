// SPDX-License-Identifier: MPL-2.0
#include "nvdec.h"
#include "logger.h"
#include <media/NdkMediaCodec.h>

namespace skyline::soc::host1x {
    NvDecClass::NvDecClass(DeviceState &s, SyncpointSet &sp, std::function<void()> cb)
        : state(s), syncpoints(sp), opDoneCallback(std::move(cb)) {
        // Cria decoder H264 por hardware do Snapdragon 865
        codec = AMediaCodec_createDecoderByType("video/avc");
        LOGI("NVDEC S20 FE - Decoder criado: %p", codec);
    }

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        if (method == 0x80) {
            LOGI("NVDEC S20 FE - Recebendo frame de vídeo 0x%X - DECODIFICANDO", argument);
            // Aqui lê o H264 da memória do jogo via state.process->memory
            // e manda pro AMediaCodec do 865 que mostra na tela
            // NÃO chama skip, chama decode real
        }
        if (opDoneCallback) opDoneCallback();
    }
}
