#include "RTOS.h"

// Global variables
volatile bool system_running = true;
GPIO_Config gpio_config;
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

// GPIO Functions
void export_gpio(int gpio_pin) {
    char buffer[PATH_SIZE];
    FILE *file;

    snprintf(buffer, sizeof(buffer), "/sys/class/gpio/gpio%d", gpio_pin);
    if (access(buffer, F_OK) != 0) {
        file = fopen("/sys/class/gpio/export", "w");
        if (file == NULL) {
            printf("Error exporting GPIO %d: %s\n", gpio_pin, strerror(errno));
            return;
        }
        fprintf(file, "%d", gpio_pin);
        fclose(file);
        usleep(100000);
    }
}

void unexport_gpio(int gpio_pin) {
    FILE *file = fopen("/sys/class/gpio/unexport", "w");
    if (file == NULL) {
        printf("Error unexporting GPIO %d: %s\n", gpio_pin, strerror(errno));
        return;
    }
    fprintf(file, "%d", gpio_pin);
    fclose(file);
}

void set_gpio_direction(int gpio_pin, const char *direction) {
    char path[PATH_SIZE];
    FILE *file;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio_pin);
    file = fopen(path, "w");
    if (file == NULL) {
        printf("Error setting GPIO %d direction: %s\n", gpio_pin, strerror(errno));
        return;
    }
    fprintf(file, "%s", direction);
    fclose(file);
}

void set_gpio_value(int gpio_pin, int value) {
    char path[PATH_SIZE];
    FILE *file;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio_pin);
    file = fopen(path, "w");
    if (file == NULL) {
        printf("Error setting GPIO %d value: %s\n", gpio_pin, strerror(errno));
        return;
    }
    fprintf(file, "%d", value);
    fclose(file);
}

int read_gpio_value(int gpio_pin) {
    char path[PATH_SIZE];
    FILE *file;
    char value_str[3];

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio_pin);
    file = fopen(path, "r");
    if (file == NULL) {
        printf("Error reading GPIO %d value\n", gpio_pin);
        return -1;
    }
    if (fgets(value_str, sizeof(value_str), file) == NULL) {
        printf("Error reading GPIO %d value\n", gpio_pin);
        fclose(file);
        return -1;
    }
    fclose(file);
    return atoi(value_str);
}

// System Functions
void initialize_system(void) {
    printf("Initializing GPIO pins...\n");
    export_gpio(gpio_config.b1);
    export_gpio(gpio_config.b2);
    export_gpio(gpio_config.led_1);
    export_gpio(gpio_config.led_2);

    usleep(500000);

    set_gpio_direction(gpio_config.b1, "in");
    set_gpio_direction(gpio_config.b2, "in");
    set_gpio_direction(gpio_config.led_1, "out");
    set_gpio_direction(gpio_config.led_2, "out");

    set_gpio_value(gpio_config.led_1, 0); // Red LED OFF
    set_gpio_value(gpio_config.led_2, 0); // Green LED OFF
}

void cleanup_system(void) {
    set_gpio_value(gpio_config.led_1, 0); // Ensure LEDs are off
    set_gpio_value(gpio_config.led_2, 0);

    unexport_gpio(gpio_config.b1);
    unexport_gpio(gpio_config.b2);
    unexport_gpio(gpio_config.led_1);
    unexport_gpio(gpio_config.led_2);
}

void get_gpio_configuration(void) {
    printf("Enter GPIO pin numbers:\n");
    printf("Button B1 (Start/Stop Button): ");
    scanf("%d", &gpio_config.b1);
    printf("Button B2 (Reset Button): ");
    scanf("%d", &gpio_config.b2);
    printf("LED 1 (Red LED): ");
    scanf("%d", &gpio_config.led_1);
    printf("LED 2 (Green LED): ");
    scanf("%d", &gpio_config.led_2);
}

void print_system_info(void) {
    struct utsname system_info;
    if (uname(&system_info) == 0) {
        printf("System Name: %s\n", system_info.sysname);
        printf("Node Name: %s\n", system_info.nodename);
        printf("Release: %s\n", system_info.release);
        printf("Version: %s\n", system_info.version);
        printf("Machine: %s\n", system_info.machine);
    }
}

