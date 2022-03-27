import board
import adafruit_ahtx0
from datetime import datetime

html_dir = "/var/www/html/temp"
data_fname = "temperature.csv"
html_fname = "index.html"

sensor = adafruit_ahtx0.AHTx0(board.I2C())

temp = sensor.temperature
hum  = sensor.relative_humidity
now = datetime.now()

# open data file
history = []

with open(html_dir + "/" + data_fname, "r") as f:
	for l in f.readlines():
		fields = l.split(";")
		dt = datetime.strptime(fields[0], "%d/%m/%y %H:%M")
		data = [dt, float(fields[1]), float(fields[2])]
		history.append(data)

history.append([now, temp, hum])

t_data = []
h_data = []
dt_data= []

for h in history:
	print(h[0])
	t_data.append("{:.1f}".format(h[1]))
	h_data.append("{:.1f}".format(h[2]))
	dt_data.append(h[0].strftime("'%Y-%m-%d %H:%m'"))
print(t_data)
print(h_data)
print(dt_data)

with open(html_dir + "/" + data_fname, "a") as f:
	dt = now.strftime("%d/%m/%y %H:%M")
	f.write("{};{};{}\n".format(dt, temp, hum))

head="""<html>
<head>
<title>Temperature</title>
</head>
<body>\n"""
body1 = """
<div id="plotly-chart"></div>
<script type=\"text/javascript\" src=\"./plotly-latest.min.js\"></script>
<script type=\"text/javascript\" >
var temp = {
  x: ["""
body2="],\n  y: ["
body3="""],
  name: 'temperature',
  type: 'scatter'
};

var hum = {
  x: ["""
body4="],\n  y: ["
body5="""],
  yaxis: 'y2',
  name: 'humidity',
  type: 'scatter'
};

var data = [temp, hum];

var layout = {
  title: 'Double Y Axis Example',
  yaxis: {title: 'Temperature'},
  yaxis2: {
    title: 'Humidity',
    overlaying: 'y',
    side: 'right'
  }
};

Plotly.newPlot('plotly-chart', data, layout);
</script>
"""

tail="</body></html>\n"

with open(html_dir + "/" + html_fname, "w") as f:
	f.write(head)
	f.write(body1)
	f.write(",".join(dt_data))
	f.write(body2)
	f.write(",".join(t_data))
	f.write(body3)
	f.write(",".join(dt_data))
	f.write(body4)
	f.write(",".join(h_data))
	f.write(body5)
	f.write(tail)

		
