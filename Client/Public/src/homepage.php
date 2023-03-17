<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>DCA</title>

    <link rel="stylesheet" type="text/css" href="../assets/css/navbar.css">
    <link rel="stylesheet" type="text/css" href="../assets/css/hamburgermenu.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css" crossorigin="anonymous">
    <link rel="icon" type="image/x-icon" href="../assets/img/favicon.ico">

    <!-- Highcharts   -->
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <script src="https://code.highcharts.com/modules/data.js"></script>
    <script src="https://code.highcharts.com/modules/exporting.js"></script>
    <script src="https://code.highcharts.com/modules/export-data.js"></script>

    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script src="../assets/js/sessionkeycheck.js"></script>
    <script src="../assets/js/highcharts.js"></script>
    <script src="../assets/js/data.js"></script>

    <script>
        document.addEventListener("DOMContentLoaded", function() {
            // Get the menu toggle button and the menu container
            const menuToggle = document.getElementById('menu-toggle');
            const menuContainer = document.getElementById('menu-container');
            const contentContainer = document.getElementById('content');

            // Opens menu on default
            menuContainer.classList.add('active');
            contentContainer.classList.add('menu-open');

            // Add a click event listener to the menu toggle button
            menuToggle.addEventListener('click', function() {
                // Toggle the 'active' class on the menu container to open/close it
                menuContainer.classList.toggle('active');

                // Add/remove the 'menu-open' class to the content container to adjust the margin left
                contentContainer.classList.toggle('menu-open');
            });
        });
    </script>
</head>
<body>
    <!-- Navbar -->
    <nav class="navbar">
        <!-- Hamburger menu icon -->
        <button id="menu-toggle" class="menu-toggle">
            <i class="fas fa-bars"></i>
        </button>
        <!-- Logo -->
        <div class="logo-container">
            <img src="../assets/img/DCA_logo.png" alt="Logo">
        </div>
        <!-- Login button -->
        <div class="login-button">
            <span id="UserEmail"></span>
            <span class="tooltip" data-tooltip="Logout">
                <button id="login-button">
                <i class="fas fa-lock"></i>
            </button>
                <div class="tooltip-text">Logout</div>
            </span>
            <i class="fas fa-question-circle"></i>
        </div>
    </nav>

    <!-- Menu -->
    <div id="menu-container" class="menu-container">
        <ul class="menu">
            <li><i class="fas fa-lock menuicons"></i><a href="homepage.php#Demand">Demand</a></li>
            <li><i class="fas fa-cog menuicons"></i><a href="consumption.php#Consumption">Consumption</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-bug menuicons"></i><a href="#">N/A</a></li>
            <li><i class="fas fa-lock menuicons"></i><a href="configuration.php#Configuration">Configuration</a></li>
            <li><i class="fas fa-graduation-cap menuicons"></i><a href="about.php#About">About</a></li>
            <li><i class="fas fa-question-circle menuicons"></i><a href="support.php#Support">Support</a></li>
        </ul>
    </div>

    <!-- Content -->
    <div id="content" class="content">
        <h1>Demand - <span id="clock"></span></h1>
        <h4>1 Min: <span id="volume1min"></span></h4>
        <h4>1 Hr: <span id="volume1hr"></span></h4>
        <h4>Total: <span id="volumeTotal"></span></h4>
        <h4>Flow Speed: <span id="flowSpeed"></span></h4>
        <button onclick="sendPostRequest()">Send POST request now</button>
        <div id="chart-container"></div>
    </div>

    <script>
        // Set up the chart
        var chart = Highcharts.chart('chart-container', {
            chart: {
                type: 'line',
                events: {
                    load: function () {
                        // set up the updating of the chart every 5 seconds
                        var series = this.series[0];
                        setInterval(function () {
                            var x = (new Date()).getTime(), // current time
                                y = Math.random();
                            series.addPoint([x, y], true, true);
                        }, 5000);
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
                    color: '#808080'
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
    </script>

    <!-- Footer -->
    <footer class="footer">
        <span>&copy; 2023 DCA</span>
    </footer>
</body>
</html>