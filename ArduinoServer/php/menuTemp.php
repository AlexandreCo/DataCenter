<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">


<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>
<body>

<center>
<?php
function get_val ( $val )
{
	return $_GET[$val];
}

	$lieu = get_val('lieu');
	if ( $lieu == '' )
	{

		$lieu=3;
	}

	$type = get_val('type');
	if ( $type == '' )
	{

		echo("<p>You didn't select any graph!</p>\n");
	}
	else
	{
		
		switch(	$type)
		{
			case 0 : 
				echo("<p>Mini maxi du mois (periode un mois)</p>\n");
				echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/detailF.php?title=Temperature en °C&prof=30&MinMax=1&lieu='.$lieu.'" />');
			break;

			case 1 : 
				echo("<p>Temperature de la semaine</p>\n");
				echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/detailF.php?title=Temperature en °C&prof=7&lieu='.$lieu.'" />');

			break;

			case 2 : 
				echo("<p>Mini maxi de l'ann&eacute;e</p>\n");
				echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/detailF.php?title=Temperature en °C&prof=365&MinMax=1&lieu='.$lieu.'" />');
			break;

			case 3 : 
				echo("<p>Temperature du jour</p>\n");
				echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/detailF.php?title=Temperature en °C&prof=0&lieu='.$lieu.'" />');
			break;

		}
		if ( $type == 0) 
		{
		}
		else
		{
		}
	}
?>
	<form action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" method="post">
	<select onchange="window.location='menuTemp.php?lieu=<?php echo $lieu; ?>&type='+this.value">
	<option value="4">Type...</option>
	<option value="3">Temperature du jour</option>
	<option value="1">Temperature de la semaine</option>
	<option value="0">Mini maxi Journalier (periode un mois)</option>
	<option value="2">Mini maxi Journalier (periode un an)</option>
	</select><br>
</form>
</body>
</html>
