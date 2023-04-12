// Define the data to be sent in the POST request
const dataDemand = {
    Cmd: "getdata"
};

const params = new URLSearchParams();
params.append('Cmd', dataDemand.Cmd);

// Set the URL of the server you want to send the POST request to
const url = 'https://darmorgamz.ca/api.php?' + params.toString();

// Define the function to send the POST request
function sendPostRequest() {
    fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
    })
        .then(response => response.json())
        .then(json => {
            const data = json.Data;
            document.getElementById('volume1min').textContent = data.Min1;
            document.getElementById('volume15min').textContent = data.Min15;
            document.getElementById('volume1hr').textContent = data.Hour;

        })
        .catch(error => console.error(error));
}

// Send the POST request every minute using setInterval()
sendPostRequest();
const interval = setInterval(sendPostRequest, 1000); // 60000 ms = 1 minute

// Override the interval and send a POST request immediately on button click
const button = document.querySelector('button');
button.addEventListener('click', () => {
    sendPostRequest();
    clearInterval(interval);
});