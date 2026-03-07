@echo off
REM 构建 ROS 开发容器镜像

echo ========================================
echo   构建 ROS + ODB + EMQX 开发镜像
echo ========================================
echo.
echo 此过程可能需要 10-30 分钟，取决于网络速度
echo 镜像大小约 4-5 GB
echo.

REM 获取脚本所在目录
set SCRIPT_DIR=%~dp0

echo 开始构建...
echo.

cd /d "%SCRIPT_DIR%"
docker build -f Dockerfile -t ros-dev ..

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo   镜像构建成功！
    echo ========================================
    echo.
    echo 镜像名称: ros-dev
    echo.
    echo 下一步:
    echo   1. 双击运行: run-ros-container.bat
    echo   2. 或在 VS Code 中使用 Dev Containers
    echo.
) else (
    echo.
    echo ========================================
    echo   镜像构建失败
    echo ========================================
    echo.
    echo 请检查错误信息并重试
    echo.
)

pause
