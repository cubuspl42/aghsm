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

#include "Assembler.h"

#include "Lexer.h"
#include "Parser.h"
#include "CodeEmitter.h"

std::vector<Word> Assembler::compile() {
    Lexer lexer(_sourceStream);
    TokenStream tokenStream = lexer.lex();

    //tokenStream.print(std::cout);

    Parser parser{tokenStream};
    Ast ast = parser.parse();

    //ast.print(std::cout);

    CodeEmitter codeGenerator(ast);
    std::vector<Word> program = codeGenerator.emitCode();

    //printProgram(std::cout, program);

    return program;
}
