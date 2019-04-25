"""
'lux.py'
==================================
Example of sending analog sensor
values to an Adafruit IO feed.

Author(s): Brent Rubell

Dependencies:
    - Adafruit IO Python Client
        (https://github.com/adafruit/io-client-python)
"""

# import standard python modules.
import time

# import Adafruit IO REST client.
from Adafruit_IO import Client, Feed

# For I2C
import smbus

# Delay in-between sensor readings, in seconds.
LUX_READ_TIMEOUT = 5

# Set to your Adafruit IO key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!
ADAFRUIT_IO_KEY = '074d7b1885134e54b31be0ace276c88c'

# Set to your Adafruit IO username.
# (go to https://accounts.adafruit.com to find your username).
ADAFRUIT_IO_USERNAME = 'suls_inc'

# Create an instance of the REST client.
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# Set up Adafruit IO Feeds.
lux_feed = aio.feeds('lux')

# Set up TSL2561 Sensor.
# Get I2C bus
bus = smbus.SMBus(1)

# TSL2561 address, 0x39(57)
# Select control register, 0x00(00) with command register, 0x80(128)
#		0x03(03)	Power ON mode
bus.write_byte_data(0x39, 0x00 | 0x80, 0x03)
 
while True:
    # TSL2561 address, 0x39(57)
    # Select timing register, 0x01(01) with command register, 0x80(128)
    #		0x02(02)	Nominal integration time = 402ms
    bus.write_byte_data(0x39, 0x01 | 0x80, 0x02)

    time.sleep(0.5)
    
    # Read data back from 0x0C(12) with command register, 0x80(128), 2 bytes
    # ch0 LSB, ch0 MSB
    data = bus.read_i2c_block_data(0x39, 0x0C | 0x80, 2)
    
    # Read data back from 0x0E(14) with command register, 0x80(128), 2 bytes
    # ch1 LSB, ch1 MSB
    data1 = bus.read_i2c_block_data(0x39, 0x0E | 0x80, 2)
    
    # Convert the data
    ch0 = data[1] * 256 + data[0]
    ch1 = data1[1] * 256 + data1[0]

    lux = ch0

    if lux is not None:
        print('Full spectrum={0:0.1f} lux'.format(lux))
        # Send lux feeds to Adafruit IO
        lux = '%.2f'%(lux)
        aio.send(lux_feed.key, str(lux))
    else:
        print('Failed to get TSL2561 Reading, trying again in ', LUX_READ_TIMEOUT, 'seconds')

    # Timeout to avoid flooding Adafruit IO
    time.sleep(LUX_READ_TIMEOUT)
