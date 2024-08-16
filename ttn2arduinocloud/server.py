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
import asyncio


import threading
from arduino_iot_cloud import ArduinoCloudClient
from arduino_iot_cloud import Task


def restart_program():
    python = sys.executable
    os.execl(python, python, *sys.argv)

data={} 
previous_data={}
data_check = []
warning_count = {}
pH_warning_range = {}

DEVICE_ID = b"28d7800d-0c2d-4f38-afc5-748caee71d84"
SECRET_KEY = b"!5IILjphe#Dha0FtnoF7T43lv"

esp_client = ArduinoCloudClient(device_id=DEVICE_ID, username=DEVICE_ID, password=SECRET_KEY)
def logging_func():
    logging.basicConfig(
        datefmt="%H:%M:%S",
        format="%(asctime)s.%(msecs)03d %(message)s",
        level=logging.INFO,
    )   


def create_task(key):
    def user_task(client):
        # print("Data: ", data)
        if key in data:
            # esp_client[key] = float(data[key])#BUggggggggggggg
            if "warning" in key.split('_'):
                # if data[key] != "":
                esp_client[key] = data[key]
                warning_count[key] = 0 
            else:
                esp_client[key] = float(data[key])#BUggggggggggggg
        else:
            print(f"Not key in data")
            pass

    return user_task

# def on_led_changed(client, value):
#   if value:
#       print("bật")
#   else:
#       print("tắt")


def create_callback_pH_warning(key_warning_target):
    def on_pH_target_change(client, value):
        print(key_warning_target,":",value)
        if "high" or "low" in key_warning_target:
            key_warning_range= key_warning_target.split("_")[0]+"_warning_range"
            if key_warning_range not in pH_warning_range:
                pH_warning_range[key_warning_range] = [None, None]

            if "high" in key_warning_target:
                pH_warning_range[key_warning_range][0] = float(value)
            elif "low" in key_warning_target:
                pH_warning_range[key_warning_range][1] = float(value)
    return on_pH_target_change


def arduino_cloud_thread():
    try:
        global previous_data
        previous_data = data.copy()
        print(previous_data)
        # print("============")
        # print(warning_count)

        if previous_data == {}:
            raise Exception("Disconnected to TTN")
        logging_func()
        for key in previous_data:
            # variable = key[0].lower() + key[1:]
            # print(variable)
            task_function = create_task(key)
            if key == "m1_van1" or key == "motor_pump":
                esp_client.register(Task(key, on_run=task_function, interval=1.0))
            elif "warning" in key.split('_'):
                esp_client.register(Task(key, on_run=task_function, interval=600.0))
            else:
                esp_client.register(Task(key, on_run=task_function, interval=10.0))
            #PH warning
            if "pH" in key and 'temp' not in key:
                key_warning_range = [key + "_high", key + "_low"]
                print(key_warning_range)
                pH_warning_high= create_callback_pH_warning(key_warning_range[0])
                pH_warning_low= create_callback_pH_warning(key_warning_range[1])
                esp_client.register(key_warning_range[0], value=None, on_write=pH_warning_high)
                esp_client.register(key_warning_range[1], value=None, on_write=pH_warning_low)

        esp_client.start()
    except KeyboardInterrupt:
        print("Thread is interrupt")
    except Exception as e:
        restart_program()

