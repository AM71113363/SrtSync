#include <windows.h>
#include <string.h>
#include <stdio.h>
char szClassName[ ] = "SrtSyncTimer";

#define YES       1
#define NO        0

#define ID_SEARCH_PREV 4000
#define ID_SEARCH_NEXT 4001
#define ID_CALCULATE   4002
#define ID_SYNC        4003

//-----------------------------------------------
HINSTANCE ins;
HWND hWnd,logger;
HWND hSearch,hValue;

WNDPROC OldTimerProc,OldSearchEditProc;

UCHAR dropped[MAX_PATH];
UCHAR *buffer=NULL;
DWORD bufferLen=0;

typedef struct _TIMES
{
    HWND hHours;
    HWND hMinutes;
    HWND hSeconds; 
}TIMES;

TIMES hSrtTime;
TIMES hRealTime;

//-----------------------------------------------
UCHAR ReadDataFile(UCHAR *name);
void CenterOnScreen(HWND hnd);
void SMS(UCHAR *a,UCHAR *b);

//-----------------------------------------------

void ReadSrt()
{
   EnableWindow(GetDlgItem(hWnd,ID_SEARCH_PREV),0);
   EnableWindow(GetDlgItem(hWnd,ID_SEARCH_NEXT),0);
   EnableWindow(GetDlgItem(hWnd,ID_SYNC),0);
   EnableWindow(GetDlgItem(hWnd,ID_CALCULATE),0);
   SetWindowText(logger,"");
   if(ReadDataFile(dropped)==YES)
   { 
       UCHAR *p=strstr(buffer,"-->"); //just checking
       if(p)
       {
          EnableWindow(hSearch,1);
          EnableWindow(GetDlgItem(hWnd,ID_SYNC),1);
       }else{ SetWindowText(logger,"Is not a .srt file"); }
   }
}


LRESULT CALLBACK TimerProc(HWND hnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static UCHAR bfText[4];
    static UCHAR bText[4];
	switch (message) 
	{
		case WM_LBUTTONDOWN:
		{
			SetWindowText(hnd,"");
			memset(bfText,0,4);
		}
		break;
    	case WM_KEYUP: 
        {
			bfText[0] = 0;
			bfText[1] = 0;
			bfText[2] = 0;
			if(GetWindowText(hnd,bfText,3))
			{                          
			   if(atoi(bfText) >= 60)
			   {
			      SetWindowText(hnd,"");
			   }
			}	
		}
		break;  			
	case WM_KILLFOCUS:
	{
        memset(bText,0,4); 
	    if(GetWindowText(hnd,bText,3))
	    {
	        if(bText[1] == 0)
	        {
               bText[1] = bText[0];
               bText[0] = '0'; 
               SetWindowText(hnd,bText);        
            }
        }
        else
        {
            SetWindowText(hnd,"00");
        }
	}
		break;

	default:
		break;
	}
	return CallWindowProc(OldTimerProc, hnd, message, wParam, lParam);
}
//--------------------- SEARCH -------------------------------
DWORD SearchIndexNext;
DWORD SearchIndexPrev;
UCHAR temp[33];
DWORD tempLen;
UCHAR FOUND[MAX_PATH];
static UCHAR INDEX[]=">--\0";
static UCHAR UnicodePrefix[]={0xEF,0xBB,0xBF};

void StringToTime(UCHAR *str)
{
    UCHAR t[]="00\0";
    //HH:MM:SS,mls --> HH:MM:SS,mls
    t[0]=str[0];
    t[1]=str[1];
    SetWindowText(hSrtTime.hHours,t);
    t[0]=str[3];
    t[1]=str[4];
    SetWindowText(hSrtTime.hMinutes,t);
    t[0]=str[6];
    t[1]=str[7];
    SetWindowText(hSrtTime.hSeconds,t);   
}

