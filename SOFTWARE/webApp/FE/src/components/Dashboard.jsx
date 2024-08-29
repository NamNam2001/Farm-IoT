import React, { useEffect, useState } from 'react';
import ChartComponent from './ChartComponent';
import {Card} from "@mui/material"
import "./styles.css"
import { Title } from 'react-admin';
import axios from 'axios';


const Dashboard = ({name, devices}) => {
  const [data, setData] = useState({});
  const [listTypesDevice, setListTypesDevice] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  let dataDashboard= {
    "dashboardId": name,
    "name": name,
    "data": []
  }
  

  useEffect(() => {
    const fetchData = async () => {
      try {
        const responses = await Promise.all(
          devices.map(deviceId =>
            axios.get(`https://farmiot-api.onrender.com/dashboards/${deviceId}/listTypes`)
          )
        );
  
        const updatedDataDashboard = { ...dataDashboard };
  
        responses.forEach(response => {
          const { deviceId, types } = response.data;
          types.forEach(type => {
            const deviceTypeId = `${deviceId}-${type}`;
            updatedDataDashboard.data.push({
              "device_id": deviceTypeId,
              "name": deviceTypeId,
              "data": [],
              "date": []
            });
          });
        });
  
        setListTypesDevice(updatedDataDashboard.data.map(item => item.device_id));
        setData(updatedDataDashboard);
      } catch (error) {
        setError(error);
      }
    };
  
    fetchData();
  }, [name, devices]);
  console.log(data)
  return (
    <div className="dashboard">
      {Object.keys(data).length > 0 && (
      <Card>
        <Title title = {name} />
        <div className="chart-container">
          {data.data.map((chartData) => (
            <ChartComponent
              key={chartData.device_id}
              dashboardId={data.name}
              id={chartData.device_id}
              name={chartData.name}
              data={chartData.data}
              date={chartData.date}
            />
          ))}
        </div>
        </Card>
      )}
    </div>
  );
};

export default Dashboard;