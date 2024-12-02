# FakeNix

Control a fake nixie clock in the style of an Elekstube-R using an ESP32 controller. Provides wireless networking with pretty basic homeassistant integration using MQTT. You can control color and brightness.

In order to use it on the original Elekstube-R clock, you need to remove the original AVR controller board and provide your own ESP controller. Basically, you only need one IO pin to drive the WS2812 like LEDs, so nearly an ESP will do.
