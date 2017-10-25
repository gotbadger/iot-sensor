# iot-sensor
IoT Sensor for ESP8266 with DHT22 sensor

## Features
- mDNS / Zeroconfig for discovery
- captive portal config with [WiFiManager](https://github.com/tzapu/WiFiManager#quick-start)
- JSON interface

## mDNS Discovery Example (OSX)
`dns-sd -B _sensor._tcp .`

```
Browsing for _sensor._tcp
DATE: ---Tue 06 Jun 2017---
22:07:10.668  ...STARTING...
Timestamp     A/R    Flags  if Domain               Service Type         Instance Name
22:07:11.897  Add        2   4 local.               _sensor._tcp.        sensor-d6c3c4
```

## API Example
`curl sensor-d6c3c4.local/dht`                                                              
```
{ 
  "error": false,
  "temp": 28.30,
  "humidity":99.90
}
```

Error is set to `true` when there is a problem communicating with the DHT module

## Data collection

Companion program https://github.com/gotbadger/mdns_data_logger can be used to store data in InfluxDB allowing easy graphing in https://grafana.com/
