#include <env.h>
#include <pmap.h>
#include <printf.h>

#define PRI(pri) ((pri) & 0xff)
#define PRI_FUNC1(pri) (((pri) >> 8) & 0xff)
#define PRI_FUNC2(pri) (((pri) >> 16) & 0xff)
#define PRI_FUNC3(pri) (((pri) >> 24) & 0xff)
#define UPDATE_PRI(pri, new_pri) (((pri) & 0xffffff00) | ((new_pri) & 0xff))

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

    if (curenv != NULL) {
        new_pri = (int)PRI(curenv->env_pri);
        new_pri -= PRI_FUNC1(curenv->env_pri);
        if (new_pri < 0) {
            new_pri = 0;
        }
        curenv->env_pri = UPDATE_PRI(curenv->env_pri, (u_int)new_pri);

        if (++curenv->env_runs == PRI_FUNC2(curenv->env_pri)) {
            curenv->env_runs = 0;
            if (PRI_FUNC3(curenv->env_pri) > 0) {
                curenv->env_status = ENV_NOT_RUNNABLE;
            }
        }
    }

    LIST_FOREACH(tmp_e, &env_sched_list[0], env_sched_link) {
        if (tmp_e != curenv && tmp_e->env_status == ENV_NOT_RUNNABLE) {
            if (++tmp_e->env_runs == PRI_FUNC3(curenv->env_pri)) {
                curenv->env_runs = 0;
                curenv->env_status = ENV_RUNNABLE;
            }
        }
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
