#pragma once

#include "VFDSpindle.h"

/*
    HuanyangSpindle.h

    Part of Grbl_ESP32
    2020 -    Bart Dring
    2020 -  Stefan de Bruijn

    Grbl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Grbl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

*/

namespace Spindles {
    class Huanyang : public VFD {
    private:
        int reg;

    protected:
        void default_modbus_settings(uart_config_t& uart) override;

        void direction_command(SpindleState mode, ModbusCommand& data) override;
        void set_speed_command(uint32_t rpm, ModbusCommand& data) override;

        response_parser get_status_ok(ModbusCommand& data) override;
        /* TODO TEST THIS:
        response_parser get_current_rpm(ModbusCommand& data) override;

        bool supports_actual_rpm() const override { return true; }
        */
    };
}
