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

#include "process.h"

namespace OS {

// ---------------------------------------

Arch::Cpu *cpu;

Process current_process;

PageTable kernel_page_table;

static std::string command_buffer;

static bool load_program(const std::string& file) {
	return true;
}

static void process_command (const std::string_view cmd)
{
	if (cmd == "sair") {
		terminal_println(cpu, Terminal::Kernel, "Saindo...");
		cpu->turn_off();
	}

	else if (cmd.starts_with("carregar ")) {

		std::string file(cmd.substr(9));

		if (load_program(file)) {
			terminal_println(cpu, Terminal::Kernel, "programa ", file, " carregado");
		} else {
			terminal_println(cpu, Terminal::Kernel, "falha ao carregar programa");
		}

	}

	else {
		terminal_println(cpu, Terminal::Command, "Comando nao encontrado: ", cmd);
		terminal_print(cpu, Terminal::Command, "> ");
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

// [todo] implementar os cases para as outras interrupcoes
static void handle_cpu_exception()
{
	switch (cpu->get_ref_cpu_exception().type)
	{
		case Arch::Cpu::CpuException::Type::VmemPageFault:
			terminal_println(cpu, Terminal::Kernel, "falha ao acessar pagina inexistente");
			break;
		
		default:
			break;
	}
}

// inicializa todas as paginas da memória
static void page_table_initialize()
{
	for (uint16_t i = 0; i < Config::ptes_per_table; i++)
    {
        kernel_page_table[i] = 0;
        kernel_page_table[i].set(Arch::Cpu::PteField::PhyFrameID, i);
        kernel_page_table[i].set(Arch::Cpu::PteField::Present, 1);
        kernel_page_table[i].set(Arch::Cpu::PteField::Readable, 1);
        kernel_page_table[i].set(Arch::Cpu::PteField::Writable, 1);
        kernel_page_table[i].set(Arch::Cpu::PteField::Executable, 1);
    }
}

// ---------------------------------------

void boot (Arch::Cpu *cpu_)
{
	cpu = cpu_;

	page_table_initialize();

	// registr a tabela de paginas do so
    cpu->set_page_table(&kernel_page_table);
	cpu->set_vmem_mode(Arch::Cpu::VmemMode::Paging);

	command_buffer.clear();

	terminal_println(cpu, Terminal::Command, "=== Terminal de Comandos ===");
	terminal_println(cpu, Terminal::Command, "Comando: sair");
	terminal_print(cpu, Terminal::Command, "> ");

	terminal_println(cpu, Terminal::App, "=== Saida  ===");
	terminal_println(cpu, Terminal::Kernel, "=== Kernel Log ===");
	terminal_println(cpu, Terminal::Kernel, "Sistema operacional iniciado.");
}

// ---------------------------------------

void interrupt (const InterruptCode interrupt_code)
{
	switch (interrupt_code) {
		case InterruptCode::Keyboard:
			handle_keyboard();
			break;

		case InterruptCode::CpuException:
			handle_cpu_exception();
			break;

		default:
			terminal_println(cpu, Terminal::Kernel, "Interrupcao nao mapeada: ", static_cast<int>(interrupt_code));
			break;
	}
}

// ---------------------------------------

void syscall ()
{

}

// ---------------------------------------

} // end namespace OS
