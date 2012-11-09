<?php // content="text/plain; charset=utf-8"
require_once ('jpgraph/jpgraph.php');
require_once ('jpgraph/jpgraph_line.php');
require_once ('jpgraph/jpgraph_mgraph.php');

function ScanDirectory($Directory,&$tab_file){

  $MyDirectory = opendir($Directory) or die('Erreur');
	while($Entry = @readdir($MyDirectory)) {
		if(is_dir($Directory.'/'.$Entry)&& $Entry != '.' && $Entry != '..') 
		{
			//rien a faire pas recursif
		}
		else {
			echo '<li>'.$Entry.'</li>';
                }
	}
  closedir($MyDirectory);
}



function get_val ( $val )
{
	return $_GET[$val];
}



$calSalon=0;
$calVeranda=0;
$calBureau1=+1.5;
$calBureau2=-1.5;
$calExt=0;

//Temperature
$TSalon = array();
$TVeranda = array();
$TBureau1 = array();
$TBureau2 = array();
$TExt = array();

//Autres données
$capPression = "Salon";
$Pression = array();
$Humi = array();

$prefix = get_val('date');
if ( $prefix == '' )
{
	
	$file="data.txt";
}
else
{
	if ( $prefix == date ("Y-n-j") )
	{
		$file="data.txt";
	}
	else
	{
		//todo faille ici
		$file=$prefix."_data.txt";
	}
}



$filepath="http://gnome.sage.free.fr/GnOmeDataCenter/";
$filecomplete=$filepath.$file;

$delimiteur = ";";
$taille=1024;
$prof = get_val('prof');
if ( $prof == '' )
{
	$prof=0;
}
$datedebut = time() - ( $prof * 24 * 60 * 60);
//echo $prof." ".date ("Y-n-j",$datedebut)." ".date ("Y-n-j")."<br>";
$nbJour=$prof;
for($prof;$prof>-1;$prof--)
{
	$prefix=date ("Y-n-j",time() - ( $prof * 24 * 60 * 60));

	if ( $prefix == date ("Y-n-j") )
	{
		$file="data.txt";
	}
	else
	{
		$file=$prefix."_data.txt";
	}
	//echo $file."<br>";

	$handle = @fopen($file, "r" );
	if ($handle)
	{

		while ($ligne = fgetcsv($handle, $taille, $delimiteur)) 
		{
			switch ($ligne[0])
			{
				//Salon
				case "192.168.0.0":

					if(is_numeric ($ligne[3])&&is_numeric ($ligne[4]))
					{
						$DateSalon[] 	= $ligne[1];
						$TimeSalon[] 	= $ligne[2];					
						$TSalon[] 	= $ligne[3]+$calSalon;

						$Pression[] 	= $ligne[4];

					}
				break;

				//Veranda
				case "192.168.0.1":
					if(is_numeric ($ligne[3]))
					{
						$DateVeranda[] 	= $ligne[1];
						$TimeVeranda[] 	= $ligne[2];
						$TVeranda[] 	= $ligne[3]+$calVeranda;
					}
				break;

				//Bureau
				case "192.168.0.2":
					if(is_numeric ($ligne[3])&&is_numeric ($ligne[4])&&is_numeric ($ligne[5])&&is_numeric ($ligne[6]))
					{
						$DateBureau[] 	= $ligne[1];
						$TimeBureau[] 	= $ligne[2];
						$TBureau1[] 	= $ligne[3]+$calBureau1;
						//$Pression[] 	= $ligne[4];
						$Humi[] 	= $ligne[5];
						$TBureau2[] 	= $ligne[6]+$calBureau2;	//Calibration du sht11
					}
				break;

				//Exterieur
				case "192.168.0.3":
					if(is_numeric ($ligne[3]))
					{
						$DateExt[] 	= $ligne[1];
						$TimeExt[] 	= $ligne[2];
						$TExt[] 	= $ligne[3]+$calExt;
					}
				break;
				default :
					#echo "erreur" .$ligne[0];
				break;
			}
		}
		fclose($handle);
	}
	else
	{

	}

	
}



if ($TSalon)
{
	// Setup the graph
	$gSalon = new Graph(400,220);
	$gSalon->SetMargin(40,40,20,60);
	$gSalon->SetScale('intlin',min($TSalon)-1,max($TSalon)+1,0,0);
	$gSalon->SetMarginColor('darkgreen@0.8');
	$gSalon->title->Set('T Salon en °C');
	if($nbJour>2)
		$gSalon->xaxis->SetTickLabels($DateSalon);
	else
		$gSalon->xaxis->SetTickLabels($TimeSalon);
	$gSalon->xaxis->SetLabelAngle(50);

	// Create the line
	$T1 = new LinePlot($TSalon);
	$T1->SetColor("blue");
	$T1->SetWeight(1);
	$T1->SetFillGradient('red','yellow');
	$gSalon->Add($T1);
}

