#include <stdexcept>
#include <string>
#include <string_view>

#include <cstdint>
#include <cstdlib>

#include "../arch/cpu.h"
#include "../config.h"
#include "../lib.h"
#include "../arch/arch.h"
#include "os.h"
#include "os-lib.h"


namespace OS {

// ---------------------------------------

Arch::Cpu *cpu;

static std::string command_buffer;

static void process_command (const std::string_view cmd)
{
	if (cmd == "sair") {
		terminal_println(cpu, Terminal::Kernel, "Saindo...");
		cpu->turn_off();
	}
	else {
		terminal_println(cpu, Terminal::Command, "Comando nao encontrado: ", cmd);
	}
}

// ---------------------------------------

static void handle_keyboard ()
{
	const char c = static_cast<char>(cpu->read_io(IO_Port::TerminalReadTypedChar));

	if (terminal_is_return(c)) {
		// funcionamento do enter 
		if (!command_buffer.empty()) {
			terminal_println(cpu, Terminal::Command, ""); // pula linha
			process_command(command_buffer);
			command_buffer.clear();
		}
	}
	else if (terminal_is_backspace(c)) {
		// Apaga o ultimo caractere
		if (!command_buffer.empty()) {
			command_buffer.pop_back();
			// Redesenha a linha com \r
			cpu->write_io(IO_Port::TerminalSet, static_cast<uint16_t>(Terminal::Command));
			cpu->write_io(IO_Port::TerminalUpload, '\r');
			terminal_print(cpu, Terminal::Command, "> ", command_buffer);
		}
	}
	else {
		// Caractere normal vai acumulando e mostrando
		command_buffer += c;
		cpu->write_io(IO_Port::TerminalSet, static_cast<uint16_t>(Terminal::Command));
		cpu->write_io(IO_Port::TerminalUpload, static_cast<uint16_t>(c));
	}
}

// ---------------------------------------

void boot (Arch::Cpu *cpu_)
{
	cpu = cpu_;
	command_buffer.clear();

	terminal_println(cpu, Terminal::Command, "=== OS Terminal de Comandos ===");
	terminal_println(cpu, Terminal::Command, "Comando: sair");
	terminal_print(cpu, Terminal::Command, "> ");

	terminal_println(cpu, Terminal::App, "=== Saida  ===");
	terminal_println(cpu, Terminal::Kernel, "=== Kernel Log ===");
	terminal_println(cpu, Terminal::Kernel, "OS booted sucesso.");
}

// ---------------------------------------

void interrupt (const InterruptCode interrupt_code)
{
	switch (interrupt_code) {
		case InterruptCode::Keyboard:
			handle_keyboard();
			break;

		default:
			terminal_println(cpu, Terminal::Kernel, "Unhandled interrupt: ", static_cast<int>(interrupt_code));
			break;
	}
}

// ---------------------------------------

void syscall ()
{

}

// ---------------------------------------

} // end namespace OS
