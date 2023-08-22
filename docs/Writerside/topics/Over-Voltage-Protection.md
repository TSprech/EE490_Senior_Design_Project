# Over Voltage Protection

The Over Voltage Protection PCB board has two pins which connect to GPIO pins on the RP2040. 

- One control line allows us to monitor if the circuit is in over voltage protection which exceeds the voltage threshold of 4.17 volts or not.
- The other line is forces a shutdown by user control.


**Comparator**: Compares the voltage of the battery to a known voltage value we set to be at 4.17V. When the battery
voltage exceeds this amount, the connection from the power supply to the battery is shut off.

Our comparator has 60mV hysteresis before it turns back on.
