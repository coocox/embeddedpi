_estack = 0x20000400;

MEMORY
{
    FLASH_ON_CHIP   (rx)    : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM_ON_CHIP    (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    .text : {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH_ON_CHIP

	_sidata = .;

/*    .data : AT(ADDR(.text) + SIZEOF(.text)) {*/
    .data : AT(_sidata) {
        _sdata = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM_ON_CHIP

    .bss : {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM_ON_CHIP
}
