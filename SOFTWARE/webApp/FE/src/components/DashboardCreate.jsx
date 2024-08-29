// src/components/users/UserCreate.js

import {
  Create,
  SimpleForm,
  TextInput,
  NumberInput,
  SelectArrayInput,
} from "react-admin";
import React from "react";

export const DashboardCreate = (devices) => {
  return (
    <Create title="Create Dashboard">
      <SimpleForm>
        {/* <TextInput source="id" /> */}
        <TextInput source="name" />
        <SelectArrayInput
          source="devices"
          choices= {devices.map((device) => ({
            id: device.id,
            name: device.name,
          }))}
        />
      </SimpleForm>
    </Create>
  );
};
