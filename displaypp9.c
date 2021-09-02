#include "system.h"
#include "display.h"

#ifdef PP9_VIRTUAL
    #include "com.h"
#endif 

char  lock_disp_clock;
char  disp_buffer[_LENTH_DISPLAY];
char  lcd_buffer[64];
INT16 lcd_len; 
char vfd_buffer[64];

//=========================================================================
void disp_init(void){
    
    disp_set_menu("Menu                    ");
}
//=========================================================================
void disp_clear(void)
{
#ifndef PP9_VIRTUAL
    SetLcdPos(_DEV_VFD, 0); // Seleccion de Display _DEV_VFD
        printf("         ");
    SetLcdPos(0,0); 
#else 
    return;
#endif
}
//=========================================================================
void disp_event(char linea)
{
#ifndef PP9_VIRTUAL
    SetLcdPos(linea, 0); // Seleccion de Display _DEV_VFD
        printf(disp_buffer);
    SetLcdPos(0,0); 
#else 
    com_putstring(disp_buffer);
#endif
}
//=========================================================================
void disp_enviar(char *buf, char linea){
        
#ifndef PP9_VIRTUAL

    if(linea == _LINEA_1)
    {
        memset(disp_buffer,0x00,sizeof(disp_buffer));
        memcpy(disp_buffer,buf,strlen(buf));
        Event_WriEvtAndPar(evDisp, (UAuto16) linea);
    }
    else
    {
        SetLcdPos(linea, 0); // Seleccion del LCD
        printf(buf);
        SetLcdPos(0,0);
    }   
#else 
    com_putstring(buf);
    com_put_char('\n');
#endif
}
//=========================================================================
void disp_clock(void)
{   
#ifndef PP9_VIRTUAL
    
    if((t_wait_display != 0)) return;

    SetLcdPos(_DEV_VFD, 0); // Seleccion de Display _DEV_VFD
        rtc_get_time(&Now);     
        printf(" %02d-%02d-%02d",Now.s.Hora,Now.s.Min,Now.s.Seg);
    SetLcdPos(0,0);
    
#else 
    return;
#endif
}
//=========================================================================
void disp_clock_lcd(void)
{
#ifndef PP9_VIRTUAL
#ifdef INCLUDE_RTCC     
        static String FlashIcon;
        char cAux[20];
        char cAux2[20];

        buffer_clear(cAux,sizeof(cAux));        
        lcd_len = 0;    
        FlashIcon = (FlashIcon == ' ') ? ':' : ' ';
        rtc_get_time(&Now);
  #ifdef INCLUDE_MULTILINE_LCD
        ClrMtLcd();         //======== Validacion de Caracteres de Carga ================
        sprintf(cAux,"%02d-%02d-%02d",Now.s.Dia,Now.s.Mes, Now.s.Ano);
        fm_centrado(cAux2,cAux,' ',MAXCHD);

        if(sPeripherals.b.is_charge_up)
        {
            cAux2[MAXCHD-1] = 0x01;
        }
        else
        {
            cAux2[MAXCHD-1] = 0x02;
        }
        //==========Niveles de Carga de la Bateria =================
            if(sPeripherals.b.nivel_charge_2 == FALSE && sPeripherals.b.nivel_charge_1 == FALSE)
            cAux2[MAXCHD-2] = 0x03;
        else if(sPeripherals.b.nivel_charge_2 == FALSE && sPeripherals.b.nivel_charge_1 == TRUE)
            cAux2[MAXCHD-2] = 0x04;
        else if(sPeripherals.b.nivel_charge_2 == TRUE && sPeripherals.b.nivel_charge_1 == FALSE)
            cAux2[MAXCHD-2] = 0x05;
        else if(sPeripherals.b.nivel_charge_2 == TRUE && sPeripherals.b.nivel_charge_1 == TRUE)
            cAux2[MAXCHD-2] = 0x06;
        //===============================================

        SetLcdPos(_LINEA_2, 0); // Seleccion del LCD
        printf(cAux2);

        // en modo entrenamiento, alterno la hora con el mensaje de no fiscal
        if((mf_val.s.MF_Status==MODO_ENTREN_ON)&&((Now.s.Seg&0x04)==0x04))
        {
            mensaje_leer(cAux2,MENSAJE_TRAIN);
            fm_centrado(cAux,cAux2,' ',MAXCHD);
            disp_enviar(cAux,_LINEA_3);
        }
        // en modo entrenamiento, alterno la hora con el mensaje de no fiscal
        else if((mf_val.s.MF_Status==MODO_SERVICIO)&&((Now.s.Seg&0x04)==0x04))
        {
            mensaje_leer(cAux2,MENSAJE_MODO_SERVICIO);
            fm_centrado(cAux,cAux2,' ',MAXCHD);
            disp_enviar(cAux,_LINEA_3);
        }
        else if((mf_val.s.MF_Status==MODO_FM_LECTURA)&&((Now.s.Seg&0x04)==0x04))
        {
            mensaje_leer(cAux2,MENSAJE_FM_SOLO_LECTURA);
            fm_centrado(cAux,cAux2,' ',MAXCHD);
            disp_enviar(cAux,_LINEA_3);
        }
        // en modo entrenamiento, alterno la hora con el mensaje de no fiscal
        else if((control.s.cBloqueo==ERROR_INTER_TEC)&&((Now.s.Seg&0x04)==0x04))
        {
            mensaje_leer(cAux2,MENSAJE_INTER_TECNICA);
            fm_centrado(cAux,cAux2,' ',MAXCHD);
            disp_enviar(cAux,_LINEA_3);
        }
        else
        {
            //Manejo de Formato de la Hora en LCD
            SetLcdPos(_DEV_MULTILINE+((MLCDLINE - 2) / 2)+1,(MLCDBUFLEN-5)/2);
        
             //Manejo de Formato de la Fecha en LCD
            printf("%02d%c%02d",Now.s.Hora, FlashIcon, Now.s.Min);
        }   
        
        
  #endif

#endif
#else 
    char cAux[20];

    com_putstring("THE FACTORY HKA, C.A ");

    rtc_get_time(&Now);

    sprintf(cAux,"%02d-%02d-%02d ",Now.s.Dia,Now.s.Mes, Now.s.Ano);

    com_putstring(cAux);

    sprintf(cAux,"%02d:%02d\n",Now.s.Hora, Now.s.Min);

    com_putstring(cAux);
#endif

}
//=========================================================================
void disp_inicio(void){}
//=========================================================================
void disp_alarma(void)
{

    char cAux[MAXCHD+5];
    char cAux2[MAXCHD+5];

    buffer_clear(cAux,sizeof(cAux));
    buffer_clear(cAux2,sizeof(cAux2));

    led_blink((cErrorLog - ERROR_ALARMA)*2);

    if(op_flag.s.DisplayError == 1)
    {
        sprintf(cAux,"   E-%04d",(cErrorLog- ERROR_ALARMA));
        
        #ifndef PP9_VIRTUAL
            SetLcdPos(_DEV_VFD, 0); // Seleccion de Display _DEV_VFD
                printf(cAux);
            SetLcdPos(0,0); 

            t_wait_display = 5;
        #endif
        
    }

    // leo el mensaje de alarma
    error_mensaje(cAux,ERROR_ALARMA);
    fm_centrado(cAux2,cAux,' ',MAXCHD);
    disp_enviar(cAux2,_LINEA_2);

    error_mensaje(cAux,cErrorLog);
    fm_centrado(cAux2,cAux,' ',MAXCHD);
    disp_enviar(cAux2,_LINEA_3);

    t_wait_lcd = 5;

}
//=========================================================================
void disp_monto(char *buf, INT64 monto)
{

    char cAux[MAXCHD+2];
    char cAux2[MAXCHD+2];
    UINT16 pos = 0,len = 0,ajusPos = 0;
    tFMT_MONTO  fmtMontoAux;
    

    
    // borror el display
    if((op_flag.s.DisplayInit & 0x01)==0) disp_clear(); 
    
    fmtMontoAux = fmtMonto;

    fmtMontoAux.sMil = 0;//El Display numérico no se le asigna el caracter de mil

    sprintf(fmtMontoAux.sMon,"");

    fmtMontoAux.nDiv= MONTO_DIV;

    buffer_clear(cAux2,sizeof(cAux2));
    
    pos = fm_monto(cAux2,monto,fmtMontoAux);

    if(monto > MAX_WIDE_DISPLAY)
    {
        ajusPos  = MAXCHDN;
    }
    else
    {
        ajusPos = pos - 1;
    }

    buffer_blank(cAux,MAXCHD);  
    
    buffer_copy(cAux+(MAXCHDN - ajusPos),cAux2,pos);
    
#ifndef PP9_VIRTUAL
    SetLcdPos(_DEV_VFD, 0); // Seleccion de Display _DEV_VFD
        printf(cAux);
    SetLcdPos(0,0); 
#endif
//============== Manejo de LCD =====================

    fmtMontoAux = fmtMonto;

    fmtMontoAux.nDiv= MONTO_DIV;

    pos = fm_monto(cAux2,monto,fmtMontoAux);    

    len = strlen(buf);

    if(len >MAXCHD) len = MAXCHD;

    buffer_blank(cAux,sizeof(cAux));

    buffer_copy(cAux,buf,len);  

    fm_ascii_display(cAux);

    disp_enviar(cAux,_LINEA_2);
    
    buffer_blank(cAux,MAXCHD);

    //Linea del precio

    if(pos > MAXCHD)
    {
        handler_lcd_buffer(0x01,cAux2,pos);
        pos = MAXCHD;
    }
    
    buffer_copy(cAux+MAXCHD-pos,cAux2,pos); 
    
    disp_enviar(cAux,_LINEA_3);

#ifndef PP9_VIRTUAL
    t_wait_lcd = 10;

    t_wait_display = 10;
#endif  
}
//=========================================================================
//==================== Funciones Auxiliares de Manejo de LCD===========================
void handler_lcd_buffer(BYTE tipo,char * buff,INT16 len)
{   
    char c;
    char cBuff[64]; 
    
    if(tipo == 0x01)
    {
        //copia el buffer.
        buffer_clear(lcd_buffer,sizeof(lcd_buffer));
        if(len > sizeof(lcd_buffer)) len = sizeof(lcd_buffer);
        buffer_copy(lcd_buffer,buff,len);
        lcd_buffer[len] = ' ';
        lcd_len = len+1;
    }
    else 
    {
        if(lcd_len<=MAXCHD) return;
    
        //Genera la rotacion.
        buffer_copy(cBuff,lcd_buffer,lcd_len);      
        c = lcd_buffer[0];
        buffer_copy(lcd_buffer,cBuff+1,lcd_len-1);
        lcd_buffer[lcd_len-1] = c;          
        disp_enviar(lcd_buffer,_LINEA_3);
    }
}

void disp_high_light(char _linea, char _enable)
{
    INT16 indice, pos;
    char cBuffer[MLCDLINE*MLCDBUFLEN];

    indice = 0;

    if(_linea != _LINEA_2 && _linea != _LINEA_3) return;
        
    if(_linea == _LINEA_3) indice = 1;

    SaveLcdPropBuf((UFix08 *)cBuffer);

    pos = indice*MLCDBUFLEN;

    buffer_set(cBuffer+pos,_enable,MLCDBUFLEN);

    RestoreLcdPropBuf((UFix08 *)cBuffer);   

    #if defined(INCLUDE_LCD_32x144)
            LcdCharToLcdDot_32x144();
    #endif  
    
    
}
//==================================================================
void disp_set_menu(char * _buf)
{
    RestoreSoftLcdBuf((UFix08 *) _buf); 
}
//==================================================================


