#include <stdio.h>
#include <zephyr/kernel.h>

#include <zephyr/net/socket.h>
#include <arpa/inet.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <vl53l5cx.h>


/* Custom attribute and channel definitions (matching your driver) */
#define SENSOR_ATTR_VL53L5CX_RANGING_STATE SENSOR_ATTR_PRIV_START
#define SENSOR_CHAN_VL53L5CX_DISTANCE_BASE SENSOR_CHAN_PRIV_START


#define DEST_ADDR "255.255.255.255" // Linux PC IP (broadcast)
#define SERVER_PORT 5005

struct range_image {
    float num_zones ;
    float distances[64];
};

int vl53l5cx_get_range_array(const struct device *dev, struct sensor_value buff, int num_zones, float *output_array) {
    for (int i = 0; i < num_zones; i++) {
        sensor_channel_get(dev, (enum sensor_channel)(SENSOR_CHAN_VL53L5CX_DISTANCE_BASE + i), &buff);
        output_array[i] = buff.val1 + buff.val2 / 1000.0f;
    }
    return 0;
}

int main(void)
{

    int sock;
    struct sockaddr_in destination_addr;

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        return 0;
    }

    // Configure destination address
    destination_addr.sin_family = AF_INET;
    destination_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, DEST_ADDR, &destination_addr.sin_addr);

    const struct device *dev = DEVICE_DT_GET_ANY(st_vl53l5cx);
    struct sensor_value enable = { .val1 = 1 };
    struct sensor_value buff_sensor_value;
    struct range_image img;
	int ret;

    if (!device_is_ready(dev)) {
        printf("Sensor device not ready\n");
        return 0;
    }

    ret = sensor_attr_set(dev, SENSOR_CHAN_ALL, (enum sensor_attribute)SENSOR_ATTR_VL53L5CX_RANGING_STATE, &enable);
    if (ret != 0) {
        printf("Failed to start ranging: %d\n", ret);
        return 0;
    }

    printf("VL53L5CX Ranging Started...\n");

    buff_sensor_value = (struct sensor_value){0};
	ret =sensor_attr_get(dev, SENSOR_CHAN_ALL, SENSOR_ATTR_RESOLUTION, &buff_sensor_value);
    if (ret != 0) {
        printf("Failed to get resolution: %d\n", ret);
		return 0;
    }
	int num_zones = buff_sensor_value.val1;

	buff_sensor_value = (struct sensor_value){0};
	ret = sensor_attr_get(dev, SENSOR_CHAN_ALL, SENSOR_ATTR_SAMPLING_FREQUENCY, &buff_sensor_value);
	if (ret != 0) {
		printf("Failed to get sampling frequency: %d\n", ret);
		return 0;
	}
	int freq_hz = buff_sensor_value.val1;

	printf("VL53L5CX Resolution: %dx%d (%d zones)\n", 
			 num_zones == 64 ? 8 : 4, 
			 num_zones == 64 ? 8 : 4, 
			 num_zones);
	printf("VL53L5CX Sampling Frequency: %d Hz\n", freq_hz);

    while (1) {
        ret = sensor_sample_fetch(dev);
        if (ret != 0) {
            printf("Fetch failed: %d\n", ret);
            k_msleep(1000/freq_hz);
            continue;
        }

        vl53l5cx_get_range_array(dev, buff_sensor_value , num_zones, img.distances);
        img.num_zones = num_zones;

        sendto(sock, &img, sizeof(img), 0, (struct sockaddr *)&destination_addr, sizeof(destination_addr));
        k_msleep(1000/freq_hz);
    }

    return 0;
}