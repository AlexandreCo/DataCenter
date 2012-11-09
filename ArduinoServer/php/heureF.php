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

define("VERANDA",0);
define("SALON",1);
define("BUREAU", 2);
define("EXTERIEUR",3);

define("ERR_LIEU",-1);
define("ERR_HEURE",-1);
define("ERR_DATE",-1);
define("ERR_HUMI",-1);
define("ERR_PRESS",-1);
define("ERR_TEMP",-300);

define("LABEL",0);
define("DATE",1);
define("HEURE",2);
define("TEMP",3);
define("PRESS", 4);
define("HUMI",5);
define("NB_SAMPLE",6); 
define("MIN", 7);
define("MAX",8);
define("MAX_FIELD",9); 


function getLieu($ligne)
{
	switch ($ligne[LABEL])
	{
		//Salon
		case "192.168.0.0":
			return VERANDA;
		break;
		//Veranda
		case "192.168.0.1":
			return SALON;
		break;
		//Bureau
		case "192.168.0.2":
			return BUREAU;
		break;
		//Exterieur
		case "192.168.0.3":
			return EXTERIEUR;
		break;
		default :
			#echo "erreur" .$ligne[0];
			return ERR_LIEU;
		break;
	}
}

function getStrLieu($lieu)
{
	switch ($lieu)
	{
		//Salon
		case 0 : 
			return "Salon";
		break;
		//Veranda
		case 1 : 
			return "Veranda";
		break;
		//Bureau
		case 2 : 
			return "Bureau";
		break;
		//Exterieur
		case 3 : 
			return "Exterieur";
		break;
		default :
			return "Inconnu";
		break;
	}
}

function getMesType($ligne,$type)
{
	
	if(is_numeric ($ligne[$type]))
	{

		switch($type)
		{
			case TEMP:
				if(($ligne[$type]<150)&&($ligne[$type]>-150))
					return $ligne[$type];
				else
					return ERR_TEMP;
			break;
	
			case HUMI:
				if(($ligne[$type]<100)&&($ligne[$type]>0))
					return $ligne[$type];
				else
					return ERR_TEMP;
			break;

			case PRESS:
				if(($ligne[$type]<1100)&&($ligne[$type]>800))
					return $ligne[$type];
				else
					return ERR_TEMP;
			break;


		}		
	}
	else
	{
		return ERR_TEMP;
	}
}

function getTemp($ligne)
{
	
	if(is_numeric ($ligne[TEMP]))
	{
		return $ligne[TEMP];
	}
	else
	{
		return ERR_TEMP;
	}
}

function getPress($ligne)
{
	
	if(is_numeric ($ligne[PRESS]))
	{
		return $ligne[PRESS];
	}
	else
	{
		return ERR_PRESS;
	}
}

function getHumi($ligne)
{
	
	if(is_numeric ($ligne[HUMI]))
	{
		return $ligne[HUMI];
	}
	else
	{
		return ERR_HUMI;
	}
}


function getJour($ligne)
{
	return $ligne[DATE];
}

function getHeure($ligne)
{
	$strf = strptime($ligne[HEURE],"%H:%M:%S");
	return $strf["tm_hour"];
}



