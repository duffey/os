[GLOBAL idt_flush]    ; Allows the C code to call idt_flush().

idt_flush:
    mov rax, [rsp+8]  ; Get the pointer to the IDT, passed as a parameter.
    lidt [rax]        ; Load the IDT pointer.
    ret
