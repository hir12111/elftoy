#ifndef ELFUTILS_H
#define ELFUTILS_H
#include <elf.h>
#define PAGE_SIZE (0x1000)

struct map_entry {
    void *m_addr;
    size_t m_sz;
};

struct segment_entry {
    Elf64_Off s_offset;
    Elf64_Addr   s_addr;
    size_t       s_sz;
};

int map_file(char *filepath, struct map_entry *map_entry);

void *unmap_file(struct map_entry *map_entry);

Elf64_Addr get_addr_after_segments(struct map_entry *target_file);

Elf64_Phdr *get_phdr(int type, struct map_entry *target_file);

uint64_t get_dyn_val(int tag, struct map_entry *target_file);

Elf64_Sym *get_dynsym(char *name, size_t *index, struct map_entry *target_file);

Elf64_Rela *get_jmprel_for(char *name, struct map_entry *target_file);

Elf64_Off get_offset(Elf64_Addr addr, struct map_entry *target_file);

void *get_dyn_ptr(int type, struct map_entry *target_file);

size_t get_dyn_sym_num(struct map_entry *target_file);

#endif
