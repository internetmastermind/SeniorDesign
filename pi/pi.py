#!/usr/bin/env python

from __future__ import print_function
import time
from RF24 import *
import RPi.GPIO as GPIO
import spidev

log_file = "log.txt"

pipes = [0xF0F0F0F0E1, 0xF0F0F0F0D2]
irq_gpio_pin = None
inp_role = 'none'
send_payload = b'0101'

MIN_PAYLOAD_SIZE = 4
MAX_PAYLOAD_SIZE = 32

radio = RF24(6, 8)


##########################################
# log_write: write data to log file
##########################################
def log_write(*argv):
    try:
        with open(log_file, "a") as f:
            for arg in argv:
                f.write("{}\t".format(arg))
                # f.write("\n")
            print("Wrote to file.")
    except IOError:
        print("Opening/Creating File Error")


##########################################
# try_read_data: attempt to read data from pipe.
##########################################
def try_read_data(channel=0):
    if radio.available():
        while radio.available():
            # Get payload length
            #len = radio.payloadSize()
            # Timestamp read
            ts = time.ctime(time.time())
            # Read payload
            receive_payload = radio.read(4)
            
            # Write to data to log
            print('got response size={} value="{}"'.format(len, receive_payload.decode('utf-8')))
            log_write(receive_payload.decode('utf-8'), ts)

            # First, stop listening so we can talk
            radio.stopListening()

            # Send the final one back.
            radio.write(receive_payload)
            print('Sent response.')

            # Now, resume listening so we catch the next packets.
            radio.startListening()





radio.begin()
radio.setRetries(5, 15)
radio.printDetails()

# Hardcoded as receiver
radio.openWritingPipe(pipes[1])
radio.openReadingPipe(1, pipes[0])
radio.startListening()

while(1):
    if irq_gpio_pin is None:
        # no irq pin is set up -> poll it
        try_read_data()
    else:
        # callback routine set for irq pin takes care for reading -
        # do nothing, just sleeps in order not to burn cpu by looping
        time.sleep(1000)
