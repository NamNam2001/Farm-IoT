import React, {useState, useEffect} from "react";
import { Resource, Admin, ListGuesser, Layout, Menu, defaultTheme, CustomRoutes, Title , EditGuesser, Create} from 'react-admin';
import { Route } from 'react-router-dom';
import simpleRestProvider from "ra-data-simple-rest"
import CustomDashboard from './CustomDashboard';
import Dashboard from "./components/Dashboard";
import { DeviceList } from "./components/DeviceList";
import { DeviceCreate } from "./components/DeviceCreate";
import { DashboardCreate } from "./components/DashboardCreate";
import { DashboardEdit } from "./components/DashboardEdit";
import { DeviceEdit } from "./components/DeviceEdit";
import axios from "axios";

const lightTheme = defaultTheme;
const darkTheme = { ...defaultTheme, palette: { mode: 'dark' } };

const App = () => {
  const URL = "https://farmiot-api.onrender.com";
  const [dashboardRoutes, setDashboardRoutes] = useState([]);
  const [devices, setDevices] = useState([]);

  useEffect(() => {
    const fetchDashboardData = async () => {
      try {
        const response = await axios.get(`${URL}/dashboards`);
        const data = await response.data;
        // console.log(data)
        const formattedDashboardRoutes = data.map(dashboard => ({
          id: dashboard.id,
          name: dashboard.name,
          devices: dashboard.devices
        }));
        
        setDashboardRoutes(formattedDashboardRoutes); 
        // console.log(dashboardRoutes)
      } catch (error) {
        console.error('Error fetching dashboard data:', error);
      }
    };

    fetchDashboardData();
  }, []);
useEffect(() => {
    const fetchDeviceData = async () => {
      try {
        const response = await axios.get(`${URL}/devices`);
        const data = await response.data;
         
        setDevices(data); 
        // console.log(dashboardRoutes)
      } catch (error) {
        console.error('Error fetching devices data:', error);
      }
    };

    fetchDeviceData();
  }, []);


  return (
    // {dashboardRoutes.length > 0 && (
    <div className="App">
      
        <Admin dataProvider={simpleRestProvider(URL)} dashboard={() => <CustomDashboard dashboardRoutes={dashboardRoutes} />}>
          <Resource name="devices" list={DeviceList(devices)} edit={DeviceEdit} create={DeviceCreate} />
          <Resource name="dashboards" create={DashboardCreate(devices)} edit={DashboardEdit(devices)}/>
          <CustomRoutes>
            {dashboardRoutes.map((dashboard) => (
              <Route
                key={dashboard.id}
                path={`/dashboards/${dashboard.id}/show`}
                element={<Dashboard name={dashboard.name} devices={dashboard.devices}/>}
              />
            ))}
          </CustomRoutes>
        </Admin>
    </div>
  );
}

export default App;


