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

#include "Language.h"
#include "Parser.h"

#include <string>

static std::string indent = "    ";

void AstNode::print(std::ostream &os, std::string prefix) {
    os << prefix << this->sValue  << std::endl;
    for(auto &child : this->children) {
        child.print(os, prefix + indent);
    }
}

Ast::Ast() : rootNode{AstNode::RootNode} {
    rootNode.sValue = "<ast>";
}

void Ast::print(std::ostream &os) {
    this->rootNode.print(os, "");
}

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

void Parser::parserError(std::string errorMessage, Token token) {
    std::stringstream ss;
    ss << ":" << token.lineNumber << ":" << token.columnNumber << ": Parser error: " << errorMessage;
    throw ParserError{ ss.str() };
}

Token Parser::readToken() {
    Token token = _tokenStream.getTokenAt(_currentTokenNo);
    ++_currentTokenNo;
    return token;
}

Ast Parser::parse() {
    while(peekToken().type != Token::NullToken) {
        parseLine();
    }

    return std::move(_ast);
}

Token Parser::peekToken() {
    if(_currentTokenNo < _tokenStream.size()) {
        Token token = _tokenStream.getTokenAt(_currentTokenNo);
        return token;
    } else {
        return Token{};
    }
}

AstNode Parser::parseExpression() {
    Token firstToken = readToken();
    if(firstToken.type == Token::IdentifierToken) {
        auto referenceNode = AstNode{AstNode::ReferenceNode};
        referenceNode.sValue = firstToken.tokenData;

        return referenceNode;
    } else if(firstToken.type == Token::NumberToken) {
        if(peekToken().type == Token::DelimiterToken && peekToken().tokenData == "#") {
            readToken();

            Token secondToken = readToken();
            if(secondToken.type != Token::NumberToken) {
                parserError("expected number", secondToken);
            }

            auto multinumberNode = AstNode{AstNode::MultinumberNode};
            multinumberNode.aValue = std::stoi(firstToken.tokenData);
            multinumberNode.bValue = std::stoi(secondToken.tokenData);
            multinumberNode.sValue = firstToken.tokenData + "#" + secondToken.tokenData;

            return multinumberNode;
        } else {
            auto numberNode = AstNode{AstNode::NumberNode};
            numberNode.aValue = std::stoi(firstToken.tokenData);
            numberNode.sValue = firstToken.tokenData;

            return numberNode;
        }
    } else if(firstToken.type == Token::RegisterToken) {
        auto registerNode = AstNode{AstNode::RegisterNode};
        registerNode.sValue = firstToken.tokenData;

        return registerNode;
    } else if(firstToken.type == Token::DelimiterToken && firstToken.tokenData == "(") {
        auto parenNode = AstNode{AstNode::ParenNode};
        parenNode.sValue = "()";
        auto innerNode = parseExpression();
        parenNode.children.push_back(innerNode);

        Token nextToken = readToken();
        if(nextToken.type != Token::DelimiterToken || nextToken.tokenData != ")") {
            parserError("unclosed bracket", nextToken);
        }

        return parenNode;
    }
    parserError("unexpected token", firstToken);
    return AstNode{}; // unreachable
}

void Parser::parseLine() {
    Token labelToken = peekToken();

    // Parse label (optional)

    if(labelToken.type == Token::IdentifierToken) {
        labelToken = readToken();
        Token colonToken = readToken();

        if(colonToken.type != Token::DelimiterToken || colonToken.tokenData != ":") {
            parserError("colon expected", colonToken);
        }

        auto labelNode = AstNode{AstNode::LabelNode};
        labelNode.sValue = labelToken.tokenData;
        _ast.rootNode.children.push_back(std::move(labelNode));
    }

    // Parse keyword

    if(peekToken().type == Token::LineTerminatorToken) {
        readToken(); // Line with just label
        return;
    }

    Token keywordToken = readToken();

    if(keywordToken.type != Token::KeywordToken) {
        parserError("keyword expected", keywordToken);
    }

    AstNode instructionNode;
    if(_directives.find(keywordToken.tokenData) != _directives.end()) {
        instructionNode.type = AstNode::DirectiveNode;
    } else if(_instructions.find(keywordToken.tokenData) != _instructions.end()) {
        instructionNode.type = AstNode::InstructionNode;
    } else {
        parserError("unrecognized keyword", keywordToken);
    }

    instructionNode.sValue = keywordToken.tokenData;

    // Parse args

    Token nextToken = peekToken();

    while(nextToken.type == Token::DelimiterToken && nextToken.tokenData == ",") {
        readToken();
        if(peekToken().type != Token::LineTerminatorToken) {
            AstNode expressionNode = parseExpression();
            instructionNode.children.push_back(expressionNode);
        }
        nextToken = peekToken();
    }

    _ast.rootNode.children.push_back(instructionNode);

    nextToken = readToken();
    if(nextToken.type != Token::LineTerminatorToken) {
        parserError("line terminator expected", nextToken);
    }

}

Parser::Parser(const TokenStream &tokenStream) : _tokenStream{tokenStream} {
    for(auto directive : directives) {
        _directives.insert(directive);
    }
    for(auto instruction : instructions) {
        _instructions.insert(instruction);
    }
}
