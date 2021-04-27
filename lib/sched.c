#include <env.h>
#include <pmap.h>
#include <printf.h>

#define PRI(pri) ((pri) & 0xff)
#define PRI_FUNC1(pri) (((pri) >> 8) & 0xff)
#define BUILD_PRI(pri, func1) ((pri) | ((func1) << 8))

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.14 ***/
void sched_yield(void)
{
    struct Env *e = NULL, *tmp_e;
    int new_pri;
    u_int func1;

    if (curenv != NULL) {
        new_pri = (int)PRI(curenv->env_pri);
        func1 = PRI_FUNC1(curenv->env_pri);
        new_pri -= func1;
        if (new_pri < 0) {
            new_pri = 0;
        }
        curenv->env_pri = BUILD_PRI(new_pri, func1);
    }

    LIST_FOREACH(tmp_e, &env_sched_list[0], env_sched_link) {
        if (tmp_e->env_status == ENV_RUNNABLE) {
            if (e == NULL || PRI(tmp_e->env_pri) > PRI(e->env_pri)) {
                e = tmp_e;
            }
        }
    }
    if (e != NULL) {
        env_run(e);
    }
}
