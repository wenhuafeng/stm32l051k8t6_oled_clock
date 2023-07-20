一，基本功能说明：
1，stm32l051k8t6，OLED12864，esp8266_at，si7021。
2，OLED显示时钟、日期，温湿度显示。
3，WiFi通过AT命令通讯，自动更新时间功能。
4，有调试log输出，可配置为UART打印，可配置为RTT打印。
5，一个运行LED闪烁。

二，编译、下载、RTT、putty，使用说明：
1，进入source目录。
2，用gcc编译，执行“python build.py g”编译。
3，用mdk编译，执行“python build.py ms”编译。
4，用jlink下载固件，执行“python build.py js”下载。
5，用daplink下载固件，执行“python build.py daplink”下载。
6，用stlink下载固件，执行“python build.py stlink”下载。
7，用putty查看openocd daplink RTT LOG，执行命令“python build.py daplink_rtt”。
8，用putty查看openocd stlink RTT LOG，执行命令“python build.py stlink_rtt”。
9，第7、8步后，打开putty，选择telnet，输入IP:172.0.0.1，端口:8888(命令内端口号rtt server start 8888 0)