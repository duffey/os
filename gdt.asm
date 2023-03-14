gdtr DW 0 ; For limit storage
     DD 0 ; For base storage

global set_gdt
set_gdt:
   MOV   AX, [esp + 4]
   MOV   [gdtr], AX
   MOV   EAX, [ESP + 8]
   MOV   [gdtr + 2], EAX
   LGDT  [gdtr]
   RET
