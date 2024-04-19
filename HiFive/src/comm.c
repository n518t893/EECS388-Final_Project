#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
    uint16_t dist = 0;
    if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {
        //reading bits 3 and 4
        uint8_t bit3 = ser_read(devid);
        uint8_t bit4 = ser_read(devid);
        //shifting bit 4 by 8 and Or ing with bit 3, since bit 4 goes first
        dist = (bit4 << 8) | bit3;

        if (dist <= 60) {
            gpio_write(GREEN_LED, OFF);
            //turn on flashing red light
            gpio_write(RED_LED, ON);
            delay(1000);
            gpio_write(RED_LED, OFF);
            delay(1000);
        }
        else if (dist <= 100 && dist > 60) {
            gpio_write(GREEN_LED, OFF);
            //turn on steady red only
            gpio_write(RED_LED, ON);
            delay(1000);
        }
        else if (dist <= 200 && dist > 100) {
            //turn on steady yellow
            gpio_write(RED_LED, ON);
            gpio_write(GREEN_LED, ON);
            delay(1000);
        }
        else if (dist > 200) {
            gpio_write(RED_LED, OFF);
            //turn on steady green
            gpio_write(GREEN_LED, ON);
            delay(1000);
        }
    }
    
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.
    char line[10]; // Assuming angle values are sent as strings with maximum length 10
    int angle = 0;
    
    // Read a line from the serial connection
    ser_readline(devid, sizeof(line), line);
    
    // Parse the line to extract angle
    sscanf(line, "%d\n", &angle);
    
    return angle;
}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
    // int pwm = (SERVO_PULSE_MIN + ((SERVO_PULSE_MAX - SERVO_PULSE_MIN) / 180) * pos);
    // gpio_write(gpio, ON);
    // delay_usec(pwm);
    // gpio_write(gpio, OFF);
    // delay_usec(SERVO_PERIOD - pwm);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