void SearchPrev()
{
     UCHAR *p=buffer;
     DWORD len=bufferLen;   
     DWORD i = 0;
     DWORD n=tempLen-1;
     UCHAR *q;
     for(i=SearchIndexPrev;i>0;i--)
     {
       if(tolower(p[i]) == tolower(temp[n])){  n--; } else{ n=tempLen-1; }            
       if(n==0)
          break;
     }
     if(n!=0)
      return;
     SearchIndexNext=i+tempLen;
     //search the "-->" at the begin of frame
     n=0;
     for(i;i>0;i--)
     {
       if(p[i] == INDEX[n]){  n++; } else{ n=0; }            
       if(n==3)
          break;
     }
     if(n!=3) 
      return;
//try to search the  0xA at the begin of frame
     for(i;i>0;i--)
     {
       if(p[i] == 0xA)
          break;
     }
     if(i==1) //didn't found,then nothing
      return;
      SearchIndexPrev=i;
//try to search the last 0xA & 0xA
     for(n=SearchIndexNext;n<len-1;n++)
     {
       if((p[n] == 0xA) && (p[n+1] == 0xA) )
          break;
     }  
     SearchIndexNext=n;   
     n=SearchIndexNext-i;
     if(n>=MAX_PATH) 
        n=MAX_PATH-1;
     memset(FOUND,0,MAX_PATH);
     strncpy(FOUND,&p[i+1],n);
     for(i=0;i<n-1;i++){ if((FOUND[i]==0xA) && (FOUND[i+1]==0xA)){ FOUND[i]=0xD;i++; } }
     SetWindowText(logger,FOUND); 
     StringToTime(FOUND);        
}

void SearchNext()
{
     UCHAR *p=buffer;
     DWORD len=bufferLen;   
     DWORD i = 0;
     DWORD n=0;
     UCHAR *q;
     for(i=SearchIndexNext;i<len;i++)
     {
       if(tolower(p[i]) == tolower(temp[n])){  n++; } else{ n=0; }            
       if(n==tempLen)
          break;
     }
     if(n!=tempLen)
      return;
     SearchIndexNext=i;
     memset(FOUND,0,MAX_PATH);
     //try to search the "-->"
     n=0;
     for(i=SearchIndexNext;i>0;i--)
     {
       if(p[i] == INDEX[n]){  n++; } else{ n=0; }            
       if(n==3)
          break;
     }
     if(n!=3) //didn't found,then nothing
      return;
//try to search the  0xA at the begin of frame
     for(i;i>0;i--)
     {
       if(p[i] == 0xA)
          break;
     }
     if(i==1) //didn't found,then nothing
      return;
      SearchIndexPrev=i;
//try to search the last 0xA & 0xA
     for(n=SearchIndexNext;n<len-1;n++)
     {
       if((p[n] == 0xA) || (p[n+1] == 0xA) )
          break;
     }  
     SearchIndexNext=n;   
     n=SearchIndexNext-i;
     if(n>=MAX_PATH) 
        n=MAX_PATH-1;
     strncpy(FOUND,&p[i+1],n);
     for(i=0;i<n-1;i++){ if((FOUND[i]==0xA) && (FOUND[i+1]==0xA)){ FOUND[i]=0xD;i++; } }
     SetWindowText(logger,FOUND); 
     StringToTime(FOUND);           
}


void SearchIN()
{
   SearchIndexNext=SearchIndexPrev=0;
   memset(temp,0,33);
   tempLen=GetWindowText(hSearch,temp,32);
   if(buffer==NULL)
   tempLen=0; 
   if(tempLen>3)
   {
        EnableWindow(GetDlgItem(hWnd,ID_SEARCH_PREV),1);
        EnableWindow(GetDlgItem(hWnd,ID_SEARCH_NEXT),1); 
        EnableWindow(GetDlgItem(hWnd,ID_CALCULATE),1); 
        SearchNext();
   }
   else
   {
        EnableWindow(GetDlgItem(hWnd,ID_SEARCH_PREV),0);
        EnableWindow(GetDlgItem(hWnd,ID_SEARCH_NEXT),0);
        EnableWindow(GetDlgItem(hWnd,ID_CALCULATE),0);
   }
}

