#ifndef RTOS_H
#define RTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <float.h>

// Constants
#define PATH_SIZE 256

// GPIO Configuration Structure
typedef struct {
    int b1;
    int b2;
    int led_1;
    int led_2;
} GPIO_Config;

// Global Variables
extern volatile bool system_running;
extern GPIO_Config gpio_config;

// GPIO Function Prototypes
void export_gpio(int gpio_pin);
void unexport_gpio(int gpio_pin);
void set_gpio_direction(int gpio_pin, const char *direction);
void set_gpio_value(int gpio_pin, int value);
int read_gpio_value(int gpio_pin);

// System Functions
void initialize_system(void);
void cleanup_system(void);
void get_gpio_configuration(void);
void print_system_info(void);

// Thread Function Prototypes
void* button_thread_function(void* arg);
void* counter_thread_function(void* arg);
void* display_thread_function(void* arg);

#endif

