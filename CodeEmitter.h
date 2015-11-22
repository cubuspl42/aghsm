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

#ifndef AGHSM_CODEGENERATOR_H
#define AGHSM_CODEGENERATOR_H


#include "Parser.h"

#include <unordered_map>

struct Instruction {
    unsigned code : 8;
    unsigned usr : 1;
    unsigned acu : 1;
    unsigned unused1 : 2;
    unsigned mod : 2;
    unsigned unused2 : 2;
    int16_t adr : 16;
};

static_assert(sizeof(Instruction) == 4, "sizeof(Instruction) != 32 bits");

union Word {
    Instruction instruction;
    int32_t data;
};

void printProgram(std::ostream &os, const std::vector<Word> &words);

static_assert(sizeof(Word) == 4, "sizeof(Word) != 32 bits");

class CodeEmitter {
public:
    class CodeEmitterError : public std::logic_error {
    public:
        CodeEmitterError(std::string errorMessage)
                : std::logic_error(errorMessage)
        {}
    };

    enum Section {
        NullSection,
        UnitSection,
        DataSection,
        CodeSection,
        EndSection
    };

    CodeEmitter(const Ast &ast);

    std::vector<Word> emitCode();

private:
    void emitterError(std::string errorMessage);

    void emitWord(Word word);

    void emitDataWords(std::vector<AstNode> words);

    void emitValue(AstNode valueNode, Word word);

    void emitInstruction(AstNode node);

    void markReference(std::string);

    void resolveReferences();

    std::unordered_map<std::string, int> _opcodes;
    const Ast &_ast;
    std::vector<Word> _words;
    Section _currentSection = NullSection;
    std::unordered_map<std::string, int> _labels;
    int _mainLabel = 0;
    std::vector<std::pair<std::string, int>> _references;
};


#endif //AGHSM_CODEGENERATOR_H
