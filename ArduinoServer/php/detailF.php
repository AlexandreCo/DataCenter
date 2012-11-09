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
define("MIN", 4);
define("MAX",5);

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
	
	return $ligne[HEURE];

}



function readData( $prefix, $cpt,$Idx, $dblarray)
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

	$handle = @fopen($file, "r" );
	
	if ($handle)
	{
		while ($ligne = fgetcsv($handle, $taille, $delimiteur)) 
		{

			$lieu=getLieu($ligne);
			if($cpt==$lieu)
			{

				$dblarray[TEMP][$Idx]		=getTemp($ligne);
				$dblarray[HUMI][$Idx]		=getHumi($ligne);
				$dblarray[PRESS][$Idx]		=getPress($ligne);
				$dblarray[DATE][$Idx] 		=getJour($ligne);
				$dblarray[HEURE][$Idx] 		=getHeure($ligne);	
				
				
				if($dblarray[TEMP][$Idx]==ERR_TEMP)
				{
					if($Idx==0)
					{
						$dblarray[TEMP][$Idx]=15;
					}
					else
					{
						$dblarray[TEMP][$Idx]=$dblarray[TEMP][$Idx-1];
					}
				}

				if($dblarray[PRESS][$Idx]==ERR_PRESS)
				{
					if($Idx==0)
					{
						$dblarray[PRESS][$Idx]=1015;
					}
					else
					{
						$dblarray[PRESS][$Idx]=$dblarray[PRESS][$Idx-1];
					}
				}

				if($dblarray[HUMI][$Idx]==ERR_HUMI)
				{
					if($Idx==0)
					{
						$dblarray[HUMI][$Idx]=50;
					}
					else
					{
						$dblarray[HUMI][$Idx]=$dblarray[HUMI][$Idx-1];
					}
				}
				$Idx++;
			}

		}
		fclose($handle);
		
	}
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
	$graph->title->Set($Title);
	if($bHeure)
		$graph->xaxis->SetTickLabels($dblarray[HEURE]);
	else
		$graph->xaxis->SetTickLabels($dblarray[DATE]);
	$graph->xaxis->SetLabelAngle(50);

	if($bMinMax)
	{
		$plotMax = new LinePlot($dblarray[MAX]);
		$plotMax->SetColor("blue");
		$plotMax->SetWeight(1);
		$plotMax->SetFillGradient('red','yellow');
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
for($prof;$prof>-1;$prof--)
{
	$start=time() - ( $prof * 24 * 60 * 60) - ( $offset * 24 * 60 * 60);
	$prefix=date ("Y-n-j",$start);

	if($bMinMax)
	{
		$index=0;
		$dblarrayTemp= array(array());
		readData( $prefix, $lieu,&$index, &$dblarrayTemp);
		
		$count=count($dblarrayTemp[TEMP]);
		if($count!=0)
		{
			$min=min($dblarrayTemp[TEMP]);
			$max=max($dblarrayTemp[TEMP]);
			$dblarray[DATE][$idx]=$prefix;
			$dblarray[MIN][$idx]=$min;
			$dblarray[MAX][$idx]=$max;
			$idx++;
		}

	}

	else
	{
		readData( $prefix, $lieu,&$index, &$dblarray);
	}

}
$label=getStrLieu($lieu)." ".$title;
if($nbJour==0)
	display($dblarray,$type,$label,1,$bMinMax);
else
	display($dblarray,$type,$label,0,$bMinMax);
?>


