<?php
/*
	TODOs
	1. Update docs to reflect hypothenuse calculating functions

        graffiti.php
        Cloud based graffiti artist

        Driver software requests action with URI args:
        1. x - x coordinate
        2. y - y coordinate
        3. p - spray can pressed status 0 - not pressed, 1 - pressed
        4. action - browser flag, if present request comes from browser, if not present request comes from graffiti artist
        NB x and y now mean length of hypotenuse for x and y steppers.
*/

// configuration variables
// margins (mm)
$iMx = 150; 
$iMy = 150;
// canvas size (mm) as per canvas.php
$iCx = 1100;
$iCy = 700; // not used

$action = (isset($_GET['action']) ? $_GET['action'] : null);
if($action == "") {
        $cmd = "tail -1 /tmp/graffiti.txt";
        $cmdout = shell_exec($cmd);
        $vars = explode("|", $cmdout);
	$iHypX = getHypX($vars[0], $vars[1]);
	$iHypY = getHypY($vars[0], $vars[1]);
	echo "$iHypX|$iHypY|$vars[2]";
} else {
        $x = (isset($_GET['x']) ? $_GET['x'] : null);
        $y = (isset($_GET['y']) ? $_GET['y'] : null);
        $p = (isset($_GET['p']) ? $_GET['p'] : null);
        $cmd = "echo \"$x|$y|$p\" >> /tmp/graffiti.txt";
        echo $cmd;
        shell_exec($cmd);
}

function getHypX($x, $y) {
        $b = $GLOBALS['iMx'] + $x;
	$a = $GLOBALS['iMy'] + $y;
	return intval(sqrt(pow($a, 2) + pow($b, 2)));
}

function getHypY($x, $y) {
        $b = $GLOBALS['iMx'] + $GLOBALS['iCx'] - $x;
        $a = $GLOBALS['iMy'] + $y;
        return intval(sqrt(pow($a, 2) + pow($b, 2)));
}

?>
