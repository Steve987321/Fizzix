#include "CPPBinding.h"

class VM;
struct VMRegister;

namespace SimLib
{
	VMRegister DoSome(VM& vm, const std::vector<VMRegister>& args);

	CPPLib GetSimLib();

	void RegisterToVM(VM& vm);
}
