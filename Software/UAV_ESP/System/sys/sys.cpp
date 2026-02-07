#include "sys.h"
#include "esp_log.h"


// ============ 系统初始化 ============
int system_init(void)
{
    // 初始化定时器
    if (PID_time.init()) {
        // 启动定时器
        if (PID_time.start()) {
            return SYS_ERR_OK;
        } else {
            return SYS_ERR_START;
        }
    } else {
        return SYS_ERR_INIT;
    }
}

// ============ 算法定时器回调函数 ============
void arithmetic_time(void)
{

}
