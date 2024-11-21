#include "RTOS.h"

// GPIO Pin Assignments
#define START_BUTTON gpio_config.b1       // Start/Stop button
#define RESET_BUTTON gpio_config.b2          // Reset button
#define LED_RED gpio_config.led_1         // Red LED
#define LED_GREEN gpio_config.led_2       // Green LED

// Global variables
volatile float elapsed_time = 0.0;       // Stopwatch time in seconds
volatile bool is_running = false;        // Stopwatch running state
volatile bool is_initialized = false;    // To track the first start/stop button press
pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function to handle button actions
void* handle_button_input(void* arg) {
    (void)arg;
    struct timespec delay = {0, 10000000};  // 10 ms delay
    int prev_start_btn_state = 0;          // Previous state of Start/Stop button (released = 0)
    int prev_reset_btn_state = 0;          // Previous state of Reset button (released = 0)

    while (system_running) {
        int start_btn_pressed = read_gpio_value(START_BUTTON) == 1;  // Active high
        int reset_btn_pressed = read_gpio_value(RESET_BUTTON) == 1;     // Active high

        if (start_btn_pressed && prev_start_btn_state == 0) {
            pthread_mutex_lock(&time_mutex);
            if (!is_initialized) {
                // First start/stop press after initialization
                is_initialized = true;
                elapsed_time = 0.0;  // Ensure timer starts at 0
                set_gpio_value(LED_RED, 0);  // Turn off red LED
                set_gpio_value(LED_GREEN, 1);  // Turn on green LED
                is_running = true;            // Start the timer
                printf("Stopwatch started for the first time.\n");
            } else {
                // Toggle the running state after the first press
                is_running = !is_running;
                printf("Start/Stop button pressed. Running state toggled to: %s\n", is_running ? "ON" : "OFF");
                if (is_running) {
                    set_gpio_value(LED_RED, 0);  // Turn off red LED
                    set_gpio_value(LED_GREEN, 1);  // Turn on green LED
                } else {
                    set_gpio_value(LED_RED, 1);  // Turn on red LED
                    set_gpio_value(LED_GREEN, 0);  // Turn off green LED
                }
            }
            pthread_mutex_unlock(&time_mutex);
            prev_start_btn_state = 1;  // Mark button as handled
        } else if (!start_btn_pressed) {
            prev_start_btn_state = 0;  // Reset state when button is released
        }

        if (reset_btn_pressed && prev_reset_btn_state == 0 && is_initialized) {
            // Handle reset without changing initialization state
            pthread_mutex_lock(&time_mutex);
            elapsed_time = 0.0;  // Reset stopwatch
            is_running = false;  // Pause the stopwatch
            set_gpio_value(LED_RED, 1);  // Turn on red LED
            set_gpio_value(LED_GREEN, 0);  // Turn off green LED
            printf("Reset button pressed. Stopwatch reset to 0.0 seconds.\n");
            pthread_mutex_unlock(&time_mutex);
            prev_reset_btn_state = 1;  // Mark button as handled
        } else if (!reset_btn_pressed) {
            prev_reset_btn_state = 0;  // Reset state when button is released
        }

        nanosleep(&delay, NULL);  // Sleep for 10ms
    }
    return NULL;
}

// Thread function to update the stopwatch counter
void* update_timer(void* arg) {
    (void)arg;
    struct timespec delay = {0, 10000000};  // 10 ms delay

    //float rollover_threshold = 10.0; 
    // Replacing FLT_MAX with smaller value for testing
    //whether or not the counter rolls over to zero when a certain threshold is exceeded

    while (system_running) {
        pthread_mutex_lock(&time_mutex);
        if (is_running) {  // Only increment if running
            elapsed_time += 0.01;  // Increment stopwatch time by 10ms
            if (elapsed_time >= FLT_MAX) { //if (elapsed_time >= rollover_threshold)
                elapsed_time = 0.0;
            }
        }
        pthread_mutex_unlock(&time_mutex);

        nanosleep(&delay, NULL);  // Sleep for 10ms
    }

    return NULL;
}

// Thread function to display the stopwatch time
void* display_timer(void* arg) {
    (void)arg;
    struct timespec delay = {0, 100000000};  // 100 ms delay
    float last_displayed_time = -1.0;       // Keep track of the last displayed time
    bool was_running = false;              // Track the previous running state

    while (system_running) {
        pthread_mutex_lock(&time_mutex);
        if (is_running) {
            if (!was_running) {
                printf("Stopwatch resumed.\n");
                was_running = true;
            }

            if (elapsed_time != last_displayed_time) {
                printf("Current Stopwatch Time: %.1f seconds\n", elapsed_time);
                last_displayed_time = elapsed_time;  // Update last displayed time
            }
        } else if (was_running) {
            printf("Stopwatch paused.\n");
            printf("Current Stopwatch Time: %.2f seconds\n", elapsed_time);
            was_running = false;
        }
        pthread_mutex_unlock(&time_mutex);

        nanosleep(&delay, NULL);  // Sleep for 100ms
    }

    return NULL;
}

// Signal handler for cleanup
void handle_signal(int signal) {
    system_running = false;
}

int main(void) {
    pthread_t button_thread, timer_thread, display_thread;
    pthread_attr_t attr;
    struct sched_param param;

    // Print system info and team details
    print_system_info();
    printf("\nTeam Members:\n");
    printf("Nanda Kishore Nallagopu G01447294\n");
    printf("Amlan Chowdhury G01465085\n\n");

    // Get GPIO configuration
    get_gpio_configuration();

    // Initialize system GPIOs
    initialize_system();
    printf("Starting stopwatch application...\n");

    // Configure GPIO directions
    set_gpio_direction(START_BUTTON, "in");
    set_gpio_direction(RESET_BUTTON, "in");
    set_gpio_direction(LED_RED, "out");
    set_gpio_direction(LED_GREEN, "out");

    // Set initial LED states
    set_gpio_value(LED_RED, 1);   // Red LED ON
    set_gpio_value(LED_GREEN, 0); // Green LED OFF

    // Debounce GPIO states during initialization
    int initial_start_btn_state = read_gpio_value(START_BUTTON);
    int initial_reset_btn_state = read_gpio_value(RESET_BUTTON);

    // Wait for all buttons to be released before starting threads
    while (initial_start_btn_state == 1 || initial_reset_btn_state == 1) {
        initial_start_btn_state = read_gpio_value(START_BUTTON);
        initial_reset_btn_state = read_gpio_value(RESET_BUTTON);
    }

    // Initialize thread attributes for RMS
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // Get the maximum and minimum priority for SCHED_FIFO
    int max_priority = sched_get_priority_max(SCHED_FIFO);
    int min_priority = sched_get_priority_min(SCHED_FIFO);

    // Timer thread (highest priority)
    param.sched_priority = max_priority;  // Highest priority
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&timer_thread, &attr, update_timer, NULL);

    // Display thread (medium priority)
    param.sched_priority = max_priority - 1;  // Medium priority
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&display_thread, &attr, display_timer, NULL);

    // Button thread (lowest priority)
    param.sched_priority = min_priority;  // Lowest priority
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&button_thread, &attr, handle_button_input, NULL);

    // Wait for threads to finish
    pthread_join(button_thread, NULL);
    pthread_join(timer_thread, NULL);
    pthread_join(display_thread, NULL);

    // Cleanup GPIOs
    cleanup_system();
    printf("Stopwatch application terminated.\n");

    return 0;
}
