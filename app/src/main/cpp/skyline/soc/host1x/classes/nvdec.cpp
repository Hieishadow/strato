// SPDX-License-Identifier: MPL-2.0
// Copyright © 2021 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include "nvdec.h"

namespace skyline::soc::host1x {
    NvDecClass::NvDecClass(std::function<void()> opDoneCallback)
        : opDoneCallback(std::move(opDoneCallback)) {}

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        // FIX LINK'S AWAKENING - TELA PRETA PRAIA
        // Ao invés de travar esperando vídeo, já avisa que terminou
        // Assim o jogo pula a cutscene e continua na praia
        LOGW("NVDEC method 0x{:X} arg 0x{:X} - skipping video (fix beach black screen)", method, argument);
        
        if (opDoneCallback)
            opDoneCallback();
    }
}
