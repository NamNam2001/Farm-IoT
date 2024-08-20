from fastapi import FastAPI, WebSocket, Request, Query
# import influxdb_client_3 as InfluxDBClient3
import pandas as pd
import numpy as np
from influxdb_client_3 import flight_client_options
import certifi
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
import matplotlib.pyplot as plt
import random
import asyncio
from pymongo import MongoClient
from bson.json_util import dumps
from datetime import datetime, timezone, timedelta
import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
import json
import uvicorn
import string


token = "3AHLeH9p7pbWiMTTykyPZpAXKSqJuYELavzt9qMR7KaKys1BqHcaP4JgGAOUWG8fVRLO8JYP6BrLHHX3ziqhbg=="
org = "HTR"
url = "http://localhost:8086"

client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
bucket="Farm_Iot"

write_api = client.write_api(write_options=SYNCHRONOUS)
query_api = client.query_api()

# # $env:INFLUXDB_TOKEN = "LHAC3K5CPhccJv4T63Glx7mxZi8jrptjWjpodbiFkPmbRza8QcSYeKai5MfaFm2vetBY-NDe_s0pNMd6yQVHwg=="
# token="LHAC3K5CPhccJv4T63Glx7mxZi8jrptjWjpodbiFkPmbRza8QcSYeKai5MfaFm2vetBY-NDe_s0pNMd6yQVHwg=="
# host="https://us-east-1-1.aws.cloud2.influxdata.com"
# org="Dev"
# database="data_monitoring"

# # connect MongoDB
# client = MongoClient("mongodb://localhost:27017/")
# db = client["sensors_db"]
# collection = db['test']

# data_to_insert = {
#     "device_id": "esp32_001",
#     "status": "on",
#     'time': datetime.now(),
# }

# # Thêm dữ liệu vào cơ sở dữ liệu
# result = collection.insert_one(data_to_insert)

# InfluxDB connect local 


#initialize the server
app = FastAPI()
currentValue = {}

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["GET", "POST", "PUT", "DELETE"],
    allow_headers=["*"],
    )

# # gRPC read root certificates (only Windown)
# fh = open(certifi.where(), "r")
# cert = fh.read()
# fh.close()
# client = InfluxDBClient3.InfluxDBClient3(host=host, token=token, org=org, database = database , flight_client_options=flight_client_options(
#         tls_root_certs=cert))

@app.get('/')
async def home():
    return "Farm_iot"

@app.get('/dashboards')
async def get_data():
    values_query = []
    query = """
    from(bucket: "Farm_Iot")
        |> range(start: -7d)
        |> filter(fn: (r) => r["_measurement"] == "dashboard_data")
        |> keep(columns: ["dashboard_id"])
        |> distinct(column: "dashboard_id")
        |> sort(columns: ["dashboard_id"])
    """
    tables = query_api.query(query, org="HTR")

    for table in tables:
        for record in table.records:
            # time_record = (record["_time"]+ timedelta(hours=7)).strftime("%Y-%m-%d %H:%M:%S")
            value_query = {
                # "deviceId": record["device_id"],
                # "day": time_record.split()[0],
                # "time": time_record.split()[1],
                # "value": record["_value"]
                "dashboardId": record["dashboard_id"],
            }
            values_query.append(value_query)
    return values_query

from typing import List

from pydantic import BaseModel
from fastapi import Path
class DeviceData(BaseModel):
    device_id: str
    name: str
    data: List[float]
    date: List[str]

class DashboardData(BaseModel):
    dashboardId: str
    name: str
    data: List[DeviceData]

@app.get('/dashboards/{dashboardId}')
async def get_data(dashboardId: str = Path(...)):
    dashboards_query = {
        "dashboardId": dashboardId,
        "name": dashboardId,
        "data": []
    }
    
    # query = f"""
    # from(bucket: "Farm_Iot")
    #     |> range(start: -7d, stop: now())
    #     |> filter(fn: (r) => r["_measurement"] == "dashboard_data")
    #     |> filter(fn: (r) => r["dashboard_id"] == "{dashboardId}")
    # """
    query = f"""
    import "influxdata/influxdb/v1"
    v1.tagValues(
        bucket: "Farm_Iot",
        tag: "device_id",
        predicate: (r) => r["_measurement"] == "dashboard_data" and r["dashboard_id"] == "{dashboardId}"
    )
    """
    tables = query_api.query(query, org="HTR")
    
    for table in tables:
        for record in table.records:
            device_id = record["_value"]
            print(device_id)
    #         time_record = (record["_time"] + timedelta(hours=7)).strftime("%Y-%m-%d %H:%M:%S")
    #         value = record["_value"]
            
    #         # Check if device_id already exists in data list
    #         device_exists = False
    #         for device_query in dashboards_query["data"]:
    #             if device_query["device_id"] == device_id:
    #                 device_exists = True
    #                 # device_query["data"].append(value)
    #                 # device_query["date"].append(time_record)
    #                 break
            
    #         # If device_id does not exist, create new entry
    #         if not device_exists:
            new_device_query = {
                "device_id": device_id,
                "name": device_id,
                "data": [],
                "date": []
            }
            dashboards_query["data"].append(new_device_query)
    # print(dashboards_query)
    return dashboards_query

