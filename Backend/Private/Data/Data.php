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
            if(!isset($aVarsIn1['DataType'])) { /* Error */ return false; }
            $aVarsIn1['DateMsUtcStop'] = time();
            $aVarsIn1['DateMsUtcStart'] = $aVarsIn1['DateMsUtcStop']-(60*15);

            // Get User.
            if(!$this->oDb->DataGsad("Get", $aVarsIn1, $aVarsOut1) || !isset($aVarsOut1['Data'])) { /* Error */ return false; }

            // Reverse the data array
            $aVarsOut1['Data'] = array_reverse($aVarsOut1['Data']);

            // Iterate through the reversed data array and update the initialized array
            $aTemperatureData = [];
            foreach ($aVarsOut1['Data'] as $Data) {
                    $aTemperatureData[] = ['y' => (float)$Data['Value'], 'x' => $Data['DateMsUtc'] * 1000];
            }

            // Set the output data array
            $aVarsOut['Data']['Values'] = $aTemperatureData;

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
            // Init vars.
            if(!isset($aVarsIn['DataType']) || empty($aVarsIn['DateMsUtcStop']) || empty($aVarsIn['DateMsUtcStart'])) { /* Error */ return false; }
            $iDataType = $aVarsIn['DataType'];

            $sQry = "SELECT * FROM WeatherData WHERE 1 AND DataType={$iDataType} AND DateMsUtc<={$aVarsIn['DateMsUtcStop']} AND DateMsUtc>={$aVarsIn['DateMsUtcStart']} ORDER BY Id DESC LIMIT 900";
            if(($oRes = $this->oDb->query($sQry)) === false) { /* Error */ return false; }
            while($aRow = $oRes->fetch_assoc()) { $aVarsOut['Data'][] = $aRow; }

        } else if("Set" == $sCmd) {

        } else if("Add" == $sCmd) {

        } else if("Del" == $sCmd) {

        } else { /* Error */ }

        // Return success
        return true;
    }
}