/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>

#define EI_DATA         5
#define ELFDATANONE     0
#define ELFDATALSB      1
#define ELFDATAMSB      2

char check_host_endian()
{
        int x = 1;
        if (*(char *)&x == 1) {
                return ELFDATALSB;
        } else {
                return ELFDATAMSB;
        }
}

uint16_t fix_u16_dummy(uint16_t x)
{
        return x;
}

uint16_t fix_u16_swap(uint16_t x)
{
        char *buf = (char *)&x;
        char t;
        t = buf[0];
        buf[0] = buf[1];
        buf[1] = t;
        return x;
}

uint32_t fix_u32_dummy(uint32_t x)
{
        return x;
}

uint32_t fix_u32_swap(uint32_t x)
{
        char *buf = (char *)&x;
        char t;
        t = buf[0];
        buf[0] = buf[3];
        buf[3] = t;
        t = buf[1];
        buf[1] = buf[2];
        buf[2] = t;
        return x;
}

/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
        if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
                ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
                ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
                ehdr->e_ident[EI_MAG3] == ELFMAG3) {
                return 1;
        }

        return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
    Exercise 1.2. Please complete func "readelf". 
*/
int readelf(u_char *binary, int size)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

        int Nr;

        Elf32_Phdr *phdr = NULL;

        u_char *ptr_ph_table = NULL;
        Elf32_Half ph_entry_count;
        Elf32_Half ph_entry_size;

        char bin_endian;
        char host_endian;
        uint16_t (*fix_u16)(uint16_t);
        uint32_t (*fix_u32)(uint32_t);

        // check whether `binary` is a ELF file.
        if (size < 4 || !is_elf_format(binary)) {
                printf("not a standard elf format\n");
                return 0;
        }

        bin_endian = ehdr->e_ident[EI_DATA];
        host_endian = check_host_endian();
        if (bin_endian == ELFDATANONE || bin_endian == host_endian) {
                fix_u16 = fix_u16_dummy;
                fix_u32 = fix_u32_dummy;
        } else {
                fix_u16 = fix_u16_swap;
                fix_u32 = fix_u32_swap;
        }

        // get section table addr, section header number and section header size.
        ptr_ph_table = binary + fix_u32(ehdr->e_phoff);
        ph_entry_count = fix_u16(ehdr->e_phnum);
        ph_entry_size = fix_u16(ehdr->e_phentsize);

        // for each section header, output section number and section addr. 
        // hint: section number starts at 0.
        for (Nr = 0; Nr < ph_entry_count; Nr++) {
                phdr = (Elf32_Phdr *)(ptr_ph_table + Nr * ph_entry_size);
                printf("%d:0x%x,0x%x\n", Nr, fix_u32(phdr->p_offset), fix_u32(phdr->p_align));
        }

        return 0;
}