DWORD TextToDword(HWND hnd)
{
     UCHAR t[4]; DWORD ret;
     memset(t,0,4); 
     if(!GetWindowText(hnd,t,3)) return 0;
     ret=atoi(t);
     return ret;
}


void Calculate()
{
    DWORD SrtTime, RealTime;
    UCHAR t[32];
    SrtTime=TextToDword(hSrtTime.hHours);
    SrtTime*=60;
    SrtTime+=TextToDword(hSrtTime.hMinutes);
    SrtTime*=60;
    SrtTime+=TextToDword(hSrtTime.hSeconds);

    RealTime=TextToDword(hRealTime.hHours);
    RealTime*=60;
    RealTime+=TextToDword(hRealTime.hMinutes);
    RealTime*=60;
    RealTime+=TextToDword(hRealTime.hSeconds);
     
    if(RealTime==SrtTime)
       return;
    if(RealTime>SrtTime)
    { 
       sprintf(t,"%d\0",RealTime-SrtTime);
    }
    else
    { 
        sprintf(t,"-%d\0",SrtTime-RealTime);
    }
    SetWindowText(hValue,t);
}
//---------------------SYNC-------------------
static DWORD StringToDWORD(UCHAR *str)
{
    UCHAR t[]="00\0"; DWORD ret;
    //HH:MM:SS,mls --> HH:MM:SS,mls
    t[0]=str[0];
    t[1]=str[1];
    ret=atoi(t);
    ret*=60;
    t[0]=str[3];
    t[1]=str[4];
    ret+=atoi(t);
    ret*=60;
    t[0]=str[6];
    t[1]=str[7];
    ret+=atoi(t);
    return ret;  
}