function readData( $prefix, $cpt,$Idx, $dblarray,$type)
{
	
	$taille=1024;
	$delimiteur = ";";	
	if ( $prefix == date ("Y-n-j") )
	{
		$file="data.txt";
	}
	else
	{
		$file=$prefix."_data.txt";
	}
	for($i=0;$i<24;$i++)
	{
		$dblarray[TEMP][$i+$Idx]=0;
		$dblarray[HUMI][$i+$Idx]=0;
		$dblarray[PRESS][$i+$Idx]=0;
		$dblarray[NB_SAMPLE][$i+$Idx]=0;
		$dblarray[HEURE][$i+$Idx]=$i;

	}
	$handle = @fopen($file, "r" );

	
	if ($handle)
	{
		while ($ligne = fgetcsv($handle, $taille, $delimiteur)) 
		{
			$lieu=getLieu($ligne);
			if($cpt==$lieu)
			{

				$heure=getHeure($ligne);
				if($type!=NB_SAMPLE)
				{
					$data=getMesType($ligne,$type);
					if($data!=ERR_TEMP)
					{
						//mesure correct
						$dblarray[$type][$heure+$Idx]+=$data;
						$dblarray[NB_SAMPLE][$heure+$Idx]++;
						$dblarray[DATE][$heure+$Idx]=getJour($ligne);
					}
				}
				else
				{
					//cas special comptage des sample
					$dblarray[NB_SAMPLE][$heure+$Idx]++;
				}

			}
		}
		fclose($handle);
	}

	for($i=0;$i<24;$i++)
	{
		if($dblarray[NB_SAMPLE][$i+$Idx]==0)
		{
			//aucun sample durant cette heure
			if(($i+$Idx)!=0)
			{	//on prend la mesure davant
				$dblarray[TEMP][$i+$Idx]=$dblarray[TEMP][$i+$Idx-1];
				$dblarray[HUMI][$i+$Idx]=$dblarray[HUMI][$i+$Idx-1];
				$dblarray[PRESS][$i+$Idx]=$dblarray[PRESS][$i+$Idx-1];
			}
			else
			{
				//cas special premiere mesure on prend ERR_TEMP
				$dblarray[TEMP][$i+$Idx]=ERR_TEMP;
				$dblarray[HUMI][$i+$Idx]=ERR_TEMP;
				$dblarray[PRESS][$i+$Idx]=ERR_TEMP;
				
			}				
		}
		else
		{
			$dblarray[TEMP][$i+$Idx]=$dblarray[TEMP][$i+$Idx]/$dblarray[NB_SAMPLE][$i+$Idx];
			$dblarray[HUMI][$i+$Idx]=$dblarray[HUMI][$i+$Idx]/$dblarray[NB_SAMPLE][$i+$Idx];
			$dblarray[PRESS][$i+$Idx]=$dblarray[PRESS][$i+$Idx]/$dblarray[NB_SAMPLE][$i+$Idx];
		}


		/*echo($dblarray[DATE][$i+$Idx].",".$i.",".$dblarray[NB_SAMPLE][$i+$Idx].",".$dblarray[TEMP][$i+$Idx].",".$dblarray[HUMI][$i+$Idx].",".$dblarray[PRESS][$i+$Idx]);
		echo("<br>");*/
	}
	$Idx+=24;
}

function display($dblarray,$type,$Title,$bHeure,$bMinMax)
{
	$mgraph = new MGraph();

	$graph = new Graph(400,220);
	$graph->SetMargin(40,40,20,60);
	if($bMinMax)
	{
		$graph->SetScale('intlin',min($dblarray[MIN])-1,max($dblarray[MAX])+1,0,0);
	}
	else
	{
		$graph->SetScale('intlin',min($dblarray[$type])-1,max($dblarray[$type])+1,0,0);
	}


	$graph->SetMarginColor('darkgreen@0.8');
	switch($type)
	{
		case 3 : 
			$Title=$Title." Temperature en °C";
		break;
		case 4 : 
			$Title=$Title." Preseion en hPa";
		break;
		case 5 : 
			$Title=$Title." Humiditée en %";
		break;
		case 6 : 
			$Title=$Title." Sample par heure";
		break;

	}	
	$graph->title->Set($Title);
	if($bHeure)
	{
		//echo ("heure");
		$graph->xaxis->SetTickLabels($dblarray[HEURE]);
	}
	else
	{
		//echo ("day");
		$graph->xaxis->SetTickLabels($dblarray[DATE]);
	}
	$graph->xaxis->SetLabelAngle(50);

	if($bMinMax)
	{
		$plotMax = new LinePlot($dblarray[MAX]);
		$plotMax->SetColor("blue");
		$plotMax->SetWeight(1);
		if($type==TEMP)
			$plotMax->SetFillGradient('red','yellow');
		else
			if($type==HUMI)
				$plotMax->SetFillGradient('blue','white');
			else	
				$plotMax->SetFillGradient('green','white');
		$graph->Add($plotMax);

		$plotMin = new LinePlot($dblarray[MIN]);
		$plotMin->SetColor("blue");
		$plotMin->SetWeight(1);
		$plotMin->SetFillColor('white');
		$graph->Add($plotMin);
	}
	else
	{	$plot = new LinePlot($dblarray[$type]);
		$plot->SetColor("blue");
		$plot->SetWeight(1);
		if($type==TEMP)
			$plot->SetFillGradient('red','yellow');
		else
			if($type==HUMI)
				$plot->SetFillGradient('blue','white');
			else	
				$plot->SetFillGradient('green','white');
			
		$graph->Add($plot);
	}
	$mgraph->Add($graph,0,0);
	$mgraph->Stroke();
}

