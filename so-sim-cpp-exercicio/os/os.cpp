#include <stdexcept>
#include <string>
#include <string_view>

#include <cstdint>
#include <cstdlib>

#include "../config.h"
#include "../lib.h"
#include "../arch/arch.h"
#include "os.h"
#include "os-lib.h"


namespace OS {

// ---------------------------------------

Arch::Cpu *cpu;

void boot (Arch::Cpu *cpu)
{
	OS::cpu = cpu;
	terminal_println(cpu, Terminal::Command, "Type commands here");
	terminal_println(cpu, Terminal::App, "Apps output here");
	terminal_println(cpu, Terminal::Kernel, "Kernel output here");
}

// ---------------------------------------

void interrupt (const InterruptCode interrupt)
{
	terminal_println(cpu, Terminal::Kernel, "Teste");

}

// ---------------------------------------

void syscall ()
{

}

// ---------------------------------------

} // end namespace OS
