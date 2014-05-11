vasttrafik-pebble
=================

Pebble app for Västtrafik


![ScreenShot](/images/screenshot.png)

A simple app that right now shows all departures for a specific station

The src/js/pebble-js-app.js-sample is a sample file with my API key removed.

To change station change the ID in
```
var response = HTTPGET("http://api.vasttrafik.se/bin/rest.exe/v1/departureBoard?" +
            "authKey=<API-KEY-HERE>&format=json" +
            "&id=9021014007300000&timeSpan=60&maxDeparturesPerLine=1");
```
in src/js/pebble-js-app.jjs-sample
where id is departure station (here Vasaplatsen).

To install, just run pebble build and pebble install.