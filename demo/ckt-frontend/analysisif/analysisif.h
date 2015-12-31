#ifndef _ANALYSISIF_H_
#define _ANALYSISIF_H_

int AnaIfInit(void);
int FromBgEnvInit(void);
int ToBgEnvInit(void);
int FromFrontEnvInit(void);
int ToFrontEnvInit(void);

void ToFrontEnvDeInit(void);
void FromBgEnvDeInit(void);
void ToBgEnvDeInit(void);
void FromFrontEnvDeInit(void);
void AnaIfDeInit(void);

int GetDataFromFront(void);
int GetDataFromBg(void);
int DataTransfer2Front(void);
int DataTransfer2Bg(void);

#endif
