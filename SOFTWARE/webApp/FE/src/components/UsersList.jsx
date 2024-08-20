import React from 'react';
import {List, Datagrid, TextField, DateField, EditButton, DeleteButton} from "react-admin"

const UsersList = (props) => {
    return (
        <div>
            <List {...props}>
                <Datagrid>
                    <TextField source = "id"/> 
                    <TextField source = "name"/> 
                    <TextField source = "pass"/> 
                    <EditButton/>
                    <DeleteButton/>
                </Datagrid>
            </List>
        </div>
    );
};

export default UsersList;
