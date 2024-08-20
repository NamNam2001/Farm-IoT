// CustomDashboard.js
import React from 'react';
import { Card, CardContent, Typography } from '@mui/material';
import { Button } from '@mui/base/Button';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import { Link } from 'react-router-dom';
import { Title, List, Datagrid, TextField, useRecordContext, useRedirect, SimpleList } from 'react-admin';

const DashboardLink = () => {
    const record = useRecordContext();
    const redirect = useRedirect();

    const handleClick = () => {
        redirect(`/${record.name}`);
    };

    return (
        <ArrowForwardIcon onClick={handleClick} />
    );
};

const LinkField = ({ source }) => {
    const record = useRecordContext();
    if (!record) return null;
    return (
        <Link to={`/${record[source]}`}>
            {record[source]}
        </Link>
    );
};


const CustomDashboard = ({ dashboardRoutes }) => {
    console.log(dashboardRoutes)
    return (
        <Card>
            {/* <Title title = "Dashboard"/> */}
            <CardContent>
                <Typography variant="h5">Dashboard List</Typography>
                {dashboardRoutes.map((dashboard) => (
                    <Typography key={dashboard.id} variant="h6">
                        <Link to={`/dashboards/${dashboard.name}`}>
                            {dashboard.name}
                        </Link>
                    </Typography>
                ))}

                {/* <Datagrid>
                        <TextField source="id" />
                    <TextField source="name"/>
                        <LinkField source="name" >
                            </LinkField> 
                        <DashboardLink/>
                
                        <Link to="/dashboard1" />
                        
                    </Datagrid> */}

            </CardContent>
        </Card>
    );
};

export default CustomDashboard;
