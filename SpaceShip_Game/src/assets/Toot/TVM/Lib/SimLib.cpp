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

    add.x = x_reg.value.flt;
    add.y = y_reg.value.flt;

    sim.polygons[0].rb.velocity += add;

    return {};
}

VMRegister SimLib::GetSome(VM& vm, const std::vector<VMRegister>& args)
{
    VMRegister res; 
    res.type = VMRegisterType::FLOAT;
    res.value.flt = 0.5f;
    return res;
}

VMRegister SimLib::GetY(VM& vm, const std::vector<VMRegister>& args)
{
    fz::Sim& sim = Sim::GetSim();
    VMRegister res;
    res.type = VMRegisterType::FLOAT;
    res.value.flt = sim.polygons[0].rb.center.y;
    return res; 
}

VMRegister SimLib::GetDY(VM& vm, const std::vector<VMRegister>& args)
{
    fz::Sim& sim = Sim::GetSim();
    VMRegister res;
    res.type = VMRegisterType::FLOAT;
    res.value.flt = Sim::d_y;
    return res; 
}

// VMRegister SimLib::GetTargetY(VM& vm, const std::vector<VMRegister>& args)
// {
//     VMRegister res; 
//     res.type = VMRegisterType::FLOAT;
//     res.value.flt = -10.f;
//     return res;
// }

VMRegister SimLib::DrawCrossXY(VM &vm, const std::vector<VMRegister> &args)
{
    VMRegister x_reg = args[1];
    VMRegister y_reg = args[2];

    while (x_reg.type == VMRegisterType::REGISTER)
        x_reg = vm.GetReg(x_reg.value.flt);
    while (y_reg.type == VMRegisterType::REGISTER)
        y_reg = vm.GetReg(y_reg.value.flt);

    float cross_size = 10.f;
    Toad::Vec2f position = {x_reg.value.flt, y_reg.value.flt};
    position -= cross_size / 2.f;

    Toad::DrawingCanvas::DrawArrow(position, {cross_size, cross_size},  2.f);    
    position.y += cross_size / 2.f;
    Toad::DrawingCanvas::DrawArrow(position, {-cross_size, -cross_size},  2.f);    
   
    return {};
}

CPPLib SimLib::GetSimLib()
{
    CPPLib l;
    l.name = "SimLib";
    
    REGISTER_LIBFUNC(l, DoSome, "registerregister")
    REGISTER_LIBFUNC(l, DrawCrossXY, "registerregister")
    // REGISTER_LIBFUNC(l, GetTargetY, "")
    REGISTER_LIBFUNC(l, GetY, "")
    REGISTER_LIBFUNC(l, GetDY, "")
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
