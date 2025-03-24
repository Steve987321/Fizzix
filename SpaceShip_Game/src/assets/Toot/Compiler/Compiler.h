#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Toot/TVM/TVM.h"

namespace Compiler
{

    enum class CompileResult
    {
        NONE,
        ERR
    };

    inline std::vector<std::string> error_msgs;
    CompileResult CompileString(std::string_view str, std::vector<VM::Instruction>& res, std::function<void()> pre_parse_callback = {});

}