USER = "fire-warn@ttn"
PASSWORD = "NNSXS.VBPAQ7FVULT3W6NWIY5CJAX5ALAH7RFJPJIAXUQ.SJTFUBIFR4ATX4DPEYFJCMEP7FBRLWYF2BETPCPKBQHUTX5Y6DDA"
PUBLIC_TLS_ADDRESS = "eu1.cloud.thethings.network"
PUBLIC_TLS_ADDRESS_PORT = 1883
DEVICE_ID_TTN = "stm32-motor-current-4"
DEVICE_ID_TTN1 = "uca-board-6"
ALL_DEVICES = True
#control device
topic = "v3/"+USER+"/devices/"+DEVICE_ID_TTN+"/down/replace"
topic1 = "v3/"+USER+"/devices/"+DEVICE_ID_TTN1+"/down/replace"
message_on = '{"downlinks":[{"f_port":1,"frm_payload":"AQ==","priority":"NORMAL"}]}'
message_off = '{"downlinks":[{"f_port":1,"frm_payload":"AA==","priority":"NORMAL"}]}'
message_error = '{"downlinks":[{"f_port":1,"frm_payload":"Ag==","priority":"NORMAL"}]}'

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
            # Get rssi
            if "rx_metadata" in uplink_message:
                rx_metadata = uplink_message["rx_metadata"][0]
                if "rssi" in rx_metadata:
                    # Lấy key đầu tiên trong decoded_payload
                    # Danh sách tạm để lưu các khóa đã tách
                    key_temp = []

                    for key in list(decoded_payload.keys()):
                        split_key = key.split('_')
                        key_temp.append(split_key)

                    for split_key in key_temp:
                        if "pH" in split_key:
                            pH_index = split_key.index("pH")
                            if pH_index > 0:
                                prefix = split_key[pH_index - 1]
                                # print(prefix)
                                new_key = f"{prefix}_rssi"
                                decoded_payload[new_key] = rx_metadata["rssi"]

            # # Check warning PH value
            # for key in decoded_payload.keys():
            #     split_key = key.split('_')
            #     if "pH" in split_key and "temp" not in split_key:
            #         key_warning = f"{split_key[0]}_warning"
            #         decoded_payload[key_warning] = ""
            #         break

               # Check warning PH value
            for key in list(decoded_payload.keys()):
                split_key = key.split('_')
                if "pH" in split_key and "temp" not in split_key:
                    key_warning = f"{split_key[0]}_warning"
                    if key_warning not in data: 
                    # if decoded_payload[key] > 8 or decoded_payload[key] < 7.7:

                    # else:
                        decoded_payload[key_warning] = ""
                    else:
                        decoded_payload[key_warning] = data[key_warning]

        else: 
            decoded_payload = ""
    else: 
        decoded_payload = ""
    # print("{} Received message: {}".format(i,decoded_payload))
    # print(warning_count)

    for payload in decoded_payload:
        if payload not in data:
            data.update({payload: decoded_payload[payload]})
        else:
            if decoded_payload[payload] != data[payload]: 
                data[payload]= decoded_payload[payload]
            else:
                # print("namdeptrai")
                pass
        check_values_avilable(60, payload)

# //Reset program when device is turned off
next_time = 0

# def check_pH_warning():
#     global warning_count
#     for payload in data:
#         split_key = payload.split('_')
#         if "pH" in split_key and "temp" not in split_key:
#             key_warning = f"{split_key[0]}_warning"
#             if key_warning not in warning_count:
#                 warning_count[key_warning] = 0

#             # Kiểm tra điều kiện
#             # print(data[payload])
#             # print(data[key_warning])

#             if (data[payload] > 8 or data[payload] < 7.7):
#                 warning_count[key_warning] += 1  
#             else:
#                 warning_count[key_warning] = 0 


#             if warning_count[key_warning] >= 2:
#                 data[key_warning] = str(data[payload])
#                 warning_count[key_warning] = 0 


def check_values_avilable(interval, payload):
    current_time = time.time()
    global next_time, previous_data, data_check

    if payload not in data_check:
        data_check.append(payload)
    if current_time >= next_time:
        next_time = current_time + interval
        for payload in data:
            # print(payload)
            if payload not in data_check:
                previous_data = {}
        data_check= []
        # check_pH_warning()

    # print(data_check)    
#control device
# async def controlDevice(varDevice, topic, client):
    # statusDeviceOn = True 
    # statusDeviceOFF = True 
    # statusError= True
    # statusError1= True
    # # print("Dang chay ne")  
    # intervalDevice= 0
    # timesSend= 0
    # timeErrorSend=0
    # while True:
    #     if varDevice in data:
    #         # print(data[varDevice])
    #         if data[varDevice] == 1 and statusDeviceOn:
    #             if not statusDeviceOFF and timesSend==3 or statusError1:
    #                 intervalDevice= 0
    #                 timesSend= 1
    #                 statusDeviceOFF = True
    #                 statusError1 = False
    #                 client.publish(topic, message_on) 
    #                 print(varDevice + ": Dang bat")
    #             if intervalDevice ==10:
    #                 client.publish(topic, message_on) 
    #                 print(varDevice + ": Dang bat")
    #                 intervalDevice = 0 
    #                 timesSend +=1
    #             if timesSend == 5:
    #                 statusDeviceOn = False
    #             statusError = True

    #         elif data[varDevice] == 0 and statusDeviceOFF:
    #             if not statusDeviceOn and timesSend==3 or statusError:
    #                 intervalDevice= 0
    #                 timesSend= 1
    #                 statusDeviceOn = True
    #                 statusError = False
    #                 client.publish(topic, message_off) 
    #                 print(varDevice + ": Dang tat")

    #             if intervalDevice ==10:
    #                 client.publish(topic, message_off) 
    #                 print(varDevice + ": Dang tat")
    #                 intervalDevice = 0 
    #                 timesSend +=1
    #             if timesSend == 5:
    #                 statusDeviceOFF = False
    #             statusError1 = True
    #         # print("==================")
    #         # print(timesSend)
    #         # print(intervalDevice)
    #     elif varDevice =="motor_pump" not in data:
    #         if intervalDevice % 10==0 and timeErrorSend<5:
    #             client.publish(topic, message_error) 
    #             print(varDevice + ": Dang loi sap mat")
    #             timeErrorSend +=1

    #     intervalDevice +=1 
    #     await asyncio.sleep(1)
