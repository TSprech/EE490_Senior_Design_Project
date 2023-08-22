# Current Voltage Sensors

# Voltage Sensor
Contains: Resistor Divider with an OP Amp with a unity gain
# Analog Digital Converter
Op Amp goes into an Analog Digital Converter (ADC) in both current and voltage sensor

- 1 Mega sample per second
- 12 bit resolution
- Uses SPI to communicate

# Current Sensor 
Has a very low resistance shunt resistor to not dissipate much additional power
Connected to a 200 volt per volt current sense amplifier which has built in PWM rejection
