ENTRY(_start_uapp)

SECTIONS
{
    . = 0x0;
    _start_uapp = .;

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