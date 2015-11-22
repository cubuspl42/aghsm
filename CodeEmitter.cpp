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

#include "CodeEmitter.h"
#include "Language.h"

void printProgram(std::ostream &os, const std::vector<Word> &words) {
    const int numInstructions = sizeof(instructions) / sizeof(instructions[0]);
    int i = 0;
    for(const Word &word : words) {
        os << i * 4 << ": ";
        ++i;

        Instruction inst = word.instruction;

        if(inst.code < numInstructions) {
            os << instructions[inst.code] << ' ';

            if(inst.acu < 2) {
                char acu = 'A' + (char)inst.acu;
                os << "@" << acu << ' ';
            }

            std::string p0, p1;

            if(inst.mod == 0) {
                p0 = "{", p1 = "}";
            } else if(inst.mod == 1) {
                p0 = "(", p1 = ")";
            } else {
                p0 = "((", p1 = "))";
            }

            os << p0 << inst.adr << p1 << ' ';
        }

        os << "[" << word.data << "]";
        os << std::endl;
    }
}

CodeEmitter::CodeEmitter(const Ast &ast) : _ast(ast) {
    int i = 0;
    for(auto instruction : instructions) {
        _opcodes[instruction] = i;
        ++i;
    }
}

#if 1

std::vector<Word> CodeEmitter::emitCode() {
    Word main;
    main.data = 0;
    emitWord(main);

    for(const AstNode node : _ast.rootNode.children) {
        switch(_currentSection) {
            case NullSection: {
                if(node.type == AstNode::DirectiveNode && node.sValue == ".UNIT") {
                    _currentSection = UnitSection;
                } else {
                    emitterError("code should start with .UNIT section");
                }
                break;
            }
            case UnitSection: {
                if(node.type == AstNode::DirectiveNode && node.sValue == ".DATA") {
                    _currentSection = DataSection;
                } else {
                    emitterError("data section should follow after .UNIT section");
                }
                break;
            }
            case DataSection: {
                if(node.type == AstNode::DirectiveNode && node.sValue == ".CODE") {
                    _mainLabel = _words.size();
                    _currentSection = CodeSection;
                } else if(node.type == AstNode::LabelNode) {
                    _labels[node.sValue] = _words.size();
                } else if(node.type == AstNode::DirectiveNode && node.sValue == ".WORD") {
                    emitDataWords(node.children);
                } else {
                    emitterError("data section can contain only .WORD directives and references");
                }
                break;
            }
            case CodeSection: {
                if(node.type == AstNode::DirectiveNode && node.sValue == ".END") {
                    _currentSection = EndSection;
                } else if(node.type == AstNode::LabelNode) {
                    _labels[node.sValue] = _words.size();
                } else if(node.type == AstNode::InstructionNode) {
                    emitInstruction(node);
                } else {
                    emitterError("data section can contain only .WORD directives and labels");
                }
                break;
            }
            case EndSection: {
                emitterError("no code allowed after .END");
                break;
            }
            default: {
                emitterError("critical error");
            }
        }
    }

    resolveReferences();

    _words[0].data = _mainLabel * 4;

    return _words;
}

#endif

void CodeEmitter::emitterError(std::string errorMessage) {
    std::stringstream ss;
    ss << "Code emitter error: " << errorMessage;
    throw CodeEmitterError{ ss.str() };
}

void CodeEmitter::emitWord(Word word) {
    _words.push_back(word);
}

void CodeEmitter::emitDataWords(std::vector<AstNode> words) {
    Word word;
    for(AstNode node : words) {
        if(node.type == AstNode::NumberNode) {
            word.data = node.aValue;
            emitWord(word);
        } else if(node.type == AstNode::MultinumberNode) {
            for(int i = 0; i < node.aValue; ++i) {
                word.data = node.bValue;
                emitWord(word);
            }
        } else {
            emitterError("only numbers can follow .WORD directive");
        }
    }

}

void CodeEmitter::emitValue(AstNode valueNode, Word word) {
    if(valueNode.type == AstNode::ReferenceNode) {
        word.instruction.mod = 0;
        word.instruction.adr = -1;
        markReference(valueNode.sValue);
        emitWord(word);
    } else if(valueNode.type == AstNode::ParenNode) {
        word.instruction.adr = -1;

        if(valueNode.children.front().type == AstNode::ReferenceNode) {
            AstNode referenceNode = valueNode.children.front();
            word.instruction.mod = 1;
            markReference(referenceNode.sValue);
        } else if(valueNode.children.front().type == AstNode::ParenNode) {
            AstNode secondParenNode = valueNode.children.front();
            if(secondParenNode.children.front().type == AstNode::ReferenceNode) {
                AstNode referenceNode = secondParenNode.children.front();
                word.instruction.mod = 2;
                markReference(referenceNode.sValue);
            } else {
                emitterError("wrong paren content");
            }
        } else {
            emitterError("wrong paren content");
        }

        emitWord(word);
    } else if(valueNode.type == AstNode::NumberNode) {
        word.instruction.mod = 0;
        word.instruction.adr = valueNode.aValue;
        emitWord(word);
    } else {
        emitterError("wrong instruction argument");
    }
}

#if 1

void CodeEmitter::emitInstruction(AstNode node) {
    std::string name = node.sValue;
    Word word;
    word.data = 0;
    word.instruction.code = _opcodes[name];

    if(name == "null" || name == "halt") {
        emitWord(word);
    } else if(name[0] == 'j') {
        if(node.children.size() == 1 && node.children.front().type == AstNode::ReferenceNode) {
            markReference(node.children.front().sValue);
            word.instruction.adr = -1;
            emitWord(word);
        } else {
            emitterError("wrong jump arguments");
        }
    } else if(name == "print") {
        if(node.children.size() == 1) {
            if(node.children.front().type == AstNode::RegisterNode) {
                word.instruction.acu = node.children.front().sValue == "B" ? 1 : 0;
                word.instruction.usr = 0;
                emitWord(word);
            } else {
                word.instruction.usr = 1;
                emitValue(node.children.front(), word);
            }
        } else {
            emitterError("too many print arguments");
        }
    } else {
        if(node.children.size() == 2 && node.children.front().type == AstNode::RegisterNode) {
            AstNode registerNode = node.children.front();

            if (registerNode.sValue != "A" && registerNode.sValue != "B") {
                emitterError("wrong register name");
            }

            word.instruction.acu = registerNode.sValue == "B" ? 1 : 0;

            AstNode valueNode = node.children.back();

            emitValue(valueNode, word);
        } else {
            emitterError("wrong instruction arguments");
        }
    }
}

#endif

void CodeEmitter::markReference(std::string reference) {
    //std::cout << "marking reference: " << reference << ' ' << _words.size() * 4 << std::endl;
    _references.push_back({reference, _words.size()});
}

#if 1

void CodeEmitter::resolveReferences() {
    for(auto p : _references) {
        std::string reference = p.first;
        int referenceWordIndex = p.second;
        auto it = _labels.find(reference);
        if(it == _labels.end()) {
            emitterError("unresolved reference");
        } else {
            int labelWordIndex = it->second;
            _words[referenceWordIndex].instruction.adr = labelWordIndex * 4;
            //std::cout << "resolving reference: " << reference << ' ' << referenceWordIndex * 4 << ' ' << labelWordIndex * 4 << std::endl;
        }
    }
}

#endif
