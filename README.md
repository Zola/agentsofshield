# agentsofshield 

##  Zola Zhou  zuola.com@gmail.com 

## agentsofshield watch face，麦步表盘，神盾局特工
使用C语言，要先安装麦步SDK，请到 http://dev.maibu.cc/ 下载SDK并注册自己的UUID
这个表盘实现了每分钟更新时间、温度和海拔高度，但由于任何地方的气压并不是处于稳定状态中，高度数据并不准确，计划在将来通过与手机通信获取高度作校正。
计划实现判断蓝牙状态和电量状态

## 神盾局特工表盘修改方法

- 去http://dev.maibu.cc/sdk/watch 下载 Arm 编译环境
http://resource.maibu.cc/resources/sdk/watch/patch/Arm.zip

## 安装ARM编译环境。

- 然后在 http://dev.maibu.cc/sdk/watch  下载麦步手表 SDK 
http://resource.maibu.cc/resources/sdk/watch/MaibuOS_SDK_1.1.zip 

- 解压SDK，得到 V1.1/Pack  , V1.1/Doc   V1.1/Demo三个文件夹


## 然后下载神盾局特工表盘源代码

https://github.com/Zola/agentsofshield/archive/master.zip

## 解压缩后复制里边的 appinfo.json  src   resources 到 SDK里的 Pack文件夹里。

- 然后进入V1.1/Pack/resourcesimages/ 里，用WINDOWS的小画板打开  test.bmp，改成你要的样子，保存为单色位图。

- 双击 V1.1/Pack/build_pack.bat  就会生成一个OUT.MAI 的程序。

##登录微信网页版，用文件传输助手发到自己的手机，在手机上选择用麦步手表打开，就安装成功了。

## 想改源代码就去  src 里改。