async def controlDevice(varDevice, varDevice1, topic, client):
    timeErrorSend=0
    timeErrorSend1=0
    while True:
        if varDevice in data and varDevice1 in data:
            if data[varDevice] != data[varDevice1]:
                if data[varDevice] == 0:
                    for _ in range(3):
                        client.publish(topic, message_off)  
                    print(varDevice +" : Dang Tat")
                else:
                    for _ in range(3):
                        client.publish(topic, message_on)
                    print(varDevice +" : Dang Bat")
                await asyncio.sleep(5) 
                timeErrorSend=0
                timeErrorSend1=0

        elif varDevice =="motor_pump" and varDevice not in data:
            if timeErrorSend<5:
                client.publish(topic, message_error) 
                print(varDevice + ": Dang loi motor")
                timeErrorSend +=1
            await asyncio.sleep(5)
        elif varDevice =="m1_van1" and varDevice not in data:
            if timeErrorSend1<5:
                client.publish(topic, message_off)  
                print(varDevice + ": Dang loi van")
                timeErrorSend1 +=1
            await asyncio.sleep(5)

        await asyncio.sleep(1) 



async def check_pH_warning():
    while True:
        for payload in data:
            if "pH" in payload and "temp" not in payload:
                split_key = payload.split('_')
                key_warning = f"{split_key[0]}_warning"
                key_warning_range = f"{split_key[0]}_warning_range"
                # initial
                if key_warning not in warning_count:
                    warning_count[key_warning] = 0

                # condition
                if key_warning_range in pH_warning_range:
                    if data[payload] > pH_warning_range[key_warning_range][0] or data[payload] < pH_warning_range[key_warning_range][1]:
                        warning_count[key_warning] += 1
                else:  
                    if data[payload] > 8 or data[payload] < 7.7:
                        warning_count[key_warning] += 1  


                # update value with times define
                if warning_count[key_warning] >= 8:
                    data[key_warning] = str(data[payload])
                else:
                    data[key_warning] = ""

        # print("=======warning_count======")
        # print(warning_count)
        # print("=======pH_warning_range======")
        # print(pH_warning_range)
        await asyncio.sleep(60)

# Create MQTT Client instance
def mqtt_thread():
    client = mqtt.Client()
    client.username_pw_set(USER, PASSWORD)

    # Connect to the MQTT Broker
    # client.tls_set()
    client.connect(PUBLIC_TLS_ADDRESS, PUBLIC_TLS_ADDRESS_PORT, keepalive=60)

    # Set the callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    # Start the MQTT loop to receive messages
    client.loop_start()

    # Chạy vòng lặp sự kiện asyncio trong cùng thread với client.loop_start()
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(asyncio.gather(
        controlDevice('m1_van1', 'motor_pump', topic ,client),
        controlDevice('motor_pump', 'status_pump', topic1 ,client),
        check_pH_warning()
    ))

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



try:
    # Khởi tạo và chạy các threads
    thread1 = threading.Thread(target=mqtt_thread)
    thread1.start()

    time.sleep(30)
    print("30s later======================================")
    # Khởi tạo và chạy luồng Arduino Cloud sau khi chờ 10 giây

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
                print(previous_data)
                # keepGoing = False
                # previous_data = data.copy()
                print("Stopping Thread 2...")
                # time.sleep(1)
                print("Restarting program...")
                restart_program()
except Exception as e:
    restart_program()