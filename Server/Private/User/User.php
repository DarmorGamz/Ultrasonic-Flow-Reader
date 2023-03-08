<?php

// Ensure this file is being included by a parent file and not directly accessed
if(!defined('_PEXEC')) die('Direct Access to this location is not allowed.');

final class User {
    // Properties.
    private $oDb;

    /**
     * Constructor.
     */
    final public function __construct() {
        // Init objects
        $this->oDb = UserDb::GetInstance();
    }
    /**
     * Destructor.
     */
    final public function __destruct() { }

    final public function UserGsad(string $sCmd, array $aVarsIn, array &$aVarsOut): bool {
        // Init vars
        $aVarsOut = [];

        // Run Command.
        if("User.Get" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];

            // Get User.
            if(!$this->oDb->UserGsad("Get", $aVarsIn1, $aVarsOut1)) { /* Error */ return false; }

            // Set Response.
            $aVarsOut = $aVarsOut1;
        } else if("User.Set" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];

            // Get User.
            if(!$this->oDb->UserGsad("Set", $aVarsIn1, $aVarsOut1)) { /* Error */ return false; }

            // Set Response.
            $aVarsOut = $aVarsOut1;
        } else if("User.Add" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];

            // Get User.
            if(!$this->oDb->UserGsad("Add", $aVarsIn1, $aVarsOut1)) { /* Error */ return false; }

            // Set Response.
            $aVarsOut = $aVarsOut1;
        } else if("User.Del" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];

            // Get User.
            if(!$this->oDb->UserGsad("Del", $aVarsIn1, $aVarsOut1)) { /* Error */ return false; }

            // Set Response.
            $aVarsOut = $aVarsOut1;
        } else { /* Error */ }

        // Return success
        return true;
    }
}

require_once '/var/www/html/Private/Common/Db.php';

class UserDb extends Db {
    // Members - Single instance (singleton mode)
    private static $_instance;

    /**
     * Constructor.
     */
    protected function __construct() { parent::__construct("Capstone"); }
    /**
     * Destructor.
     */
    public function __destruct() { parent::__destruct(); }

    /**
     *  Get the single instance of this object.
     */
    public static function &GetInstance(): UserDb {
        if(!self::$_instance instanceof self) self::$_instance = new self();
        return self::$_instance;
    }

    final public function UserGsad(string $sCmd, array $aVarsIn, array &$aVarsOut): bool {
        // Init vars
        $aVarsOut = [];

        // Run Command.
        if("Get" == $sCmd) {
            // Init vars.
            if(empty($aVarsIn['Email']) || empty($aVarsIn['Password'])) { /* Error */ return false;}
            $sEmail = $aVarsIn['Email']; $sPassword = $aVarsIn['Password'];
            $sEmail = strtolower($sEmail);

            $sQry = "SELECT * FROM User WHERE Email='{$sEmail}' AND Password='{$sPassword}' LIMIT 1";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            if(!$oRes->num_rows) { /* Error */ return false; }
            if(!$aRow = $oRes->fetch_assoc()) { /* Error */ return false; }
            $aUserInfo = $aRow;

            if(empty($aUserInfo['Id'])) { /* Error */ return false; }
            $iTimeNow = time();

            $sQry = "SELECT * FROM UserSession WHERE UserId={$aUserInfo['Id']} AND DateMsUtc>{$iTimeNow} LIMIT 1";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            if($oRes->num_rows > 0) {
                if(!$aRow = $oRes->fetch_assoc()) { /* Error */ return false; }
                if(empty($aRow['SessionKey'])) { /* Error */ return false; }
                $aUserInfo['SessionKey'] = $aRow['SessionKey'];
            } else {
                $sSessionKey = bin2hex(random_bytes(16)); // generates a random binary string of length 16 and converts it to hexadecimal
                $sSessionKey = substr($sSessionKey, 0, 25); // extracts the first 25 characters from the hexadecimal string
                $iDateExpire = $iTimeNow+(60*60);
                $sQry = "INSERT INTO UserSession (UserId, SessionKey, DateMsUtc) VALUES ({$aUserInfo['Id']},'{$sSessionKey}', {$iDateExpire}) ON DUPLICATE KEY UPDATE SessionKey='{$sSessionKey}'";
                if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
                $aUserInfo['SessionKey'] = $sSessionKey;
            }

            // Set Response.
            $aVarsOut['UserInfo'] = $aUserInfo;

        } else if("Set" == $sCmd) {

        } else if("Add" == $sCmd) {
            // Init vars.
            if(empty($aVarsIn['Email']) || empty($aVarsIn['Password'])) { /* Error */ return false;}
            $sEmail = $aVarsIn['Email']; $sPassword = $aVarsIn['Password'];
            $sEmail = strtolower($sEmail);

            // Check to see if user already exists.
            $sQry = "SELECT * FROM User WHERE Email='{$sEmail}' LIMIT 1";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            if($oRes->num_rows != 0) { /* Error */ return false; } // User already exists.

            // Add User.
            $sQry = "INSERT INTO User (Email, Password) VALUES ('{$sEmail}', '{$sPassword}')";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }

            // Get UserInfo.
            $sQry = "SELECT * FROM User WHERE Email='{$sEmail}' LIMIT 1";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            if(!$oRes->num_rows) { /* Error */ return false; }
            if(!$aRow = $oRes->fetch_assoc()) { /* Error */ return false; }
            $aUserInfo = $aRow;

            if(empty($aUserInfo['Id'])) { /* Error */ return false; }
            $iTimeNow = time();

            $sQry = "SELECT * FROM UserSession WHERE UserId={$aUserInfo['Id']} AND DateMsUtc>{$iTimeNow} LIMIT 1";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            if($oRes->num_rows > 0) {
                if(!$aRow = $oRes->fetch_assoc()) { /* Error */ return false; }
                if(empty($aRow['SessionKey'])) { /* Error */ return false; }
                $aUserInfo['SessionKey'] = $aRow['SessionKey'];
            } else {
                $sSessionKey = bin2hex(random_bytes(16)); // generates a random binary string of length 16 and converts it to hexadecimal
                $sSessionKey = substr($sSessionKey, 0, 25); // extracts the first 25 characters from the hexadecimal string

                $iDateExpire = $iTimeNow+(60*60);
                $sQry = "INSERT INTO UserSession (UserId, SessionKey, DateMsUtc) VALUES ({$aUserInfo['Id']},'{$sSessionKey}', {$iDateExpire})";
                if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
                $aUserInfo['SessionKey'] = $sSessionKey;
            }

            // Set Response.
            $aVarsOut['UserInfo'] = $aUserInfo;
        } else if("Del" == $sCmd) {

        } else { /* Error */ }

        // Return success
        return true;
    }
}