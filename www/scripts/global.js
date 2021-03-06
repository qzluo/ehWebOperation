function getHttp()
{
	var xmlhttp;
	if (window.XMLHttpRequest) // code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	else // code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");

	return xmlhttp;
}

function addEvent(event)
{
	var oldLoad = document.onload;
	if (!oldLoad) {
		document.onload = event();	
	}
	else {
		document.onload = function() {
			oldLoad();
			event();
		}
	}
}

function readDev(whichDev)
{
	if (!document.getElementsByTagName)
		return false;

	//取得地址
	var img = whichDev.getElementsByTagName("img");
	var devno = img[0].getAttribute("title");
	if (!devno)
		return false;

	//请求
	var xmlhttp = getHttp();
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState==4 && xmlhttp.status==200) {
			var rsp = xmlhttp.responseText;
			if (rsp > 0) 
				img[0].setAttribute("src", "images/lampOn.ico");
			else
				img[0].setAttribute("src", "images/lampOff.ico");
		}
	}

	var str="addr=" + devno;
	xmlhttp.open("POST","/cgi-bin/readdev.cgi", true);
	xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded"); 
	xmlhttp.send(str);
}

function writeDev(whichDev)
{
	if (!document.getElementsByTagName)
		return false;

	//取得地址
	var img = whichDev.getElementsByTagName("img");
	var devno = img[0].getAttribute("title");
	if (!devno)
		return false;

	//取得操作数
	var src = img[0].getAttribute("src");
	var value = 0;
	if (src.match("lampOn"))
		value = 0;
	else
		value = 1;

	var xmlhttp = getHttp();
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState==4 && xmlhttp.status==200) {
			readDev(whichDev);
		}
	}

	str="addr=" + devno + "&value=" + value;
	xmlhttp.open("POST","/cgi-bin/writedev.cgi", true);
	xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded"); 
	xmlhttp.send(str);
}

function prepareLink()
{
	if (!document.getElementsByTagName)
		return false;

	if (!document.getElementById)
		return false;

	var devices = document.getElementById("devices");
	if (!devices)
		return false;

	var devlinks = devices.getElementsByTagName("a");
	for (var i = 0; i < devlinks.length; i++) {
		devlinks[i].onclick = function() {
			writeDev(this);
			return false;
		}
		
		//读设备
		readDev(devlinks[i]);
	}
}

addEvent(prepareLink);
