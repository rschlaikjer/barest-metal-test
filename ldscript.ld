MEMORY
{
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 128K
}

/* Force generation of vector table */
EXTERN (vector_table)

/* Define entrypoint */
EXTERN (reset_handler)

SECTIONS {
/* Place the ram section in memory */
.ram : {
    __ram_start = .; /* VMA start */
    *(ram)
    . = ALIGN(4);
    __ram_end = .; /* VMA end */
} >ram AT>rom
__ram_loadaddr = LOADADDR(.ram);

/* Text */
.text : {
    *(.vectors) /* Vector table */
    *(.text*)   /* Program code */
    . = ALIGN(4);
    *(.rodata*) /* Read-only data */
    . = ALIGN(4);
} >rom

/* C++ Static constructors/destructors, also used for __attribute__
 * ((constructor)) and the likes */
.preinit_array : {
    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;
} >rom
.init_array : {
    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    __init_array_end = .;
} >rom
.fini_array : {
    . = ALIGN(4);
    __fini_array_start = .;
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    __fini_array_end = .;
} >rom

/*
 * Another section used by C++ stuff, appears when using newlib with
 * 64bit (long long) printf support
 */
.ARM.extab : {
    *(.ARM.extab*)
} >rom
.ARM.exidx : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
} >rom

. = ALIGN(4);
_etext = .;

/* Read-write initialized data */
.data : {
    _data = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
} >ram AT >rom
_data_loadaddr = LOADADDR(.data);

/* Read-write zero initialized data */
.bss : {
    __bss_start__ = .;
    *(.bss*)
    __bss_end__ = .;
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
} >ram

. = ALIGN(4);
end = .;

}

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));
