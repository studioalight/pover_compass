import serial
import socket
import paho.mqtt.client as mqtt

BROKER = 'broker.hivemq.com'
hostname = socket.gethostname() + "heading"

client = mqtt.Client(hostname)

def on_disconnect(client, userdata, rc):
    if rc != 0:
        exit(1)

try:
    client.connect(BROKER)
except:
    exit(1)

serialPort = serial.Serial('/dev/ttyACM0', 115200)

client.on_disconnect = on_disconnect

while(1):

    # Wait until there is data waiting in the serial buffer
    if(serialPort.in_waiting > 0):

        # Read data out of the buffer until a carraige return / new line is found
        serialString = serialPort.readline()

        # Print the contents of the serial data
        client.publish("pover/" + hostname + "/heading",serialString.decode('Ascii'))
