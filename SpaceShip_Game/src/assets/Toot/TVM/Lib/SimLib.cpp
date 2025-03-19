#include "SimLib.h"
#include "Toot/TVM/TVM.h"

#include "framework/Framework.h"
#include "scripts/Sim.h"
#include "Fizzix/FZSim.h"

VMRegister SimLib::DoSome(VM &vm, const std::vector<VMRegister> &args)
{
    fz::Sim& sim = Sim::GetSim();
    if (sim.polygons.empty())
        return {};
    
    Toad::Vec2f add;
    VMRegister x_reg = args[1];
    VMRegister y_reg = args[2];

    while (x_reg.type == VMRegisterType::REGISTER)
        x_reg = vm.GetReg(x_reg.value.num);
    while (y_reg.type == VMRegisterType::REGISTER)
        y_reg = vm.GetReg(y_reg.value.num);

    add.x = (float)x_reg.value.num;
    add.y = (float)y_reg.value.num;

    sim.polygons[0].rb.velocity += add;

    return {};
}

VMRegister SimLib::GetSome(VM& vm, const std::vector<VMRegister>& arsg)
{
    fz::Sim& sim = Sim::GetSim();
    VMRegister res; 
    res.type = VMRegisterType::FLOAT;
    res.value.flt = 0.5f;

    return res;
}

CPPLib SimLib::GetSimLib()
{
    CPPLib l;
    l.name = "SimLib";
    
    REGISTER_LIBFUNC(l, DoSome, "registerregister")
    REGISTER_LIBFUNC(l, GetSome, "")

    return l;
}

void SimLib::RegisterToVM(VM& vm)
{
    CPPLib lib = GetSimLib();
    for (const CPPFunction& f : lib.functions)
    {
        vm.functions[f.function_sig] = f;
    }
}
