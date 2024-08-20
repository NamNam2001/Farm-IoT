import { Gauge } from '@mui/x-charts/Gauge';

import React from 'react';

const GaugeChart = () => {
    return (
        <div>
            <Gauge width={100} height={100} value={50} valueMin={0} valueMax={100} startAngle={-110} endAngle={110} style = {{color: '#52b202'}}/>
        </div>
    );
};

export default GaugeChart;
