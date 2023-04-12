<?php

// Ensure this file is being included by a parent file and not directly accessed
if(!defined('_PEXEC')) die('Direct Access to this location is not allowed.');

final class Contact {
    // Properties.
    private $oDb;

    /**
     * Constructor.
     */
    final public function __construct() {
        // Init objects
        $this->oDb = ContactDb::GetInstance();
    }
    /**
     * Destructor.
     */
    final public function __destruct() { }

    final public function ContactGsad(string $sCmd, array $aVarsIn, array &$aVarsOut): bool {
        // Init vars
        $aVarsOut = [];

        // Run Command.
        if("Contact.Add" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];
            if(empty($aVarsIn1['FirstName']) || empty($aVarsIn1['LastName']) || empty($aVarsIn1['Email']) || empty($aVarsIn1['SupportReason'])
            || empty($aVarsIn1['Message'])) { /* Error */ return false; }

            // Get User.
            if(!$this->oDb->ContactGsad("Add", $aVarsIn1, $aVarsOut1)) { /* Error */ return false; }

            // Set Response.
            $aVarsOut = $aVarsOut1;

        } else { /* Error */ }

        // Return success
        return true;
    }
}

require_once '/var/www/html/Private/Common/Db.php';

class ContactDb extends Db {
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
    public static function &GetInstance(): ContactDb {
        if(!self::$_instance instanceof self) self::$_instance = new self();
        return self::$_instance;
    }

    final public function ContactGsad(string $sCmd, array $aVarsIn, array &$aVarsOut): bool {
        // Init vars
        $aVarsOut = [];

        // Run Command.
        if("Add" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];
            if(empty($aVarsIn1['FirstName']) || empty($aVarsIn1['LastName']) || empty($aVarsIn1['Email']) || empty($aVarsIn1['SupportReason'])
                || empty($aVarsIn1['Message'])) { /* Error */ return false; }

            // Set vars.
            $sFirstName = $aVarsIn1['FirstName']; $sLastName = $aVarsIn1['LastName']; $sEmail = $aVarsIn1['Email'];
            $iSupportReason = $aVarsIn1['SupportReason']; $sMessage = $aVarsIn1['Message'];

            // Insert row.
            $sQry = "INSERT INTO Contact (FirstName, LastName, Email, SupportReason, Message) VALUES('{$sFirstName}','{$sLastName}','{$sEmail}',{$iSupportReason},'{$sMessage}')";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }

        } else { /* Error */ return false; }

        // Return success.
        return true;
    }

}