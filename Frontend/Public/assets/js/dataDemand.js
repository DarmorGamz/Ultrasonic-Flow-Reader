// Define the data to be sent in the POST request
const dataDemand = {
    Cmd: "getdata"
};

const params = new URLSearchParams();
params.append('Cmd', dataDemand.Cmd);

// Set the URL of the server you want to send the POST request to
const url = 'https://darmorgamz.ca/api.php?' + params.toString();

//Define js object
var demand = {
    min1: 0,
    min15: 0,
    hour: 0
};

// Define the function to send the POST request
function sendPostRequest() {
    fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
    })
        .then(response => response.json())
        .then(json => {
            const data = json.Data;
            document.getElementById('volume1min').textContent = data.Min1;
            demand.min1 = data.Min1;
            document.getElementById('volume15min').textContent = data.Min15;
            demand.min15 = data.Min15;
            document.getElementById('volume1hr').textContent = data.Hour;
            demand.hour = data.Hour;
        })
        .catch(error => console.error(error));
}

// Send the POST request every minute using setInterval()
sendPostRequest();
const interval = setInterval(sendPostRequest, 1000); // 60000 ms = 1 minute

// Override the interval and send a POST request immediately on button click
const button = document.querySelector('button');
button.addEventListener('click', () => {
    sendPostRequest();
    clearInterval(interval);
});

function highchartSetup() {
    // Set up the chart
    var chart = Highcharts.chart('chart-container', {
        chart: {
            type: 'line',
            events: {
                load: function () {
                    // set up the updating of the chart every 1 second
                    var series = this.series[0];
                    setInterval(
                        function () {
                            var x = (new Date()).getTime(), // current time
                                y = demand.min1;
                            series.addPoint([x, y], true, true);
                        }, 1000);
                }
            }
        },
        title: {
            text: 'Live random data'
        },
        xAxis: {
            type: 'datetime',
            tickPixelInterval: 150
        },
        yAxis: {
            title: {
                text: 'Value'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#663399'
            }]
        },
        tooltip: {
            formatter: function () {
                return '<b>' + this.series.name + '</b><br/>' +
                    Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '<br/>' +
                    Highcharts.numberFormat(this.y, 2);
            }
        },
        legend: {
            enabled: false
        },
        exporting: {
            enabled: false
        },
        series: [{
            name: 'Random data',
            data: (function () {
                // generate an array of random data
                var data = [],
                    time = (new Date()).getTime(),
                    i;

                for (i = -19; i <= 0; i += 1) {
                    data.push({
                        x: time + i * 1000,
                        y: Math.random()
                    });
                }
                return data;
            }())
        }]
    });
}