/* linker/stm32f767zi.ld */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
  RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 512K
}

/* Define the stack pointer */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .vectors :
  {
    KEEP(*(.vectors))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .data : AT (_etext)
  {
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >RAM

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
}