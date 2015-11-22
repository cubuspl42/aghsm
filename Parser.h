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

#ifndef AGHSM_PARSER_H
#define AGHSM_PARSER_H

#include "Lexer.h"

#include <memory>
#include <vector>

struct AstNode {
    enum Type {
        NullNode,
        RootNode,
        LabelNode,
        DirectiveNode,
        InstructionNode,
        RegisterNode,
        NumberNode,
        MultinumberNode,
        ParenNode,
        ReferenceNode,
    };

    AstNode(Type type = NullNode) : type(type) {}

    void print(std::ostream &os, std::string prefix);

    Type type;
    int64_t aValue = 0;
    int64_t bValue = 0;
    std::string sValue;
    std::vector<AstNode> children;
};

struct Ast {
    AstNode rootNode;

    Ast();

    void print(std::ostream &os);
};

class Parser {
public:
    class ParserError : public std::logic_error {
    public:
        ParserError(std::string errorMessage)
                : std::logic_error(errorMessage)
        {}
    };

    Parser(const TokenStream &tokenStream);

    Ast parse();

private:

    void parserError(std::string errorMessage, Token token);

    Token readToken();

    Token peekToken();

    AstNode parseExpression();

    void parseLine();

    std::unordered_set<std::string> _directives;
    std::unordered_set<std::string> _instructions;

    int _currentTokenNo = 0;
    TokenStream _tokenStream;
    Ast _ast;
};

#endif //AGHSM_PARSER_H
