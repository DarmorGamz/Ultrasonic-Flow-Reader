<?php

// Includes
require_once '/var/www/html/Private/Globals.php';

final class ApiAccess {
    // Methods
    private $aResp = [], $aIpVar = [];

    /**
     * Constructor.
     */
    final public function __construct() {
        // Init vars.
        $this->aResp['Errors'] = [];
        $this->aResp['DateMsUtc'] = time();
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
        if(empty($this->aIpVar['Cmd'])) { $this->SetError(1, "Cmd Not Found."); return false; }
        $sCmd = $this->aIpVar['Cmd'];
        $this->aResp['Cmd'] = $sCmd;

        switch($sCmd) {
            case "getdata":
            case "Data.Get":
            case "Data.GetList":
            case "Data.Add":
            case "Data.Set":
            case "Data.Del":
                if($sCmd == "getdata") $sCmd = "Data.Get";
                if(!$this->DataGsad($sCmd)) { $this->SetError(1, "DataGsad Failed."); return false; }
                break;
            case "Login":
            case "Signup":
            case "ResetPassword":
            case "User.Get":
            case "User.Add":
            case "User.Set":
            case "User.Del":
                if($sCmd == "Signup") $sCmd = "User.Add";
                if(!$this->UserGsad($sCmd)) { $this->SetError(1, "UserGsad Failed."); return false; }
                break;
            default: // Not a valid endpoint.
                $this->SetError(1, "Cmd not found."); return false;
                break;
        }

        // Return Success.
        return true;
    }

    private function UserGsad(string $sCmd) : bool {
        require_once ROOTPATH.'/Private/User/User.php';
        $oUser = new User();

        // Init vars.
        $aVarsIn = $aVarsOut = [];

        if($sCmd == 'User.Get') {

        } elseif($sCmd == 'User.Add') {
            // Init vars.
            $sEmail = $sPwd = '';
            if(!$this->GetInputVar('Email', $sEmail) || !$this->GetInputVar('Password', $sPwd)) { $this->SetError(2, "Invalid Input vars."); return false; }

            // Set Vars.
            $aVarsIn['Email'] = $sEmail; $aVarsIn['PwdMd5'] = md5($sPwd);
            if(!$oUser->UserGsad("User.Add", $aVarsIn, $aVarsOut) || empty($aVarsOut['UserInfo'])) { $this->SetError(2, "User.Add Failed."); return false; }
            $aUserInfo = $aVarsOut['UserInfo'];
            unset($aUserInfo['PwdMd5']); // Security idk.

            // Set Response.
            $this->aResp['UserInfo'] = $aUserInfo;
        } elseif($sCmd == 'User.Set') {
            if(!$oUser->UserGsad("User.Set", $aVarsIn, $aVarsOut)) { $this->SetError(2, "User.Set Failed."); return false; }
        } elseif($sCmd == 'User.Del') {
            // Init vars.
            $sEmail = $sPwd = '';
            if(!$this->GetInputVar('Email', $sEmail) || !$this->GetInputVar('Password', $sPwd)) { $this->SetError(2, "Invalid Input vars."); return false; }

            // Set Vars.
            $aVarsIn['Email'] = $sEmail; $aVarsIn['PwdMd5'] = md5($sPwd);
            if(!$oUser->UserGsad("User.Del", $aVarsIn, $aVarsOut)) { $this->SetError(2, "User.Del Failed."); return false; }

        } elseif($sCmd == 'Login') {
            // Init vars.
            $sEmail = $sPwd = '';
            if(!$this->GetInputVar('Email', $sEmail) || !$this->GetInputVar('Password', $sPwd)) { $this->SetError(2, "Invalid Input vars."); return false; }

            // Set Vars.
            $aVarsIn['Email'] = $sEmail; $aVarsIn['PwdMd5'] = md5($sPwd);
            if(!$oUser->UserGsad("Login", $aVarsIn, $aVarsOut) || empty($aVarsOut['UserInfo'])) { $this->SetError(2, "Login Failed."); return false; }
            $aUserInfo = $aVarsOut['UserInfo'];
            unset($aUserInfo['PwdMd5']); // Security idk.

            // Set Response.
            $this->aResp['UserInfo'] = $aUserInfo;
        } elseif($sCmd == 'ResetPassword') {
            // Init vars.
            $sEmail = $sPwd = $sOldPwd = '';
            if(!$this->GetInputVar('Email', $sEmail) || !$this->GetInputVar('Password', $sPwd) || !$this->GetInputVar('PasswordOld', $sOldPwd)) { $this->SetError(2, "Invalid Input vars."); return false; }

            // Set Vars.
            $aVarsIn['Email'] = $sEmail; $aVarsIn['PwdOldMd5'] = md5($sOldPwd); $aVarsIn['PwdMd5'] = md5($sPwd);
            if(!$oUser->UserGsad("ResetPassword", $aVarsIn, $aVarsOut)) { $this->SetError(2, "ResetPassword Failed."); return false; }

        } else { return false; }

        // Return Success.
        return true;
    }

    private function DataGsad(string $sCmd) : bool {
        require_once ROOTPATH.'/Private/Data/Data.php';
        $oData = new Data();

        // Init vars.
        $aVarsIn = $aVarsOut = [];
        if($sCmd == 'Data.Get') {
            if(!$oData->DataGsad("Data.Get", $aVarsIn, $aVarsOut) || !isset($aVarsOut['Data'])) { $this->SetError(2, "Data.Get Failed."); return false; }

            // Set Response.
            $this->aResp['Data'] = $aVarsOut['Data'];
        } else { return false; }


        // Return Success.
        return true;
    }

    final private function SetError(int $iCode, string $errorMsg): void {
        $this->aResp['Errors'][] = [$iCode, $errorMsg];
    }

    final private function GetInputVar(string $IpVar, &$IpVarOut): bool {
        switch($IpVar) {
            case "Email":
                // Checks if Email is valid.
                if((!isset($this->aIpVar['Email'])) || (!filter_var($this->aIpVar['Email'], FILTER_VALIDATE_EMAIL)) || strlen($this->aIpVar['Email']) > 100) return false;
                // Returns the valid email.
                $IpVarOut = $this->aIpVar['Email'];
                break;
            case "Password":
                // Checks if Email is valid.
                if(!isset($this->aIpVar['Password']) || empty($this->aIpVar['Password']) || strlen($this->aIpVar['Password']) > 25) return false;
                // Returns the valid email.
                $IpVarOut = $this->aIpVar['Password'];
                break;
            case "PasswordOld":
                // Checks if Email is valid.
                if(!isset($this->aIpVar['PasswordOld']) || empty($this->aIpVar['PasswordOld']) || strlen($this->aIpVar['PasswordOld']) > 25) return false;
                // Returns the valid email.
                $IpVarOut = $this->aIpVar['PasswordOld'];
                break;
            default:
                return false;
                break;
        }
        // Return Success.
        return true;
    }

    final public function SendResponse(): bool {
        // JSON encode array
        $sResp = json_encode($this->aResp);
        // Add text/plain header so receiving application knows the type of data
        header("Access-Control-Allow-Origin: *");
        header("Access-Control-Allow-Methods: GET, POST, OPTIONS");
        header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
        header('Content-Type: application/json');

        // Return response
        echo($sResp);

        // Return success.
        return true;
    }

}