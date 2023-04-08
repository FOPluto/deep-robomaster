# deep-robomaster

*燕山大学 "Ysu-Robomaster" (作者：`可莉不知道哦`)的深度学习模块代码,环境如下：*

**Platform：**

​ Ubuntu 18.04

**environment：**

1. CMake：3.26
2. OpenCV：3.4.9
3. OpenVINO：version 2022.3

## 项目环境配置

首先确保您的ubuntu系统具备以下环境：

Anaconda 在官网

opencv 3.4.9 参考嘉宇写的脚本

**openvino环境配置：**

也可参考openvino官网步骤配置

### 拉取并编译 OpenVINO 源码

从 OpenVINO 官网下载最新版本的源码，或者从 GitHub 上下载最新版本的源码，然后将其解压到你的机器上。

```shell
git clone -b 2022.3.0 https://github.com/openvinotoolkit/openvino.git
```

此时会在当前文件夹下创建一个openvino的项目文件夹，进入文件夹，创建build文件夹，进入build文件夹

```shell
cd openvino
mkdir build
cd build
```

cmake一下，在使用 CMake 构建 OpenVINO 时，可以使用 `-DCMAKE_INSTALL_PREFIX` 选项来指定安装位置。例如，以下命令将 OpenVINO 安装到 `/opt/intel/openvino` 目录：

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/intel/openvino ..
```

make编译：

```shell
make -j$(nproc)
```

等待几分钟，和opencv一样，但是不会出啥Bug，最后再安装：

```shell
sudo make install
```

### 配置 OpenVINO 环境变量并检验是否安装完成

```shell
cd /opt/intel/openvino/  # 进入安装目录
source openvino.sh
```

可以将以下命令添加到 ~/.bashrc 文件中：

```shell
source /opt/intel/openvino/openvino.sh
```

这样每次启动命令行就会自动检测是否配置好，并且export一下环境变量。

## 项目结构

```shell
├── best                             # 模型目录
│   ├── best.mapping                 # mapping为可视化文件
│   ├── best.onnx                    # onnx为原始模型
│   ├── best.bin                     # bin文件
│   └── best.xml                     # xml文件和bin文件为代码中调用的权重文件
├── build                            # build目录
│   ├── CMakeCache.txt
│   └── CMakeFiles
│       ├── 3.22.1
│       │   └── CMakeSystem.cmake
│       ├── cmake.check_cache
│       └── CMakeOutput.log
├── CMakeLists.txt                   # CMakeLists文件
├── demo                             # 测试数据存储路径
├── demo.cpp                         # 测试代码 
├── README.md
├── test.avi
├── Yolov5.cpp                       # Yolov5模型检测相关
└── Yolov5.h
```

## openvino介绍

## V2RanA

1. <https://today.abyss.moe/guide/entry.html> 在这个网址中，`V2rana`软件这个软件可以实现管理订阅

2. 安装 V2Ray 内核：
v2rayA 提供的镜像脚本（推荐）

```shell
curl -Ls https://mirrors.v2raya.org/go.sh | sudo bash
```

3. 启动服务：

```shell
sudo v2rana
```

