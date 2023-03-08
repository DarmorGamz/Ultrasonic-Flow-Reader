<?php
// Includes.
require_once '/var/www/html//Private/Globals.php';

// Ensure this file is being included by a parent file and not directly accessed
if(!defined('_PEXEC')) die('Direct Access to this location is not allowed.');

class Db {
    // Properties.
    protected $oDb;

    protected $sHOST, $sUSER, $sPWD;
    protected $sDB = '';

    /**
     * Constructor.
     */
    protected function __construct(string $sDB) {
        // Init vars
        $this->sHOST = defined('DB_HOST') ? DB_HOST : '';
        $this->sUSER = defined('DB_USER') ? DB_USER : '';
        $this->sPWD = defined('DB_USER_PWD') ? DB_USER_PWD : '';
        $this->sDB = $sDB;

        // Init objects
        if(!$this->oDb = mysqli_connect($this->sHOST, $this->sUSER, $this->sPWD, $this->sDB)) { }
    }
    /**
     * Destructor.
     */
    protected function __destruct() { $this->oDb->close(); }
}