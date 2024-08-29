// src/components/users/UserCreate.js

import { Create, SimpleForm, TextInput, NumberInput } from "react-admin";
import React from "react";

export const DeviceCreate = () => {
  return (
    <Create title="Create Device">
      <SimpleForm>
        {/* <TextInput source="id" /> */}
        <TextInput source="name" />
      </SimpleForm>
    </Create>
  );
};