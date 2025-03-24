#include "Compiler.h"
#include "TVM.h"

const char* source =
R"(
	// string s = "stringa"; // X
    once float f = 0.0;

    if (f > 0.2)
    {
        WriteOut(f);
    }

    once
    {
        WriteOut(543);
    }

        WriteOut(2222);

    // Nothing();
	//int a = -5;
    //Nothing(a,-a);
	
	// WriteOut(f"c = {c}"); // Test the format string 
	// WriteOut(c); 
	// WriteOut(f); 
	// WriteOut(1000);
	// WriteOut(100*20);
)";

#include <iostream>
int main()
{
    std::vector<VM::Instruction> byte_codes;
    
    if (Compiler::CompileString(source, byte_codes) != Compiler::CompileResult::ERR)
    {
        VM vm;
        IO::RegisterToVM(vm);

        vm.instructions = byte_codes;
        vm.Init();
        vm.Run();
        vm.instruction_pointer = 0;
        vm.Run();
    }
    
    for (const std::string& err : Compiler::error_msgs)
    {
        std::cout << err << std::endl;
    }

}
