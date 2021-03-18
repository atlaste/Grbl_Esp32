#pragma once

namespace Custom
{
    enum class CustomCode
    {
        Default,
        CoreXY,
        /* Add more specializations here. */
    };

    template <CustomCode specialization>
    class CustomBehavior
    {
    public:
        inline void machine_init() { }
        inline bool user_defined_homing(uint8_t cycle_mask) { return false; }
        inline void inverse_kinematics(float* position) {}
        // inline void inverse_kinematics(float* target, plan_line_data_t* pl_data, float* position) {}
        inline void forward_kinematics(float* position) {}
        inline bool kinematics_pre_homing(uint8_t cycle_mask) {}
        // etc, whatever you want to customize
    };
}
