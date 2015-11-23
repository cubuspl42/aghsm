/// Copyright (c) 2015 Jakub Trzebiatowski
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "VM.h"
#include "Language.h"

VM::VM() {

}

void VM::load(std::vector<Word> program) {
    _program = program;
}

void VM::run() {
    RR.run = 1;
    IR = {0};
    OR = 0;
    PC = 0;
    A = 0;
    B = 0;

    PC = word(0).data;

    while(RR.run) {
        //print(std::cout);
        loadNextInstruction();
        computeEffectiveAddress();
        executeNextInstruction();
    }
}

void VM::print(std::ostream &os) {
    os << "< " << "@PC = " << PC << " @A = " << A << " @B = " << B << " >" << std::endl;
    printProgram(os, _program);
}

Word &VM::word(unsigned address) {
    if(address % 4) {
        throw VMException{"unaligned memory access"};
    }
    if(address / 4 >= _program.size()) {
        throw VMException{"out of program memory access"};
    }
    return _program[address / 4];
}

int32_t &VM::Mem(unsigned address) {
    return word(address).data;
}

void VM::loadNextInstruction() {
    IR = word(PC).instruction;
    PC += 4;
}

void VM::computeEffectiveAddress() {
    switch(IR.mod) {
        case 0:
            OR = IR.adr;
            break;
        case 1:
            OR = Mem(IR.adr);
            break;
        case 2:
            OR = Mem(Mem(IR.adr));
            break;
        default:
            throw VMException{"unsupported addressing mode"};
    }
}

void VM::executeNextInstruction() {
    std::string name = instructions[IR.code];

    {
        int32_t AC = _AC ? *_AC : 0;

        switch(IR.code) {
            case JumpInstruction:
                PC = OR;
                return;
            case JzeroInstruction:
                if (AC == 0) PC = OR;
                return;
            case JnzeroInstruction:
                if (AC != 0) PC = OR;
                return;
            case JposInstruction:
                if (AC > 0) PC = OR;
                return;
            case JnegInstruction:
                if (AC < 0) PC = OR;
                return;
        }
    }

    {
        int32_t &AC = IR.acu == 0 ? A : B;

        switch(IR.code) {
            case NullInstruction:
                return;
            case HaltInstruction:
                RR.run = 0;
                return;
            case LoadInstruction:
                AC = OR;
                _AC = &AC;
                return;
            case StoreInstruction:
                Mem(OR) = AC;
                _AC = &AC;
                return;
            case AddInstruction:
                AC = AC + OR;
                _AC = &AC;
                return;
            case SubInstruction:
                AC = AC - OR;
                _AC = &AC;
                return;
            case MultInstruction:
                AC = AC * OR;
                _AC = &AC;
                return;
            case DivInstruction:
                AC = AC / OR;
                _AC = &AC;
                return;
            case PrintInstruction:
                if(IR.usr) {
                    std::cout << OR << std::endl;
                } else {
                    std::cout << AC << std::endl;
                }
                return;
            case DumpInstruction:
                this->print(std::cout);
                return;
        }
    }

    throw VMException{"unrecognized instruction"};
}
