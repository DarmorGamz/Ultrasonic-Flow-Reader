<!DOCTYPE html>
<html>
<head>
    <title>DCA</title>
    <link rel="stylesheet" type="text/css" href="Public/assets/css/navbar.css">
    <link rel="stylesheet" type="text/css" href="Public/assets/css/loginform.css">

    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css" crossorigin="anonymous">
    <link rel="icon" type="image/x-icon" href="Public/assets/img/favicon.ico">

    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script src="Public/assets/js/login.js"></script>
</head>
<body>
    <!-- Navbar -->
    <nav class="navbar">
        <!-- Logo -->
        <img src="Public/assets/img/DCA_logo.png" alt="Logo">
        <!-- Login button -->
        <div class="login-button">
            <i class="fas fa-lock-open"></i>
            <i class="fas fa-question-circle"></i>
        </div>
    </nav>

    <!-- Login form -->
    <div class="login-container">
        <div class="login-form">
            <form>
                <div class="form-group">
                    <input id="email-input" type="email" placeholder="Email">
                </div>
                <div class="form-group">
                    <input id="password-input" type="password" placeholder="Password">
                </div>
                <div class="form-group form-check">
                    <label class="form-check-label" for="rememberMe">Remember me</label>
                    <input type="checkbox" class="form-check-input" id="rememberMe">
                </div>
                <div class="form-group">
                    <button id="signin-button" type="submit">Sign in</button>
                </div>
                <div class="form-group">
                    <button type="button" class="signup-button" onclick="window.location.href='Public/src/resetpassword.php'">Reset password</button>
                    <button type="button" class="signup-button" onclick="window.location.href='Public/src/createaccount.php'">Create account</button>
                </div>
            </form>
        </div>
    </div>


    <!-- Footer -->
    <footer class="footer">
        <span>Copyright &copy; 2023 DCA</span>
    </footer>
</body>
</html>