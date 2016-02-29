#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <pthread.h>
#include "libfahw.h"
#include "utils.h"
#include "module.h"
#include "ckt_analysis.h"
#include "module.h"

unpack_source *unpack_src = NULL;
static pthread_t ReadId, WriteId;


void module_control()
{
    int ret;
    control_str *control_struct = NULL;
    control_struct = (control_str*)malloc(sizeof(control_str));
    if(NULL == control_struct)
        {
            PDEBUG(LERR, "Control_struct malloc erro!!!");
            pthread_exit("module_control malloc error");
        }

    do
    {
        memset(&(unpack_src->package), 0, sizeof(package));
        memset(control_struct, 0, sizeof(control_str));
        ret = get_package(unpack_src->read_fd, &(unpack_src->package));
        if(0 != ret)
        {
            PDEBUG(LERR, "Get data from background error!!!!!");
            continue;
        }

        ret = analysis_package(unpack_src->package, control_struct);
        if(0 != ret)
            {
                PDEBUG(LERR, "Analysis package error!!!!");
                pthread_exit("analysis_package error");
            }

        ret = do_control(control_struct);
        if(0 != ret)
        {
            PDEBUG(LERR, "Do control error!!!!");
            //pthread_exit("do_control error");
        }
    }while (0 == ret);

    free(control_struct);
}

void module_response()
{
#if 0
    int ret = 0;

    printf("%s\n", __func__);
    do
    {
        ret = GetDataFromFront();
        if (0 != ret)
        {
            printf("Get data from front error!!\n");
            pthread_exit("errro");
        }

        ret = DataTransfer2Bg();
        if(0 > ret)
        {
            printf("Send data to background error!!\n");
            pthread_exit("error");
        }
    }while (0 == ret);
#endif
}

int EnvIinit(unpack_source *unpack_src)
{
    if (0 != unpack_env_int(&unpack_src->read_fd, &unpack_src->write_fd))
    {
        PDEBUG(LERR, " unpack env init error!");
        return -1;
    }

//control module init
    if (0 != ctr_env_init())
    {
        PDEBUG(LERR, "ctr_env_init error~~");
        return -1;
    }

    return 0;
}

void EnvDeIinit(unpack_source *unpack_src)
{
    pthread_join(ReadId, NULL);
    PDEBUG(LINFO,"---Module control thread exit");

    pthread_join(WriteId, NULL);
    PDEBUG(LINFO, "---Module response thread exit");

    PDEBUG(LINFO, "Unpack env deinit");
    unpack_env_deinit(unpack_src->read_fd, unpack_src->write_fd);

    PDEBUG(LINFO, "Control env deinit");
    ctr_env_deinit();

    PDEBUG(LINFO, "---all deinit exit\n");
}


int main(int argc, char *argv[])
{
    int ret;

    PDEBUG(LDEBUG, "Env init begin!!!\n");

    system("mx&");

    unpack_src = (unpack_source *)malloc(sizeof(unpack_source));

    if (0 != EnvIinit(unpack_src))
    {
        PDEBUG(LERR, "Envinit error!\n");
        free(unpack_src);
        return -1;
    }

    PDEBUG(LINFO, "Creat module_control thread begin!!!\n");
    ret=pthread_create(&ReadId,NULL,(void *) module_control,NULL);
    if(0 != ret)
    {
        PDEBUG(LERR, "Create module_control thread error!\n");
        goto ERR;
    }

    PDEBUG(LINFO, "Creat module_response thread begin!!!\n");
    ret=pthread_create(&WriteId,NULL,(void *) module_response,NULL);
    if(0 != ret)
    {
        PDEBUG(LERR, "Create module_response thread error!\n");
        goto ERR;
    }
    EnvDeIinit(unpack_src);
    free(unpack_src);
    PDEBUG(LINFO, "Process exit !!!!!!!!!\n");
    return 0;

ERR:
    PDEBUG(LERR, "error exit!!\n");
    EnvDeIinit(unpack_src);
    free(unpack_src);
    return -1;
}
