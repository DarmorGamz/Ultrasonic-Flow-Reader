function isValidEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
}

$(document).ready(function() {
    $('#resetpassword-button').click(function(event) {
        event.preventDefault(); // Prevent the form from submitting normally
        var Email = $('#email-input').val();
        var OldPassword = $('#current-password-input').val();
        var Password = $('#new-password-input').val();
        var PasswordConfirm = $('#confirm-new-password-input').val();

        if (Email.trim() === '' || !isValidEmail(Email)) { alert('Email not valid.'); return; }

        if (OldPassword.trim() === '' || OldPassword.length < 1 || OldPassword.length > 25) { alert('Old Password not valid.'); return; }

        if (Password.trim() === '' || Password.length < 1 || Password.length > 25) { alert('New Password not valid.'); return; }
        if (PasswordConfirm.trim() === '' || PasswordConfirm.length < 1 || PasswordConfirm.length > 25) { alert('New Password Confirm not valid.'); return; }
        if (Password !== PasswordConfirm) { alert('Passwords do not match.'); return;}

        if ($('#email-input').val().trim() === ''  || $('#current-password-input').val().trim() === '' || $('#new-password-input').val().trim() === '' || $('#confirm-new-password-input').val().trim() === '') { alert('Please fill out all fields.'); return; }
        const data = {
            Cmd: "ResetPassword",
            email: Email,
            passwordOld: OldPassword,
            password: Password,
        };

        const params = new URLSearchParams();
        params.append('Cmd', data.Cmd);
        params.append('Email', data.email);
        params.append('PasswordOld', data.passwordOld);
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

                if (!response.Errors || response.Errors.length !== 0) { alert("Failed to Reset Password."); return; }

                alert('Password Reset');

                // Remove the password cookie
                document.cookie = 'password=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

                // Redirect to dashboard or do something else
                window.location.replace("/index2.php");
            },
            error: function(error) {
                // Handle the error
                console.error(error);
            }
        });
    });
});
