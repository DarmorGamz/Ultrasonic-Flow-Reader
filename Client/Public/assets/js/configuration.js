$(document).ready(function() {
    $('#delete-user-button').click(function(event) {
        event.preventDefault(); // Prevent the form from submitting normally
        var Email = document.getElementById('UserEmail').innerText;
        var Password = $('#password-input').val();

        // GodAdmin doesn't exist in DB.
        if(Email == "God" || Email == "GodAdmin") { return; }

        if (Password.trim() === '' || Password.length < 1 || Password.length > 25) { alert('New Password not valid.'); return; }

        const data = {
            Cmd: "User.Del",
            email: Email,
            password: Password,
        };

        const params = new URLSearchParams();
        params.append('Cmd', data.Cmd);
        params.append('Email', data.email);
        params.append('Password', data.password);

        const Url = 'https://darmorgamz.ca/api.php?' + params.toString();
        // Send the POST request
        $.ajax({
            url: Url,
            type: 'POST',
            success: function(response) {
                // Handle the response from the server
                console.log(response);

                if (!response.Errors || response.Errors.length !== 0) { alert("Failed to Delete User."); return; }

                alert('User Deleted');

                // Remove the username cookie
                document.cookie = 'username=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

                // Remove the password cookie
                document.cookie = 'password=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

                // Remove the username cookie
                document.cookie = 'sessionKey=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/';

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
