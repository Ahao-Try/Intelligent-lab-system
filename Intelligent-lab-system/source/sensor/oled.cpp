#include "../../include/sensor/oled.h"
#include "../../include/sensor/error.h"


//显示字符
void show_char(int x,int y,int offset){
    int i = 0;

    for(i = 0;i < 8;i++){
        buffer[y * 128 + x + i] = F8X16[offset*16 + i];
    }
    y += 1;
    for(i = 0;i < 8;i++){
        buffer[y * 128 + x + i] = F8X16[offset*16 + 8 + i];
    }
}

//显示字符串
void show_string(int xx,int yy,char* s){
    int len = 0;
    int i = 0;
    int offset = 0;//ascll码偏移量
    int x = xx;
    int y = yy;

    len = strlen(s);
    for(;i < len;i++){
        offset = s[i] - ' ';
        if(x>128){
            x = 0;
            y += 2;
        }
        show_char(x,y,offset);
        x += 8;
    }
}

//数字转字符串
char* Int2String(int num){
    std::string s;
    char* c = (char*)malloc(16);

    s = std::to_string(num);

    strcpy(c,s.c_str());

    return c;
}

char* Double2String(double num){
    std::string s;
    char* c = (char*)malloc(16);

    s = std::to_string(num);

    strcpy(c,s.c_str());
    
    return c;
}

int ShowOled(char* dev,char* s){
     int ret = 0;
    int fd = -1;
    int i = 0;

    fd = open(dev,O_RDWR);
    if(fd == -1){
        perror("open oled device error\r\n");
        return -OPEN_OLED_DEV_ERROR;
    }

    show_string(0,0,s);

    ret = write(fd,buffer,sizeof(buffer));
    if(ret < 0){
        perror("write oled error\r\n");
        return -WRITE_OLED_ERROR;
    }
    
    close(fd);
    return 0;
}
