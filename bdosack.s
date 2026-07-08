! IBM PC-DOS CRC.EXE-compatible CRC calculator - bdosack.s
! Copyright (c) 2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
! SPDX-License-Identifier: MIT-0
! scspell-id: 2a149d7a-7ab9-11f1-b77c-80ee73e9b8e7

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.sect .text
.extern _bdos

_bdos:
        ! Stack layout (Ack 8080, caller pushes right-to-left):
        !   (sp)   return address  [2 bytes]
        !   (sp+2) func            [2 bytes]
        !   (sp+4) param           [2 bytes]

        lxi     h,2
        dad     sp
        mov     a,m        ! A = func (BDOS function number)

        inx     h
        inx     h
        mov     e,m        ! E = param low byte
        inx     h
        mov     d,m        ! D = param high byte

        push    b          ! save BC (B = Ack frame pointer)
        mov     c,a        ! C = function number (CP/M convention)
        call    5          ! CP/M BDOS entry point
        pop     b          ! restore BC (frame pointer)

        xchg               ! DE = HL (BDOS result -> Ack return reg)
        ret

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! vim: set ft=asm ai expandtab cc=80 :
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