void SyncFile()
{
	HANDLE hFile;  DWORD dwWritten; UCHAR FRAME[32];
    UCHAR NEGATIVE=NO; UINT SyncValue=0; UCHAR ISEMPTY; DWORD isEmpty=0;
    UCHAR *p=buffer;
	if(p==NULL)
	 return;
    DWORD frame=1; int nr,i;
	UCHAR *q,*begin;
	DWORD SrtTime; DWORD len,h,m,s;
//get the value to sync	
	memset(dropped,0,MAX_PATH);
	GetWindowText(hValue,dropped,32); //yeah,I know 10 is the right number,but who cares
	if(dropped[0]=='-'){ SyncValue=1; NEGATIVE=YES; }
	SyncValue=atoi(&dropped[SyncValue]);
	if(SyncValue==0){ SetWindowText(logger,"Sync Value is Invalid"); return; }
	
    sprintf(dropped,"%X.srt\0",GetTickCount());
    hFile = CreateFile(dropped, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        MessageBox(hWnd,"ERROR: Can't CreateFile!!!","#Error",MB_OK |MB_ICONERROR);        
        return;
    } 
 //UNICODE
    if(WriteFile(hFile, UnicodePrefix, 3, &dwWritten, NULL)==0)
    {
          MessageBox(hWnd,"ERROR: Can't WriteFile!!!","#Error",MB_OK |MB_ICONERROR);
          CloseHandle(hFile);      
          return;
    }
    
	p=strstr(p,"-->");
    while(p)
	{
       if(isEmpty>=999) isEmpty=0;
       
       q= p-0xD;
       begin=q;
       SrtTime=StringToDWORD(q);
       ISEMPTY=NO;
       if(NEGATIVE==YES)
       { 
            if(SrtTime<SyncValue){ SrtTime=0; ISEMPTY=YES;}else{ SrtTime-=SyncValue; }
        }else { SrtTime+=SyncValue; }

       s=SrtTime%60;
       SrtTime-=s; SrtTime/=60;
       m=SrtTime%60;
       SrtTime-=m; SrtTime/=60;
       h=SrtTime%60;
  
       snprintf(q,2,"%02d",h); q+=3;
       snprintf(q,2,"%02d",m); q+=3;
       snprintf(q,2,"%02d",s); q+=3;
       if(ISEMPTY==YES){ snprintf(q,3,"%03d",isEmpty); isEmpty++; } 
       p+=4;
       SrtTime=StringToDWORD(p);
       ISEMPTY=NO;
       if(NEGATIVE==YES)
       { 
            if(SrtTime<SyncValue){ SrtTime=0;ISEMPTY=YES;}else{ SrtTime-=SyncValue; }
        }else { SrtTime+=SyncValue; }

       s=SrtTime%60;
       SrtTime-=s; SrtTime/=60;
       m=SrtTime%60;
       SrtTime-=m; SrtTime/=60;
       h=SrtTime%60;
       
       snprintf(p,2,"%02d",h); p+=3;
       snprintf(p,2,"%02d",m); p+=3;
       snprintf(p,2,"%02d",s); p+=3;
       if(ISEMPTY==YES){ snprintf(p,3,"%03d",isEmpty); } 
       p=strstr(p,"\n\n\n\n");
       if(!p)
         break;
       nr=sprintf(FRAME,"%d\r\n\0",frame); frame++;  
       if(WriteFile(hFile, FRAME, nr, &dwWritten, NULL)==0)
       {
          MessageBox(hWnd,"ERROR: Can't WriteFile!!!","#Error",MB_OK |MB_ICONERROR);
          CloseHandle(hFile);      
          break;
       }
       nr=p-begin;
       for(i=0;i<nr-1;i++)
       {
          if((begin[i]==0xA) && (begin[i+1]==0xA)){ begin[i]=0xD; i++; }            
       }
       if(WriteFile(hFile, begin, nr, &dwWritten, NULL)==0)
       {
          MessageBox(hWnd,"ERROR: Can't WriteFile!!!","#Error",MB_OK |MB_ICONERROR);
          CloseHandle(hFile);      
          break;
       }
       if(WriteFile(hFile,"\r\n\r\n",4, &dwWritten, NULL)==0)
       {
          MessageBox(hWnd,"ERROR: Can't WriteFile!!!","#Error",MB_OK |MB_ICONERROR);
          CloseHandle(hFile);      
          break;
       }
       p=strstr(p,"-->");      
    }
CloseHandle(hFile);
MessageBox(hWnd,dropped,"SAVED",MB_OK |MB_ICONINFORMATION);
        
}

