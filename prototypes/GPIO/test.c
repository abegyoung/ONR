#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "simplegpio.h"

unsigned int gpio = 57;

int main(int argc, char *argv[]){

  gpio_export(gpio);
  gpio_set_dir(gpio, INPUT_PIN);

  unsigned int value = HIGH;
  while(value!=LOW)
  {
    gpio_get_value(gpio, &value); 
    usleep(1000);
  }
  printf("Button pressed!\n");

  gpio_unexport(gpio);
  return 0;
}
