<?php
/*

        graffiti.php
        Cloud based graffiti artist

        Driver software requests action with URI args:
	1. x - x coordinate
	2. y - y coordinate
	3. p - spray can pressed status 0 - not pressed, 1 - pressed
	4. action - browser flag, if present request comes from browser, if not present request comes from graffiti artist
*/
$action = (isset($_GET['action']) ? $_GET['action'] : null);
if($action == "") {
	$cmd = "tail -1 /tmp/graffiti.txt";
	$cmdout = shell_exec($cmd);
	$vars = explode("|", $cmdout);	
	echo "x = $vars[0], y = $vars[1], p = $vars[2]\n";			
	echo $cmdout;
} else {
	$x = (isset($_GET['x']) ? $_GET['x'] : null);
	$y = (isset($_GET['y']) ? $_GET['y'] : null);
	$p = (isset($_GET['p']) ? $_GET['p'] : null);
	$cmd = "echo \"$x|$y|$p\" >> /tmp/graffiti.txt";
	echo $cmd;
	shell_exec($cmd);
}
?>
