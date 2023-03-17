<?php
file_put_contents("test2.txt", "Here\n", FILE_APPEND);
file_put_contents("test.txt", file_get_contents('php://input'), FILE_APPEND);

header("Content-Type: text/plain");
header("HTTP/1.1 200");
header("HTTP/1 200");     //works
header("HTTP/ 200");      //works
http_response_code(200);
echo "200 OK";