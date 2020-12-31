#pragma once

// clang-format off: include order matters here.
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
// clang-format on

#include "PinDetail.h"

namespace Pins {
    class WifiSocketPinDetail : public PinDetail {
        PinCapabilities _capabilities;
        PinAttributes   _attributes;
        int             _readWriteMask;
        volatile int    _running;

        TaskHandle_t     _cmdTaskHandle;
        volatile uint8_t _lastValue;

        static void cmdTask(void* pvParameters);

        void asyncLoop();

    public:
        WifiSocketPinDetail(uint8_t index, const PinOptionsParser& options);

        PinCapabilities capabilities() const override;
        PinAttributes   attributes() const override;

        // I/O:
        void write(int high) override;
        int  read() override;
        void setAttr(PinAttributes value) override;
        void reset() override;

        String toString() const override;

        ~WifiSocketPinDetail() override;
    };
}
