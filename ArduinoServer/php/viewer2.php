<html><head>
<script type="text/javascript" src="calendarDateInput.js">

/***********************************************
* Jason's Date Input Calendar- By Jason Moon http://calendar.moonscript.com/dateinput.cfm
* Script featured on and available at http://www.dynamicdrive.com
* Keep this notice intact for use.
***********************************************/

</script>
</head>
<?php
function get_val ( $val )
{
	return $_GET[$val];
}

$type = get_val('type');
if($type=='')
{
	$type="3";
}

$capteur = get_val('capteur');
if($capteur=='')
{
	$capteur="3";
}
$date = get_val('date');
if($date=='')
{
	$date=date("Y-m-d");
}
?>
<BODY>
<center>
<form>
	<select name='type'>
<?php	switch($type)
	{
		case "0" : 
		echo('<option value="0">Mini maxi mensuel</option>
			<option value="1">semaine</option>
			<option value="2">Mini maxi Annuel</option>
			<option value="3">jour</option>');

		break;
		case "1" : 
		echo('<option value="1">semaine</option>
			<option value="2">Mini maxi Annuel</option>
			<option value="3">jour</option>
			<option value="0">Mini maxi mensuel</option>');
		break;
		case "2" : 
		echo('<option value="2">Mini maxi Annuel</option>
			<option value="3">jour</option>
			<option value="0">Mini maxi mensuel</option>
			<option value="1">semaine</option>');
		break;
		case "3" : 
		echo('<option value="3">jour</option>
			<option value="0">Mini maxi mensuel</option>
			<option value="1">semaine</option>
			<option value="2">Mini maxi Annuel</option>');
		echo('');
		break;
	}
?>
	</select>


	<select name='capteur'>
<?php	switch($capteur)
	{
		case "3" : 
		echo('	<option value="3">Temperature</option>
			<option value="4">Pression</option>
			<option value="5">Humiditee</option>
			<option value="6">Systeme</option>');
		break;
		case "4" : 
		echo('	<option value="4">Pression</option>
			<option value="5">Humiditee</option>
			<option value="6">Systeme</option>
			<option value="3">Temperature</option>');
		break;
		case "5" : 
		echo('	<option value="5">Humiditee</option>
			<option value="6">Systeme</option>
			<option value="3">Temperature</option>
			<option value="4">Pression</option>');
		break;
		case "6" : 
		echo('	<option value="6">Systeme</option>
			<option value="3">Temperature</option>
			<option value="4">Pression</option>
			<option value="5">Humiditee</option>');
		break;
	}
?>
	</select>

<?php 

	echo("<script>DateInput('orderdate', true, 'YYYY-MM-DD','".$date."')</script>");
	
?>

<input type="button" value="ok" onClick="window.location='viewer2.php?date='+this.form.orderdate.value+'&type='+this.form.type.value+'&capteur='+this.form.capteur.value">
</form>
<?php


$format = '%F';
$strf = strptime($date,$format);
$offset=  intval((time()-mktime(0,0,0,$strf["tm_mon"]+1,$strf["tm_mday"],$strf["tm_year"]-100))/86400);

		switch ($type)
		{
		
		case 0 : 
			echo("<p>Mini maxi mensuel</p>\n");
		break;

		case 1 : 
			echo("<p>Semaine</p>\n");
		break;

		case 2 : 
			echo("<p>Mini maxi annuel</p>\n");
		break;

		case 3 : 
			echo("<p>Jour</p>\n");
		break;

		}

echo ($date."<br><table><tr><td>");

	for($lieu=3; $lieu >= 0; $lieu--)
	{	
		switch ($type)
		{
		
		case 0 : 
			echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/heureF.php?prof=30&MinMax=1&lieu='.$lieu.'&offset='.$offset.'&type='.$capteur.'" />');
		break;

		case 1 : 
			echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/heureF.php?prof=7&lieu='.$lieu.'&offset='.$offset.'&type='.$capteur.'" />');
		break;

		case 2 : 
			echo ('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/heureF.php?prof=365&MinMax=1&lieu='.$lieu.'&offset='.$offset.'&type='.$capteur.'" />');
		break;

		case 3 : 
			echo('<img src="http://gnome.sage.free.fr/GnOmeDataCenter/heureF.php?prof=0&lieu='.$lieu.'&offset='.$offset.'&type='.$capteur.'" /></img>');
		break;

		}

		if($lieu%2==0)
		{
			echo("</td></tr><tr><td>");
		}
		else
		{
			echo("</td><td>");
		}
	}

?>

</table>
</body>
</html>


