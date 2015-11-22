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

#ifndef AGHSM_LEXER_H
#define AGHSM_LEXER_H

#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_set>

struct Token {
    enum Type {
        NullToken,
        KeywordToken,
        IdentifierToken,
        DelimiterToken,
        NumberToken,
        RegisterToken,
        LineTerminatorToken
    };

    Token(Type type = NullToken) : type(type) {}

    Type type;
    int lineNumber = 0;
    int columnNumber = 0;
    std::string tokenData;
};

class TokenStream {
    std::vector<Token> _tokenStream;

public:

    Token getTokenAt(size_t i) {
        return _tokenStream.at(i);
    }

    void insert(Token token) {
        _tokenStream.push_back(token);
    }

    size_t size() {
        return _tokenStream.size();
    }

    void print(std::ostream &os) {
        for(Token token : _tokenStream) {
            std::string tokenType;

            switch (token.type) {
                case Token::NullToken:
                    tokenType = "NullToken";
                    break;
                case Token::KeywordToken:
                    tokenType = "KeywordToken";
                    break;
                case Token::IdentifierToken:
                    tokenType = "IdentifierToken";
                    break;
                case Token::DelimiterToken:
                    tokenType = "DelimiterToken";
                    break;
                case Token::NumberToken:
                    tokenType = "NumberToken";
                    break;
                case Token::RegisterToken:
                    tokenType = "RegisterToken";
                    break;
                case Token::LineTerminatorToken:
                    tokenType = "LineTerminatorToken";
                    break;
            }

            os << tokenType << "{" << token.tokenData << "} ";
            if(token.type == Token::LineTerminatorToken) {
                os << std::endl;
            }

        }
    }
};

class Lexer {
public:
    class LexerError : public std::logic_error {
    public:
        LexerError(std::string errorMessage)
                : std::logic_error(errorMessage)
        {}
    };

    Lexer(std::istream &sourceStream);

    TokenStream lex();

private:

    bool isKeyword(std::string keyword);

    void lexerError(std::string errorMessage);

    char readChar();

    char peekChar();

    Token readKeywordOrIdentifier();

    Token readDelimiter();

    Token readNumber();

    Token readRegister();

    Token readLineTerminator();

    void skipWhitespace();

    Token readToken();

    void emitToken(Token token);

    void lexLine();

    std::unordered_set<std::string> _keywords;
    std::istream &_stream;
    std::string _currentLine;
    int _currentLineNo = 0;
    int _currentColumnNo = 0;
    TokenStream _tokenStream;
};

#endif //AGHSM_LEXER_H
