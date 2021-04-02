const char html_page[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>
        Rader - esp8266
    </title>
    <style>
        #rader {
            border: 1px solid black;
        }
    </style>
</head>

<body onload='init_rader();'>
    <h1>Rader</h1>
    <canvas id='rader' width='800px' height='400px'></canvas>
    <script>
        const max_dist = 4000;
        let canvas = document.getElementById('rader');
        var ctx = canvas.getContext('2d');
        var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
        connection.onopen = function () {
            connection.send('Connect ' + new Date());
        };
        connection.onerror = function (error) {
            console.log('WebSocket Error ', error);
        };
        connection.onmessage = function (e) {
            console.log('Server: ', e.data);
            const obj = JSON.parse(e.data);
            draw_rader_beam(parseInt(obj.deg), parseInt(obj.dist));
        };
        function init_rader() {
            ctx.fillStyle = 'rgb(0,255,0)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            ctx.fillStyle = 'rgba(0,0,0,0.05)';
            for (let index = 0; index < 100; index++) {
                ctx.fillRect(0, 0, canvas.width, canvas.height);
            }
        }
        function draw_rader_beam(deg, dist) {
            let x = Math.cos(deg / 180 * Math.PI) * dist / max_dist * canvas.width / 2;
            x += (canvas.width / 2);
            let y = Math.sin(deg / 180 * Math.PI) * dist / max_dist * canvas.height;
            y = canvas.height - y;
            ctx.fillStyle = 'rgba(0,0,0,0.05)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            ctx.strokeStyle = 'rgb(0,255,0)';
            ctx.lineWidth = 30;
            ctx.beginPath();
            ctx.moveTo(canvas.width/2, canvas.height);
            ctx.lineTo(x, y);
            ctx.stroke();
        }
    </script>
</body>


</html>
)";