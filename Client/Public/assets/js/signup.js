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
    $('#createaccount-button').click(function(event) {
        event.preventDefault(); // Prevent the form from submitting normally
    var Email = $('#email-input').val();
    var EmailConfirm = $('#email-input-confirm').val();
    var Password = $('#password-input').val();
    var PasswordConfirm = $('#password-input-confirm').val();

    if (Email.trim() === '' || !isValidEmail(Email)) { alert('Email not valid.'); return; }
    if (EmailConfirm.trim() === '' || !isValidEmail(EmailConfirm)) { alert('Confirm email not valid.'); return; }
    if (Email !== EmailConfirm) { alert('Emails do not match.'); return; }

    if (Password.trim() === '' || Password.length < 1 || Password.length > 25) { alert('Password not valid.'); return; }
    if (PasswordConfirm.trim() === '' || PasswordConfirm.length < 1 || PasswordConfirm.length > 25) { alert('Password Confirm not valid.'); return; }
    if (Password !== PasswordConfirm) { alert('Passwords do not match.'); return;}

    if ($('#email-input').val().trim() === '' || $('#email-input-confirm').val().trim() === '' || $('#password-input').val().trim() === '' || $('#password-input-confirm').val().trim() === '') { alert('Please fill out all fields.'); return; }
    const data = {
        Cmd: "Signup",
        email: Email,
        password: Password,
    };

    const params = new URLSearchParams();
    params.append('Cmd', data.Cmd);
    params.append('Email', data.email);
    params.append('Password', data.password);

    const Url = 'https://darmorgamz.ca/api.php?' + params.toString();
    console.log(Url);
    // Send the POST request
    $.ajax({
        url: Url,
        type: 'POST',
        success: function(response) {
            // Handle the response from the server
            console.log(response);

            if (!response.Errors || response.Errors.length !== 0) { alert("Failed to create user1."); return; }
            var aUserInfo = response.UserInfo;

            if(!aUserInfo.Id) { alert("Failed to create user."); return; }

            // handle success response
            var sessionKey = aUserInfo.SessionKey;

            // store session key cookie for 5 minutes
            setCookie("sessionKey", sessionKey, 10);

            // Redirect to dashboard or do something else
            window.location.replace("/Public/src/homepage.php");
        },
        error: function(error) {
            // Handle the error
            console.error(error);
        }
    });
    });
});
