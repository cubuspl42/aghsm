#include "Assembler.h"
#include "VM.h"

#include <cassert>
#include <fstream>

int main(int argc, char **argv) {
	std::ifstream ifs;

	if (argc == 1) {
		ifs.open("1.asm");
		assert(ifs.good());

		Assembler assembler(ifs);
		auto program = assembler.compile();
		VM vm;
		vm.load(program);
		vm.run();
	} else if (argc == 2) {
		ifs.open(argv[1]);
		
		if (!ifs.good()) {
			std::cerr << "Unable to open file" << std::endl;
			return 1;
		}

		try {
			Assembler assembler(ifs);
			auto program = assembler.compile();
			VM vm;
			vm.load(program);
			vm.run();
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}

    return 0;
}