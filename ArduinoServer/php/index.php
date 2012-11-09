<?php
function get_val ( $val )
{

		return $_GET[$val];

}

$password = get_val ( 'pwd' );

if ( $password != '8sdf6s56s' )
{
	echo "<font color=red><b>Access denied!!!</b></font>";
	exit;
}

$client = get_val('client');
if ( $client == '' )
{
	echo "cl<br>";
}


$T1 = get_val('T1');
if ( $T1 == '' )
{
	echo "T<br>";
}

$T2 = get_val('T2');
if ( $T2 == '' )
{
	echo "T2<br>";
}

$H = get_val('H');
if ( $H == '' )
{
	echo "H<br>";
}
$P = get_val('P');
if ( $P == '' )
{
	echo "P<br>";
}


$time = date ("Y-n-j;H:i:s");
$string = $client . ';' .$time.';'.$T1.';'.$P.';'.$H.';'.$T2."\r\n";

//Save file
$delimiteur = ";";
$taille=1024;
$filename = "data.txt";

$handle = fopen("$filename", "r");
$ligne 	= fgetcsv($handle, $taille, $delimiteur);
$TimeFile = $ligne[1];

if($TimeFile==date ("Y-n-j"))
{
	#echo "archive classique<br>".$TimeFile."<br>".$time;
	fclose($handle);
}
else
{
	#echo "nouveau jour<br>".$filename."<br>".$TimeFile."_".$filename;
	fclose($handle);
	copy($filename, $TimeFile."_".$filename);
	unlink($filename);
}




$handle = fopen("$filename", "a+");
fputs($handle, $string);
fclose($handle);









?>
