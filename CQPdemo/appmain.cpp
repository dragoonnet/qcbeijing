/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include <mysql.h>
#include <map> 
#include <vector>
#include <time.h> 

using namespace std;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;

const char user[] = "root";
const char pswd[] = "123456";
const char host[] = "127.0.0.1";
const char db[] = "beijing";
unsigned int port = 3306;
MYSQL myCont;
MYSQL_RES *result;
MYSQL_ROW sql_row;
string sql = "";

map<string, string> AllUser;

//map对象中去判断是否存在
bool findKey(map<string, string>mp, const string& keyName)
{
	return (mp.find(keyName) != mp.end());
}


/*
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21  私聊消息
* subType  1.子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
* sendTime 2.时间戳
* fromQQ   3.哪个QQ发过来的
* msg      4.消息内容
* font     字体
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_IGNORE;
}


bool strIndexOf(string src, char * inChar) {
	if (src.find(inChar) != std::string::npos) {
		return true;
	}
	return false;
}


//往一个群发消息
void mysendDataByuni(int64_t from_uin, string msg1, int saytype)
{
	if (saytype == 0)
		CQ_sendGroupMsg(ac, from_uin, msg1.c_str());
	else
		CQ_sendDiscussMsg(ac, from_uin, msg1.c_str());
}


vector< map<string, string> > execsql(string sql) {
	int res;
	CQ_addLog(ac, CQLOG_DEBUG, "执行sql", sql.c_str());

	mysql_init(&myCont);
	vector< map<string, string> > v;
	if (mysql_real_connect(&myCont, host, user, pswd, db, port, NULL, 0)) {
		mysql_query(&myCont, "SET NAMES GBK"); //设置编码格式
		res = mysql_query(&myCont, sql.c_str());//查询
		if (!res) {
			result = mysql_store_result(&myCont);
			if (result) {
				//获取并输出表头       
				int fieldscount = mysql_num_fields(result);//字段数
				MYSQL_FIELD *fieldNames = mysql_fetch_fields(result);
				//CQ_addLog(ac, CQLOG_DEBUG, "字段总数", to_string(fieldscount).c_str());

				while (sql_row = mysql_fetch_row(result)) {
					//CQ_addLog(ac, CQLOG_DEBUG, "有数据", "");
					//string tmpstr2 = "";
					map<string, string>   my_Map;

					for (int i = 0; i < fieldscount; i++) {
						//tmpstr2 += ",";
						if (sql_row[i] != 0) {
							//tmpstr2 += sql_row[i];
							my_Map[fieldNames[i].name] = sql_row[i];
						}
						else my_Map[fieldNames[i].name] = "";
					}
					//CQ_addLog(ac, CQLOG_DEBUG, "获取到的第一个数据为", tmpstr2.c_str());
					v.push_back(my_Map);
				}
				mysql_close(&myCont);
				return v;
			}
			else
			{
				//insert 函数 需要返回id
				map<string, string>   my_Map;
				my_Map["id"] = to_string(myCont.insert_id);
				my_Map["affected_rows"] = to_string(myCont.affected_rows);
				v.push_back(my_Map);
			}

		}
		else {
			CQ_addLog(ac, CQLOG_DEBUG, "查询空数据", "");
		}
	}
	else {
		CQ_addLog(ac, CQLOG_DEBUG, "连接数据库失败", "");
	}
	if (result != NULL)	mysql_free_result(result);
	mysql_close(&myCont);
	return v;
}



void gamer(int64_t from_uin, int64_t fromQQ, string sanysay, int saytype) {
	if (AllUser.empty()) {
		vector< map<string, string> > v = execsql("select qq from qquser");
		if (!v.empty())
		{
			for (size_t i = 0; i < v.size(); i++) {
				map<string, string> mp = v[i];
				//从数据库获取游戏开关状态
				AllUser[mp["qq"]] = mp["isclose"];
			}
		}
	}
	//开启游戏指令
	if (sanysay == "帝都浮生记") {
		//判断用户是否存在,
		vector< map<string, string> > v = execsql("select * from qquser where qq=" + to_string(fromQQ));
		if (v.empty()) {
			//新用户
			string str = "insert into qquser(qq) values('" + to_string(fromQQ) + "')";
			execsql(str);
			//初始化游戏数据
			str = "INSERT INTO shangping(name, val, rate, qq,sp) SELECT name, val, (RAND() * 20 + 2) / 10, '" + to_string(fromQQ) + "',sp FROM shangping  where qq = '0'";
			execsql(str);
		}
		else
		{
			v.clear();
		}
		AllUser[to_string(fromQQ)] = "0";

		string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
			+ "您成功开启了《帝都浮生记》游戏!\n序幕:作为一个苦逼的北漂,你向村长借了2500元的高利贷,来帝都租了个2000的房进行闯荡,您需要30天内赚够钱回村...\n请输入:功能\n"
			+ "获取游戏帮助,更多想法或问题反馈请联系QQ:8248963\n";
		mysendDataByuni(from_uin, msg1, saytype);
		return;
	}

	if (findKey(AllUser, to_string(fromQQ))) {
		if (AllUser[to_string(fromQQ)] == "1") {
			return;
		}

		if (sanysay == "功能" || sanysay == "帮助") {
			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "《帝都浮生记》\n"
				+ "您可以输入:\n"
				+ "[CQ:emoji,id=128092]买卖:查看可以做点什么生意.\n"
				+ "[CQ:emoji,id=128652]去哪儿:查看可以去哪儿摆摊.\n"
				+ "[CQ:emoji,id=127859]查询:查看个人资料\n"
				+ "[CQ:emoji,id=127881]排名:查看其他人的排名\n"
				+ "[CQ:emoji,id=127978]租房:查看今日房价\n"
				+ "[CQ:emoji,id=127968]仓库:查看你的库存\n"
				+ "[CQ:emoji,id=128666]汇款:输入(汇款100),可以偿还100元的高利贷\n"
				+ "[CQ:emoji,id=128083]存款:输入(存款100),可以往银行存款100元,每天还有利息哦!\n"
				+ "[CQ:emoji,id=128104]取款:输入(取款100),可以到银行取款100元!\n"
				+ "[CQ:emoji,id=10060]关闭帝都浮生记:关闭游戏，如需重新打开，输入游戏名称即可！";
			mysendDataByuni(from_uin, msg1, saytype);
		}
		else if (sanysay == "关闭帝都浮生记") {
			sql = "update qquser set isclose=1 where qq='+ to_string(fromQQ)  +'";
			execsql(sql);					//设置数据库封印
			AllUser[to_string(fromQQ)] = "1";//设置内存封印

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "[CQ:emoji,id=128561][CQ:emoji,id=128561][CQ:emoji,id=128561]花心了呗，玩腻了就想封印我！好吧《帝都浮生记》就此对你停止服务，\n"
				+ "封印成功，期待你呼唤我的名字再次开启我的一天...\n";
			mysendDataByuni(from_uin, msg1, saytype);

		}
		else if (sanysay == "买卖") {
			sql = "select name,(s.val * s.rate) as val,s.owernum,s.cost,q.money,q.smoney,q.debt,s.enable from shangping s,qquser q where s.sp=0 and q.qq=s.qq and s.qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "《帝都浮生记》\n";
				msg1 += "您有[CQ:face,id=158]现金" + vv[0]["money"] + "元，[CQ:emoji,id=127975]存款" + vv[0]["smoney"] + "元，[CQ:emoji,id=128176]高利贷" + vv[0]["debt"] + "元\n";
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];

					if (mp["enable"] == "1") {//不显示禁止买卖的商品
						mp.clear();
						continue;
					}
					msg1 += "买/卖" + to_string(i + 1) + ":";
					msg1 += mp["name"];
					if (mp["owernum"] == "0") {
						msg1 += "[CQ:face,id=158]￥" + mp["val"] + " 个/元\n";
					}
					else msg1 += "[CQ:face,id=158]￥" + mp["val"] + " 个/元  库存" + mp["owernum"] + "个 [CQ:emoji,id=128092]成本￥" + mp["cost"] + "个/元\n";
					mp.clear();
				}
				msg1 += "帮助:输入\"买1*100\" 或者 \"卖1*100\" 或者\"买1*最大\" 可以买卖商品.";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "去哪儿") {
			sql = "select * from weizhi";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "《帝都浮生记》\n";
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];
					msg1 += "去" + to_string(i + 1) + ": ";
					msg1 += mp["addr"] + "\n";
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "我" || sanysay == "查询") {
			sql = "select * from qquser where qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "《帝都浮生记》\n";
				if (vv.size() > 0) {
					map<string, string> mp = vv[0];
					msg1 += "现在是第" + mp["days"] + "天\n";
					msg1 += "[CQ:emoji,id=128180]您的现金:" + mp["money"] + "\n";
					msg1 += "[CQ:emoji,id=128179]您的存款:" + mp["smoney"] + "\n";
					msg1 += "[CQ:emoji,id=128184]您的欠债:" + mp["debt"] + "\n";
					msg1 += "[CQ:emoji,id=9994]您的健康:" + mp["hp"] + "\n";
					msg1 += "[CQ:emoji,id=128591]您的名声:" + mp["honor"] + "\n";
					msg1 += "[CQ:emoji,id=127969]房间大小:" + mp["useroom"] + "/" + mp["room"] + "\n";
					msg1 += "[CQ:emoji,id=128185]历史最高:" + mp["maxmon"] + "元";
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "排名" || sanysay == "排行") {
			sql = "select qq,maxmon from qquser order by maxmon desc limit 0,10";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "《帝都浮生记》\n";
				msg1 += "前10名榜单\n";
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];
					msg1 += "排名" + to_string(i + 1) + ": " + "[CQ:emoji,id=12854"+to_string(i)+"][CQ:at,qq=" + mp["qq"] + "]" + mp["qq"] + " 得分：" + mp["maxmon"] + "分\n";
					mp.clear();
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "租房") {
			sql = "select name,(val * rate) as val,owernum,cost from shangping where sp=1 and qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "《帝都浮生记》\n黑心中介公布的房价:\n";

				msg1 += "今日特价房源[CQ:emoji,id=127970]:只需" + vv[0]["val"] + "元,立即拥有超大客厅让你可以多装10个货物。";
				msg1 += "早买早享福，明天就涨价了,大兄弟是不是要租房?\n输入\"确定租房\",您立即就成为帝都人了[CQ:emoji,id=10084][CQ:emoji,id=10084]!";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}

		}
		else if (sanysay == "确定租房") {
			sql = "select name, (val * rate) as val, owernum, cost, qquser.`money`,(qquser.`money`-(val * rate)) remain from shangping, qquser where sp = 1 and shangping.`qq` = qquser.qq and qquser.qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string remain = vv[0]["remain"];
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n《北京浮生记》\n";
				if (remain.find("-") == string::npos) {
					//结果2 钱够了 提示房子样子
					msg1 += "租房成功!\n黑心中介半眯着眼挤出来的笑着:\n大兄弟,遇到我是你的福气,最后一套租给你我们就涨价了,你这辈子终于选对了...\n您的房间增大了10个空间!";
					msg1 += "(也不知道自己被中介骗了多少)";
					//+空间-钱
					sql = "update qquser set money=" + remain + ",room=room+10 where qq='" + to_string(fromQQ) + "'";
					execsql(sql);

					//涨价涨10倍//房价暴涨,避免一直买
					sql = "update shangping set rate=rate+5 where qq='" + to_string(fromQQ) + "'";
					execsql(sql);
				}
				else {
					//结果1 钱不够
					msg1 += "黑心中介对你咆哮道:你个穷逼[CQ:emoji,id=128074]钱不够还好意思来帝都,滚回你的乡下去[CQ:emoji,id=128298][CQ:emoji,id=128298]...(还有一堆不堪入目的话)";
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "房间" || sanysay == "仓库" || sanysay == "库存") {
			//获取我的房间大小和里面的货物
			sql = "select * from shangping where owernum>0 and enable=0 and qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "《帝都浮生记》\n库存情况:\n";

			if (!vv.empty()){
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];
					msg1 += mp["name"];
					msg1 += mp["owernum"] + "个 成本￥" + mp["cost"] + "个/元\n";
					mp.clear();
				}
				msg1 += "帮助:输入\"买1*100\" 或者 \"卖1*100\" 可以买卖商品.";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
			else
			{
				msg1 += "你着急忙慌的跑回家，看见空空荡荡的屋子，什么都没有[CQ:emoji,id=128588][CQ:emoji,id=128588]...\n";
				msg1 += "帮助:输入\"买1*100\" 或者 \"卖1*100\" 可以买卖商品.";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (strIndexOf(sanysay, "买")) {
			//期望 买1*100 或者 买1*最大

			int pos1 = sanysay.find("*");
			string goodsno = sanysay.substr(2, pos1 - 2);//获取  1

			if (goodsno.length() > 2) return;//输入的货物代码过长 退出
			int iGoodsNo = std::atoi(goodsno.c_str());
			if (iGoodsNo == 0)return; //输入的货物代码为0 退出

			iGoodsNo--;// 用户输入1 就是要第0个商品

			string goodsNum = sanysay.substr(pos1 + 1, sanysay.length() - pos1);//获取  100

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "《帝都浮生记》\n";
			int iGoodsNum = 0;
			string remainMoney = "0";

			//获取你想买的数量
			if (goodsNum.length() > 9)return;//购买数据长度过长 退出


			iGoodsNum = std::atoi(goodsNum.c_str());
			sql = "select *,money-buynum*val*rate as remain,val * rate as cost, money-buynum2*val*rate as remain2,money - remainroom * val * rate as remain3  from (select t1.*,t2.room,t2.room-t2.useroom as remainroom,t2.money,FLOOR(t2.money/(t1.val*t1.rate)) as buynum," + to_string(iGoodsNum) + " as buynum2 from shangping t1, qquser t2 where t2.qq='"
				+ to_string(fromQQ) + "' and t1.qq = t2.qq order by t1.id asc limit " + to_string(iGoodsNo) + ",1)t";
			vector< map<string, string> >  vv = execsql(sql);

			if (!vv.empty()) {
				if (vv[0]["enable"] == "1") {
					msg1 += "购买失败:没人进行交易!\n其他小贩嘲笑你[CQ:emoji,id=128568]:你是在厕所里喝稀饭,来错了地方了,...";
					mysendDataByuni(from_uin, msg1, saytype);
					vv.clear();
					return;//货物码不匹配
				}
			}

			int iRemainRoom = std::atoi(vv[0]["remainroom"].c_str());
			if (iRemainRoom == 0) {
				msg1 += "购买失败:房间没有空间了!\n其他小贩嘲笑你[CQ:emoji,id=128568]:你家也像猪圈一样小[CQ:emoji,id=128023], 去换套别墅吧, 不然你睡觉都只能站着了...\n\n帮助：输入\"租房\"查看今日房价。";
				vv.clear();
				mysendDataByuni(from_uin, msg1, saytype);
				return;//房间空间不足
			}

			if (goodsNum == "最大") {
				//购买最大量
				//判断最大量买是不是一个都不能买
				if (vv[0]["buynum"] == "0") {
					msg1 += "购买失败:您的现金不足!\n其他小贩嘲笑你[CQ:emoji,id=128568]:这点钱上厕所够不够,土包子充大款了[CQ:emoji,id=128576]...";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//按最大数量购买金额不足,退出
				}
				//记录用户想买的数量
				iGoodsNum = std::atoi(vv[0]["buynum"].c_str());//所有钱能购买的数量
				remainMoney = vv[0]["remain"];   //所有钱都买了剩下多少钱

												 //如果所有钱买，空间会不足， 剩余多少，买多少
				if (iGoodsNum > iRemainRoom) {
					iGoodsNum = iRemainRoom;
					remainMoney = vv[0]["remain3"];//remain3 将剩余空间装满收剩下多少钱 
				}
			}
			else
			{
				//购买数量为数字

				if (iGoodsNum == 0) return;//转换出的数量为0 退出

				int canBuy = std::atoi(vv[0]["buynum"].c_str());
				//判断要买的数量钱够不
				if (iGoodsNum > canBuy) {
					msg1 += "购买失败:您的现金不足!\n其他小贩嘲笑你[CQ:emoji,id=128568]:现金不足,回去问爸妈要够了钱再来买...";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//输入的数量不足以购买,退出
				}
				remainMoney = vv[0]["remain2"];
			}

			//判断房间是否足够大小
			if (iGoodsNum > iRemainRoom) {
				msg1 += "购买失败:房间没有空间了!\n其他小贩嘲笑你[CQ:emoji,id=128568]:你家也像猪圈一样小[CQ:emoji,id=128023], 去换套别墅吧, 不然你睡觉都只能站着了...\n\n帮助：输入\"租房\"查看今日房价。";
				vv.clear();
				mysendDataByuni(from_uin, msg1, saytype);
				return;//输入的数量不足以购买,退出
			}

			//修改房间大小与钱大小 和 声誉
			//上海宝贝和 毒酒需要 -20 -10 的声誉****************************************************************************************************
			sql = "update qquser set useroom = useroom + " + to_string(iGoodsNum) + " , money = " + remainMoney + " where qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//修改库存
			sql = "update shangping set owernum = owernum + " + to_string(iGoodsNum) + " , cost = '" + vv[0]["cost"] + "' where id='" + vv[0]["id"] + "'";
			execsql(sql);
			msg1 += "购买成功[CQ:emoji,id=10004]!!\n您购买了" + to_string(iGoodsNum) + "个" + vv[0]["name"] + "\n剩余现金[CQ:emoji,id=128180]:" + remainMoney + "元\n";
			msg1 += "帮助:您可以输入\"仓库\" 查看库存,也可以继续买买买,还可以输入\"去哪儿\"到其它地方去看看!!";
			mysendDataByuni(from_uin, msg1, saytype);
			return;
		}
		else if (strIndexOf(sanysay, "卖")) {
			//期望 卖1*100 或者 卖1*最大

			int pos1 = sanysay.find("*");
			string goodsno = sanysay.substr(2, pos1 - 2);//获取  1

			if (goodsno.length() > 2) return;//输入的货物代码过长 退出
			int iGoodsNo = std::atoi(goodsno.c_str());
			if (iGoodsNo == 0)return; //输入的货物代码为0 退出

			iGoodsNo--;// 用户输入1 就是要第0个商品

			string goodsNum = sanysay.substr(pos1 + 1, sanysay.length() - pos1);//获取  100


			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "《帝都浮生记》\n";
			int iGoodsNum = 0;
			string remainMoney = "0";

			//获取你想买的数量
			if (goodsNum.length() > 9)return;//购买数据长度过长 退出

			iGoodsNum = std::atoi(goodsNum.c_str());
			sql = "select *,money+owernum*val*rate as remain,val * rate as cost, money+buynum2*val*rate as remain2 from (select t1.*,t2.room,t2.room-t2.useroom as remainroom,t2.money,FLOOR(t2.money/(t1.val*t1.rate)) as buynum," + to_string(iGoodsNum) + " as buynum2 from shangping t1, qquser t2 where t2.qq='"
				+ to_string(fromQQ) + "' and t1.qq = t2.qq order by t1.id asc limit " + to_string(iGoodsNo) + ",1)t";
			vector< map<string, string> >  vv = execsql(sql);

			if (!vv.empty()) {
				if (vv[0]["enable"] == "1") {
					msg1 += "购买失败:没人进行交易!\n其他小贩嘲笑你[CQ:emoji,id=128568]:你是在厕所里喝稀饭,来错了地方了[CQ:emoji,id=127917][CQ:emoji,id=127917][CQ:emoji,id=127917]...";
					mysendDataByuni(from_uin, msg1, saytype);
					vv.clear();
					return;//货物码不匹配
				}
			}

			if (goodsNum == "最大") {
				//购买最大量
				//判断最大量买是不是一个都不能买
				if (vv[0]["owernum"] == "0") {
					msg1 += "购买失败:你没有这个货物!\n其他小贩嘲笑你[CQ:emoji,id=128568]:牛粪也准备当黄金卖了,[CQ:emoji,id=128169][CQ:emoji,id=128169][CQ:emoji,id=128169]想要钱去抢银行来得快...";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//没有这个货物的库存
				}

				//记录用户想卖的数量
				iGoodsNum = std::atoi(vv[0]["owernum"].c_str());
				//贩卖后最终会获得的钱.
				remainMoney = vv[0]["remain"];
			}
			else
			{
				//购买数量为数字

				if (iGoodsNum == 0) return;//转换出的数量为0 退出

				int canSell = std::atoi(vv[0]["owernum"].c_str());
				//判断够不够卖
				if (iGoodsNum > canSell) {
					msg1 += "售卖失败:货物数量不足!\n其他小贩嘲笑你[CQ:emoji,id=128568]:半斤想当八两卖,[CQ:emoji,id=128026]没有看见过你这样奸商.....";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//输入的数量不足以购买,退出
				}
				remainMoney = vv[0]["remain2"];
			}

			//修改房间大小与钱大小 和 声誉
			//上海宝贝和 毒酒需要 -20 -10 的声誉****************************************************************************************************
			sql = "update qquser set useroom = useroom - " + to_string(iGoodsNum) + " , money = " + remainMoney + " where qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//修改库存
			sql = "update shangping set owernum = owernum - " + to_string(iGoodsNum) + " where id='" + vv[0]["id"] + "'";
			execsql(sql);
			msg1 += "售卖成功[CQ:emoji,id=10024]!!\n您售卖了" + to_string(iGoodsNum) + "个" + vv[0]["name"] + "\n剩余现金[CQ:emoji,id=128180]:" + remainMoney + "元\n";
			msg1 += "帮助:您可以输入\"仓库\" 查看库存,也可以继续卖卖卖,还可以输入\"去哪儿\"到其它地方去看看!!";
			mysendDataByuni(from_uin, msg1, saytype);
			return;
		}
		else if (strIndexOf(sanysay, "去")) {
			//期望 去1  去2
			string goNO = sanysay.substr(2, sanysay.length() - 2);//获取  1
			if (goNO.length() >3)return;//去的长度过大
			int iGoNO = std::atoi(goNO.c_str());

			if (iGoNO == 0) return;//去的位置不是数字

			iGoNO--; // 用户输入1 就是要第0个地方

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "《帝都浮生记》\n";

			srand((unsigned)time(NULL));
			int stopGoods = rand() % 7;
			//启用所有商品,并重新  更新利率
			sql = "update shangping set rate=(RAND()*20 + 2)/10 ,enable = 0 where qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//没有的商品不显示成本
			sql = "update shangping set cost = 0 where owernum=0 and qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//禁用一个商品
			//sql = "update shangping set enable = 1 where qq='" + to_string(fromQQ) + "' order by id asc  limit " + to_string(stopGoods) + ",1";
			sql = "select id from shangping where qq='" + to_string(fromQQ) + "' order by id asc  limit " + to_string(stopGoods) + ",1";
			string delid = execsql(sql)[0]["id"];
			sql = "update shangping set shangping.`enable` = 1 where id=" + delid;
			execsql(sql);

			// +天数 +利息 +负债
			sql = "update qquser set days=days+1,debt=debt*107/100,smoney= smoney*101/100   where  qq='" + to_string(fromQQ) + "'";
			execsql(sql);
			//生成随机事件,更新商品价格 或者影响 健康 名誉 ************************************
			//
			//健康判断 ********************

			//名誉判断 ********************

			//负债判断 ********************

			//获取天数顺便把商品信息获取了，可以用于显示。
			sql = "select name,(s.val * s.rate) as val,s.owernum,s.cost,q.money,q.smoney,q.debt,q.days,s.enable from shangping s,qquser q where s.sp=0 and q.qq=s.qq and s.qq='" + to_string(fromQQ) + "'";

			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty()) {
				//获取游戏天数
				int iDay = std::atoi(vv[0]["days"].c_str());
				if (iDay > 30) {
					//游戏结束
					sql = "select t.cc,win.*,(t.cc-qquser.`maxmon` )ismax from (select sum(val*rate*owernum) as cc from shangping where qq='" + to_string(fromQQ) + "')t ,qquser ,win where qq='" + to_string(fromQQ) + "' and t.cc > win.money order by id desc limit 0,1";
					vector< map<string, string> >  vv2 = execsql(sql);

					//重置数据
					//启用所有商品,并重新  更新利率
					sql = "update shangping set rate=(RAND()*20 + 2)/10 ,owernum=0,cost=0,enable = 0 where qq='" + to_string(fromQQ) + "'";
					execsql(sql);

					string newRecord = vv2[0]["cc"];//默认为新纪录

					if (vv2[0]["ismax"].find("-") != string::npos) {
						//不是最新的记录
						newRecord = "maxmon";
					}
					//如果比之前的高才更新分数,重置金额   顺便重置分数
					sql = "update qquser set useroom=0,gametime=gametime+1,days=1, room=100,honor=100, hp=100, money=500,smoney=0,debt=2000,maxmon =" + newRecord + " where qq='" + to_string(fromQQ) + "'";
					execsql(sql);

					//用当前金额去获取排名
					sql = "select count(*) cc from qquser where maxmon >= " + vv2[0]["cc"];
					vector< map<string, string> >  vv3 = execsql(sql);

					if (!vv2.empty()) {
						msg1 += "[CQ:emoji,id=9728][CQ:emoji,id=9728][CQ:emoji,id=9728]您贩卖了所有货物,最终获取了" + vv2[0]["cc"] + "元\n";
						msg1 += "最终:" + vv2[0]["msg"];
						msg1 += "本次游戏排名:[CQ:emoji,id=127881]第 " + vv3[0]["cc"] + " 名[CQ:emoji,id=127881]";

					}
					mysendDataByuni(from_uin, msg1, saytype);
				}
				else
				{
					iDay++;
					msg1 += "[CQ:emoji,id=9728]现在是第" + to_string(iDay) + "天\n经过一天的跋涉,你终于走到了...\n";
					msg1 += "您有现金[CQ:emoji,id=128180]" + vv[0]["money"] + "元，存款[CQ:emoji,id=128179]" + vv[0]["smoney"] + "元，高利贷" + vv[0]["debt"] + "元\n";
					for (size_t i = 0; i < vv.size(); i++) {
						map<string, string> mp = vv[i];

						//隐藏禁卖商品
						if (mp["enable"] == "1")continue;

						msg1 += "买/卖" + to_string(i + 1) + ":";
						msg1 += mp["name"];
						if (mp["owernum"] == "0") {
							msg1 += "￥" + mp["val"] + " 个/元\n";
						}
						else msg1 += "￥" + mp["val"] + " 个/元  库存" + mp["owernum"] + "个 成本￥" + mp["cost"] + "个/元\n";
						mp.clear();
					}
					msg1 += "帮助:输入命令（买1*最大）或者（卖1*57) 或者（买1*24）可以买卖商品.(命令不含括号)";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
				}
			}
		}
		else if (strIndexOf(sanysay, "汇款")) {

			string repay = sanysay.substr(4, sanysay.length() - 4);//获取  1
			if (repay.length() >9)return;//还钱的长度过大
			int iRepay = std::atoi(repay.c_str());
			if (iRepay == 0) return;//还钱金额为0 ,忽略
			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"	+ "《帝都浮生记》\n";
			sql = "update qquser t2 set t2.smoney=t2.smoney- " + to_string(iRepay) + " ,t2.debt=t2.debt-" + to_string(iRepay) + " where t2.qq='" + to_string(fromQQ) + "' and smoney >=" + to_string(iRepay);
			vector< map<string, string> >  vv = execsql(sql);
			if (vv[0]["affected_rows"] != "0") {
				//更新成功
				msg1 += "你给村长家汇去了" + repay + "元,老村长在电话里给你说,小伙子将信用,回村给你找个大闺女....";
			}
			else {
				msg1 += "银行存款不足!\n你头脑发热,想给老家汇[CQ:emoji,id=128180]" + repay + "元回去,看着银行存折里的余额高呼：“我的钱呢？[CQ:emoji,id=128561]”，站在邮局发起呆来......";
			}
			mysendDataByuni(from_uin, msg1, saytype);
		}
		else if (strIndexOf(sanysay, "存款")) {
			//期望 存款100
			string smoney = sanysay.substr(4, sanysay.length() - 4);//获取  1
			if (smoney.length() >9)return;//还钱的长度过大
			int ismoney = std::atoi(smoney.c_str());

			if (ismoney == 0) return;//还钱金额为0 ,忽略

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + "《帝都浮生记》\n";

			sql = "update qquser t2 set t2.smoney=t2.smoney+ " + to_string(ismoney) + ",t2.money=t2.money- " + to_string(ismoney) + "  where t2.qq='" + to_string(fromQQ) + "' and money >=" + to_string(ismoney);
			vector< map<string, string> >  vv = execsql(sql);
			if (vv[0]["affected_rows"] != "0") {
				//更新成功
				msg1 += "存款成功!\n你看着一天天增加的存款,口水不住的流了出来,不知道邻村的小花[CQ:emoji,id=128111]是否还是那年模样....";

			}
			else {
				msg1 += "存款失败!\n你冲到了银行,填了一张[CQ:emoji,id=128221]" + to_string(ismoney) + "元的存单,然后呆呆的望着银行小妹,小妹白了你一眼说:\n看什么看,给钱啊,没钱赶快滚.......";
			}
			mysendDataByuni(from_uin, msg1, saytype);

		}
		else if (strIndexOf(sanysay, "取款")) {
			//期望 取款100  
			string smoney = sanysay.substr(4, sanysay.length() - 4);//获取  1
			if (smoney.length() >9)return;//还钱的长度过大
			int ismoney = std::atoi(smoney.c_str());

			if (ismoney == 0) return;//还钱金额为0 ,忽略

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "《帝都浮生记》\n";

			sql = "update qquser t2 set t2.smoney=t2.smoney-" + to_string(ismoney) + ",t2.money=t2.money+ " + to_string(ismoney) + "  where t2.qq='" + to_string(fromQQ) + "' and smoney >=" + to_string(ismoney);
			vector< map<string, string> >  vv = execsql(sql);
			if (vv[0]["affected_rows"] != "0") {
				//更新成功
				msg1 += "[CQ:emoji,id=128180]取款成功![CQ:emoji,id=128180]\n你看着手里的钞票,坚定了赚钱的信心,默默念道:\n小花[CQ:emoji,id=128111]你等我,我赚了钱就回来陪你放牛....";

			}
			else {
				msg1 += "[CQ:emoji,id=128557]取款失败![CQ:emoji,id=128557]\n你发疯似的冲进ATM机,插入卡后输入取款" + to_string(ismoney) + "元,ATM上突然大声放出:\n\"穷逼!!!你余额不足,取个屁啊!!!',你满脸通红的取出卡后跑了出去....";
			}
			mysendDataByuni(from_uin, msg1, saytype);
		}

	}
}



/*
* Type=2 群消息
param:
int32_t subType				1.子类型，实测全是1
int32_t sendTime			2.时间戳
int64_t fromGroup			3.哪个群发来的消息
int64_t fromQQ				4.哪个QQ发过来的
const char *fromAnonymous	5.匿名者的标识
const char *msg				6.群消息内容
int32_t font				7.字体

*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	gamer(fromGroup, fromQQ, msg, 0);
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}






/*
* Type=4 讨论组消息
subType 子类型
sendTime 时间戳
fromDiscuss 哪个讨论组发来的消息
fromQQ 哪个QQ发过来的
msg 消息内容
font 字体
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	std::string tmpstr = std::string(msg);
	//收到菜单两个字时候
	gamer(fromDiscuss, fromQQ, tmpstr, 1);

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
* fromGroup 群号
* beingOperateQQ 被操作者QQ号
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* fromGroup 群号
* fromQQ 操作者QQ号（管理员，仅subType为2、3时存在）
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* sendTime 时间戳
* fromGroup 群号
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
实测效果
* 1/被取消管理员 2/被设置管理员 1/群员离开 2/群员被踢 1/管理员已同意 2/管理员邀请 六个功能的subtype验证正常，**3/自己(即登录号)被踢**没有出现，被踢时表现为**2/群员被踢**；
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
* subType 子类型，
* sendTime 时间戳
* fromGroup 群号
* fromQQ 操作者QQ
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* subType 子类型，
* sendTime 时间戳
* fromQQ 操作者QQ
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* sendTime 时间戳
* fromGroup 群号
* fromQQ 操作者QQ
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {


	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	//MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);


	return 0;
}

CQEVENT(int32_t, __menuC, 0)() {
	//MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);

	return 0;
}

CQEVENT(int32_t, __menuD, 0)() {

	return 0;
}
