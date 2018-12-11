#!/usr/bin/env python

from __future__ import print_function
import time
from RF24 import *
import RPi.GPIO as GPIO
import spidev

log_file = "log.txt"

pipes = [0xF0F0F0F0E1, 0xF0F0F0F0D2]
clients = [b'01', b'02']
irq_gpio_pin = None
inp_role = 'none'
send_request = b'REQUESTING'

SET_PAYLOAD_SIZE = 4
MIN_PAYLOAD_SIZE = 4
MAX_PAYLOAD_SIZE = 32

radio = RF24(6, 8)


##########################################
# log_write: write data to log file
##########################################
def log_write(*argv):
    l = []
    try:
        with open(log_file, "a") as f:
            for arg in argv:
                l.append(arg)
            
            f.write("{}\t{}".format(*l))
            
            f.write("\n")
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
            len = radio.getDynamicPayloadSize()
            # Timestamp read
            ts = time.ctime(time.time())
            # Read payload
            receive_payload = radio.read(len)

            # Write to data to log
            #print('got response size={} value="{}"'.format(len, receive_payload.decode('utf-8')))
            #log_write(receive_payload.decode('utf-8'), ts)

            #print('Machine ID = {} Relay State = {}'.format(ids, state))

            # First, stop listening so we can talk
            radio.stopListening()

            # Send the final one back.
            radio.write(receive_payload)
            print('Sent response.')

            # Now, resume listening so we catch the next packets.
            radio.startListening()
            return receive_payload

def try_send_request(id=b'00', msg, client):
    sent_msg = False
    radio.stopListening()

    radio.write(msg + client)

    radio.startListening()

    started_waiting_at = millis()
    timeout = False
    while(not radio.available()) and (not timeout):
        if(millis() - started_waiting_at) > 500:
            timeout = True

    if timeout:
        print('failed, response timed out.')
    else:
        len = radio.getDynamicPayloadSize()
        receive_payload = radio.read(len)
        sent_msg = True


    time.sleep(0.1)
    return sent_msg


radio.begin()
radio.enableDynamicPayLoads()
radio.setRetries(5, 15)
radio.printDetails()
print("=========================================")

# Hardcoded as receiver
radio.openWritingPipe(pipes[1])
radio.openReadingPipe(1, pipes[0])
radio.startListening()

while(1):
    if irq_gpio_pin is None:
        for c in clients:
            if(try_send_request(c, b'REQUESTING ', c)):
                response = try_read_data()
                if(response == b'NOTHING'):
                    pass
                else:
                    machine_id, state = response.decode('utf-8').split()
                    print('Machine ID = {} Relay State = {}'.format(machine_id, state))
                    log_write(receive_payload.decode('utf-8'), ts)
    else:
        time.sleep(1000)
