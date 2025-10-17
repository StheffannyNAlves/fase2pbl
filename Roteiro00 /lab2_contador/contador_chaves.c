#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "./hps_0.h"  // contém PIO_LED_BASE e PIO_SW_BASE

#define LW_BRIDGE_BASE 0xFF200000
#define LW_BRIDGE_SPAN 0x00005000

int main() {
    // Setup
    int fd;
    void *virtual_base;
    volatile int *LEDR_ptr;
    volatile int *SW_ptr;

    // Abre o /dev/mem para acesso à memória física
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        printf("Erro: não foi possível abrir /dev/mem...\n");
        return 1;
    }

    //  mapeamento de memoria
    virtual_base = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE),
                        MAP_SHARED, fd, LW_BRIDGE_BASE);

    if (virtual_base == MAP_FAILED) {
        printf("Erro: mmap falhou...\n");
        close(fd);
        return 1;
    }

    //Ponteiros para registradores
    LEDR_ptr = (volatile int *)(virtual_base + PIO_LED_BASE);
    SW_ptr   = (volatile int *)(virtual_base + PIO_SW_BASE);

    printf("\n=== Programa Iniciado ===\n");
    printf("Mostrando em tempo real o estado das chaves e LEDs\n");
    printf("Pressione Ctrl+C para encerrar.\n\n");

    // loop  principal
    while (1) {
        // Passo A: Ler o valor das chaves
        int switch_value = *SW_ptr;

        // conta quantas chaves estão ligadas (bits '1')
        int count = 0;
        int temp = switch_value;
        while (temp > 0) {
            if (temp & 1) count++;
            temp >>= 1;
        }

        // gera o valor progressivo para os LEDs
        int led_value = (count == 0) ? 0 : ((1 << count) - 1);

        // escreve o valor
        *LEDR_ptr = led_value;

        // Exibição no terminal
        printf("\rChaves ligadas: %2d | Valor SW: 0x%02X | LEDs: 0x%04X ", count, switch_value, led_value);
        fflush(stdout);

        usleep(100000); // 100 ms
    }

    // desmapeamento de memoria
    if (munmap(virtual_base, LW_BRIDGE_SPAN) != 0) {
        printf("Erro: munmap falhou.\n");
    }
    close(fd);

    return 0;
}
