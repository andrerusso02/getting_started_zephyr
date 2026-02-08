# Ethernet over USB
```
west build -p auto -b stm32h750b_dk/stm32h750xx/ext_flash_app --sysbuild workspace/getting_started/dumb_http_server/ -- -DEXTRA_DTC_OVERLAY_FILE="ethernet-usbd.overlay" -DEXTRA_CONF_FILE="overlay-ethernet-usbd.conf"
```

# Native Ethernet
```
west build -p auto -b stm32h750b_dk/stm32h750xx/ext_flash_app --sysbuild workspace/getting_started/dumb_http_server/ -- -DEXTRA_DTC_OVERLAY_FILE="ethernet-native.overlay" -DEXTRA_CONF_FILE="overlay-ethernet-native.conf"
```