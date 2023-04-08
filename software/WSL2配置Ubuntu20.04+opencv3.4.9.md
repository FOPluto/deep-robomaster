# WSL2配置Ubuntu20.04+opencv3.4.9

注1：Ubuntu命令行中的粘贴是点击右键

注2：配置结果：在WSL中

​  实现Ubuntu20.04的配置

​  实现opencv3.4.9+opencv_contrib的配置

​  实现Ubuntu换国内源（阿里云）

​  实现Cmake，C++相关内容的安装

​  实现工摄驱动的安装（此处调用章睿彬大佬的脚本完成驱动安装）

注3：尽量保持配置过程的网络通畅，否则可能出现不可预料的错误

## 1.对windows设置

==为确保指令的有效性，建议以下在windows终端运行的指令，都要在管理员模式打开的终端/powershell中运行==

方法一：

在Windows终端中执行指令

~~~powershell
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
~~~

方法二：

打开：控制面板->程序->启用或关闭Windows功能

将==适用于Linux的windows子系统和虚拟机平台两项勾选==

## 2.WSL

在windows的cmd或者powershell中输入

~~~powershell
wsl --install
wsl --set-default-version 2
# 第二条指令设置wsl2为默认版本
~~~

==注意执行上述指令之后需要重启计算机==

## 3.安装Ubuntu20.04子系统

### 1.下载

打开MicroSoft Store，搜索Ubuntu，选择Ubuntu20.04，获取

### 2.自定义子系统镜像位置

将Ubuntu子系统放在C盘以外的盘符下

在MicroSoft Store安装结束之后，先不要打开Ubuntu，在目标盘符新建一个文件夹，然后在C:\Program Files\WindowsApps\路径下，==windowsApps文件夹可能会提示需要权限，参照<https://jingyan.baidu.com/article/9c69d48f4aad6a52c8024e7c.html> 进行设置即可打开==

在WindowsApps文件夹下找CanonicalGroupLimited开头的文件（会有多个，但是只有一个文件夹里面有ubuntu.exe文件，一般来说是第四个文件夹），将其中的所有内容复制到目标文件夹

在目标文件夹下双击运行Ubuntu.exe

### 3.正常情况

打开Ubuntu子系统，会提示等待一会儿，然后要求输入账户和密码

==密码需要输入两遍，并且输入密码的时候不会有显示，不要以为出错了==

输入结束之后，就可以进入系统了

### 4.不正常情况

==错误1==

~~~powershell
Error code:Wsl/Service/Createlnstance/"ERROR_FILE_NOT_FOUND'
~~~

这种情况原因未知，在windows的终端中运行

~~~powershell
wsl -l
~~~

查看已经存在的子系统

~~~powershell
wsl --unregister 子系统名称
~~~

然后重新启动Ubuntu子系统即可解决

==错误2==

~~~powershell
WslRegisterDistribution failed with error: 0x80071772
~~~

这个错误是因为Ubuntu子系统安装位置错误

将原先下载好的Ubuntu程序卸载，重新下载安装，即可解决

==其他错误==

见：<https://opsdev.blog.csdn.net/article/details/106862520?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-106862520-blog-122514626.pc_relevant_multi_platform_whitelistv6&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-106862520-blog-122514626.pc_relevant_multi_platform_whitelistv6&utm_relevant_index=1>

的2.1.8部分

==如果还是无法解决，建议谷歌或者其他，CSDN真不行==

可以在windows终端中运行指令

~~~shell
wsl --list --verbose
~~~

如果VERSION显示为2，即为WSL2.

## 4.使用opencv.sh脚本进行环境配置

### 1.前置知识

在Ubuntu子系统中

~~~shell
cd /mnt
~~~

wsl将windows下的盘符都归到了这个目录下，所以进入这个目录下就可以直接调用windows中的文件。例如

~~~shell
cd /mnt/e
~~~

就是Windows的E盘的位置

自己选择一个位置，创建工作目录

### 2.运行脚本

下载附件中的opencv.sh，本示例将脚本文件存于E:/mybash下

打开Ubuntu子系统

~~~shell
cd /mnt/e/mybash
# 改变opencv.sh的权限
sudo chmod +x opencv.sh
# 运行opencv.sh
sudo sh opencv.sh
~~~

等待一段时间，约十几（或者更长，期间必须保证网络畅通）分钟

直到出现以下字样即为成功

~~~shell
gdb,success!
opencv,success!
Please  restart system  now!!!
~~~

==重启电脑完成配置==

### 3.错误处理

经过个人测试可行，不会出问题，如果中途出现意外错误，可以打开Windows终端，输入指令删除系统镜像。

~~~shell
wsl --unregister Ubuntu-20.04
~~~

然后打开最初在自定义位置存放的Ubuntu子系统文件夹，双击运行Ubuntu2004.exe重新生成系统镜像，重复以上操作。

==如果重复之后在同一个位置出现错误，请尽快联系本人，QQ：1642075736==

## 5.限制WSL内存占用

WSL在运行的时候会占据大量的内存空间，如果觉得内存爆满影响正常的使用，可以参照下面的博客进行设置

~~~shell
https://www.cnblogs.com/coding8832/p/14445835.html
~~~

# WSL2+VS Code+CMake环境搭建

## 安装VS Code

1.Windows环境下安装VS Code

连接如下

~~~http
https://code.visualstudio.com/
~~~

安装过程自己解决。

在WSL任意目录下运行指令

~~~shell
code .
~~~

