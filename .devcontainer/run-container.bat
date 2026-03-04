@echo off
REM ROS + ODB + EMQX 开发容器启动脚本 (Windows)

echo ========================================
echo   ROS Noetic + ODB + EMQX 开发容器
echo ========================================
echo.

REM 获取脚本所在目录的绝对路径
set SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%

REM 获取项目根目录
for %%I in ("%SCRIPT_DIR%\..") do set PROJECT_DIR=%%~fI

echo 项目根目录: %PROJECT_DIR%
echo 容器工作目录: /workspace
echo.
echo 包含组件:
echo   - ROS Noetic (完整桌面版)
echo   - ODB ORM (C++ 对象关系映射)
echo   - EMQX 5.8 (MQTT Broker)
echo   - cJSON 库
echo   - Vosk 语音识别
echo   - 所有 robot.sh 依赖
echo.

REM 检查镜像是否存在
docker images | findstr "ros-dev" >nul
if %errorlevel% neq 0 (
    echo [警告] 镜像 'ros-dev' 不存在
    echo 请先构建镜像:
    echo   cd .devcontainer
    echo   docker build -f Dockerfile.ros -t ros-dev .
    echo.
    pause
    exit /b 1
)

echo 正在启动容器...
echo.

REM 启动容器
docker run -it --rm ^
  -v "%PROJECT_DIR%:/workspace" ^
  --network=host ^
  --privileged ^
  -e DISPLAY=host.docker.internal:0 ^
  ros-dev

echo.
echo ========================================
echo   容器已退出
echo ========================================
pause