function displayBad()
{
	$graph = new Graph(400,220);
	$graph->SetScale('linlin',5,200,0,24);
	$caption=new Text("No data!!!",200,100);
	$graph->AddText($caption); 
	$graph->Stroke() ;

}

function supprimeEnTete($dblarray,$nbMax)
{
	//echo("------supprimeEnTete------<br>");
	$dblTemp= array(array());
	for($lect=0;$lect<$nbMax;$lect++)
	{
		//echo($dblarray[DATE][$lect].";".$dblarray[NB_SAMPLE][$lect].";".$dblarray[MIN][$lect].";".$dblarray[MAX][$lect]."<br>");
		if($dblarray[NB_SAMPLE][$lect]!=0)
		{	
			//premier element trouvé	
			//echo("trouvé");
			break;
		}
	}
	if($lect==$nbMax)
	{
		//aucun element danc le tableau		
		//echo ("aucun element danc le tableau<br>");
		return false;
	}
	for($ecr=0;$lect<$nbMax;$ecr++,$lect++)
	{
		//echo($dblarray[NB_SAMPLE][$lect].$dblarray[DATE][$lect].";".$dblarray[MIN][$lect].";".$dblarray[MAX][$lect]."<br>");		
		
		for($i=0;$i<MAX_FIELD;$i++)
		{			
			$dblTemp[$i][$ecr]=$dblarray[$i][$lect];
		}
	}
	$dblarray=$dblTemp;
	return true;
}



$prof = get_val('prof');
if ( $prof == '' )
{
	$prof=7;
}

$offset = get_val('offset');
if ( $offset == '' )
{
	$offset=0;
}
if($offset <0)
{
	displayBad();
}

$lieu = get_val('lieu');
if ( $lieu == '' )
{
	$lieu=EXTERIEUR;
}

$type = get_val('type');
if ( $type == '' )
{
	$type=TEMP;
}

$title= get_val('title');
if ( $title == '' )
{
	$title='';
}

$bMinMax= get_val('MinMax');
if ( $bMinMax == '' )
{
	$bMinMax=0;
}
else
{
	$bMinMax=1;
}


$dblarray = array(array());
$nbJour=$prof;
$index=0;
$idx=0;

/*function minus($dblarray)
{
	echo("minus");
	$min=$dblarray[0];
	foreach ($dblarray as &$value) 
    	{
		echo($value.";<br>");
		if($value<$min)
		{
			echo("trouve ".$value." ".$min."<br>");
			$min=$value;
		}
	}
	return $min;
}*/

for($prof;$prof>-1;$prof--)
{
	$start=time() - ( $prof * 24 * 60 * 60) - ( $offset * 24 * 60 * 60);
	$prefix=date ("Y-n-j",$start);

	if($bMinMax)
	{
		
		$index=0;
		$dblarrayTemp= array(array());
		readData( $prefix, $lieu,&$index, &$dblarrayTemp,$type);
		
		$count=count($dblarrayTemp[$type]);
		$temp=max($dblarrayTemp[NB_SAMPLE]);
		$min=min($dblarrayTemp[$type]);
		$max=max($dblarrayTemp[$type]);

		if(($temp!=0)&&($min!=ERR_TEMP))
		{
			
			//mesure min et max ok 
			$dblarray[DATE][$idx]=$prefix;
			$dblarray[MIN][$idx]=$min;
			$dblarray[MAX][$idx]=$max;
			$dblarray[NB_SAMPLE][$idx]=$count;
			
			
		}
		else
		{
			//pas de mesure on prend la mesure precedente
			$dblarray[DATE][$idx]=$prefix;
			$dblarray[MIN][$idx]=$dblarray[MIN][$idx-1];
			$dblarray[MAX][$idx]=$dblarray[MAX][$idx-1];
			$dblarray[NB_SAMPLE][$idx]=0;
		}
		$idx++;
		$index=$idx;
	}
	else
	{
		readData( $prefix, $lieu,&$index, &$dblarray,$type);
	}

}


if(supprimeEnTete(&$dblarray,$index)==true)
{
	$label=getStrLieu($lieu)." ".$title;
	if($nbJour==0)
	{
		//echo("heure");
		display($dblarray,$type,$label,1,$bMinMax);
		
	}
	else
	{
		//echo("day");
		display($dblarray,$type,$label,0,$bMinMax);	
		
	}
}
else
{
	displayBad();	
}





?>


