int spi_init(char filename[40]);
char * spi_read(int addr,int nbytes,int file);
void spi_write(int addr,int nbytes,char value[10],int file);
void spi_generic(int nbytes,char value[10],int file);

