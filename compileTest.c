#include "libopencm3/stm32/rcc.h"   
#include "libopencm3/stm32/gpio.h"  

//Needed to enable clocks for particular GPIO ports
//Needed to define things on the GPIO

//GPIO Port Name, GPIO Mode, GPIO Push Up Pull Down Mode, GPIO Pin Number

//gpio_set(GPIOC, GPIO8); // Sets C8. i.e. THE LATCH. It must be high for you show what is in memory. active low.

/**
 * C6 => input signal
 * C7 => clock signal
 * C8 => latch (active low)
 * C2-5 => row selection
**/

//The datastore used to hold what we want the panel to look like.
int renderingData[16][192];

void clear_screen(void) {
    for (int j = 0; j < 16; j++) {
        select_row(j);
        for (int i = 0; i<(192); i++) {
            gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
            gpio_clear(GPIOC, GPIO6);
            gpio_clear(GPIOC, GPIO7); //SETS CLOCK LOW
        }
    }
}

void clear_data(void) {
    for(int j = 0; j < 16; j++) {
        for(int i = 0, i < 192; i++) {
            renderingData[j][i] = 0;
        }
    }
}

//Bottom panel is an extension of the top panel to its left

//Function for selecting a row from 0-16
void select_row(int row) {

    int bit_0 = row & (1 << 3);  //Bitmasking to select a certain bit and check if its there
    int bit_1 = row & (1 << 2);
    int bit_2 = row & (1 << 1);
    int bit_3 = row & 1;

    if (bit_0 == 0) { //Checking the specific bit and using it to select the row
        gpio_clear(GPIOC, GPIO5);
    } else {
        gpio_set(GPIOC, GPIO5);
    }
    if (bit_1 == 0) {
        gpio_clear(GPIOC, GPIO4);
    } else {
        gpio_set(GPIOC, GPIO4);
    }
    if (bit_2 == 0) {
        gpio_clear(GPIOC, GPIO3);
    } else {
        gpio_set(GPIOC, GPIO3);
    }
    if (bit_3 == 0) {
        gpio_clear(GPIOC, GPIO2);
    } else {
        gpio_set(GPIOC, GPIO2);
    }
}

//This function renders one 'frame' of data.
void renderingFunction(void) {
    int row = 0;

    while(row < 16) {
        gpio_clear(GPIOC, GPIO8); //Shut the latch so we can load in the data
        select_row(row);

        for (int i = 0; i < 192; i++) {
            gpio_set(GPIOC, GPIO7); //sets clock high

            if (renderingData[row][i] == 0) {
                gpio_clear(GPIOC, GPIO6); //sets pixel to 0
            } else {
                gpio_set(GPIOC, GPIO6); //sets pixel to 1
            }
            
            gpio_clear(GPIOC, GPIO7); //sets clock low
        }

        gpio_set(GPIOC, GPIO8); //Open the latch to show what is in the memory
        row++; 
    }
}

//This function accepts coordinates as you would see if you labelled the actual panel, 32x32
void addDot(int y, int x, int[3] col) {

    //If the check is equal, we don't need to change anything
    //If the check is different, we need to shift over 92 pixels
    //to represent the lower half of the panel
    y %= 16;
    int check = y % 32;
    if(y != check) {
        x += 92;
    }

    int i = 0;
    while( i < 3) {
        //Complicated, but this is mathematics to convert from the coordinate system
        //of the panel into what the panel actually accepts as an input, 16x192
        renderingData[y][192 - (x + 32*i)] = col[2 - i];
        i++;
    }
}

void drawPaddleRight(int y) {
    int counter = 6; //This is the height of each paddle
    int x = 2; //This is the column the paddle will travel in

    //We recursively draw dots to make the paddle
    while(counter >= 0) {
        addDot(y, x,[1,1,1]);
        y++;
        counter--;
    }
}

void drawPaddleLeft(int y) {
    int counter = 6; //This is the height of each paddle
    int x = 29; //This is the column the paddle will travel in

    //We recursively draw dots to make the paddle
    while(counter >= 0) {
        addDot(y, x,[1,1,1]);
        y++;
        counter--;
    }
}

void drawBall(int y, int x) {
    //Add a dot in each pixel to make the ball
    addDot(y,x,[1,1,1]);
    addDot(y+1,x,[1,1,1]);
    addDot(y,x+1,[1,1,1]);
    addDot(y+1,x+1,[1,1,1])
}

void main(void) {

    //START SETUP //

    //Enable clock for GPIO Port C
    rcc_periph_clock_enable(RCC_GPIOC);
    
    //"Opens" 2-8 pins and enables. 
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);

    //Configures the way each pin is setup to interpret instructions
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO7);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO6);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO5);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO4);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO3);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO2);   

    // END SETUP //


    //Example code loop
    while(true){
        clear_screen();
        clear_data();
        drawBall(16,16);
        drawPaddleLeft(3);
        drawPaddleRight(3);
        renderingFunction();
    }

}
