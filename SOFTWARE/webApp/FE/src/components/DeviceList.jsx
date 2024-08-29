import {
  List,
  Datagrid,
  TextField,
  EmailField,
  DeleteButton,
  EditButton
} from "react-admin";
import React from "react";

export const DeviceList = () => {
  return (
    <List>
      <Datagrid rowClick="edit">
        <TextField source="id" />
        <TextField source="name" />
        <EditButton/>
      </Datagrid>
    </List>
  );
};
