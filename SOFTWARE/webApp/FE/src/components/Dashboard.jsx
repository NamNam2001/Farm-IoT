import React, { useEffect, useState } from 'react';
import ChartComponent from './ChartComponent';
import {Card} from "@mui/material"
import "./styles.css"
import { Title } from 'react-admin';
import axios from 'axios';


// let date = [
//   '',
//   '2024/05/05 10:33:20',
//   '2024/05/05 10:33:29',
//   '2024/05/05 10:34:39',
//   '2024/05/05 10:35:00',
//   '2024/05/06 10:33:20',
//   '2024/05/06 10:33:29',
//   '2024/05/06 10:34:39',
//   '2024/05/06 10:35:00',
//   '2024/05/07 10:33:20',
//   '2024/05/07 10:33:29',
//   '2024/05/07 10:34:39',
//   '2024/05/07 10:35:00',
//   '2024/05/08 10:33:20',
//   '2024/05/08 10:33:29',
//   '2024/05/08 10:34:39',
//   '2024/05/08 10:35:00',
//   '2024/05/09 10:33:29',
//   '2024/05/09 10:34:39',
//   '2024/05/09 10:35:00',
//   '2024/05/06 10:33:20',
//   '2024/05/10 10:33:29',
//   '2024/05/10 10:34:39',
//   '2024/05/10 10:35:00',
//   '2024/05/11 10:33:20',
//   '2024/05/11 10:33:29',
//   '2024/05/11 10:34:39',
//   '2024/05/12 10:35:00',
//   '2024/05/12 10:33:20',
//   '2024/05/13 10:33:29',
//   '2024/05/13 10:34:39',
//   '2024/05/13 10:35:00'
// ];
// let data = [2, 3, 4, 5,7,8,6,4,3,5,6,7,8,6,5,7, 2, 3, 4, 5,7,8,6,4,3,5,6,7,8, 6,7,8];
// let data1 = [5, 15, 40, 55,70,80,60,40,30,50,60,70,80,60,50,70, 20, 30, 4, 5,7,8,6,4,3,5,6,7,8, 6,7,8];

const Dashboard = ({name}) => {
  const URL = "http://192.168.1.60:8000";
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await axios.get(`${URL}/dashboards/${name}`);
        // console.log(response)
        setData(response.data);
      } catch (error) {
        setError(error.message);
      }
    };

    fetchData();
  }, [name]);
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