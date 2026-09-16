#pragma once
#include <common.h>
#include <soc/host1x/syncpoint.h>

namespace skyline {
    namespace soc { namespace host1x { class Host1xDevice; } }
    struct DeviceState;
}

namespace skyline::soc::host1x {
    class NvDecClass {
      private:
        DeviceState &state;
        SyncpointSet &syncpoints;
        std::function<void()> opDoneCallback;
      public:
        NvDecClass(DeviceState &state, SyncpointSet &syncpoints, std::function<void()> opDoneCallback);
        void CallMethod(u32 method, u32 argument);
    };
}
