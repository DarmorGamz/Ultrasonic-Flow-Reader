<?php

// Ensure this file is being included by a parent file and not directly accessed
if(!defined('_PEXEC')) die('Direct Access to this location is not allowed.');

final class Data {
    // Properties.
    private $oDb;

    /**
     * Constructor.
     */
    final public function __construct() {
        // Init objects
        $this->oDb = DataDb::GetInstance();
    }
    /**
     * Destructor.
     */
    final public function __destruct() { }

    final public function DataGsad(string $sCmd, array $aVarsIn, array &$aVarsOut): bool {
        // Init vars
        $aVarsOut = [];

        // Run Command.
        if("Data.Get" == $sCmd) {
            // Init vars
            $aVarsIn1 = $aVarsIn; $aVarsOut1 = [];

            // Get User.
            if(!$this->oDb->DataGsad("Get", $aVarsIn1, $aVarsOut1) || !isset($aVarsOut1['Data'])) { /* Error */ return false; }


            // Initialize totals
            $aVarsOut['Data']['Total'] = 0;
            $aVarsOut['Data']['Month'] = 0;
            $aVarsOut['Data']['Day'] = 0;
            $aVarsOut['Data']['Hour'] = 0;
            $aVarsOut['Data']['Min15'] = 0;
            $aVarsOut['Data']['Min1'] = 0;

            // Define time periods in seconds
            $month = 30 * 24 * 60 * 60;
            $day = 24 * 60 * 60;
            $hour = 60 * 60;
            $min15 = 15 * 60;
            $min1 = 60;

            // Get current timestamp
            $current_time = time();

            // Loop through the array and compute totals
            foreach ($aVarsOut1['Data'] as $item) {
                $value = intval($item['Value']);
                $time = intval($item['DateMsUtc']);

                // Compute total for all values
                $aVarsOut['Data']['Total'] += $value;

                // Compute total for values within a month
                if ($current_time - $time <= $month) {
                    $aVarsOut['Data']['Month'] += $value;
                }

                // Compute total for values within a day
                if ($current_time - $time <= $day) {
                    $aVarsOut['Data']['Day'] += $value;
                }

                // Compute total for values within an hour
                if ($current_time - $time <= $hour) {
                    $aVarsOut['Data']['Hour'] += $value;
                }

                // Compute total for values within 15 mins
                if ($current_time - $time <= $min15) {
                    $aVarsOut['Data']['Min15'] += $value;
                }

                // Compute total for values within 1 min
                if ($current_time - $time <= $min1) {
                    $aVarsOut['Data']['Min1'] += $value;
                }
            }
        } else if("Data.GetList" == $sCmd) {

        } else if("Data.Set" == $sCmd) {

        } else if("Data.Add" == $sCmd) {

        } else if("Data.Del" == $sCmd) {

        } else { /* Error */ }

        // Return success
        return true;
    }
}

require_once '/var/www/html/Private/Common/Db.php';

class DataDb extends Db {
    // Members - Single instance (singleton mode)
    private static $_instance;

    /**
     * Constructor.
     */
    protected function __construct() { parent::__construct("Data"); }
    /**
     * Destructor.
     */
    public function __destruct() { parent::__destruct(); }

    /**
     *  Get the single instance of this object.
     */
    public static function &GetInstance(): DataDb {
        if(!self::$_instance instanceof self) self::$_instance = new self();
        return self::$_instance;
    }

    final public function DataGsad(string $sCmd, array $aVarsIn, array &$aVarsOut): bool {
        // Init vars
        $aVarsOut = [];

        // Run Command.
        if("Get" == $sCmd) {
            $sQry = "SELECT * FROM RawData WHERE 1";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            while($aRow = $oRes->fetch_assoc()) {
                $aVarsOut['Data'][] = $aRow;
            }
        } else if("Set" == $sCmd) {

        } else if("Add" == $sCmd) {

        } else if("Del" == $sCmd) {

        } else { /* Error */ }

        // Return success
        return true;
    }
}