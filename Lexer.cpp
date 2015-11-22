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
#include "Lexer.h"

#include <unordered_set>

static bool isDelimeter(char c) {
    switch(c) {
        case ',':
        case '(':
        case ')':
        case ':':
        case '#':
        case '\0':
            return true;
        default:
            return std::isspace(c);
    }
}

void Lexer::lexerError(std::string lexerError) {
    std::stringstream ss;
    ss << ":" << _currentLineNo + 1 << ":" << _currentColumnNo + 1 << ": Lexer error: " << lexerError;
    throw LexerError{ ss.str() };
}

char Lexer::readChar() {
    if(_currentColumnNo <= _currentLine.size()) {
        char c = _currentLine[_currentColumnNo];
        ++_currentColumnNo;
        return c;
    } else {
        lexerError("unexpected newline");
    }

    return '\0'; // unreachable
}

char Lexer::peekChar() {
    char c = readChar();
    --_currentColumnNo;
    return c;
}

Token Lexer::readKeywordOrIdentifier() {
    Token token{Token::IdentifierToken};

    while(std::isalnum(peekChar()) || peekChar() == '.' || peekChar() == '_') {
        token.tokenData += readChar();
    }

    if(!isDelimeter(peekChar())) {
        lexerError("identifier contains illegal characters");
    }

    if(isKeyword(token.tokenData)) {
        token.type = Token::KeywordToken;
    }

    return token;
}

Token Lexer::readDelimiter() {
    Token token{Token::DelimiterToken};

    token.tokenData = readChar();

    return token;
}

Token Lexer::readNumber() {
    Token token{Token::NumberToken};

    int m = 1;

    if(peekChar() == '-') {
        token.tokenData = readChar();
    }

    while(std::isdigit(peekChar())) {
        token.tokenData += readChar();
    }

    if(!isDelimeter(peekChar())) {
        lexerError("incorrect number");
    }

    return token;
}

Token Lexer::readRegister() {
    Token token{Token::RegisterToken};

    char at = readChar();
    assert(at == '@');

    while(std::isalpha(peekChar())) {
        token.tokenData += readChar();
    }

    if(!isDelimeter(peekChar())) {
        throw LexerError("incorrect register name");
    }

    return token;
}

Token Lexer::readLineTerminator() {
    Token token{Token::LineTerminatorToken};
    return token;
}

void Lexer::skipWhitespace() {
    char c = peekChar();
    while (isspace(c) && c != '\0') {
        readChar();
        c = peekChar();
    }
}

Token Lexer::readToken() {
    Token token{Token::NullToken};

    int lineNo = _currentLineNo + 1;
    int colNo = _currentColumnNo + 1;

    char c = peekChar();

    if(std::isalpha(c) || c == '.' || c == '_') {
        token = readKeywordOrIdentifier();
    } else if(std::isdigit(c) || c == '-') {
        token = readNumber();
    } else {
        switch(c) {
//            case '.':
//                token = readKeyword();
//                break;
            case '@':
                token = readRegister();
                break;
            case '\0':
                token = readLineTerminator();
                break;
            case ',':
            case '(':
            case ')':
            case ':':
            case '#':
                token = readDelimiter();
                break;
            default:
                lexerError(std::string{} + "unexpected character: '" + c + "'");
        }
    }

    token.lineNumber = lineNo;
    token.columnNumber = colNo;

    skipWhitespace();

    return token;
}

void Lexer::emitToken(Token token) {
    _tokenStream.insert(token);
}

void Lexer::lexLine() {
    while(_currentColumnNo != _currentLine.size()) {
        assert(_currentColumnNo < _currentLine.size());
        Token token{Token::NullToken};
        do {
            token = readToken();
            emitToken(token);
        } while(token.type != Token::LineTerminatorToken);
    }
}

TokenStream Lexer::lex() {
    for(std::string line; getline(_stream, line); ) {
        _currentLine = line;
        _currentColumnNo = 0;
        lexLine();
        ++_currentLineNo;
    }

    return _tokenStream;
}

Lexer::Lexer(std::istream &sourceStream) : _stream(sourceStream) {
    for(auto directive : directives) {
        _keywords.insert(directive);
    }
    for(auto instruction : instructions) {
        _keywords.insert(instruction);
    }
}

bool Lexer::isKeyword(std::string keyword) {
    if(_keywords.find(keyword) != _keywords.end()) {
        return true;
    }
    return false;
}
