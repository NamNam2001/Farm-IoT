// CustomDashboard.js
import React from 'react';
import { Card, CardContent, Typography } from '@mui/material';
import { Link } from 'react-router-dom';
import { Title, List, Datagrid, TextField, useRecordContext, useRedirect, SimpleList, CreateButton, EditButton} from 'react-admin';


const CustomDashboard = ({ dashboardRoutes }) => {
    console.log(dashboardRoutes)
    return (
        <Card>
            {/* <Title title = "Dashboard"/> */}
            <CardContent>
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <Typography variant="h5">Dashboard List</Typography>
                <CreateButton label="Create Dashboard"  resource="dashboards"></CreateButton>
                </div>
                <div >
                    {dashboardRoutes.map((dashboard) => (
                        <div key={dashboard.id} style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                        <Typography variant="h6">
                            <Link to={`/dashboards/${dashboard.id}/show`}>
                            {dashboard.name}
                            </Link>
                        </Typography>
                        <EditButton resource="dashboards" record={dashboard} />
                        </div>
                    ))}
                </div>
            </CardContent>
        </Card>
    );
};

export default CustomDashboard;
