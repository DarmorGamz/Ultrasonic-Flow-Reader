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
    <link rel="stylesheet" type="text/css" href="../assets/css/about.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" crossorigin="anonymous">
    <link rel="icon" type="image/x-icon" href="../assets/img/favicon.ico">

    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script src="../assets/js/sessionkeycheck.js"></script>
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
            <a href="homepage.php#Demand"><li><i class="fas fa-faucet-drip menuicons"></i>Demand</li></a>
            <a href="consumption.php#Consumption"><li><i class="fas fa-glass-water menuicons"></i>Consumption</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="#"><li><i class="fas fa-bug menuicons"></i>N/A</li></a>
            <a href="configuration.php#Configuration"><li><i class="fas fa-cog menuicons"></i>Configuration</li></a>
            <a href="about.php#About"><li><i class="fas fa-graduation-cap menuicons"></i>About</li></a>
            <a href="support.php#Support"><li><i class="fas fa-question-circle menuicons"></i>Support</li></a>
        </ul>
    </div>

    <!-- Content -->
    <div id="content" class="content">
        <h1>About</h1>
        <p class="section-text">
            IoT Ultrasonic Flow Monitoring is a cutting-edge solution designed to collect real-time water flow data in pipes using ultrasonic waves. Harnessing the power of the SAMD20 microcontroller, our non-invasive IoT device measures water usage and sends accurate readings to our secure AWS server, where the data is stored in an RDS database for further analysis and monitoring.
            Eliminating the need for costly and time-consuming inline installations, the IoT Ultrasonic Flow Monitoring system empowers homeowners, residential and industrial building owners, and property managers to track water usage efficiently, promoting water conservation and potential cost savings.
        </p>
        <h3 class="section-title">Key Features</h3>
        <ul class="section-text">
            <li><span class="highlight">Real-time data collection:</span> Accurate velocity, and volume readings are collected by the IoT device, giving users up-to-date information on their local usage.</li>
            <li><span class="highlight">AWS Integration:</span> Our robust AWS infrastructure ensures data reliability and security, with servers hosted on AWS EC2 and data storage on RDS databases.</li>
            <li><span class="highlight">Responsive API:</span> Our PHP backend provides an intuitive and efficient API for frontend users, ensuring a seamless experience.</li>
        </ul>

        <!-- Technologies Used -->
        <h2 class="section-title">Technologies Used</h2>
        <div class="section-text tech-items-container">
            <div class="tech-item">
                <img src="../assets/img/microchip.svg" alt="Stm32" class="tech-logo">
                <div class="tech-label">Atmel</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/c.svg" alt="C" class="tech-logo">
                <div class="tech-label">C</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/html5.svg" alt="HTML5" class="tech-logo">
                <div class="tech-label">HTML5</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/css3.svg" alt="CSS3" class="tech-logo">
                <div class="tech-label">CSS3</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/javascript.svg" alt="JavaScript" class="tech-logo">
                <div class="tech-label">JavaScript</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/php.svg" alt="PHP" class="tech-logo">
                <div class="tech-label">PHP</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/mysql.svg" alt="MySQL" class="tech-logo">
                <div class="tech-label">MySQL</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/amazonaws.svg" alt="AWS" class="tech-logo">
                <div class="tech-label">AWS</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/amazonec2.svg" alt="EC2" class="tech-logo">
                <div class="tech-label">Amazon EC2</div>
            </div>
            <div class="tech-item">
                <img src="../assets/img/amazonrds.svg" alt="RDS" class="tech-logo">
                <div class="tech-label">Amazon RDS</div>
            </div>
        </div>

        <h3 class="section-title">Get Started</h3>
        <p class="section-text">
            To start using IoT Water Usage Monitoring and experience the benefits of real-time environmental data, simply register for an account and follow the setup instructions. Our user-friendly interface makes it easy for you to access, analyze, and compare your collected data.
        </p>
        <p class="section-text">
            If you have any questions or need assistance, please don't hesitate to <a href="support.php" class="highlight">contact us</a>. Our team is always happy to help.
        </p>
    </div>

    <!-- Footer -->
    <footer class="footer">
        <span>&copy; 2023 DCA</span>
    </footer>
</body>
</html>
