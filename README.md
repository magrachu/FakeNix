# FakeNix

Control a fake nixie clock in the style of an Elekstube-R using an ESP32 controller. Provides wireless networking with pretty basic homeassistant integration using MQTT. You can control color, brightness and color temperature.

In order to use it on the original Elekstube-R clock, you need to remove the original AVR controller board and provide your own ESP controller. Basically, you only need one IO pin to drive the WS2812 like LEDs, so nearly any ESP will do.

A very basic web interface enables you to change preferences and network connections.

