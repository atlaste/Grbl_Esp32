#pragma once

#include "CustomCode.h"
#include "CoreXY.h"

// ... Include all other custom code we want to add.
//
// Don't worry about getting a long list here; as long as these #include files don't #include a lot of files,
// it won't generate any extra code. Compilers will _only_ compile template specializations that are actually 
// being used. Nothing else.
// 
// Remember that we defined an enum in CustomCode? If we want to use a specific template specialization, 
// we can now do something like this, which we can reference. We can set 'Default' based on a #define:
// 
// Custom::CustomBehavior<Custom::CustomCode::Default> CustomBehaviors;
