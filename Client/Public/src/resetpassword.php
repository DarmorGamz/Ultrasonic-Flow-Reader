<!DOCTYPE html>
<html>
<head>
    <title>DCA</title>
    <link rel="stylesheet" type="text/css" href="../assets/css/navbar.css">
    <link rel="stylesheet" type="text/css" href="../assets/css/resetpassword.css">
    <link rel="icon" type="image/x-icon" href="../assets/img/favicon.ico">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css" crossorigin="anonymous">

    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
</head>
<body>
<!-- Navbar -->
<nav class="navbar">
    <!-- Logo -->
    <img src="../assets/img/DCA_logo.png" alt="Logo">
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
                <label id="form-label">Forgot your password...</label>
            </div>
            <div class="form-group">
                <input id="email-input" type="email" placeholder="Email">
            </div>
            <div class="form-group">
                <button id="resetpassword-button" type="submit">Reset Password</button>
            </div>
            <div class="form-group">
                <button type="button" class="signup-button" onclick="window.location.href='/'">Back to Login</button>
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