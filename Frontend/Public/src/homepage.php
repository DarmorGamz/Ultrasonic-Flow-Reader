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
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" crossorigin="anonymous">
    <link rel="icon" type="image/x-icon" href="../assets/img/favicon.ico">

    <!-- Highcharts   -->
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <script src="https://code.highcharts.com/modules/data.js"></script>
    <script src="https://code.highcharts.com/modules/exporting.js"></script>
    <script src="https://code.highcharts.com/modules/export-data.js"></script>

    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script src="../assets/js/sessionkeycheck.js"></script>
    <script src="../assets/js/youtube.js"></script>

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
    <nav class="navbar sticky">
        <!-- Hamburger menu icon -->
        <button id="menu-toggle" class="menu-toggle">
            <i class="fas fa-bars"></i>
        </button>
        <!-- Logo -->
        <div class="logo-container">
            <img src="../assets/img/logo.png" alt="Logo">
        </div>
        <!-- Login button -->
        <div class="login-button">
            <span id="UserEmail"></span>
            <span class="tooltip" data-tooltip="Logout">
                <button id="login-button"><i class="fas fa-lock"></i></button>
                <div class="tooltip-text">Logout</div>
            </span>
            <span class="tooltip" data-tooltip="Support">
                <button id="Support-button"><i class="fas fa-question-circle"></i></button>
                <div class="tooltip-text">Support</div>
            </span>
        </div>
    </nav>

    <!-- Menu -->
    <div id="menu-container" class="menu-container">
        <ul class="menu">
            <a href="homepage.php#Demand"><li><i class="fas fa-faucet-drip  menuicons"></i>Demand</li></a>
            <a href="consumption.php#Consumption"><li><i class="fas fa-glass-water menuicons"></i>Consumption</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a id='rickroll' href="#rickroll"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="configuration.php#Configuration"><li><i class="fas fa-cog menuicons"></i>Configuration</li></a>
            <a href="about.php#About"><li><i class="fas fa-graduation-cap menuicons"></i>About</li></a>
            <a href="support.php#Support"><li><i class="fas fa-question-circle menuicons"></i>Support</li></a>
        </ul>
    </div>

    <!-- Content -->
    <div id="content" class="content">
        <h1>Demand - Flow</h1>

    </div>

    <!-- Footer -->
    <footer class="footer">
        <span>&copy; 2023 DCA</span>
    </footer>
    <div id="player"></div>
</body>
</html>
