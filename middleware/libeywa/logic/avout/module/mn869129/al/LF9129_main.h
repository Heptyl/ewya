#ifndef __HV_VERSION_H__
#define __HV_VERSION_H__



int LF9129_main(void);

int LF9129_detect(void);

int LF9129_Init(void);

int LF9129_Set_VideoFormat(int format);

int LF9129_Get_VideoFormat(char* info);

int LF9129_HDCP_Mute(int bEnable);

int LF9129_Get_HDCP_Mute(bool* info);

int LF9129_CheckState_Loop(void);

int LF9129_reset_hdmi(void);

int LF9129_open_hdmi(void);

int LF9129_initSPDIF(void);

#endif
