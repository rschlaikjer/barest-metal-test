#include <stdint.h>

unsigned _data_loadaddr, _data, _edata, _ebss, _stack;

static const unsigned IRQ_COUNT{150};

typedef void (*vector_table_entry)(void);
typedef void (*void_fun)(void);

extern "C" {
void reset_handler(void);
void nmi_handler(void);
void hard_fault_handler(void);
void mem_manage_handler(void);
void bus_fault_handler(void);
void usage_fault_handler(void);
void debug_monitor_handler(void);
void sv_call_handler(void);
void pend_sv_handler(void);
void sys_tick_handler(void);
void nop_handler(void);
void blocking_handler(void);
}

struct vector_table {
  unsigned *initial_stack_pointer;
  vector_table_entry reset;
  vector_table_entry nmi;
  vector_table_entry hard_fault;
  vector_table_entry mem_manage_fault;
  vector_table_entry bus_fault;
  vector_table_entry usage_fault;
  vector_table_entry reserved_x001c[4];
  vector_table_entry sv_call;
  vector_table_entry debug_monitor;
  vector_table_entry reserved_x0034;
  vector_table_entry pend_sv;
  vector_table_entry systick;
  vector_table_entry irq[IRQ_COUNT];
};

// clang-format off
__attribute__((section(".vectors")))
struct vector_table vector_table = {
  .initial_stack_pointer = &_stack,
  .reset = reset_handler,
  .nmi = nmi_handler,
  .hard_fault = hard_fault_handler,
  .mem_manage_fault = mem_manage_handler,
  .bus_fault = bus_fault_handler,
  .usage_fault = usage_fault_handler,
  .reserved_x001c = {0x0, 0x0, 0x0, 0x0},
  .sv_call = sv_call_handler,
  .debug_monitor = debug_monitor_handler,
  .reserved_x0034 = 0x0,
  .pend_sv = pend_sv_handler,
  .systick = sys_tick_handler,
  .irq = {blocking_handler}
};
// clang-format on

extern void_fun __preinit_array_start, __preinit_array_end;
extern void_fun __init_array_start, __init_array_end;
extern void_fun __fini_array_start, __fini_array_end;

extern "C" {
int main(void);
}

void reset_handler(void) {
  // Load the initialized .data section into place
  volatile unsigned *src, *dest;
  for (src = &_data_loadaddr, dest = &_data; dest < &_edata; src++, dest++) {
    *dest = *src;
  }

  // Handle C++ constructors / anything with __attribute__(constructor)
  void_fun *fp;
  for (fp = &__preinit_array_start; fp < &__preinit_array_end; fp++) {
    (*fp)();
  }
  for (fp = &__init_array_start; fp < &__init_array_end; fp++) {
    (*fp)();
  }

  // Invoke our actual application
  main();

  // Run destructors
  for (fp = &__fini_array_start; fp < &__fini_array_end; fp++) {
    (*fp)();
  }
}

void nop_handler() {}
void blocking_handler() {
  __asm__ volatile("BKPT");
  while (1) {
  }
}

#pragma weak nmi_handler = nop_handler
#pragma weak hard_fault_handler = blocking_handler
#pragma weak sv_call_handler = nop_handler
#pragma weak pend_sv_handler = nop_handler
#pragma weak sys_tick_handler = nop_handler
#pragma weak mem_manage_handler = blocking_handler
#pragma weak bus_fault_handler = blocking_handler
#pragma weak usage_fault_handler = blocking_handler
#pragma weak debug_monitor_handler = nop_handler
