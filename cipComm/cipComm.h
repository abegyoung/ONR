#include <stdint.h>

#define MUX_ADDR 0x20

#define MAXARGSIZE 128
#define WHITESPACE(ch)         ( (ch) == ' ' || (ch) == '\t' || (ch) == '\n' || (ch) == '\r' )

void i2c_write(int addr, int reg, int a);
uint32_t i2c_read(int addr, int reg);

int spi_init(char filename[40]);
char *spi_read(int nbytes,int file);
void spi_write(int nbytes,char value[10],int file);
char *spi_read_reg(int addr,int nbytes,int file);

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd);

int TranslateCIP(int command);
int SendCommand(char * args, int indx);
float SendCommand_F(char * args, int indx);


