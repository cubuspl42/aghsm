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

#ifndef AGHSM_VM_H
#define AGHSM_VM_H

#include "CodeEmitter.h"

class VM {
public:
    class VMException : public std::logic_error {
    public:
        VMException(std::string errorMessage)
                : std::logic_error(errorMessage)
        {}
    };

    VM();

    void load(std::vector<Word> program);

    void run();

    void print(std::ostream &os);

private:

    Word &word(unsigned address);

    int32_t &Mem(unsigned addres);

    void loadNextInstruction();

    void computeEffectiveAddress();

    void executeNextInstruction();

    struct {
        unsigned run : 1;
    } RR;

    Instruction IR;
    int32_t OR;
    int32_t PC;
    int32_t A, B;

    int32_t *_AC = nullptr;

    std::vector<Word> _program;

};


#endif //AGHSM_VM_H
