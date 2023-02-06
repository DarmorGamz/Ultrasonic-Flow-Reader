<?php

// Includes
require_once './Private/Globals.php';

final class ApiAccess {
    // Methods
    private $aResp = [], $aIpVar = [];

    /**
     * Constructor.
     */
    final public function __construct() {
        $this->ProcCmd();
    }
    /**
     * Destructor.
     */
    final public function __destruct() {}

    final public function ProcCmd(): bool {
        // Get Post/Get vars.
        $aInput = $_POST; $aInput += $_GET;
        foreach($aInput as $key => $val) {
            $sKey = html_entity_decode(urldecode($key));
            if(!preg_match('/^[a-zA-Z0-9]+$/', $sKey)) continue;
            $sVal = html_entity_decode(urldecode($val));
            if(empty($sVal)) { //  && ($sVal != 0)
                $this->aIpVar[$sKey] = '';
            } else {
                if(!preg_match('/^[\x20-\x7E]+$/', $sVal)) continue;
                $this->aIpVar[$sKey] = $sVal;
            }
        }

        $sCmd = '';
        if(empty($this->aIpVar['Cmd'])) return false;
        $sCmd = $this->aIpVar['Cmd'];
        $this->aResp['Cmd'] = $sCmd;

        switch($sCmd) {
            case 'setData':

                break;
            default: // Not a valid enpoint.
                break;
        }

        // Return Success.
        return true;
    }

    final public function SendResponse(): bool {
        // JSON encode array
        $sResp = json_encode($this->aResp);
        // Add text/plain header so receiving application knows the type of data
        header('Content-Type: application/json');
        //header('Content-Type: application/json');

        // Return response
        echo($sResp);

        // Return success.
        return true;
    }

}