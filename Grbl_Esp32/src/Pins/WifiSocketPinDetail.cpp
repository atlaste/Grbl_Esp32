#include "WifiSocketPinDetail.h"

#include <WiFi.h>
#include <lwip/sockets.h>

#include "../Assert.h"
#include "../Grbl.h"  // for error messages

namespace Pins {
    const int IDLE_TIME_MS = 10;

    WifiSocketPinDetail::WifiSocketPinDetail(uint8_t index, const PinOptionsParser& options) :
        PinDetail(index), _capabilities(PinCapabilities::Output), _attributes(Pins::PinAttributes::Undefined), _readWriteMask(0),
        _running(0), _cmdTaskHandle(nullptr), _lastValue(0) {
        // User defined pin capabilities
        for (auto opt : options) {
            if (opt.is("low")) {
                _attributes = _attributes | PinAttributes::ActiveLow;
            } else if (opt.is("high")) {
                // Default: Active HIGH.
            } else {
                Assert(false, "Bad I2S option passed to pin %d: %s", int(index), opt());
            }
        }

        // Update the R/W mask for ActiveLow setting
        if (_attributes.has(PinAttributes::ActiveLow)) {
            _readWriteMask = HIGH;
        } else {
            _readWriteMask = LOW;
        }
    }

    PinCapabilities WifiSocketPinDetail::capabilities() const { return PinCapabilities::Output; }
    PinAttributes   WifiSocketPinDetail::attributes() const { return _attributes; }

    void WifiSocketPinDetail::cmdTask(void* pvParameters) {
        auto inst = static_cast<WifiSocketPinDetail*>(pvParameters);
        inst->asyncLoop();
    }

    void WifiSocketPinDetail::asyncLoop() {
        grbl_sendf(CLIENT_ALL, "Waiting for WiFi to connect...\r\n");
        while (WiFi.status() != WL_CONNECTED && _running == 1) {
            vTaskDelay(portTICK_PERIOD_MS * IDLE_TIME_MS);
        }

        grbl_sendf(CLIENT_ALL, "Waiting for WiFi pin command...\r\n");
        uint8_t currentStatus = 2;
        while (_running == 1) {
            auto newStatus = _lastValue;
            if (currentStatus != newStatus) {
                currentStatus = newStatus;
                uint8_t nextCommand = newStatus;

                grbl_sendf(CLIENT_ALL, "Change status of WiFi pin to %d...\r\n", int(nextCommand));

                const char* on =
                    "00 00 55 aa 00 00 00 03 00 00 00 0d 00 00 00 37 33 2e 33 00 00 00 00 00 00 00 19 00 09 20 e9 0d 59 d9 f9 85 89 ad d1 "
                    "1c 1a 32 7b be 7f 91 60 1b bd fd 8f 0e 14 56 30 6c c8 b1 4c 2a 4e 62 db 2f 89 7b fc 00 00 aa 55 ";
                const char* off =
                    "00 00 55 aa 00 00 00 02 00 00 00 0d 00 00 00 37 33 2e 33 00 00 00 00 00 00 00 18 00 09 20 e9 ee 4b 62 0f 5d 55 43 5a "
                    "55 69 95 4a 91 7e e1 7b 11 09 3b 2c 37 ed fe 4f a2 7a b8 af f1 fd 34 6a 42 f0 88 1a 00 00 aa 55 ";

                auto command = ((nextCommand == 1) ? on : off);

                char sendbuf[100];
                int  n = 0;
                for (const char* it = command; *it;) {
                    int v1 = (*it >= '0' && *it <= '9') ? (*it - '0') : (*it - 'a' + 10);
                    ++it;
                    int v2 = (*it >= '0' && *it <= '9') ? (*it - '0') : (*it - 'a' + 10);
                    it += 2;
                    sendbuf[n++] = char(uint8_t((v1 << 4) | v2));
                }

                // There's our little message. Send away!
                struct sockaddr_in serverAddress;
                serverAddress.sin_family = AF_INET;
                inet_pton(AF_INET, "192.168.1.115", &serverAddress.sin_addr.s_addr);
                serverAddress.sin_port = htons(6668);

                int sock   = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                int result = connect(sock, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in));
                result     = send(sock, sendbuf, n, 0);
                result     = close(sock);

                (void)result;  // TODO: Error handling of result
            } else {
                vTaskDelay(portTICK_PERIOD_MS * IDLE_TIME_MS);
            }
        }

        _running = 0;
        vTaskDelete(NULL);
    }

    void WifiSocketPinDetail::write(int high) {
        Assert(_attributes.has(PinAttributes::Output), "Pin has no output attribute defined. Cannot write to it.");

        if (_running == 0) {
            _running   = 1;
            _lastValue = 2;

            xTaskCreatePinnedToCore(cmdTask,              // task
                                    "vfd_cmdTaskHandle",  // name for task
                                    4096,                 // size of task stack
                                    this,                 // parameters
                                    1,                    // priority
                                    &_cmdTaskHandle,
                                    SUPPORT_TASK_CORE  // core
            );
        }

        uint8_t value = uint8_t(high ^ _readWriteMask);
        _lastValue    = value;
    }

    int WifiSocketPinDetail::read() { return _lastValue ^ _readWriteMask; }

    void WifiSocketPinDetail::setAttr(PinAttributes value) {
        // Check the attributes first:
        Assert(value.validateWith(this->_capabilities), "The requested attributes don't match the pin capabilities");
        Assert(!_attributes.conflictsWith(value), "Attributes on this pin have been set before, and there's a conflict.");

        _attributes = value;
        reset();
    }

    void WifiSocketPinDetail::reset() {
        if (_attributes.has(PinAttributes::InitialOn)) {
            write(HIGH ^ _readWriteMask);
        } else {
            write(LOW ^ _readWriteMask);
        }
    }

    String WifiSocketPinDetail::toString() const { return String("WifiSocket.") + int(_index); }

    WifiSocketPinDetail::~WifiSocketPinDetail() {
        if (_running == 1) {
            _running = 2;
            while (_running == 1) {
                ::delay(1);
            }
            _running = 0;
        }
    }
}
