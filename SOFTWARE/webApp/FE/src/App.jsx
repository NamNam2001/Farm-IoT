// import React from "react";
// import { Resource, Admin, ListGuesser, Layout, Menu, defaultTheme, CustomRoutes, Title } from 'react-admin';
// import { Route } from 'react-router-dom';
// import simpleRestProvider from "ra-data-simple-rest"
// // import authProvider from "./authProvider";
// import Dashboard1 from './Dashboard1';
// import Dashboard2 from './Dashboard2';
// import CustomDashboard from './CustomDashboard'; // Import CustomDashboard

// // import {listProducts} from "./components/Products"
// import Dashboard from "./components/Dashboard";
// import UsersList from "./components/UsersList";

// const lightTheme = defaultTheme;
// const darkTheme = { ...defaultTheme, palette: { mode: 'dark' } };

// export default () => {
//   const URL = "http://192.168.1.54:3001"
//   return (
//     <div className="App">
//       {/* <Admin  dashboard={Dashboard} dataProvider={simpleRestProvider(URL)} theme={lightTheme} darkTheme={darkTheme}>
//         <Resource name = 'dashboard' list/>
//         <Resource name = 'Hello' list/>
//         <Resource name = 'users' list={UsersList}/>
//       </Admin> */}
//       <Admin dataProvider={simpleRestProvider(URL)} dashboard={CustomDashboard}>
//         <Resource name='users' list={ListGuesser} />
//         <CustomRoutes>
//           {/* <Route path="/dashboard1" element={<Dashboard name = "Dashboard1"/>} />
//           <Route path="/dashboard2" element={<Dashboard name = "Dashboard2"/>}/> */}
//           {dashboardRoutes.map((dashboard) => (
//             <Route
//               key={dashboard.name} // Sử dụng key để xác định mỗi Route
//               path={`/dashboard/${dashboard.name}`}
//               element={<Dashboard name={dashboard.name} data={dashboard.data} />}
//             />
//           ))}

//         </CustomRoutes>
//       </Admin>
//     </div>
//   )
// }


import React, {useState, useEffect} from "react";
import { Resource, Admin, ListGuesser, Layout, Menu, defaultTheme, CustomRoutes, Title } from 'react-admin';
import { Route } from 'react-router-dom';
import simpleRestProvider from "ra-data-simple-rest"
// import authProvider from "./authProvider";
import Dashboard1 from './Dashboard1';
import Dashboard2 from './Dashboard2';
import CustomDashboard from './CustomDashboard'; // Import CustomDashboard

// import {listProducts} from "./components/Products"
import Dashboard from "./components/Dashboard";
import UsersList from "./components/UsersList";

const lightTheme = defaultTheme;
const darkTheme = { ...defaultTheme, palette: { mode: 'dark' } };

const App = () => {
  const URL = "http://192.168.1.60:8000";
  const [dashboardRoutes, setDashboardRoutes] = useState([]);

  useEffect(() => {
    const fetchDashboardData = async () => {
      try {
        const response = await fetch(`${URL}/dashboards`);
        const data = await response.json();
        console.log(data)
        const formattedDashboardRoutes = data.map(dashboard => ({
          id: dashboard.dashboardId,
          name: dashboard.dashboardId,
        }));
        
        setDashboardRoutes(formattedDashboardRoutes); 
        // console.log(dashboardRoutes)
      } catch (error) {
        console.error('Error fetching dashboard data:', error);
      }
    };

    fetchDashboardData();
  }, []);

  console.log(dashboardRoutes)

  return (
    // {dashboardRoutes.length > 0 && (
    <div className="App">
      {dashboardRoutes.length > 0 && (
        <Admin dataProvider={simpleRestProvider(URL)} dashboard={() => <CustomDashboard dashboardRoutes={dashboardRoutes} />}>
          <Resource name="users" list={ListGuesser} />
          <Resource name="devices" list={ListGuesser} />
          <CustomRoutes>
            {dashboardRoutes.map((dashboard) => (
              <Route
                key={dashboard.id}
                path={`/dashboards/${dashboard.name}`}
                element={<Dashboard name={dashboard.name} />}
              />
            ))}
          </CustomRoutes>
        </Admin>
      )}
    </div>
  );
}

export default App;


