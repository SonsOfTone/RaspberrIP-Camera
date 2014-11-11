<?php
if(isset($_POST['SubmitCheck'])) {
    // The form has been submited
    // Check the values!
    
  $video [] = array( 
  'hres' => $_POST['HRES'], 
  'vres' => $_POST['VRES'], 
  'bitrate' => $_POST['BITRATE'], 
  'gopsize'	=> $_POST['GOPSIZE'],
  'shutter' => $_POST['SHUTTER'], 
  'fps'	=> $_POST['FPS']
  );
  $network [] = array( 
  'rtspport' => $_POST['RTSPPORT']
  ); 
  /*$raspberripcamconfig [] = array( 
  'name' => 'Claud', 
  'age' => '20', 
  'salary' => "$2000" 
  );*/ 
   
  $doc = new DOMDocument(); 
  $doc->formatOutput = true; 
   
  $r = $doc->createElement( "raspberripcamconfig" ); 
  $doc->appendChild( $r ); 
   
  foreach($video as $video ) 
  { 
  $b = $doc->createElement( "video" ); 
   
  $hres = $doc->createElement( "hres" ); 
  $hres->appendChild( 
  $doc->createTextNode( $video['hres'] ) 
  ); 
  $b->appendChild( $hres ); 
   
  $vres = $doc->createElement( "vres" ); 
  $vres->appendChild( 
  $doc->createTextNode( $video['vres'] ) 
  ); 
  $b->appendChild( $vres ); 
   
  $bitrate = $doc->createElement( "bitrate" ); 
  $bitrate->appendChild( 
  $doc->createTextNode( $video['bitrate'] ) 
  ); 
  $b->appendChild( $bitrate ); 
  
  $gopsize = $doc->createElement( "gopsize" ); 
  $gopsize->appendChild( 
  $doc->createTextNode( $video['gopsize'] ) 
  ); 
  $b->appendChild( $gopsize );   
    
  $shutter = $doc->createElement( "shutter" ); 
  $shutter->appendChild( 
  $doc->createTextNode( $video['shutter'] ) 
  ); 
  $b->appendChild( $shutter ); 
  
  $fps = $doc->createElement( "fps" ); 
  $fps->appendChild( 
  $doc->createTextNode( $video['fps'] ) 
  ); 
  $b->appendChild( $fps );
   
  $r->appendChild( $b ); 
  }
  
  foreach($network as $network ) 
  { 
  $b = $doc->createElement( "network" ); 
   
  $rtspport = $doc->createElement( "rtspport" ); 
  $rtspport->appendChild( 
  $doc->createTextNode( $network['rtspport'] ) 
  ); 
  $b->appendChild( $rtspport ); 
      
  $r->appendChild( $b ); 
  } 
   
  $doc->saveXML();
  $doc->save("config.xml");
  
  chdir('/home/pi/RaspberrIP-Camera/');
  shell_exec("pkill RaspberrIPCam");
  shell_exec("./RaspberrIPCam -o - >/dev/null &");
  chdir($old_path);
  header("location: index.php");
}
else {
    // The form has not been posted
    // Show the form
?>
	<!DOCTYPE html>
	<html>
	<head>
	  <link rel="stylesheet" href="style.css">
	</head>
	<body class="body"></div>
	<img class="logo" src="logo_medium.png"  width="138" height="125">
	<div class="title">RaspberrIP Camera</div>
	<form id="Form1" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post">
		<div class="category1">
			Video Setting : <br>
		</div>
		<div class="items1">
			Horizontal Resolution<br>
			Vertical Resolution<br>
			Bitrate<br>
			Gop Size<br>
			Shutter<br>
			Framerate<br>
		</div>
		<div class="category2">
			Network Setting :</div>
		<div class="items2">
			RTSP Port<br>
		</div>
		<div class="inputs1">
			<input class="inputstyle" type="text" id="HRES" name="HRES"><br>
			<input class="inputstyle" type="text" id="VRES" name="VRES"><br>
			<input class="inputstyle" type="text" id="BITRATE" name="BITRATE"><br>
			<input class="inputstyle" type="text" id="GOPSIZE" name="GOPSIZE"><br>
			<input class="inputstyle" type="text" id="SHUTTER" name="SHUTTER"><br>
			<input class="inputstyle" type="text" id="FPS" name="FPS"><br>
		</div>
		<div class="inputs2">
			<input class="inputstyle" type="text" id="RTSPPORT" name="RTSPPORT"><br>
		</div>
		<input class="buttonstyle" type="hidden" name="SubmitCheck" value="sent">
		<input class="buttonstyle" type="Submit" name="Form1_Submit" value="Save">
	</form>
	</body>
	</html>
	
	<script>
		if (window.XMLHttpRequest)
		  {// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
		  }
		else
		  {// code for IE6, IE5
		  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
		  }
		xmlhttp.open("GET","config.xml",false);
		xmlhttp.send();
		xmlDoc=xmlhttp.responseXML; 

		var x=xmlDoc.getElementsByTagName("video");

		var elem = document.getElementById("HRES");
		elem.value = x[0].getElementsByTagName("hres")[0].childNodes[0].nodeValue;

		var elem = document.getElementById("VRES");
		elem.value = x[0].getElementsByTagName("vres")[0].childNodes[0].nodeValue;

		var elem = document.getElementById("BITRATE");
		elem.value = x[0].getElementsByTagName("bitrate")[0].childNodes[0].nodeValue;

		var elem = document.getElementById("GOPSIZE");
		elem.value = x[0].getElementsByTagName("gopsize")[0].childNodes[0].nodeValue;

		var elem = document.getElementById("SHUTTER");
		elem.value = x[0].getElementsByTagName("shutter")[0].childNodes[0].nodeValue;

		var elem = document.getElementById("FPS");
		elem.value = x[0].getElementsByTagName("fps")[0].childNodes[0].nodeValue;
		
		var x=xmlDoc.getElementsByTagName("network");
		
		var elem = document.getElementById("RTSPPORT");
		elem.value = x[0].getElementsByTagName("rtspport")[0].childNodes[0].nodeValue;
	</script>
<?php
}
?>
 