系统会自动完成依赖的安装，并且打开Windows环境下的VS Code

==安装插件  ：WSL（在宿主机，即windows中安装）==

==C/C++，C/C++ Extension Pack，CMake，CMake Tools，（选择为Ubuntu20.04安装）==

## 配置VS Code

1.VS Code的配置文件层级

<img src="https://upload-images.jianshu.io/upload_images/24563956-3a02d30da47b6b49.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" alt="img" style="zoom:67%;" />

因此最基本的编译器路径和智能代码提示功能的配置文件需要放在最外层的文件夹中。

而每个项目的特殊配置文件可以放在当前目录的.vscode文件夹中。

2.**c_cpp_properties.json :** 编译器路径和智能代码提示 配置文件 ;

在工作区，按Ctrl+Shift+P，搜索C/C++:Edit Configuration，此配置文件需要放置在总的工作空间文件夹下。

~~~json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include/x86_64-linux-gnu/sys",
                "/usr/local/include/opencv",
                "/usr/local/include/opencv2"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "gnu17",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
~~~

3.**tasks.json :** 编译器构建 配置文件 ;

首先新建一个cpp文件，然后在其中按Ctrl+Shift+P，搜索C/C++:添加调试配置，可以在当前目录下自动生成.vscode文件夹，其中自动包括tasks.json和launch.json

tasks.json脚本定义了一系列的自动化工具，使得在使用调试/运行功能的时候自动调用CMake对项目进行构建。

~~~json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "mkdir",
            "type": "shell",
            "command": "mkdir",
            "args": [
                "-p",
                "build"
            ],
        },
        {
            "label": "cmake",     // 命令的别名
            "type": "shell",         
            "command": "cmake",    // 命令
            "args": [                         // 参数列表
                ".."
            ],
            "options": {
                "cwd": "${workspaceFolder}/build", // 该命令的工作目录
            },
            "dependsOn": [   // 该命令执行前应该执行的命令
                "mkdir" 
            ]
        },
        {
            "label": "make",
            "type": "shell",
            "command": "make",
            "args": [],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "options": {
                "cwd": "${workspaceFolder}/build",
            },
            "dependsOn": [
                "cmake"//make命令执行前先执行cmake命令
            ]
        }
    ]
}

~~~

4.**launch.json :** 调试器设置 配置文件 ;

调试过程需要的配置文件。==使用DEBUG功能的前提是安装了gdb，安装脚本会自动安装，安装目录在/usr/bin/下==

==注意，按照当前的配置信息，CMakeLists.txt中目标代码生成的后得到的文件名，必须与项目文件夹名字相同才可以正常启动DEBUG。如需自定义，可以修改launch.json文件的”program“属性，将最后的${workspaceFolderBasename}替换为自定义的CMake生成的目标文件名==

~~~json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "C/C++: gcc-9 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            // "program": "${workspaceFolder}/build/demo",
            "program": "${workspaceFolder}/build/${workspaceFolderBasename}",//指定C/C++程序位置
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",//指定工作目录
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "将反汇编风格设置为 Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "make",//*****重要一步*****在调试前会先调用make这个task中编译构建的程序
            "miDebuggerPath": "/bin/gdb"
        }
    ]
}

~~~

## 测试

1.新建文件夹test

2.在其中新建test.cpp，内容如下

~~~C++
#include"opencv2/opencv.hpp"
using namespace cv;
int main(){
    Mat M(200,200,CV_8UC3,Scalar(0,0,255));
    imshow("myM",M);
    waitKey(0);
    return 0;
}
~~~

3.新建CMakeLists.txt，内容如下

~~~cmake
set( CMAKE_BUILD_TYPE "Debug" )
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
add_definitions(-std=c++11) #该处可以根据自己需要进行修改，例如c++14...

cmake_minimum_required(VERSION 2.8)
project( test )
find_package( OpenCV REQUIRED )
add_executable( test test.cpp )
target_link_libraries( test ${OpenCV_LIBS} )
~~~

4.按Ctrl+Shift+P，搜索CMake:Configure,选择，结束之后左侧工具栏会出现一下图标

![image-20221206192219346](C:\Users\苍玄\AppData\Roaming\Typora\typora-user-images\image-20221206192219346.png)

表明CMake工具链正常启动

5.选择test.cpp，按Ctrl+Shift+P，搜索C/C++，添加调试配置。如上述说明，将tasks.json和launch.json文件配置好

6.按Ctrl+F5，以非调试模式运行，将会有如下显示

![image-20221206192744985](C:\Users\苍玄\AppData\Roaming\Typora\typora-user-images\image-20221206192744985.png)

7.，在test.cpp中设置断点后，按F5进入调试模式，测试调试功能。

# 安装ROS

使用安装脚本来源于鱼香ROS

终端运行

~~~shell
sudo wget http://fishros.com/install -O fishros && . fishros
~~~

选择

1.一键安装ROS

2.不更换继续安装

3.foxy（ROS2）

4.foxy（ROS2）桌面版

验证

1.终端运行指令

~~~shell
cd /opt/ros/foxy
~~~

~~~shell
ls
~~~

查看是否有对应的文件

2.运行小乌龟样例

同时打开两个终端，其中一个运行指令

打开小乌龟

~~~shell
ros2 run turtlesim turtlesim_node
~~~

另一个终端运行

打开遥控器

~~~shell
ros2 run turtlesim turtle_teleop_key
~~~

在遥控器所在的终端内，通过上下左右键控制小乌龟的行动。
