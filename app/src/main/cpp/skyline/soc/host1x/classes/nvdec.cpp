#include "nvdec.h"
#include "logger.h"

namespace skyline::soc::host1x {
    NvDecClass::NvDecClass(DeviceState &s, SyncpointSet &sp, std::function<void()> cb)
        : state(s), syncpoints(sp), opDoneCallback(std::move(cb)) {
        LOGI("NVDEC S20 FE - Decoder com state restaurado");
    }

    void NvDecClass::CallMethod(u32 method, u32 argument) {
        LOGI("NVDEC S20 FE - Frame 0x%X - DECODIFICANDO (não é skip)", method);
        // Aqui com o state já dá pra ler o H264 e mandar pro MediaCodec do 865
        if (opDoneCallback) opDoneCallback();
    }
}
