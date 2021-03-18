#pragma once

#include "CustomCode.h"
#include "../Grbl.h"

namespace Custom {
    template <>
    class CustomBehavior<CustomCode::CoreXY>
    /* We could use inheritance for default behavior, no need to add virtual calls, we just want the most derived function in all cases. 
        : public CustomBehavior<CustomCode::Default> */
    {
        // The midTbot has a quirk where the x motor has to move twice as far as it would
        // on a normal T-Bot or CoreXY
        //
        // NOTE: We can make a template specialization for midTbot, that removes these #define's.
        //
        // How: change class into class 'CoreXYBehavior' and define MidtBot as CoreXYBehavior<1.0f>

#ifndef MIDTBOT
        const float geometry_factor = 1.0f;
#else
        const float geometry_factor = 2.0f;
#endif

    public:
        void machine_init() {
            // print a startup message to show the kinematics are enable

#ifdef MIDTBOT
            grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Info, "CoreXY (midTbot) Kinematics Init");
#else
            grbl_msg_sendf(CLIENT_SERIAL, MsgLevel::Info, "CoreXY Kinematics Init");
#endif
        }

        bool user_defined_homing(uint8_t cycle_mask) { return false; }
        void inverse_kinematics(float* position) {
            float motors[MAX_N_AXIS];

            motors[X_AXIS] = geometry_factor * position[X_AXIS] + position[Y_AXIS];
            motors[Y_AXIS] = geometry_factor * position[X_AXIS] - position[Y_AXIS];

            position[X_AXIS] = motors[X_AXIS];
            position[Y_AXIS] = motors[Y_AXIS];

            // Z and higher just pass through unchanged }
            // inline void inverse_kinematics(float* target, plan_line_data_t* pl_data, float* position) {}
        }
        void forward_kinematics(float* position) {
            float   calc_fwd[MAX_N_AXIS];
            float   wco[MAX_N_AXIS];
            float   print_position[N_AXIS];
            int32_t current_position[N_AXIS];  // Copy current state of the system position variable

            memcpy(current_position, sys_position, sizeof(sys_position));
            system_convert_array_steps_to_mpos(print_position, current_position);

            // determine the Work Coordinate Offsets for each axis
            auto n_axis = number_axis->get();
            for (int axis = 0; axis < n_axis; axis++) {
                // Apply work coordinate offsets and tool length offset to current position.
                wco[axis] = gc_state.coord_system[axis] + gc_state.coord_offset[axis];
                if (axis == TOOL_LENGTH_OFFSET_AXIS) {
                    wco[axis] += gc_state.tool_length_offset;
                }
            }

            // apply the forward kinemetics to the machine coordinates
            // https://corexy.com/theory.html
            //calc_fwd[X_AXIS] = 0.5 / geometry_factor * (position[X_AXIS] + position[Y_AXIS]);
            calc_fwd[X_AXIS] = ((0.5 * (print_position[X_AXIS] + print_position[Y_AXIS]) * geometry_factor) - wco[X_AXIS]);
            calc_fwd[Y_AXIS] = ((0.5 * (print_position[X_AXIS] - print_position[Y_AXIS])) - wco[Y_AXIS]);

            for (int axis = 0; axis < n_axis; axis++) {
                if (axis > Y_AXIS) {  // for axes above Y there is no kinematics
                    calc_fwd[axis] = print_position[axis] - wco[axis];
                }
                position[axis] = calc_fwd[axis];  // this is the value returned to reporting
            }
        }
        bool kinematics_pre_homing(uint8_t cycle_mask) { return false; }
        // etc, whatever you want to customize
    };
}
