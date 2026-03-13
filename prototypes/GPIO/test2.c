#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "simplegpio.h"

unsigned int gpio = 57;
unsigned int value;

int main(int argc, char *argv[]){

  gpio_export(gpio);
  gpio_set_dir(gpio, OUTPUT_PIN);

  value = HIGH;
  gpio_set_value(gpio, value); 

  usleep(70000);

  value = LOW;
  gpio_set_value(gpio, value); 

  gpio_unexport(gpio);
  return 0;
}
