<?php
//require('command.php');
define("XML_PATH","shortDOMdb.xml");
define("OK_MESSAGE","OK");

include 'database.php';

$devId = $_GET['devId']; 	// example: 5D325FBCDW36 (device unique ID number)
$request = $_GET['request'];// poll / turn / timer / ack
$cmd = $_GET['cmd'];		// on / off / onTimerOff
$cmdId = $_GET['cmdId'];	// 1,2,3,4... cmd counter used as command ID

							//$newCmdFlag=FALSE;	// flag - new command waiting for device

$db = new database(XML_PATH);
$cmdFromDb = new command();

// resolve request

switch ($request)
{
	// ####	Switch requests:
	case "poll":
		// Check for new command
		$db->load();
		if ($db->isNewCommand($devId))
		{
			// New command is waiting!
			// print the command, cmdCount, date, time
			echo "#newCmd=True,";
			//echo "<br>";
			$cmdFromDb = $db->getCommand($devId);
			echo "cmdContent=".$cmdFromDb->cmdContent.",";
			//echo "<br>";
			echo "cmdCount=".$cmdFromDb->commandId.",";
			//echo "<br>";
			echo "dateIssued=".$cmdFromDb->dateIssued.",";
			//echo "<br>";
			echo "timeIssued=".$cmdFromDb->timeIssued."#";
		}else
		{
			// No new command
			echo "#newCmd=False#";
		}
		$db->unload();
		break;
	
	/*
	case "ack":
		// report that a command was received
		break;
	*/
		
		
		
		
		
	// ### Operator requests
	case "turn":
		// turn on / off
		$db->load();
		$db->setCommand($devId, $cmd);
		echo OK_MESSAGE;
		$db->unload();
		break;
		
	/*	
	case "timer":
		break;
		
	case "checkAck";
		// check if arduino acknowledged a command
		break;
	*/
}

/*
///////////////////
// functions
function reportCmd($deviceId)
{
	// get cmdId and cmd from database 	
	
	$cmdArray = getCmd($devId);
	$cmdId = $cmdArray['cmdId'];
	$cmd = $cmdArray['cmd'];
	
	
	//$cmdArray is an asociative array:
	//$my_arr = array('my_phone' => 123456,
	//		'my_sons_number' => 3,
	//		'my_city' => 'Petah Tiqwa',
	//);
	//print $my_arr['my_phone'];
	//
	//echo "cmdId=".$cmdId;
	//echo "cmd=".$cmd;
}

switch ($newCmdFlag)
{
	case TRUE:
		$cmdArray = getCmd($devId);
		$cmdId = $cmdArray['cmdId'];
		$cmd = $cmdArray['cmd'];
				
		
		//	$cmdArray is an asociative array:
		//	$my_arr = array('my_phone' => 123456,
		//			'my_sons_number' => 3,
		//			'my_city' => 'Petah Tiqwa',
		//	);
		//	print $my_arr['my_phone'];
		
		echo "cmdId=".$cmdId;
		echo "cmd=".$cmd;
}

echo "hi php</br>";
$var1 = $_GET['a'];
print $var1;
echo $var1."</br>";
echo "<br\>";
echo "hi again";
*/