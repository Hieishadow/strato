// SPDX-License-Identifier: MPL-2.0
#include "nvdec.h"
#include "logger.h"

namespace skyline::soc::host1x {

    NvDecClass::NvDecClass(std::function<void()> opDoneCallback)
        : opDoneCallback(std::move(opDoneCallback)) {}

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        // 0x80 = submit do vídeo. No Snap 865 o original travava aqui
        if (method == 0x80) {
            LOGI("NVDEC Cutscene skip - S20 FE - arg 0x%X", argument);
            // Aqui que a gente pulava o vídeo preto. Se quiser tentar decodificar de verdade
            // depois, a gente tem que plugar o NvMap. Por enquanto só libera pra não ficar preto infinito
        }
        
        // IMPORTANTE: Libera SEMPRE no final, senão o jogo congela no preto
        if (opDoneCallback)
            opDoneCallback();
    }
}
