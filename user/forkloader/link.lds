ENTRY(_start_forkloader)

SECTIONS
{
    . = 0x0;
    _start_forkloader = .;

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