/*
 * The main configuration file.
*/


/********
 * Intervals and optimization
 *******/

/* Power supply status polling interval. Defined in miliseconds. */
#define POLLING_INTERVAL 100

/* Passive URL notification interval. Defined in seconds. */
#define NOTIFICATION_INTERVAL 300

/* Power level threshold. A value of 512 usually works and shouldn't be changed. */
#define POWER_THRESHOLD 512

/* Enable debugging through the console. */
#define DEBUG_ENABLE false



/********
 * Status LED Configuration
 *******/

/* Status LED. This LED flashes to indicate the status of the device. Dimmed flashing indicates that the device
 * is connecting to a WiFi network, while a short bright flash indicates failure. Afterwards, a short bright flash 
 * indicates successful URL opening while a long one failure . */
#define LED_ENABLE true

/* Brightness of the status LED while in idle/connecting to WiFi. Can range from 0 to 1024, but setting it to 2 is suggested. */
#define LED_BRIGHTNESS_DIM 2

/* Brightness of the status LED while during a bright flash. Can range from 0 to 1024. */
#define LED_BRIGHTNESS_ON  250
