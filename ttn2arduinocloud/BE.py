import os, time
from influxdb_client_3 import InfluxDBClient3, Point
import pandas

# $env:INFLUXDB_TOKEN = "LHAC3K5CPhccJv4T63Glx7mxZi8jrptjWjpodbiFkPmbRza8QcSYeKai5MfaFm2vetBY-NDe_s0pNMd6yQVHwg=="
token = "LHAC3K5CPhccJv4T63Glx7mxZi8jrptjWjpodbiFkPmbRza8QcSYeKai5MfaFm2vetBY-NDe_s0pNMd6yQVHwg=="
org = "Dev"
host = "https://us-east-1-1.aws.cloud2.influxdata.com"

client = InfluxDBClient3(host=host, token=token, org=org)

query = """SELECT *
FROM 'census'
WHERE time >= now() - interval '24 hours'
AND ('bees' IS NOT NULL OR 'ants' IS NOT NULL)"""
print(1+1)
# Execute the query
table = client.query(query, "data_monitoring", 'sql')

# Convert to dataframe
# df = table.to_pandas().sort_values(by="time")


