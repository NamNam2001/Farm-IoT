
import sys
import logging
import paho.mqtt.client as mqtt
import json
import csv
import random
# from datetime import datetime
import time
# from influxdb_client_3 import InfluxDBClient3, Point
import os


import threading
import _thread
from arduino_iot_cloud import ArduinoCloudClient
from arduino_iot_cloud import Task



def restart_program():
    python = sys.executable
    os.execl(python, python, *sys.argv)

data={} 
previous_data = {}
data_check= []

DEVICE_ID = b"99bcce8b-3547-4a5f-9f11-6bcd2f6896b3"
SECRET_KEY = b"C?706JwYDLigWiy4dgzWVMsHf"

esp_client = ArduinoCloudClient(device_id=DEVICE_ID, username=DEVICE_ID, password=SECRET_KEY)
def logging_func():
    logging.basicConfig(
        datefmt="%H:%M:%S",
        format="%(asctime)s.%(msecs)03d %(message)s",
        level=logging.INFO,
    )   


def create_task(key):
    def user_task(client):
        if key in data:
            esp_client[key] = data[key]
        else:
            print(f"Not key in data")
            pass

    return user_task
            
def arduino_cloud_thread():
    try:
        global previous_data
        previous_data = data.copy()
        print(previous_data)
        logging_func()
        for key in previous_data:
            # variable = key[0].lower() + key[1:]
            # print(variable)
            task_function = create_task(key)
            esp_client.register(Task(key, on_run=task_function, interval=1.0))
            
        esp_client.start()
    except KeyboardInterrupt:
        print("Thread is interrupt")

USER = "fire-warn@ttn"
PASSWORD = "NNSXS.VBPAQ7FVULT3W6NWIY5CJAX5ALAH7RFJPJIAXUQ.SJTFUBIFR4ATX4DPEYFJCMEP7FBRLWYF2BETPCPKBQHUTX5Y6DDA"
PUBLIC_TLS_ADDRESS = "eu1.cloud.thethings.network"
PUBLIC_TLS_ADDRESS_PORT = 8883
# DEVICE_ID = "eui-70B3D57ED00644EA"
ALL_DEVICES = True

i=2

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    #client.subscribe(uplink_topic)
    client.subscribe("#", 0)	# all device uplinks
    
# Callback function when a message is received
def on_message(client, userdata, msg):
    global i 
    i+=1
    payload = json.loads(msg.payload)
    # print("Received Data: ", payload)
    if "uplink_message" in payload:
        uplink_message = payload["uplink_message"]
        if "decoded_payload" in uplink_message: 
            decoded_payload = uplink_message["decoded_payload"]
        else: 
            decoded_payload = ""
    else: 
        decoded_payload = ""
    print("{} Received message: {}".format(i,decoded_payload))

    for payload in decoded_payload:
        if payload not in data:
            data.update({payload: decoded_payload[payload]})
        else:
            if decoded_payload[payload] != data[payload]: 
                data[payload]= decoded_payload[payload]
            else:
                # print("namdeptrai")
                pass
        check_values_avilable(20, payload)

next_time = 0
def check_values_avilable(interval, payload):
    current_time = time.time()
    global next_time, previous_data, data_check

    if payload not in data_check:
        data_check.append(payload)
    if current_time >= next_time:
        next_time = current_time + interval
        for payload in data:
            print(payload)
            if payload not in data_check:
                previous_data = {}
        data_check= []
        
    print(data_check)
        
        
# Create MQTT Client instance
def mqtt_thread():
    client = mqtt.Client()
    client.username_pw_set(USER, PASSWORD)

    # Set the callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    # Connect to the MQTT Broker
    client.tls_set()
    client.connect(PUBLIC_TLS_ADDRESS, PUBLIC_TLS_ADDRESS_PORT, keepalive=60)

    # Start the MQTT loop to receive messages
    client.loop_start()

    # Wait for a while to receive messages (you can add your own logic here)
    try:
        while True:
            # print(i)
            pass
    except KeyboardInterrupt:
        pass

    # Stop the MQTT loop and disconnect from the MQTT Broker
    client.loop_stop()
    client.disconnect()

# Khởi tạo và chạy các threads
thread1 = threading.Thread(target=mqtt_thread)
thread1.start()

time.sleep(10)
print("10s later======================================")
# # Khởi tạo và chạy luồng Arduino Cloud sau khi chờ 5 giây

# Khởi tạo và chạy thread 2
thread2 = threading.Thread(target=arduino_cloud_thread)
# print(data)
thread2.start()

# Thread 1 chạy và kiểm tra điều kiện i
# time.sleep(15)
# print(data)
# print(previous_data)
while True:
    for payload in list(data.keys()):
        if payload not in previous_data: 
            # keepGoing = False
            # previous_data = data.copy()
            print("Stopping Thread 2...")
            # time.sleep(1)
            print("Restarting program...")
            restart_program()
            # Đặt cờ để dừng thread 2
            # thread2.daemon = True
            # print("Thread 2 stopped.")

            # print("Restarting Thread 2...")
            # Đặt lại cờ để cho phép thread 2 khởi động lại
            # stop_event.clear()
            # thread2.interrupt()
#             # thread2.join()
