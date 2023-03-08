// Define the data to be sent in the POST request
const data = {
    Cmd: "getdata"
};

const params = new URLSearchParams();
params.append('Cmd', data.Cmd);

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
            document.getElementById('volume1min').textContent = data.Volume1Min;
            document.getElementById('volume1hr').textContent = data.Volume1Hr;
            document.getElementById('volumeTotal').textContent = data.VolumeTotal;
            document.getElementById('flowSpeed').textContent = data.FlowSpeed;
        })
        .catch(error => console.error(error));
}

// Send the POST request every minute using setInterval()
sendPostRequest();
const interval = setInterval(sendPostRequest, 60000); // 60000 ms = 1 minute

// Override the interval and send a POST request immediately on button click
const button = document.querySelector('button');
button.addEventListener('click', () => {
    sendPostRequest();
    clearInterval(interval);
});