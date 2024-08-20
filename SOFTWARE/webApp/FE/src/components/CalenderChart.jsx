import React from 'react';
import DateRangePicker from 'rsuite/DateRangePicker';
import '../../node_modules/rsuite/DateRangePicker/styles/index.css';

export default function CalenderChart({setQuery}) {
  return (
    <div style={{ fontFamily: "sans-serif", textAlign: "center" }}>
   <DateRangePicker size="sm" onOk = {(e) => setQuery(e)}/>
    </div>
  )
}
