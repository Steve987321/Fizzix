#include "TVM.h"

#include <cassert>
#include <format>
#include <string>
#include <iostream>

void VM::Init()
{
	// #todo #include brrr
	IO::RegisterToVM(*this);
	SimLib::RegisterToVM(*this);
	
	rel_reg_stack.push(0);

	relative_register_index = 0;
	instruction_pointer = 0;

	// register labels 
	size_t loc = 0;
	for (const Instruction& i : instructions)
	{
		if (i.op == OP_DEFINE_LABEL)
		{
			// add to functions or if just label add to somethign eles
			assert(i.args.size() == 1);
			OpLabel(i.args[0], loc);
			break;
		}
        
		loc++;
	}
}

const VMRegister* VM::GetValueReg(const VMRegister& reg)
{
    if (reg.type == VMRegisterType::REGISTER)
    {
        const VMRegister* res = &registers[reg.value.num + relative_register_index];
        
        while (res->type == VMRegisterType::REGISTER)
            res = &registers[reg.value.num + relative_register_index];
        
        return res;
    }
    else
    {
        return &reg;
    }
}

void VM::Run()
{
	while (instruction_pointer < instructions.size())
	{
		Instruction& i = instructions[instruction_pointer];

		switch (i.op)
		{
		case OP_MOVE:
			assert(i.args.size() == 2);
			OpMove(i.args[0], i.args[1]);
			break;
		case OP_ADD:
			assert(i.args.size() == 3);
			OpAdd(i.args[0], i.args[1], i.args[2]);
			break;
		case OP_SUBTRACT:
			assert(i.args.size() == 3);
			OpSubtract(i.args[0], i.args[1], i.args[2]);
			break;
		case OP_MULTIPLY:
			assert(i.args.size() == 3);
			OpMultiply(i.args[0], i.args[1], i.args[2]);
			break;
		case OP_DIVIDE:
			assert(i.args.size() == 3);
			OpDivide(i.args[0], i.args[1], i.args[2]);
			break;
		case OP_CALL:
			OpCall(i.args);
			break;
        case OP_CALL_MOVE:
           	OpCallMove(i.reserved, i.args);
            break;
		case OP_JUMP:
			assert(i.args.size() == 1);
			OpJump(i.args[0]);
			break;
        case OP_JUMP_IF_NOT_EQUAL:
            assert(i.args.size() == 3);
            OpJumpIfNotEqual(i.args[0], i.args[1], i.args[2]);
            break;
		case OP_RETURN:
			break;
		default: 
			break;
		}

		instruction_pointer++;
	}
}

void VM::OpMove(const VMRegister& dst, const VMRegister& src)
{
	assert(dst.type == VMRegisterType::REGISTER);
    
	VMRegister* dst_reg = &registers[dst.value.num + relative_register_index];
    VMRegister* src_reg = nullptr;
    if (src.type == VMRegisterType::REGISTER)
    {
        src_reg = &registers[src.value.num + relative_register_index];
        while (src_reg->type == VMRegisterType::REGISTER)
            src_reg = &registers[src_reg->value.num + relative_register_index];
        
        *dst_reg = *src_reg;
    }
    else
        *dst_reg = src;
}

void VM::OpAdd(const VMRegister& dst, const VMRegister& a, const VMRegister& b)
{
    const VMRegister* l = GetValueReg(a);
    const VMRegister* r = GetValueReg(b);
    
	assert(l && r);

	VMRegister* dst_reg = &registers[dst.value.num + relative_register_index];
	//*dst_reg = *l;

	switch (l->type)
	{
    case VMRegisterType::STRING:
        break;
    case VMRegisterType::FLOAT:
        dst_reg->type = VMRegisterType::FLOAT;
        dst_reg->value.flt = l->value.num + r->value.flt;
        break;
    case VMRegisterType::INT:
        dst_reg->type = VMRegisterType::INT;
        dst_reg->value.num = l->value.num + r->value.num;
        break;
	default:
		break;
	}
}

