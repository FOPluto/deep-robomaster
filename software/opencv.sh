#! /bin/bash
#测试网络通断
network()
{
    local timeout=1
    local target=www.baidu.com
    local ret_code=`curl -I -s --connect-timeout ${timeout} ${target} -w %{http_code} | tail -n1`
    if [ "x$ret_code" = "x200" ]; then
        return 1
    else
        return 0
    fi
    return 0
}
cd ~/
network
if [ $? -eq 0 ];then
echo "[ERROR]:The network is disconnected! Please connect to the network."
exit -1
fi
echo "The network is normal,continue to configure!"

cpus=$(cat /proc/cpuinfo | grep processor | wc -l)
echo "cpu cores num:$cpus"
#1.换源
Codename=$(cat /etc/os-release | grep VERSION_CODENAME |awk -F'=' '{print $2}')
echo "Your Ubuntu system version is detected as:$Codename"
sourceweb='http://mirrors.aliyun.com'
echo "backup sources.list..."
sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
echo "Set up a new mirror source..."
echo "\
deb $sourceweb/ubuntu/ $Codename main restricted universe multiverse
deb $sourceweb/ubuntu/ $Codename-security main restricted universe multiverse
deb $sourceweb/ubuntu/ $Codename-updates main restricted universe multiverse
deb $sourceweb/ubuntu/ $Codename-proposed main restricted universe multiverse
deb $sourceweb/ubuntu/ $Codename-backports main restricted universe multiverse
deb-src $sourceweb/ubuntu/ $Codename main restricted universe multiverse
deb-src $sourceweb/ubuntu/ $Codename-security main restricted universe multiverse
deb-src $sourceweb/ubuntu/ $Codename-updates main restricted universe multiverse
deb-src $sourceweb/ubuntu/ $Codename-proposed main restricted universe multiverse
deb-src $sourceweb/ubuntu/ $Codename-backports main restricted universe multiverse">/etc/apt/sources.list
echo "update source..."
sudo apt-get update


#提前解决jasper的问题
sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"
sudo apt update
sudo apt install libjasper1 libjasper-dev -y
##2.安装依赖
sudo apt-get install cmake git gcc g++ libgtk-3-dev libgtk2.0-dev pkg-config build-essential libavcodec-dev libavformat-dev libswscale-dev -y
sudo apt-get install wget ca-certificates -y
sudo apt-get -y install python3-dev python3-numpy python-dev python-numpy 
sudo apt-get -y install libpython3.6-dev libtbb2 libtbb-dev 
sudo apt-get -y install libjpeg-dev libpng-dev libtiff5-dev libjasper-dev libdc1394-22-dev libavcodec-dev libavformat-dev 
sudo apt-get -y install libswscale-dev libv4l-dev liblapacke-dev libopenexr-dev libxvidcore-dev libx264-dev 
sudo apt-get -y install libatlas-base-dev gfortran ffmpeg

sudo apt-get -y install libgstreamer-plugins-base1.0-dev
sudo apt-get -y install libgstreamer1.0-dev
sudo apt-get -y install libavresample-dev
sudo apt-get -y install libgphoto2-dev
sudo apt-get -y install libopenblas-dev

#定义安装路径
opencv_dir="/usr/local/opencv3.4.9"

if [ -f $opencv_dir ];then
  echo "flod is exit,remove it!"
  rm -f $opencv_dir
fi
#通过git获取opencv3.4.9和opencv_contrib
sudo mkdir -p $opencv_dir && cd $opencv_dir
#git获取opencv3.4.9
git clone -b 3.4.9 https://gitlab.com/immersaview/public/remotes/opencv.git

# git获取opencv_contrib
cd /usr/local/opencv3.4.9/opencv
git clone -b master https://gitlab.com/gawainsciencer/opencv_contrib.git

# git获取ippicv_2020_lnx_intel64_20191018_general.tgz
cd $opencv_dir
git clone https://gitlab.com/gawainsciencer/ippicv.git
# 修改ippicv的配置文件
cd /usr/local/opencv3.4.9/opencv/3rdparty/ippicv
sed -i 's!https://raw.githubusercontent.com/opencv/opencv_3rdparty/${IPPICV_COMMIT}/ippicv/!/usr/local/opencv3.4.9/ippicv!g' ippicv.cmake

#编译
cd /usr/local/opencv3.4.9/opencv
sudo mkdir -p build && cd build

sudo cmake -DCMAKE_BUILD_TYPE=RELEASE \
      -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
      -DBUILD_opencv_face=OFF \
      -DBUILD_opencv_xobjdetect=OFF \
      -DBUILD_opencv_xfeatures2d=OFF \
      -DPYTHON_DEFAULT_EXECUTABLE=$(python3 -c "import sys; print(sys.executable)") \
      -DPYTHON3_EXECUTABLE=$(python3 -c "import sys; print(sys.executable)") \
      -DPYTHON3_NUMPY_INCLUDE_DIRS=$(python3 -c "import numpy; print(numpy.get_include())") \
      -DPYTHON3_PACKAGES_PATH=$(python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())") \
      ..


sudo make -j$cpus
sudo make install
#完成编译

# 创建opencv的软连接，外部调用的时候可以直接使用include /usr/local/opencv

sudo ln -s /usr/local/opencv3.4.9/opencv /usr/local/opencv


#处理wsl的/sbin/ldconfig.real: /usr/lib/wsl/lib/libcuda.so.1 is not a symbolic link问题
cd /usr/lib/wsl
sudo mkdir -p lib2
sudo ln -s lib/* lib2
sed -i 's!/usr/lib/wsl/lib!/usr/lib/wsl/lib2!g' /etc/ld.so.conf.d/ld.wsl.conf
ldconfig
#使设置永久生效
cat >> /etc/wsl.conf << EOF
[automount]
ldconfig = false
EOF

#配置opencv
/bin/bash -c 'echo "include /usr/local/lib" >> /etc/ld.so.conf'
ldconfig

cd /usr/local/lib
sudo mkdir -p pkgconfig
sudo cp -u /usr/local/opencv3.4.9/opencv/build/unix-install/opencv.pc ./pkgconfig

#文件导出到环境变量
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
#输出opencv版本号，正常显示3.4.9
pkg-config opencv --modversion


#安装工摄驱动
sdk_dir="/usr/local/opencv3.4.9"
cd $sdk_dir
git clone https://gitlab.com/gawainsciencer/linuxsdk.git

cd ./linuxsdk
sudo chmod +x install.sh
sudo sh install.sh

#安装gdb
sudo apt-get install gdb -y
FILE=/usr/bin/gdb
if [ -f "$FILE" ];then
echo "gdb,success!"
else
echo "some error,please check"
fi
#对结果进行检查确定opencv安装成功
FILE=/usr/local/lib/pkgconfig/opencv.pc
if [ -f "$FILE" ];then
echo "opencv,success!"
echo "Please  restart system  now!!!"
else
echo "some error,please check"
fi