LRESULT CALLBACK SearchEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_LBUTTONDOWN)
    {
   	   SetWindowText(hwnd,"");
    }
    if((message==WM_KEYUP) && (wParam == VK_RETURN) )
    {
    CreateThread(0,0,(LPTHREAD_START_ROUTINE)SearchIN,0,0,0);
	}               
	return CallWindowProc(OldSearchEditProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  
    {
         case WM_CREATE:
         {
              HFONT hFont;
              HWND s1;
              hWnd = hwnd;
              hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
			  hSearch=CreateWindow("EDIT", "Search",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_DISABLED,2, 2, 98, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hSearch, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hSearch, EM_SETLIMITTEXT,(WPARAM)32,(LPARAM)0); 
              OldSearchEditProc= (WNDPROC) SetWindowLong(hSearch, GWL_WNDPROC, (LPARAM)SearchEditProc);

              s1=CreateWindow("BUTTON","Prev",WS_CHILD|WS_VISIBLE|WS_DISABLED,2,21,47,18,hwnd,(HMENU)ID_SEARCH_PREV,ins,NULL);
              SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              s1=CreateWindow("BUTTON","Next",WS_CHILD|WS_VISIBLE|WS_DISABLED,54,21,47,18,hwnd,(HMENU)ID_SEARCH_NEXT,ins,NULL);
              SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
//srt time
              s1=CreateWindow("BUTTON","Srt Time",WS_VISIBLE|WS_CHILD|BS_GROUPBOX|BS_CENTER,105,2,75,38,hwnd,(HMENU) 0,ins,NULL);
              SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              
              hSrtTime.hHours =CreateWindow("EDIT", "00",WS_CHILD|WS_VISIBLE|ES_READONLY,112, 22, 17, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hSrtTime.hHours, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              hSrtTime.hMinutes =CreateWindow("EDIT", "00",WS_CHILD|WS_VISIBLE|ES_READONLY,134, 22, 17, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hSrtTime.hMinutes, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              hSrtTime.hSeconds =CreateWindow("EDIT", "00",WS_CHILD|WS_VISIBLE|ES_READONLY,156, 22, 17, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hSrtTime.hSeconds, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
//real time

              s1=CreateWindow("BUTTON","Real Time",WS_VISIBLE|WS_CHILD|BS_GROUPBOX|BS_CENTER,185,2,75,38,hwnd,(HMENU) 0,ins,NULL);
              SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              
              hRealTime.hHours =CreateWindow("EDIT", "00",WS_CHILD|WS_VISIBLE|ES_NUMBER,192, 22, 17, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hRealTime.hHours, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hRealTime.hHours, EM_SETLIMITTEXT,(WPARAM)2,(LPARAM)0); 
              OldTimerProc= (WNDPROC) SetWindowLong(hRealTime.hHours, GWL_WNDPROC, (LPARAM)TimerProc);
	
              hRealTime.hMinutes =CreateWindow("EDIT", "00",WS_CHILD|WS_VISIBLE|ES_NUMBER,214, 22, 17, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hRealTime.hMinutes, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hRealTime.hMinutes, EM_SETLIMITTEXT,(WPARAM)2,(LPARAM)0); 
              OldTimerProc= (WNDPROC) SetWindowLong(hRealTime.hMinutes, GWL_WNDPROC, (LPARAM)TimerProc);

              hRealTime.hSeconds =CreateWindow("EDIT", "00",WS_CHILD|WS_VISIBLE|ES_NUMBER,236, 22, 17, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hRealTime.hSeconds, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              SNDMSG(hRealTime.hSeconds, EM_SETLIMITTEXT,(WPARAM)2,(LPARAM)0); 
              OldTimerProc= (WNDPROC) SetWindowLong(hRealTime.hSeconds, GWL_WNDPROC, (LPARAM)TimerProc);
              
              s1=CreateWindow("BUTTON","GET\nSync",WS_CHILD|WS_VISIBLE|BS_MULTILINE|WS_DISABLED,264,2,38,38,hwnd,(HMENU)ID_CALCULATE,ins,NULL);
              SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              hValue =CreateWindow("EDIT", "",WS_CHILD|WS_VISIBLE,305, 2, 47, 15, hwnd, NULL,ins, NULL);  	
              SNDMSG(hValue, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
              
              s1=CreateWindow("BUTTON","SYNC",WS_CHILD|WS_VISIBLE|WS_DISABLED,305,20,47,19,hwnd,(HMENU)ID_SYNC,ins,NULL);
              SNDMSG(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
                            	
              logger=CreateWindow("EDIT", "",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE,2, 42, 350, 98, hwnd, NULL,ins, NULL);  	
              SNDMSG(logger, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
			  CenterOnScreen(hwnd);
              DragAcceptFiles(hwnd,1);   
         }
         break;
         case WM_DROPFILES:
         {
              HDROP hDrop;
              memset(dropped,0,MAX_PATH);
              hDrop=(HDROP)wParam;
              DragQueryFile(hDrop,0,dropped,MAX_PATH);
              DragFinish(hDrop);
              CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadSrt,0,0,0); 
         }
         break; 
         case WM_COMMAND:
         {
              switch(LOWORD(wParam))
              { 
 				  case ID_SEARCH_PREV:
                  {     
                       CreateThread(0,0,(LPTHREAD_START_ROUTINE)SearchPrev,0,0,0); 
                  }
                  break; 
 				  case ID_SEARCH_NEXT:
                  {     
                       CreateThread(0,0,(LPTHREAD_START_ROUTINE)SearchNext,0,0,0); 
                  }
                  break; 
 				  case ID_CALCULATE:
                  {     
                       CreateThread(0,0,(LPTHREAD_START_ROUTINE)Calculate,0,0,0); 
                  }
                  break; 
                  case ID_SYNC:
                  {
                       CreateThread(0,0,(LPTHREAD_START_ROUTINE)SyncFile,0,0,0); 
                  }break;
              }//switch
         }
         break;

        case WM_DESTROY:
        {
              PostQuitMessage (0); 
        } 
        break;
        default:         
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
            
    MSG messages;    
    WNDCLASSEX wincl; 
    HWND hwnd;    
    ins=hThisInstance;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;  
    wincl.cbSize = sizeof (WNDCLASSEX);


    wincl.hIcon = LoadIcon (ins,"A");
    wincl.hIconSm = LoadIcon (ins,"A");
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;  
    wincl.cbClsExtra = 0;  
    wincl.cbWndExtra = 0;      

    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;


    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx(WS_EX_TOPMOST,szClassName,"SRT Sync",WS_OVERLAPPED|WS_SYSMENU,CW_USEDEFAULT,CW_USEDEFAULT,
    361,174,HWND_DESKTOP,NULL,hThisInstance,NULL );
    
    ShowWindow (hwnd, nFunsterStil);

    while (GetMessage (&messages, NULL, 0, 0))
    {
         TranslateMessage(&messages);
         DispatchMessage(&messages);
    }

     return messages.wParam;
}

void CenterOnScreen(HWND hnd)
{
  RECT rcClient, rcDesktop;
  int nX,nY;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
  GetWindowRect(hnd, &rcClient);
  nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
  nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
  SetWindowPos(hnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
return;
}

void SMS(UCHAR *a,UCHAR *b)
{
     DWORD len=GetWindowTextLength(logger);
     if(len>0)  SendMessage(logger,EM_SETSEL,(WPARAM)len,(LPARAM)len);
     if(a) SendMessage(logger,EM_REPLACESEL,(WPARAM)0,(LPARAM)a);
     if(b) SendMessage(logger,EM_REPLACESEL,(WPARAM)0,(LPARAM)b);
     SendMessage(logger,WM_VSCROLL,(WPARAM)SB_ENDSCROLL,(LPARAM)0);
     return;
}

UCHAR ReadDataFile(UCHAR *name)
{
    HANDLE fd;
    DWORD result=0;
    if(buffer!=NULL){ free(buffer); buffer=NULL; }
    fd = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
    {
       MessageBox(hWnd,name,"#Error: Can't Open File",MB_OK |MB_ICONERROR);
      return NO;
    }
    bufferLen = GetFileSize(fd,NULL);
    if(bufferLen == 0xFFFFFFFF)
    {
        MessageBox(hWnd,name,"#Error: FileSize",MB_OK |MB_ICONERROR);        
        CloseHandle(fd);
        return NO;
    }
    buffer=(UCHAR*)malloc(bufferLen+1);
    if(!buffer)
    {
        MessageBox(hWnd,name,"#Error: malloc",MB_OK |MB_ICONERROR);
        CloseHandle(fd);
        return NO;
    }
    memset(buffer,0,bufferLen+1);
    if(ReadFile(fd, buffer, bufferLen, &result, NULL) == FALSE)
    {
        MessageBox(hWnd,name,"#Error: Can't Read File",MB_OK |MB_ICONERROR);        
        CloseHandle(fd);
        return NO;
    }
    if(result != bufferLen)
    {
        MessageBox(hWnd,name,"#Error: Reading File",MB_OK |MB_ICONERROR);        
        CloseHandle(fd);
        return NO;
    }
    else
    {
      DWORD i; //I dont know if the .srt is valid only when it contains \r\n,just in case
      for(i=0;i<bufferLen;i++)  //I'm changing all \r\n to \n\n and before writing to file 
      {                         //all \n\n will be converted to \r\n
        if(buffer[i]==0xD) buffer[i]=0xA;
      }
    }
    CloseHandle(fd);
    
  return YES;
}