void VM::OpSubtract(const VMRegister& dst, const VMRegister& a, const VMRegister& b)
{
    const VMRegister* l = GetValueReg(a);
    const VMRegister* r = GetValueReg(b);
    
	assert(l && r);

	VMRegister* dst_reg = &registers[dst.value.num + relative_register_index];
	//*dst_reg = *l;

	switch (l->type)
	{
    case VMRegisterType::STRING:
		break;
    case VMRegisterType::FLOAT:
		dst_reg->type = VMRegisterType::FLOAT;
		dst_reg->value.flt = l->value.flt - r->value.flt;
		break;
    case VMRegisterType::INT:
		dst_reg->type = VMRegisterType::INT;
		dst_reg->value.num = l->value.num - r->value.num;
		break;
	default:
		break;
	}
}

void VM::OpMultiply(const VMRegister& dst, const VMRegister& a, const VMRegister& b)
{
    const VMRegister* l = GetValueReg(a);
    const VMRegister* r = GetValueReg(b);
	
	assert(l && r);
    assert(dst.type == VMRegisterType::REGISTER);

	VMRegister* dst_reg = &registers[dst.value.num + relative_register_index];

	switch (l->type)
	{
        case VMRegisterType::FLOAT:
            dst_reg->type = VMRegisterType::FLOAT;
            dst_reg->value.flt = l->value.flt * r->value.flt;
		break;
        case VMRegisterType::INT:
            dst_reg->type = VMRegisterType::INT;
            dst_reg->value.num = l->value.num * r->value.num;
		break;
	default:
		break;
	}
}

void VM::OpDivide(const VMRegister& dst, const VMRegister& a, const VMRegister& b)
{
    const VMRegister* l = GetValueReg(a);
    const VMRegister* r = GetValueReg(b);
    
	assert(l && r);

	VMRegister* dst_reg = &registers[dst.value.num + relative_register_index];
	*dst_reg = *l;

	switch (l->type)
	{
    case VMRegisterType::FLOAT:
		dst_reg->type = VMRegisterType::FLOAT;
		dst_reg->value.flt = l->value.flt / r->value.flt;
		break;
    case VMRegisterType::INT:
		dst_reg->type = VMRegisterType::INT;
		dst_reg->value.num = l->value.num / r->value.num;
		break;
	default:
		break;
	}
}

void VM::OpLabel(const VMRegister& a, size_t ip)
{
	pre_label_ip = ip;
    labels[a.value.num] = ip;
}

void VM::OpCall(const std::vector<VMRegister>& args)
{
	BeginRelativeRegIndex(instruction_pointer);

	auto function_it = functions.find(args[0].value.str);
	if (function_it != functions.end())
		function_it->second.func(*this, args);
	else
	{
		auto labels_it = labels.find(args[0].value.num);
		if (labels_it != labels.end())
		{
			// go to label 
			instruction_pointer = labels_it->second;
		}
		else
		{
			throw std::runtime_error("no function like that");
		}
	}

	EndRelativeRegIndex();
}

void VM::OpCallMove(const VMRegister& dst, const std::vector<VMRegister>& args)
{
    BeginRelativeRegIndex(instruction_pointer);

    auto function_it = functions.find(args[0].value.str);
    if (function_it != functions.end())
        OpMove(dst, function_it->second.func(*this, args));

    EndRelativeRegIndex();
}

VMRegister& VM::GetReg(uint64_t index)
{
	assert(index + relative_register_index < registers.size() && "iNCcrease Vm Registers size");

	return registers[index + relative_register_index];
}

void VM::OpJump(const VMRegister& a)
{
    instruction_pointer = labels.find(a.value.num)->second;
}

void VM::OpJumpIfNotEqual(const VMRegister& jump, const VMRegister& a, const VMRegister& b)
{
    bool v = false;
    VMRegister* l = &registers[a.value.num];
    VMRegister* r = &registers[b.value.num];
    
    assert(l && r);
    
    switch(l->type)
    {
        case VMRegisterType::INT:
            v = l->value.num != r->value.num;
            break;
        case VMRegisterType::FLOAT:
            v = l->value.flt != r->value.flt;
            break;
        default:
            break;
    }

    if (v)
    {
        instruction_pointer = labels.find(jump.value.num)->second;
    }
}

void VM::BeginRelativeRegIndex(uint64_t rel)
{
	relative_register_index += rel_reg_stack.top();
	rel_reg_stack.push(relative_register_index);
}

void VM::EndRelativeRegIndex()
{
	relative_register_index -= rel_reg_stack.top();
	rel_reg_stack.pop();
}

