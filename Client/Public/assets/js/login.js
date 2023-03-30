function isValidEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
}

function getCookie(name) {
    var value = "; " + document.cookie;
    var parts = value.split("; " + name + "=");
    if (parts.length == 2) return parts.pop().split(";").shift();
    return "";
}

function setCookie(name, value, minutes) {
    var expires = new Date();
    expires.setTime(expires.getTime() + (minutes * 60 * 1000));
    var timestamp = expires.getTime();
    document.cookie = name + "=" + value + "_" + timestamp + ";expires=" + expires.toUTCString() + ";path=/";
}

$(document).ready(function() {
    // check if the session key cookie exists
    var sessionKey = getCookie("sessionKey");
    if (sessionKey !== "") {
        // session key cookie exists, redirect to homepage
        window.location.replace("/Conestoga-Capstone/Client/Public/src/homepage.php");
        return;
    }

    var password = getCookie("password");
    var username = getCookie("username");
    if (password !== "" && username !== "") {
        $("#email-input").val(username);
        $("#password-input").val(password);
    }

    $('#signin-button').click(function() {
        event.preventDefault(); // prevent form submission
        var Email = $('#email-input').val();
        var Password = $('#password-input').val();

        // Admin login cause im lazy.
        if(Email === "God" && Password === "") {
            setCookie("sessionKey", "GodAdminPrivileges", 10);
            // Remove the username cookie
            document.cookie = 'username=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

            // Remove the password cookie
            document.cookie = 'password=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

            const cookieString = `username=GodAdmin; password=${Password}; expires=Fri, 31 Dec 9999 23:59:59 GMT;`;
            document.cookie = cookieString;

            // Redirect to dashboard or do something else
            window.location.replace("/Conestoga-Capstone/Client/Public/src/homepage.php");
        }

        // Check if email is valid
        if (!isValidEmail(Email)) {
            alert('Please enter a valid email address');
            return;
        }

        // Verify email is not empty
        if (Email === '') {
        alert('Please enter a valid email address');
            return;
        }

        // Verify password is not empty
        if (Password === '') {
            alert('Please enter your password');
            return;
        }

        const data = {
            Cmd: "Login",
            email: Email,
            password: Password,
        };

        const params = new URLSearchParams();
        params.append('Cmd', data.Cmd);
        params.append('Email', data.email);
        params.append('Password', data.password);

        const Url = 'https://darmorgamz.ca/api.php?' + params.toString();
        $.ajax({
            url: Url,
            type: 'POST',
            dataType: 'json',
            success: function(response) {
                console.log(response);

                // handle success response
                var sessionKey = response.UserInfo.SessionKey;

                // store session key cookie for 5 minutes
                setCookie("sessionKey", sessionKey, 10);

                // Remove the username cookie
                document.cookie = 'username=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

                // Remove the password cookie
                document.cookie = 'password=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

                // create a cookie that doesn't expire
                const cookieString = `username=${Email}; password=${Password}; expires=Fri, 31 Dec 9999 23:59:59 GMT;`;
                document.cookie = cookieString;

                // check if the remember me checkbox is checked
                if (document.getElementById('rememberMe').checked) {
                    const cookieString2 = `password=${Password}; expires=Fri, 31 Dec 9999 23:59:59 GMT;`;
                    document.cookie = cookieString2;
                }
                // Redirect to dashboard or do something else
                window.location.replace("/Conestoga-Capstone/Client/Public/src/homepage.php");
            },
            error: function(xhr, status, error) {
                console.log(error);
                // Handle error response
            }
        });
    });
});