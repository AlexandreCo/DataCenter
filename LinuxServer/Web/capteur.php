
<html>
<body style="background-color:black;">
<center><p style="font-family:verdana;color:white;">
GnOme Data Center
Memoire  
<?php
	exec("df -h| grep var | grep -v none | awk '{ print $5 }'",$resultat);
	echo $resultat[0];
?>
</p>

<table style="font-family:verdana;color:white;" align="center" border=1>
<tr>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp/0_1_data.txt.png">
				<img src="GnOmeDataCenter/out/temp/0_1_data.txt.png" alt="Temperature exterieure actuelle" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature exterieure actuelle
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp/0_7_data.txt.png">
				<img src="GnOmeDataCenter/out/temp/0_7_data.txt.png" alt="Temperature exterieure des sept derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature exterieure des sept derniers jours
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp/0_30_data.txt.png">
				<img src="GnOmeDataCenter/out/temp/0_30_data.txt.png" alt="Temperature exterieure des trente derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature exterieure des trente derniers jours
			</p>
		</div>
	</td>
</tr>






<tr>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp/11_1_data.txt.png">
				<img src="GnOmeDataCenter/out/temp/11_1_data.txt.png" alt="Temperature du salon actuelle" height="200" width="300">
			</a>
			<p class="todo">
				Temperature du salon actuelle
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp/11_7_data.txt.png">
				<img src="GnOmeDataCenter/out/temp/11_7_data.txt.png" alt="Temperature du salon des sept derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature du salon des sept derniers jours
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp/11_30_data.txt.png">
				<img src="GnOmeDataCenter/out/temp/11_30_data.txt.png" alt="Temperature du salon des trente derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature du salon des trente derniers jours
			</p>
		</div>
	</td>
</tr>






<tr>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp2/21_1_data.txt.png">
				<img src="GnOmeDataCenter/out/temp2/21_1_data.txt.png" alt="Temperature du Bureau actuelle" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature du Bureau actuelle
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp2/21_7_data.txt.png">
				<img src="GnOmeDataCenter/out/temp2/21_7_data.txt.png" alt="Temperature du Bureau des sept derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature du Bureau des sept derniers jours
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/temp2/21_30_data.txt.png">
				<img src="GnOmeDataCenter/out/temp2/21_30_data.txt.png" alt="Temperature du Bureau des trente derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Temperature du Bureau des trente derniers jours
			</p>
		</div>
	</td>
</tr>









<tr>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/press/21_1_data.txt.png">
				<img src="GnOmeDataCenter/out/press/21_1_data.txt.png" alt="Pression actuelle" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Pression actuelle
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/press/21_7_data.txt.png">
				<img src="GnOmeDataCenter/out/press/21_7_data.txt.png" alt="Pression des sept derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Pression des sept derniers jours
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/press/21_30_data.txt.png">
				<img src="GnOmeDataCenter/out/press/21_30_data.txt.png" alt="Pression des trente derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Pression des trente derniers jours
			</p>
		</div>
	</td>
</tr>










<tr>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/humi/21_1_data.txt.png">
				<img src="GnOmeDataCenter/out/humi/21_1_data.txt.png" alt="Humidite actuelle" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Humidite actuelle
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/humi/21_7_data.txt.png">
				<img src="GnOmeDataCenter/out/humi/21_7_data.txt.png" alt="Humidite des sept derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Humidite des sept derniers jours
			</p>
		</div>
	</td>
	<td>
		<div align=center>
			<a href="GnOmeDataCenter/out/humi/21_30_data.txt.png">
				<img src="GnOmeDataCenter/out/humi/21_30_data.txt.png" alt="Humidite des trente derniers jours" height="200" width="300">
			</a>
			<p class="todo" align=center>
				Humidite des trente derniers jours
			</p>
		</div>
	</td>
</tr>
</table>
</p>
</body>
</html>
