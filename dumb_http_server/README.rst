# Ethernet over USB
```
west build -p auto -b stm32h750b_dk/stm32h750xx/ext_flash_app --sysbuild workspace/getting_started/dumb_http_server/ -- -DEXTRA_DTC_OVERLAY_FILE="ethernet-usbd.overlay" -DEXTRA_CONF_FILE="overlay-ethernet-usbd.conf"
```

# Native Ethernet
```
west build -p auto -b stm32h750b_dk/stm32h750xx/ext_flash_app --sysbuild workspace/getting_started/dumb_http_server/ -- -DEXTRA_DTC_OVERLAY_FILE="ethernet-native.overlay" -DEXTRA_CONF_FILE="overlay-ethernet-native.conf"
```

# PPP over USART

```
west build -p always -b nucleo_f401re workspace/getting_started/dumb_http_server/ -DEXTRA_CONF_FILE="overlay-ppp-usart.conf" 
```

```
sudo pppd /dev/ttyACM1 115200 192.0.2.2:192.0.2.1 local noauth passive nodetach nocrtscts local dump debug
```