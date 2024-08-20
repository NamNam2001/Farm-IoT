import React from "react";
import { Layout } from 'react-admin';
import CustomMenu from './CustomMenu'; // Menu tùy chỉnh bạn đã tạo

const CustomLayout = (props) => <Layout {...props} menu={CustomMenu} />;

export default CustomLayout;
