<?php
// Includes
require_once '/var/www/html/Private/Globals.php';

$conn = mysqli_connect(DB_HOST, DB_USER, DB_USER_PWD, "Data");
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

// Generate 60 random numbers and insert them into the database
for ($i = 0; $i < 60; $i++) {
    $iValue = rand(1, 4);
    $iDateMsUtc = time();
    $sql = "INSERT INTO RawData (Value, DateMsUtc) VALUES ({$iValue}, {$iDateMsUtc})";
    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
    sleep(1);
}

// Close the database connection
mysqli_close($conn);

?>