/*
  kinematics.cpp - Implements simple inverse kinematics for Grbl_ESP32
  Part of Grbl_ESP32

  Copyright (c) 2019 Barton Dring @buildlog


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

	Inverse kinematics determine the joint parameters required to get to a position
	in 3D space. Grbl will still work as 3 axes of steps, but these steps could
	represent angles, etc instead of linear units.

	Unless forward kinematics are applied to the reporting, Grbl will report raw joint
	values instead of the normal Cartesian positions

	How it works...

	If you tell it to go to X10 Y10 Z10 in Cartesian space, for example, the equations
  will convert those values to the required joint values. In the case of a polar machine, X represents the radius,
	Y represents the polar degrees and Z would be unchanged.

	In most cases, a straight line in Cartesian space could cause a curve in the new system.
	To fix this, the line is broken into very small segments and each segment is converted
	to the new space. While each segment is also distorted, the amount is so small it cannot be seen.

	This segmentation is how normal Grbl draws arcs.

	Feed Rate

	Feed rate is given in steps/time. Due to the new coordinate units and non linearity issues, the
	feed rate may need to be adjusted. The ratio of the step distances in the original coordinate system
	determined and applied to the feed rate.

	TODO:
		Add y offset, for completeness
		Add ZERO_NON_HOMED_AXES option


*/
#include "grbl.h"

#ifdef USE_KINEMATICS

void inverse_kinematics(float *target, plan_line_data_t *pl_data, float *position)
{	
	// use the current tool as a flag for converting for different machine mode.
	
	// target: This is the target location for the move in machine space millimeters.
	// position: This is the previous position in machine space millimeters
	
	float offsets[N_AXIS];    // current work offsets
	float new_system[N_AXIS]; // target location in the new system
	uint8_t index;
	
	// All data is in machine space and axes have different offsets, so we need to apply the offset in corrections
	for (index = 0; index < N_AXIS; index++) {
		offsets[index] = gc_state.coord_system[index] + gc_state.coord_offset[index]; // offset from machine coordinate system
		grbl_sendf(CLIENT_SERIAL, "[MSG: Kins Offset %d %4.3f, Target %4.3f]\r\n", index, offsets[index], target[index] - offsets[index]);
	}	
	
	switch (gc_state.tool) {
		case 1: // XYZ only ignore all ABC
			grbl_sendf(CLIENT_SERIAL, "[MSG:Kin Tool 1]\r\n");
			new_system[X_AXIS] = target[X_AXIS];
			new_system[Y_AXIS] = target[Y_AXIS];
			new_system[Z_AXIS] = target[Z_AXIS];
			// remove ABC changes by setting to existing values
			new_system[A_AXIS] = position[A_AXIS];
			new_system[B_AXIS] = position[B_AXIS];
			new_system[C_AXIS] = position[C_AXIS];
			mc_line(new_system, pl_data);
		break;
		case 2: // Convert XYZ to ABC (no motion on ABC)
			// Remove XYZ by setting to existing values
			grbl_sendf(CLIENT_SERIAL, "[MSG:Kin Tool 2]\r\n");
			new_system[X_AXIS] = position[X_AXIS];
			new_system[Y_AXIS] = position[Y_AXIS];
			new_system[Z_AXIS] = position[Z_AXIS];
			// Use XYZ values for ABC
			new_system[A_AXIS] = (target[X_AXIS] - offsets[X_AXIS]) + offsets[A_AXIS]; // they have different offsets, so we need to correct for that
			new_system[B_AXIS] = (target[Y_AXIS] - offsets[Y_AXIS]) + offsets[B_AXIS];
			new_system[C_AXIS] = (target[Z_AXIS] - offsets[Z_AXIS]) + offsets[C_AXIS];
			
			
			grbl_sendf(CLIENT_SERIAL, "[MSG:Target XYZ %4.3f, %4.3f, %4.3f]\r\n", target[X_AXIS], target[Y_AXIS], target[Z_AXIS]);
			grbl_sendf(CLIENT_SERIAL, "[MSG:Offset XYZ %4.3f, %4.3f, %4.3f]\r\n", offsets[X_AXIS], offsets[Y_AXIS], offsets[Z_AXIS]);
			grbl_sendf(CLIENT_SERIAL, "[MSG:Offset ABC %4.3f, %4.3f, %4.3f]\r\n", offsets[A_AXIS], offsets[B_AXIS], offsets[C_AXIS]);
			grbl_sendf(CLIENT_SERIAL, "[MSG: New %4.3f, %4.3f, %4.3f]\r\n", new_system[A_AXIS], new_system[B_AXIS], new_system[C_AXIS]);
			
			mc_line(new_system, pl_data);
		break;
		default: // tool 0 or above 2 uses normal 6 axis code...no changes
			grbl_sendf(CLIENT_SERIAL, "[MSG:Kin normal]\r\n");
			mc_line(target, pl_data);
			grbl_sendf(CLIENT_SERIAL, "[MSG: T0 %4.3f, %4.3f, %4.3f]\r\n", target[A_AXIS], target[B_AXIS], target[C_AXIS]);
		break;
	}		
}

#endif