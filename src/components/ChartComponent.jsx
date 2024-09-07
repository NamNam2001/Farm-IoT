import React, { useRef,useEffect, useState } from "react";
import * as echarts from "echarts";
import "../../node_modules/rsuite/DateRangePicker/styles/index.css";
import DateRangePicker from "rsuite/DateRangePicker";
import "./styles.css";
import { startOfDay, endOfDay, addDays, subDays } from "date-fns";
import axios from "axios";

let data = [];
let date = [];
const option = {
  tooltip: {
    trigger: "axis",
    position: function (pt) {
      return [pt[0], "10%"];
    },
  },
  title: {
    left: "left",
    text: "",
  },
  toolbox: {
    feature: {
      dataZoom: {
        yAxisIndex: "none",
      },
      restore: {
        show: false,
      },
      saveAsImage: {
        show: false,
      },
    },
  },
  xAxis: {
    type: "category",
    boundaryGap: false,
    data: date,
  },
  yAxis: {
    type: "value",
    boundaryGap: [0, "100%"],
  },
  dataZoom: {
    type: "slider",
    start: 50,
    end: 100,
  },
  series: [
    {
      name: "Fake Data",
      type: "line",
      symbol: "circle",
      sampling: "lttb",
      itemStyle: {
        color: "rgb(33, 150, 243)",
      },
      areaStyle: {
        color: new echarts.graphic.LinearGradient(0, 0, 0, 0, [
          {
            offset: 0,
            color: "rgb(33, 150, 243)",
          },
          {
            offset: 1,
            color: "rgb(33, 150, 100)",
          },
        ]),
      },
      data: data,
    },
  ],
};

const handelDate = (date) => {
  let dates = [];
  for (let i = 1; i < date.length; i++) {
    let currentDate = new Date(date[i]);
    let previousDate = new Date(date[i - 1]);

    let currentDay = currentDate.getDate();
    let previousDay = previousDate.getDate();

    if (currentDay !== previousDay) {
      dates.push(date[i]); // Hiển thị toàn bộ ngày giờ của phần tử hiện tại nếu ngày khác với phần tử trước đó
    } else {
      dates.push(date[i].split(" ")[1]); // Chỉ hiển thị giờ của phần tử hiện tại nếu ngày giống với phần tử trước đó
    }
  }
  return dates;
};

const Ranges = [
  {
    label: "today",
    value: [startOfDay(new Date()), endOfDay(new Date())],
  },
  {
    label: "yesterday",
    value: [
      startOfDay(addDays(new Date(), -1)),
      endOfDay(addDays(new Date(), -1)),
    ],
  },
  {
    label: "last7Days",
    value: [startOfDay(subDays(new Date(), 6)), endOfDay(new Date())],
  },
  {
    label: "Last 30 Days",
    value: [startOfDay(subDays(new Date(), 30)), endOfDay(new Date())],
  },
];

const ChartComponent = ({ dashboardId, id, name, data, date }) => {
  const ip = "iot-farm-pysj.onrender.com";
  const URL = "https://"+ip;
  const [options, setOptions] = useState(option);
  const [messages, setMessages] = useState("");
  const ws = useRef(null);
  const [chartData, setChartData] = useState({
    device_id: id,
    name: id,
    data: data,
    date: date,
  });
  const Query = async (e) => {
    let rangeTimeQuery = [];
    e.map((item, index) => {
      let date = new Date(item);
      let day = date.getDate();
      let month = date.getMonth() + 1;
      let year = date.getFullYear();

      if (index > 0) {
        day = day + 1;
      }
      day = day.toString().padStart(2, "0");
      month = month.toString().padStart(2, "0");

      const formattedDate = `${year}-${month}-${day}`;
      rangeTimeQuery.push(formattedDate);
    });
    console.log(dashboardId);
    console.log(id);
    console.log(rangeTimeQuery);
    const response = await axios.get(`${URL}/data/${id}`, {
      params: {
        startDate: rangeTimeQuery[0],
        endDate: rangeTimeQuery[1],
      },
    });
    setChartData(response.data);
  };
  useEffect(() => {
    // Call Query with defaultValue when component mounts
    Query(Ranges[2].value);
  }, []);
  useEffect(() => {
    const chartDom = document.getElementById(chartData.device_id);
    const myChart = echarts.init(chartDom);
    const newOptions = { ...options };
    newOptions.series[0].data = chartData.data;
    newOptions.dataZoom.start =
      100 - Math.round(100 / Math.round(chartData.data.length / 200));
    newOptions.title.text = chartData.name;
    newOptions.xAxis.data = handelDate(chartData.date);
    console.log(name);
    setOptions(newOptions);
    myChart.setOption(options);
  }, [chartData]);

  useEffect(() => {
    ws.current = new WebSocket(`wss://${ip}/socket/${id}`);

    ws.current.onmessage = (event) => {
      console.log(event.data);
      setMessages([event.data]);
    };

    return () => {
      ws.current.close();
    };
  }, []);

  return (
    <div className="container-chart">
      <DateRangePicker
        showOneCalendar
        // format="MM/dd/yyyy HH:mm"
        size="xs"
        placeholder="Select date"
        onChange={(e) => Query(e)}
        ranges={Ranges}
        defaultValue={Ranges[2].value}
      />
      <div id={id} style={{ width: "99%", height: "280%" }} />
      <div className="value">Value: {messages}</div>
      {chartData.data.length === 0 && (
      <div
        style={{
          position: 'absolute',
          display: 'flex',
          justifyContent: 'center',
          alignItems: 'center',
          top: '40%',
          left: '40%',
          height: '20%',
          width: '20%',
          zIndex: 99
        }}
      >
        No data
      </div>
    )}
    </div>
  );
};

export default ChartComponent;
