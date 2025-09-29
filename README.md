# LED Matrix Display Controller

A simple project to control an LED panel to display and play a game of Pong

# Setup

You will need ARM Cortex-M controllers for the joysticks, and a 34x34 panel correctly configured with 16 64 bit registers, with the first half of each register for the top 16 rows of the display, and the second half for the bottom 16 rows. Load the code onto the controller, and you should be able to control two paddles and play pong with the joysticks.
