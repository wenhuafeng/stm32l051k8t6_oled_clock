基本功能说明：
1，stm32l051k8t6，OLED12864，esp8266_at，si7021。
2，OLED显示时钟、日期，温湿度显示。
3，WiFi通过AT命令通讯，自动更新时间功能。
4，有调试log输出，及一个运行LED闪烁。

编译说明：
1，进入source目录。
2，用gcc编译，执行“python build.py g”编译。
3，用mdk编译，执行“python build.py m”编译。
4，用jlink下载程序，执行“python build.py j”下载。
5，用stlink下载程序，需要用到openocd，执行“python build.py stlink”下载。
