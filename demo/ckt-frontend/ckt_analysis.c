#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "libfahw.h"
#include "utils.h"
#include "analysisif/analysisif.h"
#include "ckt_analysis.h"

static pthread_t ReadId, WriteId;


void ReadFromThread()
{
    int ret;

    printf("%s\n", __func__);
    do
    {
        ret = GetDataFromBg();
        if(0 > ret)
        {
            printf("Get data from background error!!!!!\n");
            continue;
        }

        ret = DataTransfer2Front();
        if (0 != ret)
        {
            printf("DataTransfer to fronend error!!\n");
            exit(-1);
        }

    }while (0 == ret);

}

void WriteToThread()
{
    int ret = 0;

    printf("%s\n", __func__);
    do
    {
        ret = GetDataFromFront();
        if (0 != ret)
        {
            printf("Get data from front error!!\n");
            exit(-1);
        }

        ret = DataTransfer2Bg();
        if(0 > ret)
        {
            printf("Send data to background error!!\n");
            exit(-1);
        }
    }while (0 == ret);

}

int EnvIinit()
{
    if (-1 ==AnaIfInit())
    {
        printf(" Analysis interface init error!\n");
        return -1;
    }


    if (-1 ==FromBgEnvInit())
    {
        printf(" From background env init error!\n");
        return -1;
    }

    if (-1 ==ToBgEnvInit())
    {
        printf("To background env init error!\n");
        return -1;
    }

    if (-1 == FromFrontEnvInit())
    {
        printf("From front end environment init error!\n");
        return -1;
    }

    if (-1 == ToFrontEnvInit())
    {
        printf("To front end environment init error!\n");
        return -1;
    }

    return 0;
}

void EnvDeIinit()
{
    printf("%s\n", __func__);
    pthread_join(ReadId, NULL);
    printf("%s-----Read thread exit\n", __func__);

    pthread_join(WriteId, NULL);
    printf("%s-----Write thread exit\n", __func__);

    FromBgEnvDeInit();
    ToBgEnvDeInit();
    FromFrontEnvDeInit();
    ToFrontEnvDeInit();
    AnaIfDeInit();
    printf("%s-----all deinit exit\n", __func__);
}


int main(int argc, char *argv[])
{
    int ret;

    printf("Env init begin!!!\n");
    if (0 > EnvIinit())
    {
        printf("Envinit error!\n");
        return -1;
    }

    printf("Creat Read thread begin!!!\n");
    ret=pthread_create(&ReadId,NULL,(void *) ReadFromThread,NULL);
    if(0 != ret)
    {
        printf("Create read thread error!\n");
        goto ERR;
    }

    printf("Creat Write thread begin!!!\n");
    ret=pthread_create(&WriteId,NULL,(void *) WriteToThread,NULL);
    if(0 != ret)
    {
        printf("Create wirte thread error!\n");
        goto ERR;
    }
    EnvDeIinit();
    printf("Process exit !!!!!!!!!\n");
    return 0;

ERR:
    printf("error exit!!\n");
    EnvDeIinit();
    return -1;
}
