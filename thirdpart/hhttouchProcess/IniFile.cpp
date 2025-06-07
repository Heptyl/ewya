#include <string.h>
#include <unistd.h>  
#include <fcntl.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <stdarg.h>  
#include <utils/Log.h>

#include "IniFile.h"
#define NOFILE "nofile"  
#define  MAX_PATH 260
char g_szConfigPath[MAX_PATH];

#define TAG "TouchProcess"
#define LOGD(...) //__android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型   

char *GetIniKeyString(const char *title,char *key,const char *filename)   
{   
    FILE *fp;   
    char szLine[1024];  
    static char tmpstr[1024];  
    int rtnval;  
    int i = 0;   
    int flag = 0;   
    char *tmp;  
  
    if((fp = fopen(filename, "r")) == NULL)   
    {
        LOGD("TouchProcess  have   no   such   file \n");
        return NULL;
    }
    while(!feof(fp))
    {   
        rtnval = fgetc(fp);   
        if(rtnval == EOF)
        {
	      LOGD("TouchProcess  EOF value is none \r\n");
            break;
        }
        else
        {
            szLine[i++] = rtnval;
        }
        if(rtnval == '\n')   
        {   
            i--;   
            szLine[--i] = '\0';  
            i = 0;   
            tmp = strchr(szLine, '=');   
  
            if(( tmp != NULL )&&(flag == 1))   
            {   
                if(strstr(szLine,key)!=NULL)   
                {   
			//注释行
			if ('#' == szLine[0])
			{
			
			}
			else if ( '/' == szLine[0] && '/' == szLine[1] )
			{
				
			}
			else
			{
				//找打key对应变量
				strcpy(tmpstr,tmp+1); 
				fclose(fp);
				LOGD("TouchProcess  value tmpstr = %s \r\n",tmpstr);
				return tmpstr; 
			}
                }   
            }  
            else   
            {   
                strcpy(tmpstr,"[");   
                strcat(tmpstr,title);   
                strcat(tmpstr,"]");  
                if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 )   
                {  
                    flag = 1;   
                }  
            }  
        }  
    }  
    fclose(fp);   
    LOGD("TouchProcess  value is none \r\n");
    return NULL;
}

char* GetIniSectionItem(const char* FileName,const char* Section,const char* Item)  
{  
    char *value,Sect[30],c;  
    char linebuf[1024],oldline[1024];  
    FILE *inifp;  
    int i;  
    strcpy(Sect,"[");  
    strcat(Sect,Section);  
    strcat(Sect,"]");  
    /*strcpy(Ite,"[");strcat(Ite,Item);strcat(Ite,"]");*/  
    if((inifp=fopen(FileName,"rb"))==NULL)  
    {  
        return (char*)NOFILE;  
    }  
    /*printf("Sect = %s,Item = %s /n",Sect,Item);*/  
    while((c=fgetc(inifp))!=EOF)  
    {  
        if(c=='[')  
        {  
            ungetc(c,inifp);  
            fgets(linebuf,1024,inifp);  
            if(strstr(linebuf,Sect))  
            {  
                while((c=fgetc(inifp))!='[' && c!=EOF)  
                {  
                    ungetc(c,inifp);  
                    fgets(linebuf,1024,inifp);  
                    if(strstr(linebuf,Item))  
                    {  
                        if(value=strchr(linebuf,'='))  
                        {  
                            value++;  
                            /*printf("value = %s /n",value);*/  
                            fclose(inifp);  
                            if(*value=='/n')  
                                return (char *)NULL;  
                            return value;  
                        }  
                    }  
                }  
                if(c==EOF)  
                {  
                    break;  
                }  
                ungetc(c,inifp);  
            }  
        }  
        else  
        {  
            ungetc(c,inifp);  
            fgets(linebuf,1024,inifp);  
        }  
    }  
    fclose(inifp);  
    return (char*)NULL;  
}  

int GetIniKeyInt(const char *title,const char *key,const char *filename)  
{  
    return atoi(GetIniSectionItem(filename,title,key));
    //return atoi(GetIniKeyString(title,key,filename));
}  