if ($TVeranda)
{
	// Setup the graph
	$gVeranda = new Graph(400,220);
	$gVeranda->SetMargin(40,40,20,60);		
	$gVeranda->SetScale('intlin',min($TVeranda)-1,max($TVeranda)+1,0,0);
	$gVeranda->SetMarginColor('darkgreen@0.8');
	$gVeranda->title->Set('T Veranda en °C');
	if($nbJour>2)
		$gVeranda->xaxis->SetTickLabels($DateVeranda);		
	else
		$gVeranda->xaxis->SetTickLabels($TimeVeranda);
	$gVeranda->xaxis->SetLabelAngle(50);
	#$gVeranda->xaxis->SetPos('min');

	// Create the line
	$T1 = new LinePlot($TVeranda);
	$T1->SetColor("blue");
	$T1->SetWeight(1);
	$T1->SetFillGradient('red','yellow');
	$gVeranda->Add($T1);
}

if ($TBureau1)
{
	// Setup the graph
	$Bureau = new Graph(400,220);
	$Bureau->SetMargin(40,40,20,60);		
	$Bureau->SetScale('intlin',min(min($TBureau1),min($TBureau2))-1,max(max($TBureau1),max($TBureau2))+1,0,0);
	$Bureau->SetMarginColor('darkgreen@0.8');
	$Bureau->title->Set('T Bureau en °C');
	if($nbJour>2)
		$Bureau->xaxis->SetTickLabels($DateBureau);
	else
		$Bureau->xaxis->SetTickLabels($TimeBureau);
	$Bureau->xaxis->SetLabelAngle(50);

	// Create the line
	$T1 = new LinePlot($TBureau1);
	$T1->SetColor("red");
	$T1->SetWeight(1);
	
	// Create the line
	$T2 = new LinePlot($TBureau2);
	$T2->SetColor("blue");
	$T2->SetWeight(1);
	$T2->SetFillGradient('red','yellow');

	$Bureau->Add($T2);
	$Bureau->Add($T1);
}

if ($Pression)
{
	// Setup the graph
	$gPres = new Graph(400,220);
	$gPres->SetMargin(40,40,20,60);		
	$gPres->SetScale('intlin',min($Pression)-1,max($Pression)+1,0,0);
	$gPres->SetMarginColor('darkgreen@0.8');

	$gPres->title->Set('Pression en hPa');
	//if($nbJour>2)
	//	$gPres->xaxis->SetTickLabels($DateBureau);
	//else
	//	$gPres->xaxis->SetTickLabels($TimeBureau);

	if($nbJour>2)
		$gPres->xaxis->SetTickLabels($DateSalon);
	else
		$gPres->xaxis->SetTickLabels($TimeSalon);



	$gPres->xaxis->SetLabelAngle(50);
	// Create the line
	$P1 = new LinePlot($Pression);
	$P1->SetColor("blue");
	$P1->SetWeight(1);
	$P1->SetFillGradient('green','white');
	$gPres->Add($P1);
}

if ($Humi)
{
	// Setup the graph
	$gHumi = new Graph(400,220);
	$gHumi->SetMargin(40,40,20,60);		
	$gHumi->SetScale('intlin',min($Humi)-1,max($Humi)+1,0,0);
	$gHumi->SetMarginColor('darkgreen@0.8');

	$gHumi->title->Set('Humidité en %');
	if($nbJour>2)
		$gHumi->xaxis->SetTickLabels($DateBureau);
	else
		$gHumi->xaxis->SetTickLabels($TimeBureau);
	$gHumi->xaxis->SetLabelAngle(50);
	// Create the line
	$H1 = new LinePlot($Humi);
	$H1->SetColor("blue");
	$H1->SetWeight(1);
	$H1->SetFillGradient('blue','white');
	$gHumi->Add($H1);
}

if ($TExt)
{
	// Setup the graph
	$gExt = new Graph(400,220);
	$gExt->SetMargin(40,40,20,60);		
	$gExt->SetScale('intlin',min($TExt)-2,max($TExt)+1,0,0);
	$gExt->SetMarginColor('darkgreen@0.8');
	$gExt->title->Set('T Exterieur en °C');
	if($nbJour>2)
		$gExt->xaxis->SetTickLabels($DateExt);		
	else
		$gExt->xaxis->SetTickLabels($TimeExt);
	$gExt->xaxis->SetLabelAngle(50);
	#$gVeranda->xaxis->SetPos('min');

	// Create the line
	$T1 = new LinePlot($TExt);
	$T1->SetColor("blue");
	$T1->SetWeight(1);
	$T1->SetFillGradient('red','yellow');
	$gExt->Add($T1);
}

$mgraph = new MGraph();
$xSalon=3;$ySalon=20;
$xVeranda=400;$yVeranda=20;
$xBureau=800;$yBureau=20;

$xPress=3;$yPress=243;
$xHumi=400;$yHumi=243;
$xExt=800;$yExt=243;

//Temperature
if($gSalon)
	$mgraph->Add($gSalon,$xSalon,$ySalon);
if($gVeranda)
	$mgraph->Add($gVeranda,$xVeranda,$yVeranda);
if($Bureau)
	$mgraph->Add($Bureau,$xBureau,$yBureau);
//Autres
if($gPres)
	$mgraph->Add($gPres,$xPress,$yPress);
if($gHumi)
	$mgraph->Add($gHumi,$xHumi,$yHumi);
if($gExt)
	$mgraph->Add($gExt,$xExt,$yExt);
$mgraph->title->Set($DateSalon[0]);	
$mgraph->Stroke();


?>


