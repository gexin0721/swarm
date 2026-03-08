#!/bin/bash

###############################################################################
# Logger模块测试运行脚本
# 功能：编译并运行Logger模块的单元测试
# 使用方法：./run_logger_test.sh
###############################################################################

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # 无颜色

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGE_DIR="$(dirname "$SCRIPT_DIR")"

echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}  Logger模块单元测试${NC}"
echo -e "${YELLOW}========================================${NC}"
echo ""

# 检查是否在ROS工作空间中
if [ ! -f "$PACKAGE_DIR/package.xml" ]; then
    echo -e "${RED}错误: 未找到package.xml，请确保在正确的ROS包目录中运行此脚本${NC}"
    exit 1
fi

# 查找工作空间根目录
WORKSPACE_ROOT="$PACKAGE_DIR"
while [ ! -f "$WORKSPACE_ROOT/devel/setup.bash" ] && [ "$WORKSPACE_ROOT" != "/" ]; do
    WORKSPACE_ROOT="$(dirname "$WORKSPACE_ROOT")"
done

if [ ! -f "$WORKSPACE_ROOT/devel/setup.bash" ]; then
    echo -e "${RED}错误: 未找到ROS工作空间，请先编译工作空间${NC}"
    exit 1
fi

echo -e "${GREEN}找到工作空间: $WORKSPACE_ROOT${NC}"
echo ""

# 进入工作空间根目录
cd "$WORKSPACE_ROOT"

# 加载ROS环境
echo -e "${YELLOW}加载ROS环境...${NC}"
source devel/setup.bash

# 编译测试
echo -e "${YELLOW}编译Logger测试...${NC}"
catkin_make run_tests_message_manager_gtest_test_logger

if [ $? -ne 0 ]; then
    echo -e "${RED}编译失败！${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}编译成功！${NC}"
echo ""

# 运行测试
echo -e "${YELLOW}运行测试...${NC}"
echo -e "${YELLOW}========================================${NC}"
echo ""

# 直接运行测试可执行文件
TEST_EXECUTABLE="$WORKSPACE_ROOT/devel/lib/message_manager/test_logger"

if [ -f "$TEST_EXECUTABLE" ]; then
    "$TEST_EXECUTABLE"
    TEST_RESULT=$?
else
    echo -e "${RED}错误: 未找到测试可执行文件${NC}"
    exit 1
fi

echo ""
echo -e "${YELLOW}========================================${NC}"

# 显示测试结果
if [ $TEST_RESULT -eq 0 ]; then
    echo -e "${GREEN}✓ 所有测试通过！${NC}"
else
    echo -e "${RED}✗ 测试失败${NC}"
fi

echo -e "${YELLOW}========================================${NC}"

exit $TEST_RESULT
