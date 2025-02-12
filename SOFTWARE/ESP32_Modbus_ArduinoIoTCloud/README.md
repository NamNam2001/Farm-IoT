<div align="left">
# System Overview
</div>

The system monitors **pH, water temperature, and dissolved oxygen (DO)** using the **Leadtec-PD6000** device, as well as **air temperature (ES35 sensor), light intensity (ES-ALS-01 sensor), and rainfall (ES-RAINF-01 sensor)**.  
Data is collected via the **Modbus protocol** and sent to **Arduino Cloud** for remote management.

## Features

- Configure WiFi through a local web server
- Select a pre-registered **Thing** on Arduino Cloud
- OTA firmware updates via a Git server
- Save and delete configuration settings in EEPROM
