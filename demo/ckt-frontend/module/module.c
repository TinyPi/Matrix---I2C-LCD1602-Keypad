#include <memory.h>
#include "module.h"

static struct list_head* list_head = NULL;

int ctr_module_init(void)
{
    list_head = (struct list_head*)malloc(sizeof(struct list_head));
    init_list_head(list_head);
    return 0;
}

void ctr_module_deinit()
{
    struct list_head *pos = list_head->next;
    struct list_head *tmp = NULL;

    while (pos != list_head)
    {
        tmp = pos->next;
        module_unregister(container_of(pos,struct control_module, list_head));
        pos = tmp;
    }
    free(list_head);
}
int module_register(struct control_module *control_module)
{
    struct control_module* control_temp = NULL;
    struct list_head* list_head_new = &(control_module->list_head);
    struct list_head* head = list_head;
    struct list_head* pos = NULL;

    PDEBUG(LDEBUG, "Register %s into system", control_module->name);
    list_for_each(pos, head)
    {
        control_temp = container_of(pos,struct control_module, list_head);
        if(0 == strcmp(control_temp->name, control_module->name))
            {
                PDEBUG(LWARN, "%s has been in module list!!!!", control_module->name);
                return 0;
            }
    }

    list_add_tail(list_head_new, list_head);
    PDEBUG(LDEBUG, "Register %s into system end!!!", control_module->name);
    return 0;
}

int module_unregister(struct control_module *control_module)
{
    struct control_module* control_temp = NULL;
    struct list_head* head = list_head;
    struct list_head* pos = NULL;

    list_for_each(pos, head)
    {
        control_temp = container_of(pos,struct control_module, list_head);
        if(0 == strcmp(control_temp->name, control_module->name))
        {
            list_del(&(control_module->list_head));
            free(control_module);
            PDEBUG(LDEBUG, "%s del!!!", control_module->name);
            return 0;;
        }
    }

    PDEBUG(LERR, "%s don't exist!!!", control_module->name);
    return -1;
}

struct control_module* get_module(char* module_name)
{
    struct list_head* head = list_head;
    struct list_head* pos = NULL;
    struct control_module* control_m = NULL;

    list_for_each(pos, head)
    {
        control_m = container_of(pos,struct control_module, list_head);
        if(0 == strcmp(control_m->name, module_name))
        {
            return control_m;
        }
    }

    return NULL;
}

