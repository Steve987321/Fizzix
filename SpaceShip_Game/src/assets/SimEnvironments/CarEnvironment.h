#pragma once 

inline const char* CarControllerScript = 
R"(once float spring_stiffness_factor = 1.0;

once 
{
    // saves the spring properties so this can apply a stiffness factor 
    CESaveSpringStates();
}

int key_d = 3;
int key_a = 0;
int key_space = 57;
int key_d_down = IsKeyDown(key_d);
int key_a_down = IsKeyDown(key_a);
int key_space_down = IsKeyDown(key_space);

if (key_space_down == 1)
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
    // release jump
    spring_stiffness_factor = 1.0; 
}

float gas = 0.0;

if (key_d_down == 1)
{
    gas = gas + 10.0;
}
if (key_a_down == 1)
{
    gas = gas - 10.0;
}

CESetSpringStiffnessFactor(spring_stiffness_factor);
CEApplyGas(gas);
)";

// load a scene with a spring connected vehicle and obstacles 
// pass a vm to use as controller script to control the vehicle using the SimLib functions 
void CarEnvironmentLoad();
void CarEnvironmentUpdate(float gas);
void CarEnvironmentImGui();
