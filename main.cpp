#include "Assembler.h"
#include "VM.h"

#include <cassert>
#include <fstream>

int main(int argc, char **argv) {
    std::ifstream ifs;

    if(argc == 2) {
        ifs.open(argv[1]);
    } else {
        ifs.open("1.asm");
    }
    assert(ifs.good());

    Assembler assembler(ifs);
    auto program = assembler.compile();

    VM vm;
    vm.load(program);
    vm.run();

    return 0;
}