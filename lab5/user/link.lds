ENTRY(_start)

SECTIONS
{
    . = 0x0;
    __start = .;

    .text.init : { 
        *(.text.init) 
    }
    
    .text : { 
        *(.text) 
    }
    
    .data : { 
        *(.data) 
    }
}