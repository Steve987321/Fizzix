#pragma once

#include <vector>
#include <string>

#include "Toot/TVM/TVM.h"

namespace Compiler
{

    enum class CompileResult
    {
        NONE,
        ERR
    };

    inline std::vector<std::string> error_msgs;
    CompileResult CompileString(std::string_view str, std::vector<VM::Instruction>& res);

}
