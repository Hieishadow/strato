#pragma once
#include <common.h>

namespace skyline::soc::host1x {
    class NvDecClass {
      private:
        std::function<void()> opDoneCallback;
      public:
        NvDecClass(std::function<void()> opDoneCallback);
        void CallMethod(u32 method, u32 argument);
    };
}
