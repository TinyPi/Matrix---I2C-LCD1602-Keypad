仅测试使用，bug 很多

实现功能: 手机端发送 字符串 到 开发板LCD 上显示

使用方法：
1、复制 ./android-clinet 目录下的 client 文件到 system/bin/
2、复制 ./nanopi-server 目录下的 matrix-lcd1602_keypad 到开发板的 /usr/bin/

3、开发板上电，执行 /usr/bin/matrix-lcd1602_keypad
   提示 waiting key press... 此时按下 F1, 开发板将显示 IP 和 端口号
   
4、手机端 
   adb shell
   cd system/bin/
   ./client   
   
5、此时手机端成功连接上开发板，手机端提示 Welcome to nanopi
6、手机端提示 Enter string to send: 此时输入显示字符，如 hello，开发板显示屏将显示hello