def processEndDate(endData):
    year, month, day = map(int, endData.split('-'))
    try:
        endData = datetime(year=year, month=month, day=day)
    except ValueError:
            # Process endDate 
            if month == 12:
                next_month = 1
                next_year = year + 1
            else:
                next_month = month + 1
                next_year = year
            endData = datetime(
                year=next_year, month=next_month, day=1)
    return(endData.strftime('%Y-%m-%d'))
    
@app.get('/data/{dashboardId}/{id}')
async def get_dashboard_data(
    dashboardId: str,
    id: str,
    startDate: str ,
    endDate: str ):

    endDate= processEndDate(endDate)
    print(endDate)
    # Do something with dashboardId, id, startDate, endDate
    device_query = {
        "device_id": id,
        "name": id,
        "data": [],
        "date": [""]
    }
    
    
    query = f"""
    from(bucket: "Farm_Iot")
        |> range(start: {startDate}, stop: {endDate})
        |> filter(fn: (r) => r["_measurement"] == "dashboard_data")
        |> filter(fn: (r) => r["dashboard_id"] == "{dashboardId}")
        |> filter(fn: (r) => r["device_id"] == "{id}")
    """
    tables = query_api.query(query, org="HTR")
    
    for table in tables:
        for record in table.records:
            time_record = (record["_time"] + timedelta(hours=7)).strftime("%Y-%m-%d %H:%M:%S")
            value = record["_value"]
            
            device_query["data"].append(value)
            device_query["date"].append(time_record)
    print(device_query)
    return device_query
            
    #         # If device_id does not exist, create new entry
    #         if not device_exists:
    #             new_device_query = {  
    #                 "device_id": device_id,
    #                 "name": device_id,
    #                 "data": [value],
    #                 "date": ["", time_record]
    #             }
    #             dashboards_query["data"].append(new_device_query)
    
    # return dashboards_query



@app.post('/post-data/{dashboardId}/{id}')
async def add_sensor(dashboardId: str, id: str, request: Request):
    _json = await request.json()
    global currentValue 
    data = _json["value"]
    print("Data received")
    print(_json)

    random_string = ''.join(random.choices(string.ascii_letters + string.digits, k=10))
    
    #wiring data to influxdb
    point = (Point("dashboard_data") 
        .tag("dashboard_id", dashboardId) 
        .tag("device_id", id) 
        .field("value", data))
    # point = (
    #     Point("measurement1")
    #     .tag("tagname1", "tagvalue1")
    #     .field("field1", data)
    # )
    write_api.write(bucket=bucket, org="HTR", record=point)

    #wiring data to current Value
    if dashboardId not in currentValue:
        currentValue[dashboardId] = {}

    if id not in currentValue[dashboardId]:
        currentValue[dashboardId][id] = [data, random_string]
    else:
        currentValue[dashboardId][id] = [data, random_string]

    print(currentValue)

    # print("Data sent successfully")
   
#     # return {"message": "Data received successfully"}


@app.websocket("/socket/{dashboardId}/{id}")
async def websocket_endpoint(dashboardId: str, id: str, websocket: WebSocket):
    print('a new websocket to create.')
    await websocket.accept()
    # current_value = None
    random_string = ""
    last_update_time = None
    while True:
        try:
            # Wait for any message from the client
            # await websocket.receive_text()
            # Send message to the client
            # resp = {'value': random.choice([0, 1])}
            
            # if current_value != data["status"]:
            #     current_value = data["status"]
            # data_test = random.randint(101,1000)/100
            if dashboardId in currentValue and id in currentValue[dashboardId]:
                if random_string != currentValue[dashboardId][id][1]:
                    random_string = currentValue[dashboardId][id][1]
                    last_update_time = datetime.now()
                    await websocket.send_json(currentValue[dashboardId][id][0])
                else:
                    if datetime.now() - last_update_time > timedelta(minutes=1):
                        del currentValue[dashboardId][id]
            else:
                await websocket.send_json(None)
            await asyncio.sleep(1) 
            # else:
            #     continue
        except Exception as e:
            print('error:', e)
            break
    print('Bye..')

if __name__ == "__main__":
    uvicorn.run("test:app", host="0.0.0.0", port=8000, log_level="info",reload=True)
