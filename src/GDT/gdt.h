/**********************************************************************
SalernOS Kernel
Copyright (C) 2021 - 2022 Alessandro Salerno

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**********************************************************************/


#ifndef SALERNOS_CORE_KERNEL_GDT
#define SALERNOS_CORE_KERNEL_GDT

    #include <kerntypes.h>


    typedef struct GDTDescriptor {
        uint16_t _Size;
        uint64_t _Offset;
    } __attribute__((__packed__)) gdtdesc_t;

    typedef struct GDTEntry {
        uint16_t _LimitZero;
        uint16_t _BaseZero;
        uint8_t  _BaseOne;
        uint8_t  _AccessByte;
        uint8_t  _LimitOne_Flags;
        uint8_t  _BaseTwo;
    } __attribute__((__packed__)) gdtent_t;

    typedef struct __attribute__((__aligned__(0x1000)))  GlobalDescriptorTable {
        gdtent_t _Null;
        gdtent_t _KernelCodeSegment;
        gdtent_t _KernelDataSegment;
        gdtent_t _UserNull;
        gdtent_t _UserCodeSegment;
        gdtent_t _UserDataSegment;
    } __attribute__((__aligned__(0x1000))) gdt_t;


    /********************
     *      TYPE    NAME
    ********************/
    extern  gdt_t   gdt;


    /*****************************************************************************
            RET TYPE        FUNCTION NAME                 FUNCTION ARGUMENTS
    *****************************************************************************/
    extern  void            kernel_gdt_load               (gdtdesc_t* __gdtdesck);

#endif
