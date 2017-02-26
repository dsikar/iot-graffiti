<!DOCTYPE html>

<html>
    <head>
        <meta charset = "utf-8">

        <script>
            Element.prototype.leftTopScreen = function () {
                var x = this.offsetLeft;
                var y = this.offsetTop;

                var element = this.offsetParent;

                while (element !== null) {
                    x = parseInt (x) + parseInt (element.offsetLeft);
                    y = parseInt (y) + parseInt (element.offsetTop);

                    element = element.offsetParent;
                }

                return new Array (x, y);
            }

            document.addEventListener ("DOMContentLoaded", function () {
                var flip = document.getElementById ("flip");

                var xy = flip.leftTopScreen ();

                var context = flip.getContext ("2d");

                context.fillStyle = "rgb(255,255,255)";   
                context.fillRect (0, 0, 500, 500);

                flip.addEventListener ("mousemove", function (event) {
                    var x = event.clientX;
                    var y = event.clientY;
					
					document.getElementById("x").innerHTML = x;
					document.getElementById("y").innerHTML = y;
					
					var iPress = mousePressed();
					if(iPress) 
					{
						context.fillStyle = "rgb(0, 0, 255)";  
						context.fillRect (x - xy[0], y - xy[1], 5, 5);
					}
					httpGetAsync(x, y, iPress, httpStatus);
					
                });
            }); 

			function clearCanvas()
			{
				var canvas = document.getElementById('flip'),
				ctx = canvas.getContext("2d");
				ctx.clearRect(0, 0, canvas.width, canvas.height);
			}		

			function mousePressed()
			{
				var iPress = event.button;
				document.getElementById("mouse").innerHTML = iPress;
				return iPress;
			}
			
			function httpGetAsync(x, y, iPressed, callback)
			{
				var theUrl = "graffiti.php?x=" + x + "&y=" + y + "&p=" + iPressed + "&action=browser";
				var xmlHttp = new XMLHttpRequest();
				xmlHttp.onreadystatechange = function() { 
					if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
						callback(xmlHttp.responseText);
				}
				callback(theUrl);
				xmlHttp.open("GET", theUrl, true); // true for asynchronous 
				xmlHttp.send(null);
				
			}
			
			function httpStatus(strHttpMsg)
			{
				document.getElementById("httpMsg").innerHTML = strHttpMsg;
			}

        </script>

        <style>
            #flip {
                border: 1px solid black;
                display: inline-block;

            }

            body {
                text-align: center;
            }
        </style>
    </head>

    <body>
		<table>
		 <tr>
			<td width=100 valign=top>
				<button onclick="clearCanvas()">CLEAR</button>
				<p id="x">value of x</p>
				<p id="y">value of x</p>
				<p id="mouse">Mouse State</p>
			</td>
			<td>
				<canvas id = "flip" width = "1310" height = "800">This text is displayed if your browser does not support HTML5 Canvas.</canvas>
			</td>
		<tr>
		</table>
		<p id="httpMsg">HTTP Response</p>
    </body>
	
</html>
