#pragma once 

inline const char* CarControllerScript = 
R"(

once float spring_stiffness_factor = 1.0;

if (CEIsJumpKeyPressed() == 1)
{
    if (spring_stiffness_factor > 0.0)
    {
        // charge jump
        spring_stiffness_factor = spring_stiffness_factor - GetDT();  
    }
    else 
    {
        // clamp to 0 
        spring_stiffness_factor = 0.0; 
    }
}
else 
{
    if (CEIsJumpKeyReleased() == 1)
    {
        // release jump
        spring_stiffness_factor = 1.0; 
    }
}

float gas = 0.0;

if (CEIsGasKeyPressed() == 1)
{
    gas = 10.0;
}
if (CEIsReverseKeyPressed() == 1)
{
    gas = -10.0;
}

CESetSpringStiffnessFactor(spring_stiffness_factor);
CEApplyWheelTorque(gas);

)";

// load a scene with a spring connected vehicle and obstacles 
// pass a vm to use as controller script to control the vehicle using the SimLib functions 
void CarEnvironmentLoad();
void CarEnvironmentUpdate(float gas);
void CarEnvironmentImGui();
