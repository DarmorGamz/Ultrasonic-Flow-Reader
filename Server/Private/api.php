<?php

// Set flag that this is a parent file
define( '_PEXEC', 1 );

// Includes
require_once './Private/ApiAccess.php';

$oApi = new ApiAccess();
$oApi->SendResponse();