#define CQAPPID "com.example.democ" //请修改AppID，规则见 http://d.cqp.me/Pro/开发/基础信息
#define CQAPPINFO CQAPIVERTEXT "," CQAPPID
static int isfull = 0;
static int startmon = 1000;
static int lev = 1000;
static int inzoo = 0;
static int winmoney = 0;

static int startbug = 0;  //0 关闭 1 开启 是否开启赌博
static int startguess = 0;//0 关闭 1 开启 是否开启猜数
static int maxguess = 100;
static int minguess = 1;
static int newguess = 50;
