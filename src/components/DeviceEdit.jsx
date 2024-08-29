// src/components/users/UserCreate.js

import { Edit, SimpleForm, TextInput, NumberInput } from "react-admin";
import React from "react";

export const DeviceEdit = () => {
  return (
    <Edit title="Edit Device">
      <SimpleForm>
        <TextInput source="id" disabled/>
        <TextInput source="name" />
      </SimpleForm>
    </Edit>
  );
};