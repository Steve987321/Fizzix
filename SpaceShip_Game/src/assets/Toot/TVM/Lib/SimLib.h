#include "CPPBinding.h"

class VM;
struct VMRegister;

namespace SimLib
{
	VMRegister DoSome(VM& vm, const std::vector<VMRegister>& args);
	VMRegister GetSome(VM& vm, const std::vector<VMRegister>& args);
	VMRegister GetY(VM& vm, const std::vector<VMRegister>& args);
	VMRegister GetDY(VM& vm, const std::vector<VMRegister>& args);
	VMRegister GetTargetY(VM& vm, const std::vector<VMRegister>& args);
	VMRegister DrawCrossXY(VM& vm, const std::vector<VMRegister>& args);

	CPPLib GetSimLib();

	void RegisterToVM(VM& vm);
}
