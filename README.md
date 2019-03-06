# SPI-Interface-DAC

The following is an embedded program meant to be run on the Altera DE2-115 Cyclone IV FPGA. Quartus II, Altera Monitoring Program, and Qsys were used in assisting with code debugging, and hardware configuration on the fpga.

Use the NIOS II's Timer IRQ to generate a fixed step-period for the wave. The timer interrupt service routine incrememnts the step value and outputs it to the SPI txdata register to be sent to the DAC. Assigning two buttons to separate PIO ports we can listen to the edgecapture register of each port and, acting accordingly, can double or halve the timer period based on which button was pressed.