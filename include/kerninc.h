#ifndef SALERNOS_INC_KERNEL_INCLUDES
#define SALERNOS_INC_KERNEL_INCLUDES

    // User Input includes
    #include "User/Input/Keyboard/keyboard.h"       /*   Includes the basic Kernel Keyboard Driver          */

    // User Output includes
    #include "User/Output/Text/textrenderer.h"      /*   Includes the basic Kernel Text Renderer            */
    #include "User/Output/Text/kernelshell.h"       /*   Includes the basic Kernel Text Formatter           */
    #include "User/Output/Display/kdd.h"            /*   Includes the basic Kernel Display Driver           */
    #include "User/Output/Text/cstr.h"              /*   Includes the basic Kernel String Utilities         */

    // Memory includes
    #include "Memory/efimem.h"                      /*   Includes the Kernel EFI Memory Map Interface       */
    #include "Memory/palloc.h"                      /*   Includes the Kernel Page Frame Allocator           */
    #include "Memory/paging.h"                      /*   Includes the Kernel Page Table Manager             */
    #include "Memory/bmp.h"                         /*   Includes the Kernel's definition of a Bitmap       */

    // System includes
    #include "Interrupts/handlers.h"                /*   Includes the Kernel's Interrupt Service Routines   */
    #include "Interrupts/idt.h"                     /*   Includes the Kernel's definition of the IDT        */
    #include "Interrupts/pic.h"                     /*   Includes the basic Kernel PIC Driver               */
    #include "GDT/gdt.h"                            /*   Includes the Kernel's definition of the GTD        */
    #include "IO/io.h"                              /*   Includes the basic Kernel I/O functions            */

    // Syscall includes
    #include "Syscall/dispatcher.h"                 /*   Includes the Kernel Syscall Dispatcher functions   */
    #include "Syscall/syscalls.h"                   /*   Includes all kernel Syscall declarations           */

    // Utils includes
    #include "kutils.h"                             /*   Includes all kernel initialization functions      */

#endif