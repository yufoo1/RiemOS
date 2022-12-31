#include <driver.h>
#include <kclock.h>
#include <process.h>
#include <riscv.h>

void setNextTimeInterrupt() {
    SBI_CALL_1(SBI_SET_TIMER, r_time() + INTERVAL);
}