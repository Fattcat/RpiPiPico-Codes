from machine import Pin, PWM
import utime

# Define buzzer pin
buzzer = PWM(Pin(18))  # Initialize buzzer without frequency
buzzer.freq(100)  # Set initial frequency to 100 Hz

# Define encoder pins
outA = Pin(4, mode=Pin.IN)  # Pin CLK of encoder
outB = Pin(3, mode=Pin.IN)  # Pin DT of encoder
switch = Pin(2, mode=Pin.IN, pull=Pin.PULL_UP)  # inbuilt switch on the rotary encoder, ACTIVE LOW

# Define global variables
frequency = 100  # Initial frequency
direction = 0  # Initial direction

# Interrupt handler function (IRQ) for CLK and DT pins
def encoder(pin):
    global frequency
    global direction

    # Read the current state of outA pin / CLK pin
    outA_current = outA.value()

    # If current state is not same as the last state, encoder has rotated
    if outA_current != direction:
        # Read outB pin/ DT pin
        # If DT value is not equal to CLK value, rotation is clockwise
        if outB.value() != outA_current:
            frequency += 1  # Increase frequency by 1 Hz
            if frequency > 10000:  # Limit frequency to 10 KHz
                frequency = 10000
        else:
            frequency -= 1  # Decrease frequency by 1 Hz
            if frequency < 1:  # Limit frequency to 1 Hz
                frequency = 1

        # Update buzzer frequency
        buzzer.freq(frequency)

    direction = outA_current

# Interrupt handler function (IRQ) for switch pin
def button(pin):
    global frequency
    frequency = 1  # Reset frequency to 1 Hz
    buzzer.freq(frequency)  # Update buzzer frequency

# Attach interrupts to encoder pins
outA.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=encoder)
outB.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=encoder)

# Attach interrupt to switch pin
switch.irq(trigger=Pin.IRQ_FALLING, handler=button)

while True:
    # Keep the buzzer on
    buzzer.duty_u16(32768)  # 50% duty cycle, 32768 out of 65535
    utime.sleep(0.01)
