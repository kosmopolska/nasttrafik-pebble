vasttrafik-pebble
=================

Pebble app for Västtrafik


![ScreenShot](/images/screenshot.png)

Right now only shows one line in two directions, which is configured in pebble-js-app.js.

The pebble-js-app.js-sample is a sample file with my API key removed.

To change station change the IDs in
```
response = HTTPGET("http://api.vasttrafik.se/bin/rest.exe/v1/departureBoard?authKey=<API-KEY-HERE>&format=json&id=9021014007300000&direction=9021014002025000");
```

id is departure station (here Vasaplatsen) and direction is one station (here Danaplatsen) you want the bus/tram to pass.

To change what line to show change the number in
```
for(var i=0;i<departures.length;i++)
      if(departures[i].sname==="16"){index=i;break;}
```

This code means that bus 16 will be shown.

To install, just run pebble build and pebble install.