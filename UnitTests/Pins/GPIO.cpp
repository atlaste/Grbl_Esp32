#include "gtest/gtest.h"

#include <src/Pin.h>
#include "../Support/SoftwareGPIO.h"

namespace Pins {
    Pin Init() {
        SoftwareGPIO::reset();
        PinLookup::ResetAllPins();
        Pin pin = Pin::create("GPIO.16");
        return pin;
    }

    TEST(ReadInputPin, GPIO) {
        auto pin = Init();

        pin.setAttr(Pin::Attr::Input);
        ASSERT_EQ(false, pin.read());

        SoftwareGPIO::instance().set(16, true);

        ASSERT_EQ(true, pin.read());
    }

    TEST(ReadOutputPin, GPIO) {
        auto pin = Init();

        pin.setAttr(Pin::Attr::Output);
        ASSERT_ANY_THROW(pin.read());
    }

    TEST(WriteInputPin, GPIO) {
        auto pin = Init();

        pin.setAttr(Pin::Attr::Input);
        ASSERT_ANY_THROW(pin.on());
    }

    TEST(WriteOutputPin, GPIO) {
        auto pin = Init();

        pin.setAttr(Pin::Attr::Output);

        ASSERT_EQ(false, SoftwareGPIO::instance().get(16));

        pin.on();
        ASSERT_EQ(true, SoftwareGPIO::instance().get(16));

        pin.off();
        ASSERT_EQ(false, SoftwareGPIO::instance().get(16));
    }

    TEST(ReadIOPin, GPIO) {
        auto pin = Init();

        pin.setAttr(Pin::Attr::Output | Pin::Attr::Input);
        ASSERT_EQ(false, pin.read());
        ASSERT_EQ(false, SoftwareGPIO::instance().get(16));

        pin.on();
        ASSERT_EQ(true, pin.read());
        ASSERT_EQ(true, SoftwareGPIO::instance().get(16));

        pin.off();
        ASSERT_EQ(false, pin.read());
        ASSERT_EQ(false, SoftwareGPIO::instance().get(16));
    }

    void TestISR(int deltaRising, int deltaFalling, int mode) {
        auto pin = Init();
        pin.setAttr(Pin::Attr::Input | Pin::Attr::Output | Pin::Attr::ISR);

        int hitCount = 0;
        int expected = 0;
        pin.attachInterrupt(
            [](void* arg) {
                int* hc = static_cast<int*>(arg);
                ++(*hc);
            },
            mode,
            &hitCount);

        // Two ways to set I/O:
        // 1. using on/off
        // 2. external source (e.g. set softwareio pin value)
        //
        // We read as well, because that shouldn't modify the state.

        pin.on();
        expected += deltaRising;
        ASSERT_EQ(hitCount, expected);
        ASSERT_EQ(true, pin.read());

        pin.off();
        expected += deltaFalling;
        ASSERT_EQ(hitCount, expected);
        ASSERT_EQ(false, pin.read());

        SoftwareGPIO::instance().set(16, true);
        expected += deltaRising;
        ASSERT_EQ(hitCount, expected);
        ASSERT_EQ(true, pin.read());

        SoftwareGPIO::instance().set(16, false);
        expected += deltaFalling;
        ASSERT_EQ(hitCount, expected);
        ASSERT_EQ(false, pin.read());

        // Detach interrupt. Regardless of what we do, it shouldn't change hitcount anymore.
        pin.detachInterrupt();
        pin.on();
        pin.off();
        SoftwareGPIO::instance().set(16, true);
        SoftwareGPIO::instance().set(16, false);
        ASSERT_EQ(hitCount, expected);
    }

    TEST(ISRRisingPin, GPIO) { TestISR(1, 0, RISING); }

    TEST(ISRFallingPin, GPIO) { TestISR(0, 1, FALLING); }

    TEST(ISRChangePin, GPIO) { TestISR(1, 1, CHANGE); }
}
