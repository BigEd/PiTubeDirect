#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "32016.h"
#include "mem32016.h"
#include "NSDis.h"
#include "debug.h"
#include "../cpu_debug.h"

/*****************************************************
 * CPU Debug Interface
 *****************************************************/


int n32016_debug_enabled = 0;

enum register_numbers {
   i_R0,
   i_R1,
   i_R2,
   i_R3,
   i_R4,
   i_R5,
   i_R6,
   i_R7,
   i_PC,
   i_SB,
   i_SP0,
   i_SP1,
   i_FP,
   i_INTBASE,
   i_MOD,
   i_PSR,
   i_CFG,
};

// NULL pointer terminated list of register names.
static const char *dbg_reg_names[] = {
   "R0",
   "R1",
   "R2",
   "R3",
   "R4",
   "R5",
   "R6",
   "R7",
   "PC",
   "SB",
   "SP0",
   "SP1",
   "FP",
   "INTBASE",
   "MOD",
   "PSR",
   "CFG",
   NULL
};


// enable/disable debugging on this CPU, returns previous value.
int dbg_debug_enable(int newvalue) {
   int oldvalue = n32016_debug_enabled;
   n32016_debug_enabled = newvalue;
   return oldvalue;
};

// CPU's usual memory read function for data.
uint32_t dbg_memread(uint32_t addr) {
   // use the internal version so the debugger doesn't get notified
   return read_x8_internal(addr);
};

// CPU's usual memory write function.
void dbg_memwrite(uint32_t addr, uint32_t value) {
   // use the internal version so the debugger doesn't get notified
   write_x8_internal(addr, value);
};


uint32_t dbg_disassemble(uint32_t addr, char *buf, size_t bufsize) {
   return n32016_disassemble(addr, buf, bufsize);
};

// Get a register - which is the index into the names above
uint32_t dbg_reg_get(int which) {
   switch (which) {
   case i_R0:
   case i_R1:
   case i_R2:
   case i_R3:
   case i_R4:
   case i_R5:
   case i_R6:
   case i_R7:
      return r[which - i_R0];
   case i_PC:
      return n32016_get_pc();
   case i_SB:
      return PR.SB;
   case i_SP0:
      return PR.SP;
   case i_SP1:
      return PR.USP;
   case i_FP:
      return PR.FP;
   case i_INTBASE:
      return PR.INTBASE;
   case i_MOD:
      return PR.MOD.Whole;
   case i_PSR:
      return PR.PSR.Whole;
   case i_CFG:
      return PR.CFG.Whole;
   default:
      return 0;
   }
};

// Set a register.
void  dbg_reg_set(int which, uint32_t value) {
   switch (which) {
   case i_R0:
   case i_R1:
   case i_R2:
   case i_R3:
   case i_R4:
   case i_R5:
   case i_R6:
   case i_R7:
      r[which - i_R0] = value;
      break;
   case i_PC:
      n32016_set_pc(value);
      break;
   case i_SB:
      PR.SB = value;
      break;
   case i_SP0:
      PR.SP = value;
      break;
   case i_SP1:
      PR.USP = value;
      break;
   case i_FP:
      PR.FP = value;
      break;
   case i_INTBASE:
      PR.INTBASE = value;
      break;
   case i_MOD:
      PR.MOD.Whole = value;
      break;
   case i_PSR:
      PR.PSR.Whole = value;
      break;
   case i_CFG:
      PR.CFG.Whole = value;
      break;
   default:
      break;
   }
};

// Print register value in CPU standard form.
size_t dbg_reg_print(int which, char *buf, size_t bufsize) {
   int i;
   int bit;
   char c;
   if (which == i_PSR) {
      // !       Supervisor Flags        !          User Flags           !
      // +-------------------------------+-------------------------------+
      // ! x   x   x   x ! I ! P ! S ! U ! N ! Z ! F ! V ! x ! L ! T ! C !
      // !---+---+---+---+---+---+---+---!---+---+---+---+---+---+---+---!
      //  15                           8   7                           0
      bit = 0x8000;
      for (i = 0; i < 16 && i < bufsize; i++) {
         if (0xF008 & bit) {
            c = 'x';
         } else if (psr & bit) {
            c = '1';
         } else {
            c = '0';
         }
         *(buf + i) = c;
         bit >>= 1;
      }
      if (i < bufsize) {
         *(buf + i) = '\0';
         i++;
      }
      return i;
   } if (which == i_MOD) {
      return snprintf(buf, bufsize, "%04"PRIx32, dbg_reg_get(which) & 0xFFFF);       
   } else {
      return snprintf(buf, bufsize, "%08"PRIx32, dbg_reg_get(which));       
   }
};

// Parse a value into a register.
void dbg_reg_parse(int which, char *strval) {
   uint32_t val = 0;
   sscanf(strval, "%"SCNx32, &val);
   dbg_reg_set(which, val);
};


cpu_debug_t n32016_cpu_debug = {
   .cpu_name     = "32016",
   .debug_enable = dbg_debug_enable,
   .memread      = dbg_memread,
   .memwrite     = dbg_memwrite,
   .disassemble  = dbg_disassemble,
   .reg_names    = dbg_reg_names,
   .reg_get      = dbg_reg_get,
   .reg_set      = dbg_reg_set,
   .reg_print    = dbg_reg_print,
   .reg_parse    = dbg_reg_parse
};

#if 0

// Name/model of CPU.
static const char *dbg_cpu_name = "32016";

cpu_debug_t *n32016_cpu_debug()
{
   cpu_debug.cpu_name     = dbg_cpu_name; 
   cpu_debug.debug_enable = dbg_debug_enable;
   cpu_debug.memread      = dbg_memread;
   cpu_debug.memwrite     = dbg_memwrite;
   cpu_debug.disassemble  = dbg_disassemble;
   cpu_debug.reg_names    = dbg_reg_names;
   cpu_debug.reg_get      = dbg_reg_get;
   cpu_debug.reg_set      = dbg_reg_set;
   cpu_debug.reg_print    = dbg_reg_print;
   cpu_debug.reg_parse    = dbg_reg_parse;
   return &cpu_debug;
}
#endif
