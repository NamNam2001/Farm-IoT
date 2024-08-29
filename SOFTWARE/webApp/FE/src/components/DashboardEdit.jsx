import {
    Edit,
    SimpleForm,
    TextInput,
    NumberInput,
    SelectArrayInput,
  } from "react-admin";
  import React from "react";
  
  export const DashboardEdit = (devices) => {
    return (
      console.log(devices),
      <Edit title="Edit Dashboard">
        <SimpleForm>
          <TextInput source="id" disabled/>
          <TextInput source="name" />
          <SelectArrayInput
            source="devices"
            choices= {devices.map((device) => ({
              id: device.id,
              name: device.name,
            }))}
          />
        </SimpleForm>
      </Edit>
    );
  };
  