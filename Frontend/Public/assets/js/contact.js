// default 0
// Bug 1,
// Feedback 2,
// Support 3,
// Wait for the DOM to finish loading
// Function to set the modal height to match the contact form height
function setModalHeight() {
    const contactForm = document.querySelector('.contact-form');
    const modal = document.querySelector('.modal');
    const contactFormHeight = getComputedStyle(contactForm).height;

    modal.style.height = contactFormHeight;
}

document.addEventListener('DOMContentLoaded', function() {
    // Get the form and submit button
    const form = document.querySelector('.contact-form');
    const submitBtn = form.querySelector('button[type="submit"]');

    // Get the modal dialog box and close button
    const modal = document.querySelector('.modal');
    const closeBtn = modal.querySelector('.modal-close-btn');

    // Define the aContact object with default values
    const aContact = {
        Cmd: 'Contact.Add',
        FirstName: '',
        LastName: '',
        Email: '',
        SupportReason: 0,
        Message: ''
    };

    // Define the form validation function
    function validateForm() {
        // Get the form fields
        const firstName = form.querySelector('#firstName');
        const lastName = form.querySelector('#lastName');
        const email = form.querySelector('#email');
        const messageType = form.querySelector('#messageType');
        const message = form.querySelector('#message');

        // Check if all fields are filled in
        if (
            firstName.value.trim() === '' ||
            lastName.value.trim() === '' ||
            email.value.trim() === '' ||
            messageType.value === 0 ||
            message.value.trim() === ''
        ) {
            alert('Please fill in all fields');
            return false;
        }

        // Update the aContact object with the form values
        aContact.FirstName = firstName.value.trim();
        aContact.LastName = lastName.value.trim();
        aContact.Email = email.value.trim();
        aContact.SupportReason = messageType.value;
        aContact.Message = message.value.trim();

        return true;
    }

    // Define the function to send the POST request
    function sendPostRequest() {
        const params = new URLSearchParams();
        params.append('Cmd', aContact.Cmd);
        params.append('FirstName', aContact.FirstName);
        params.append('LastName', aContact.LastName);
        params.append('Email', aContact.Email);
        params.append('SupportReason', aContact.SupportReason);
        params.append('Message', aContact.Message);

        // Set the URL of the server you want to send the POST request to
        const url = 'https://darmorgamz.ca/api.php?' + params.toString();

        fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
        })
            .then(response => {
                if (response.ok) {
                    // Show a popup message if the POST request is successful
                    modal.style.display = 'block';

                    // Reset the form
                    form.reset();
                } else {
                    throw new Error('Network response was not ok');
                }
            })
            .catch(error => console.error(error));
    }

    // Add event listener to the submit button
    submitBtn.addEventListener('click', function(event) {
        // Prevent the default form submission
        event.preventDefault();

        // Validate the form fields
        if (validateForm()) {
            // Call the sendPostRequest function with the aContact object
            sendPostRequest();
        }
    });

    closeBtn.addEventListener('click', function() {
        // Hide the modal dialog box
        modal.style.display = 'none';
    });
});
// Set the modal height when the page loads
window.addEventListener('DOMContentLoaded', setModalHeight);

// Update the modal height when the window is resized
window.addEventListener('resize', setModalHeight);