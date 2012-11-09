<?php
function getHd1()
{
	exec("df -h| grep var | grep -v none | awk '{ print $5 }'",$resultat);
	return $resultat[0];

}
?>
<HTML>
  <HEAD>
  </HEAD>

  <BODY>
<table>
	<tr>
		<td>
			<table>
				<tr>
					<td>
						<a href="GnOmeDataCenter/viewer3.php" target="iframe">Capteurs</a><br>
					</td>
				</tr>
				<tr>
					<td>
						<a href="capteur.html" target="iframe">Capteurs V2</a><br>
					</td>
				</tr>
				<tr>
					<td>
						<a href="reseau.php" target="iframe">Reseau</a>
					</td>
				</tr>
				<tr>
					<td>
						<a href="phpinfo.php" target="iframe">php</a>
					</td>
				</tr>
			</table>	
		</td>
		<td>
			<iframe align="center" width="1024" height="700" frameborder="1" scrolling="auto" src="Reseau.php" name="iframe"></iframe>
		</td>
	</tr>
</table>






  </BODY>
</HTML>



