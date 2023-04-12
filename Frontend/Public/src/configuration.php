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

    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script src="../assets/js/sessionkeycheck.js"></script>
    <script src="../assets/js/data.js"></script>
    <script src="../assets/js/configuration.js"></script>

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
    <h1>Consumption</h1>
    <div class="wrapper">
        <div class="row">
            <div class="item">
                Current Speed <br>
                <div id="current"></div>
            </div>
            <div class="item">
                Water Consumption 1min <br>
                <div id="min"></div>
            </div>
        </div>
        <div class="row">
            <div class="item">
                Water Consumption 1hr <br>
                <div id="hr"></div>
            </div>
            <div class="item">
                Water Consumption 1d <br>
                <div id="day"></div>
            </div>
        </div>
    </div>
</div>

<script>
    var second = 1000; //defines virtual length of 1 second (in ms)
    var current;
    var min = new Array(60);

    for(i = 0; i < min.length; i++){
        min[i] = Math.floor(Math.random() * (200 - 100) + 100);
    }
    
    var hr = new Array(60).fill(0);
    var day = new Array(24).fill(0);
    var minIntervalID = window.setInterval(minCallback, second);
    var hrIntervalID = window.setInterval(hrCallback, 60*second);
    var dayIntervalID = window.setInterval(dayCallback, 24*60*second);
    
    function minCallback() {
        current = Math.floor(Math.random() * (200 - 100) + 100);
        currentFixed = current.toFixed(2);
        document.getElementById('current').innerHTML = currentFixed;
        min.push(current);

        let avgMin = calculateAverage(min)
        avgMinFixed = avgMin.toFixed(2); 
        document.getElementById('min').innerHTML = avgMinFixed;
        let i = min.shift();
        i = 0;
        
    }
    function hrCallback() {
        hr.push(calculateAverage(min));
        let avgHr = calculateAverage(hr)
        avgHrFixed = avgHr.toFixed(2);
        document.getElementById('hr').innerHTML = avgHrFixed;
    }
    function dayCallback() {
        day.push(calculateAverage(hr));
        let avgDay = calculateAverage(day)
        avgDayFixed = avgDay.toFixed(2);
        document.getElementById('day').innerHTML = avgDayFixed;
    }

    function calculateAverage(array) {
        var sum = 0;
        for (var number of array) {
            sum += number;
        }
        return sum / array.length;
    }
    
</script>


<!-- Footer -->
<footer class="footer">
    <span>&copy; 2023 DCA</span>
</footer>
</body>
</html>
