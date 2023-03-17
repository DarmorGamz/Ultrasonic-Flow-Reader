<?php
    // Includes
    require_once '/var/www/html/Private/Globals.php';

    $sData = file_get_contents('php://input'); // Get the POST request body
    $aLines = explode("Event:", $sData); // Split the string into an array based on "Event:"
    $sQry = "INSERT INTO Event (EventType, Event, DateMsUtc) VALUES";

    foreach($aLines as $aItem) {
        if(strlen($aItem) == 0) { continue; }
        // Trim any whitespace from the line
        $aParts = explode(":", $aItem);
        $sEventType = (string)trim($aParts[0]);
        $sEvent = (string)trim($aParts[1]);
        $iDateMsUtc = trim($aParts[2]);

        // Create Temp Row.
        $sTempRowInsert = " (\"{$sEventType}\", \"{$sEvent}\", {$iDateMsUtc}),";

        // Append Row.
        $sQry = $sQry.$sTempRowInsert;
    }

    $sQry = substr_replace($sQry, ";", -1);

    // Init object
    if(!$oDb = mysqli_connect(DB_HOST, DB_USER, DB_USER_PWD, "Data")) { return; }

    if(($oRes = $oDb->query($sQry)) === false) { return; }

    // DeInit object.
    $oDb->close();

    // Set the response body
    $response_body = time();

    // Calculate the content length
    $content_length = strlen($response_body);

    // Set the HTTP response code to 200 OK
    http_response_code(200);

    // Set the content type to plain text
    header('Content-Type: text/plain');

    // Set the Content-Length header
    header('Content-Length: ' . $content_length);

    // Return the response body
    echo $response_body;