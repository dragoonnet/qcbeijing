/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include <mysql.h>
#include <map> 
#include <vector>
#include <time.h> 

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
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

//map������ȥ�ж��Ƿ����
bool findKey(map<string, string>mp, const string& keyName)
{
	return (mp.find(keyName) != mp.end());
}


/*
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21  ˽����Ϣ
* subType  1.�����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
* sendTime 2.ʱ���
* fromQQ   3.�ĸ�QQ��������
* msg      4.��Ϣ����
* font     ����
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_IGNORE;
}


bool strIndexOf(string src, char * inChar) {
	if (src.find(inChar) != std::string::npos) {
		return true;
	}
	return false;
}


//��һ��Ⱥ����Ϣ
void mysendDataByuni(int64_t from_uin, string msg1, int saytype)
{
	if (saytype == 0)
		CQ_sendGroupMsg(ac, from_uin, msg1.c_str());
	else
		CQ_sendDiscussMsg(ac, from_uin, msg1.c_str());
}


vector< map<string, string> > execsql(string sql) {
	int res;
	CQ_addLog(ac, CQLOG_DEBUG, "ִ��sql", sql.c_str());

	mysql_init(&myCont);
	vector< map<string, string> > v;
	if (mysql_real_connect(&myCont, host, user, pswd, db, port, NULL, 0)) {
		mysql_query(&myCont, "SET NAMES GBK"); //���ñ����ʽ
		res = mysql_query(&myCont, sql.c_str());//��ѯ
		if (!res) {
			result = mysql_store_result(&myCont);
			if (result) {
				//��ȡ�������ͷ       
				int fieldscount = mysql_num_fields(result);//�ֶ���
				MYSQL_FIELD *fieldNames = mysql_fetch_fields(result);
				//CQ_addLog(ac, CQLOG_DEBUG, "�ֶ�����", to_string(fieldscount).c_str());

				while (sql_row = mysql_fetch_row(result)) {
					//CQ_addLog(ac, CQLOG_DEBUG, "������", "");
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
					//CQ_addLog(ac, CQLOG_DEBUG, "��ȡ���ĵ�һ������Ϊ", tmpstr2.c_str());
					v.push_back(my_Map);
				}
				mysql_close(&myCont);
				return v;
			}
			else
			{
				//insert ���� ��Ҫ����id
				map<string, string>   my_Map;
				my_Map["id"] = to_string(myCont.insert_id);
				my_Map["affected_rows"] = to_string(myCont.affected_rows);
				v.push_back(my_Map);
			}

		}
		else {
			CQ_addLog(ac, CQLOG_DEBUG, "��ѯ������", "");
		}
	}
	else {
		CQ_addLog(ac, CQLOG_DEBUG, "�������ݿ�ʧ��", "");
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
				//�����ݿ��ȡ��Ϸ����״̬
				AllUser[mp["qq"]] = mp["isclose"];
			}
		}
	}
	//������Ϸָ��
	if (sanysay == "�۶�������") {
		//�ж��û��Ƿ����,
		vector< map<string, string> > v = execsql("select * from qquser where qq=" + to_string(fromQQ));
		if (v.empty()) {
			//���û�
			string str = "insert into qquser(qq) values('" + to_string(fromQQ) + "')";
			execsql(str);
			//��ʼ����Ϸ����
			str = "INSERT INTO shangping(name, val, rate, qq,sp) SELECT name, val, (RAND() * 20 + 2) / 10, '" + to_string(fromQQ) + "',sp FROM shangping  where qq = '0'";
			execsql(str);
		}
		else
		{
			v.clear();
		}
		AllUser[to_string(fromQQ)] = "0";

		string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
			+ "���ɹ������ˡ��۶������ǡ���Ϸ!\n��Ļ:��Ϊһ����Ƶı�Ư,����峤����2500Ԫ�ĸ�����,���۶����˸�2000�ķ����д���,����Ҫ30����׬��Ǯ�ش�...\n������:����\n"
			+ "��ȡ��Ϸ����,�����뷨�����ⷴ������ϵQQ:8248963\n";
		mysendDataByuni(from_uin, msg1, saytype);
		return;
	}

	if (findKey(AllUser, to_string(fromQQ))) {
		if (AllUser[to_string(fromQQ)] == "1") {
			return;
		}

		if (sanysay == "����" || sanysay == "����") {
			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "���۶������ǡ�\n"
				+ "����������:\n"
				+ "[CQ:emoji,id=128092]����:�鿴��������ʲô����.\n"
				+ "[CQ:emoji,id=128652]ȥ�Ķ�:�鿴����ȥ�Ķ���̯.\n"
				+ "[CQ:emoji,id=127859]��ѯ:�鿴��������\n"
				+ "[CQ:emoji,id=127881]����:�鿴�����˵�����\n"
				+ "[CQ:emoji,id=127978]�ⷿ:�鿴���շ���\n"
				+ "[CQ:emoji,id=127968]�ֿ�:�鿴��Ŀ��\n"
				+ "[CQ:emoji,id=128666]���:����(���100),���Գ���100Ԫ�ĸ�����\n"
				+ "[CQ:emoji,id=128083]���:����(���100),���������д��100Ԫ,ÿ�컹����ϢŶ!\n"
				+ "[CQ:emoji,id=128104]ȡ��:����(ȡ��100),���Ե�����ȡ��100Ԫ!\n"
				+ "[CQ:emoji,id=10060]�رյ۶�������:�ر���Ϸ���������´򿪣�������Ϸ���Ƽ��ɣ�";
			mysendDataByuni(from_uin, msg1, saytype);
		}
		else if (sanysay == "�رյ۶�������") {
			sql = "update qquser set isclose=1 where qq='+ to_string(fromQQ)  +'";
			execsql(sql);					//�������ݿ��ӡ
			AllUser[to_string(fromQQ)] = "1";//�����ڴ��ӡ

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "[CQ:emoji,id=128561][CQ:emoji,id=128561][CQ:emoji,id=128561]�������£������˾����ӡ�ң��ðɡ��۶������ǡ��ʹ˶���ֹͣ����\n"
				+ "��ӡ�ɹ����ڴ�������ҵ������ٴο����ҵ�һ��...\n";
			mysendDataByuni(from_uin, msg1, saytype);

		}
		else if (sanysay == "����") {
			sql = "select name,(s.val * s.rate) as val,s.owernum,s.cost,q.money,q.smoney,q.debt,s.enable from shangping s,qquser q where s.sp=0 and q.qq=s.qq and s.qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "���۶������ǡ�\n";
				msg1 += "����[CQ:face,id=158]�ֽ�" + vv[0]["money"] + "Ԫ��[CQ:emoji,id=127975]���" + vv[0]["smoney"] + "Ԫ��[CQ:emoji,id=128176]������" + vv[0]["debt"] + "Ԫ\n";
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];

					if (mp["enable"] == "1") {//����ʾ��ֹ��������Ʒ
						mp.clear();
						continue;
					}
					msg1 += "��/��" + to_string(i + 1) + ":";
					msg1 += mp["name"];
					if (mp["owernum"] == "0") {
						msg1 += "[CQ:face,id=158]��" + mp["val"] + " ��/Ԫ\n";
					}
					else msg1 += "[CQ:face,id=158]��" + mp["val"] + " ��/Ԫ  ���" + mp["owernum"] + "�� [CQ:emoji,id=128092]�ɱ���" + mp["cost"] + "��/Ԫ\n";
					mp.clear();
				}
				msg1 += "����:����\"��1*100\" ���� \"��1*100\" ����\"��1*���\" ����������Ʒ.";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "ȥ�Ķ�") {
			sql = "select * from weizhi";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "���۶������ǡ�\n";
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];
					msg1 += "ȥ" + to_string(i + 1) + ": ";
					msg1 += mp["addr"] + "\n";
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "��" || sanysay == "��ѯ") {
			sql = "select * from qquser where qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "���۶������ǡ�\n";
				if (vv.size() > 0) {
					map<string, string> mp = vv[0];
					msg1 += "�����ǵ�" + mp["days"] + "��\n";
					msg1 += "[CQ:emoji,id=128180]�����ֽ�:" + mp["money"] + "\n";
					msg1 += "[CQ:emoji,id=128179]���Ĵ��:" + mp["smoney"] + "\n";
					msg1 += "[CQ:emoji,id=128184]����Ƿծ:" + mp["debt"] + "\n";
					msg1 += "[CQ:emoji,id=9994]���Ľ���:" + mp["hp"] + "\n";
					msg1 += "[CQ:emoji,id=128591]��������:" + mp["honor"] + "\n";
					msg1 += "[CQ:emoji,id=127969]�����С:" + mp["useroom"] + "/" + mp["room"] + "\n";
					msg1 += "[CQ:emoji,id=128185]��ʷ���:" + mp["maxmon"] + "Ԫ";
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "����" || sanysay == "����") {
			sql = "select qq,maxmon from qquser order by maxmon desc limit 0,10";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "���۶������ǡ�\n";
				msg1 += "ǰ10����\n";
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];
					msg1 += "����" + to_string(i + 1) + ": " + "[CQ:emoji,id=12854"+to_string(i)+"][CQ:at,qq=" + mp["qq"] + "]" + mp["qq"] + " �÷֣�" + mp["maxmon"] + "��\n";
					mp.clear();
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "�ⷿ") {
			sql = "select name,(val * rate) as val,owernum,cost from shangping where sp=1 and qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
					+ "���۶������ǡ�\n�����н鹫���ķ���:\n";

				msg1 += "�����ؼ۷�Դ[CQ:emoji,id=127970]:ֻ��" + vv[0]["val"] + "Ԫ,����ӵ�г������������Զ�װ10�����";
				msg1 += "����������������Ǽ���,���ֵ��ǲ���Ҫ�ⷿ?\n����\"ȷ���ⷿ\",�������ͳ�Ϊ�۶�����[CQ:emoji,id=10084][CQ:emoji,id=10084]!";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}

		}
		else if (sanysay == "ȷ���ⷿ") {
			sql = "select name, (val * rate) as val, owernum, cost, qquser.`money`,(qquser.`money`-(val * rate)) remain from shangping, qquser where sp = 1 and shangping.`qq` = qquser.qq and qquser.qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty())
			{
				string remain = vv[0]["remain"];
				string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n�����������ǡ�\n";
				if (remain.find("-") == string::npos) {
					//���2 Ǯ���� ��ʾ��������
					msg1 += "�ⷿ�ɹ�!\n�����н�������ۼ�������Ц��:\n���ֵ�,����������ĸ���,���һ����������Ǿ��Ǽ���,���Ⱳ������ѡ����...\n���ķ���������10���ռ�!";
					msg1 += "(Ҳ��֪���Լ����н�ƭ�˶���)";
					//+�ռ�-Ǯ
					sql = "update qquser set money=" + remain + ",room=room+10 where qq='" + to_string(fromQQ) + "'";
					execsql(sql);

					//�Ǽ���10��//���۱���,����һֱ��
					sql = "update shangping set rate=rate+5 where qq='" + to_string(fromQQ) + "'";
					execsql(sql);
				}
				else {
					//���1 Ǯ����
					msg1 += "�����н����������:������[CQ:emoji,id=128074]Ǯ����������˼���۶�,�����������ȥ[CQ:emoji,id=128298][CQ:emoji,id=128298]...(����һ�Ѳ�����Ŀ�Ļ�)";
				}
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (sanysay == "����" || sanysay == "�ֿ�" || sanysay == "���") {
			//��ȡ�ҵķ����С������Ļ���
			sql = "select * from shangping where owernum>0 and enable=0 and qq='" + to_string(fromQQ) + "'";
			vector< map<string, string> >  vv = execsql(sql);
			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "���۶������ǡ�\n������:\n";

			if (!vv.empty()){
				for (size_t i = 0; i < vv.size(); i++) {
					map<string, string> mp = vv[i];
					msg1 += mp["name"];
					msg1 += mp["owernum"] + "�� �ɱ���" + mp["cost"] + "��/Ԫ\n";
					mp.clear();
				}
				msg1 += "����:����\"��1*100\" ���� \"��1*100\" ����������Ʒ.";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
			else
			{
				msg1 += "���ż�æ�ŵ��ܻؼң������տյ��������ӣ�ʲô��û��[CQ:emoji,id=128588][CQ:emoji,id=128588]...\n";
				msg1 += "����:����\"��1*100\" ���� \"��1*100\" ����������Ʒ.";
				mysendDataByuni(from_uin, msg1, saytype);
				vv.clear();
			}
		}
		else if (strIndexOf(sanysay, "��")) {
			//���� ��1*100 ���� ��1*���

			int pos1 = sanysay.find("*");
			string goodsno = sanysay.substr(2, pos1 - 2);//��ȡ  1

			if (goodsno.length() > 2) return;//����Ļ��������� �˳�
			int iGoodsNo = std::atoi(goodsno.c_str());
			if (iGoodsNo == 0)return; //����Ļ������Ϊ0 �˳�

			iGoodsNo--;// �û�����1 ����Ҫ��0����Ʒ

			string goodsNum = sanysay.substr(pos1 + 1, sanysay.length() - pos1);//��ȡ  100

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "���۶������ǡ�\n";
			int iGoodsNum = 0;
			string remainMoney = "0";

			//��ȡ�����������
			if (goodsNum.length() > 9)return;//�������ݳ��ȹ��� �˳�


			iGoodsNum = std::atoi(goodsNum.c_str());
			sql = "select *,money-buynum*val*rate as remain,val * rate as cost, money-buynum2*val*rate as remain2,money - remainroom * val * rate as remain3  from (select t1.*,t2.room,t2.room-t2.useroom as remainroom,t2.money,FLOOR(t2.money/(t1.val*t1.rate)) as buynum," + to_string(iGoodsNum) + " as buynum2 from shangping t1, qquser t2 where t2.qq='"
				+ to_string(fromQQ) + "' and t1.qq = t2.qq order by t1.id asc limit " + to_string(iGoodsNo) + ",1)t";
			vector< map<string, string> >  vv = execsql(sql);

			if (!vv.empty()) {
				if (vv[0]["enable"] == "1") {
					msg1 += "����ʧ��:û�˽��н���!\n����С����Ц��[CQ:emoji,id=128568]:�����ڲ������ϡ��,�����˵ط���,...";
					mysendDataByuni(from_uin, msg1, saytype);
					vv.clear();
					return;//�����벻ƥ��
				}
			}

			int iRemainRoom = std::atoi(vv[0]["remainroom"].c_str());
			if (iRemainRoom == 0) {
				msg1 += "����ʧ��:����û�пռ���!\n����С����Ц��[CQ:emoji,id=128568]:���Ҳ����Ȧһ��С[CQ:emoji,id=128023], ȥ���ױ�����, ��Ȼ��˯����ֻ��վ����...\n\n����������\"�ⷿ\"�鿴���շ��ۡ�";
				vv.clear();
				mysendDataByuni(from_uin, msg1, saytype);
				return;//����ռ䲻��
			}

			if (goodsNum == "���") {
				//���������
				//�ж���������ǲ���һ����������
				if (vv[0]["buynum"] == "0") {
					msg1 += "����ʧ��:�����ֽ���!\n����С����Ц��[CQ:emoji,id=128568]:���Ǯ�ϲ���������,�����ӳ�����[CQ:emoji,id=128576]...";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//����������������,�˳�
				}
				//��¼�û����������
				iGoodsNum = std::atoi(vv[0]["buynum"].c_str());//����Ǯ�ܹ��������
				remainMoney = vv[0]["remain"];   //����Ǯ������ʣ�¶���Ǯ

												 //�������Ǯ�򣬿ռ�᲻�㣬 ʣ����٣������
				if (iGoodsNum > iRemainRoom) {
					iGoodsNum = iRemainRoom;
					remainMoney = vv[0]["remain3"];//remain3 ��ʣ��ռ�װ����ʣ�¶���Ǯ 
				}
			}
			else
			{
				//��������Ϊ����

				if (iGoodsNum == 0) return;//ת����������Ϊ0 �˳�

				int canBuy = std::atoi(vv[0]["buynum"].c_str());
				//�ж�Ҫ�������Ǯ����
				if (iGoodsNum > canBuy) {
					msg1 += "����ʧ��:�����ֽ���!\n����С����Ц��[CQ:emoji,id=128568]:�ֽ���,��ȥ�ʰ���Ҫ����Ǯ������...";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//��������������Թ���,�˳�
				}
				remainMoney = vv[0]["remain2"];
			}

			//�жϷ����Ƿ��㹻��С
			if (iGoodsNum > iRemainRoom) {
				msg1 += "����ʧ��:����û�пռ���!\n����С����Ц��[CQ:emoji,id=128568]:���Ҳ����Ȧһ��С[CQ:emoji,id=128023], ȥ���ױ�����, ��Ȼ��˯����ֻ��վ����...\n\n����������\"�ⷿ\"�鿴���շ��ۡ�";
				vv.clear();
				mysendDataByuni(from_uin, msg1, saytype);
				return;//��������������Թ���,�˳�
			}

			//�޸ķ����С��Ǯ��С �� ����
			//�Ϻ������� ������Ҫ -20 -10 ������****************************************************************************************************
			sql = "update qquser set useroom = useroom + " + to_string(iGoodsNum) + " , money = " + remainMoney + " where qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//�޸Ŀ��
			sql = "update shangping set owernum = owernum + " + to_string(iGoodsNum) + " , cost = '" + vv[0]["cost"] + "' where id='" + vv[0]["id"] + "'";
			execsql(sql);
			msg1 += "����ɹ�[CQ:emoji,id=10004]!!\n��������" + to_string(iGoodsNum) + "��" + vv[0]["name"] + "\nʣ���ֽ�[CQ:emoji,id=128180]:" + remainMoney + "Ԫ\n";
			msg1 += "����:����������\"�ֿ�\" �鿴���,Ҳ���Լ���������,����������\"ȥ�Ķ�\"�������ط�ȥ����!!";
			mysendDataByuni(from_uin, msg1, saytype);
			return;
		}
		else if (strIndexOf(sanysay, "��")) {
			//���� ��1*100 ���� ��1*���

			int pos1 = sanysay.find("*");
			string goodsno = sanysay.substr(2, pos1 - 2);//��ȡ  1

			if (goodsno.length() > 2) return;//����Ļ��������� �˳�
			int iGoodsNo = std::atoi(goodsno.c_str());
			if (iGoodsNo == 0)return; //����Ļ������Ϊ0 �˳�

			iGoodsNo--;// �û�����1 ����Ҫ��0����Ʒ

			string goodsNum = sanysay.substr(pos1 + 1, sanysay.length() - pos1);//��ȡ  100


			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "���۶������ǡ�\n";
			int iGoodsNum = 0;
			string remainMoney = "0";

			//��ȡ�����������
			if (goodsNum.length() > 9)return;//�������ݳ��ȹ��� �˳�

			iGoodsNum = std::atoi(goodsNum.c_str());
			sql = "select *,money+owernum*val*rate as remain,val * rate as cost, money+buynum2*val*rate as remain2 from (select t1.*,t2.room,t2.room-t2.useroom as remainroom,t2.money,FLOOR(t2.money/(t1.val*t1.rate)) as buynum," + to_string(iGoodsNum) + " as buynum2 from shangping t1, qquser t2 where t2.qq='"
				+ to_string(fromQQ) + "' and t1.qq = t2.qq order by t1.id asc limit " + to_string(iGoodsNo) + ",1)t";
			vector< map<string, string> >  vv = execsql(sql);

			if (!vv.empty()) {
				if (vv[0]["enable"] == "1") {
					msg1 += "����ʧ��:û�˽��н���!\n����С����Ц��[CQ:emoji,id=128568]:�����ڲ������ϡ��,�����˵ط���[CQ:emoji,id=127917][CQ:emoji,id=127917][CQ:emoji,id=127917]...";
					mysendDataByuni(from_uin, msg1, saytype);
					vv.clear();
					return;//�����벻ƥ��
				}
			}

			if (goodsNum == "���") {
				//���������
				//�ж���������ǲ���һ����������
				if (vv[0]["owernum"] == "0") {
					msg1 += "����ʧ��:��û���������!\n����С����Ц��[CQ:emoji,id=128568]:ţ��Ҳ׼�����ƽ�����,[CQ:emoji,id=128169][CQ:emoji,id=128169][CQ:emoji,id=128169]��ҪǮȥ���������ÿ�...";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//û���������Ŀ��
				}

				//��¼�û�����������
				iGoodsNum = std::atoi(vv[0]["owernum"].c_str());
				//���������ջ��õ�Ǯ.
				remainMoney = vv[0]["remain"];
			}
			else
			{
				//��������Ϊ����

				if (iGoodsNum == 0) return;//ת����������Ϊ0 �˳�

				int canSell = std::atoi(vv[0]["owernum"].c_str());
				//�жϹ�������
				if (iGoodsNum > canSell) {
					msg1 += "����ʧ��:������������!\n����С����Ц��[CQ:emoji,id=128568]:����뵱������,[CQ:emoji,id=128026]û�п���������������.....";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
					return;//��������������Թ���,�˳�
				}
				remainMoney = vv[0]["remain2"];
			}

			//�޸ķ����С��Ǯ��С �� ����
			//�Ϻ������� ������Ҫ -20 -10 ������****************************************************************************************************
			sql = "update qquser set useroom = useroom - " + to_string(iGoodsNum) + " , money = " + remainMoney + " where qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//�޸Ŀ��
			sql = "update shangping set owernum = owernum - " + to_string(iGoodsNum) + " where id='" + vv[0]["id"] + "'";
			execsql(sql);
			msg1 += "�����ɹ�[CQ:emoji,id=10024]!!\n��������" + to_string(iGoodsNum) + "��" + vv[0]["name"] + "\nʣ���ֽ�[CQ:emoji,id=128180]:" + remainMoney + "Ԫ\n";
			msg1 += "����:����������\"�ֿ�\" �鿴���,Ҳ���Լ���������,����������\"ȥ�Ķ�\"�������ط�ȥ����!!";
			mysendDataByuni(from_uin, msg1, saytype);
			return;
		}
		else if (strIndexOf(sanysay, "ȥ")) {
			//���� ȥ1  ȥ2
			string goNO = sanysay.substr(2, sanysay.length() - 2);//��ȡ  1
			if (goNO.length() >3)return;//ȥ�ĳ��ȹ���
			int iGoNO = std::atoi(goNO.c_str());

			if (iGoNO == 0) return;//ȥ��λ�ò�������

			iGoNO--; // �û�����1 ����Ҫ��0���ط�

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "���۶������ǡ�\n";

			srand((unsigned)time(NULL));
			int stopGoods = rand() % 7;
			//����������Ʒ,������  ��������
			sql = "update shangping set rate=(RAND()*20 + 2)/10 ,enable = 0 where qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//û�е���Ʒ����ʾ�ɱ�
			sql = "update shangping set cost = 0 where owernum=0 and qq='" + to_string(fromQQ) + "'";
			execsql(sql);

			//����һ����Ʒ
			//sql = "update shangping set enable = 1 where qq='" + to_string(fromQQ) + "' order by id asc  limit " + to_string(stopGoods) + ",1";
			sql = "select id from shangping where qq='" + to_string(fromQQ) + "' order by id asc  limit " + to_string(stopGoods) + ",1";
			string delid = execsql(sql)[0]["id"];
			sql = "update shangping set shangping.`enable` = 1 where id=" + delid;
			execsql(sql);

			// +���� +��Ϣ +��ծ
			sql = "update qquser set days=days+1,debt=debt*107/100,smoney= smoney*101/100   where  qq='" + to_string(fromQQ) + "'";
			execsql(sql);
			//��������¼�,������Ʒ�۸� ����Ӱ�� ���� ���� ************************************
			//
			//�����ж� ********************

			//�����ж� ********************

			//��ծ�ж� ********************

			//��ȡ����˳�����Ʒ��Ϣ��ȡ�ˣ�����������ʾ��
			sql = "select name,(s.val * s.rate) as val,s.owernum,s.cost,q.money,q.smoney,q.debt,q.days,s.enable from shangping s,qquser q where s.sp=0 and q.qq=s.qq and s.qq='" + to_string(fromQQ) + "'";

			vector< map<string, string> >  vv = execsql(sql);
			if (!vv.empty()) {
				//��ȡ��Ϸ����
				int iDay = std::atoi(vv[0]["days"].c_str());
				if (iDay > 30) {
					//��Ϸ����
					sql = "select t.cc,win.*,(t.cc-qquser.`maxmon` )ismax from (select sum(val*rate*owernum) as cc from shangping where qq='" + to_string(fromQQ) + "')t ,qquser ,win where qq='" + to_string(fromQQ) + "' and t.cc > win.money order by id desc limit 0,1";
					vector< map<string, string> >  vv2 = execsql(sql);

					//��������
					//����������Ʒ,������  ��������
					sql = "update shangping set rate=(RAND()*20 + 2)/10 ,owernum=0,cost=0,enable = 0 where qq='" + to_string(fromQQ) + "'";
					execsql(sql);

					string newRecord = vv2[0]["cc"];//Ĭ��Ϊ�¼�¼

					if (vv2[0]["ismax"].find("-") != string::npos) {
						//�������µļ�¼
						newRecord = "maxmon";
					}
					//�����֮ǰ�ĸ߲Ÿ��·���,���ý��   ˳�����÷���
					sql = "update qquser set useroom=0,gametime=gametime+1,days=1, room=100,honor=100, hp=100, money=500,smoney=0,debt=2000,maxmon =" + newRecord + " where qq='" + to_string(fromQQ) + "'";
					execsql(sql);

					//�õ�ǰ���ȥ��ȡ����
					sql = "select count(*) cc from qquser where maxmon >= " + vv2[0]["cc"];
					vector< map<string, string> >  vv3 = execsql(sql);

					if (!vv2.empty()) {
						msg1 += "[CQ:emoji,id=9728][CQ:emoji,id=9728][CQ:emoji,id=9728]�����������л���,���ջ�ȡ��" + vv2[0]["cc"] + "Ԫ\n";
						msg1 += "����:" + vv2[0]["msg"];
						msg1 += "������Ϸ����:[CQ:emoji,id=127881]�� " + vv3[0]["cc"] + " ��[CQ:emoji,id=127881]";

					}
					mysendDataByuni(from_uin, msg1, saytype);
				}
				else
				{
					iDay++;
					msg1 += "[CQ:emoji,id=9728]�����ǵ�" + to_string(iDay) + "��\n����һ��İ���,�������ߵ���...\n";
					msg1 += "�����ֽ�[CQ:emoji,id=128180]" + vv[0]["money"] + "Ԫ�����[CQ:emoji,id=128179]" + vv[0]["smoney"] + "Ԫ��������" + vv[0]["debt"] + "Ԫ\n";
					for (size_t i = 0; i < vv.size(); i++) {
						map<string, string> mp = vv[i];

						//���ؽ�����Ʒ
						if (mp["enable"] == "1")continue;

						msg1 += "��/��" + to_string(i + 1) + ":";
						msg1 += mp["name"];
						if (mp["owernum"] == "0") {
							msg1 += "��" + mp["val"] + " ��/Ԫ\n";
						}
						else msg1 += "��" + mp["val"] + " ��/Ԫ  ���" + mp["owernum"] + "�� �ɱ���" + mp["cost"] + "��/Ԫ\n";
						mp.clear();
					}
					msg1 += "����:���������1*��󣩻��ߣ���1*57) ���ߣ���1*24������������Ʒ.(���������)";
					vv.clear();
					mysendDataByuni(from_uin, msg1, saytype);
				}
			}
		}
		else if (strIndexOf(sanysay, "���")) {

			string repay = sanysay.substr(4, sanysay.length() - 4);//��ȡ  1
			if (repay.length() >9)return;//��Ǯ�ĳ��ȹ���
			int iRepay = std::atoi(repay.c_str());
			if (iRepay == 0) return;//��Ǯ���Ϊ0 ,����
			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"	+ "���۶������ǡ�\n";
			sql = "update qquser t2 set t2.smoney=t2.smoney- " + to_string(iRepay) + " ,t2.debt=t2.debt-" + to_string(iRepay) + " where t2.qq='" + to_string(fromQQ) + "' and smoney >=" + to_string(iRepay);
			vector< map<string, string> >  vv = execsql(sql);
			if (vv[0]["affected_rows"] != "0") {
				//���³ɹ�
				msg1 += "����峤�һ�ȥ��" + repay + "Ԫ,�ϴ峤�ڵ绰�����˵,С���ӽ�����,�ش�����Ҹ����Ů....";
			}
			else {
				msg1 += "���д���!\n��ͷ�Է���,����ϼһ�[CQ:emoji,id=128180]" + repay + "Ԫ��ȥ,�������д���������ߺ������ҵ�Ǯ�أ�[CQ:emoji,id=128561]����վ���ʾַ������......";
			}
			mysendDataByuni(from_uin, msg1, saytype);
		}
		else if (strIndexOf(sanysay, "���")) {
			//���� ���100
			string smoney = sanysay.substr(4, sanysay.length() - 4);//��ȡ  1
			if (smoney.length() >9)return;//��Ǯ�ĳ��ȹ���
			int ismoney = std::atoi(smoney.c_str());

			if (ismoney == 0) return;//��Ǯ���Ϊ0 ,����

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + "���۶������ǡ�\n";

			sql = "update qquser t2 set t2.smoney=t2.smoney+ " + to_string(ismoney) + ",t2.money=t2.money- " + to_string(ismoney) + "  where t2.qq='" + to_string(fromQQ) + "' and money >=" + to_string(ismoney);
			vector< map<string, string> >  vv = execsql(sql);
			if (vv[0]["affected_rows"] != "0") {
				//���³ɹ�
				msg1 += "���ɹ�!\n�㿴��һ�������ӵĴ��,��ˮ��ס�����˳���,��֪���ڴ��С��[CQ:emoji,id=128111]�Ƿ�������ģ��....";

			}
			else {
				msg1 += "���ʧ��!\n��嵽������,����һ��[CQ:emoji,id=128221]" + to_string(ismoney) + "Ԫ�Ĵ浥,Ȼ���������������С��,С�ð�����һ��˵:\n��ʲô��,��Ǯ��,ûǮ�Ͽ��.......";
			}
			mysendDataByuni(from_uin, msg1, saytype);

		}
		else if (strIndexOf(sanysay, "ȡ��")) {
			//���� ȡ��100  
			string smoney = sanysay.substr(4, sanysay.length() - 4);//��ȡ  1
			if (smoney.length() >9)return;//��Ǯ�ĳ��ȹ���
			int ismoney = std::atoi(smoney.c_str());

			if (ismoney == 0) return;//��Ǯ���Ϊ0 ,����

			string msg1 = "[CQ:at,qq=" + to_string(fromQQ) + "]\n"
				+ "���۶������ǡ�\n";

			sql = "update qquser t2 set t2.smoney=t2.smoney-" + to_string(ismoney) + ",t2.money=t2.money+ " + to_string(ismoney) + "  where t2.qq='" + to_string(fromQQ) + "' and smoney >=" + to_string(ismoney);
			vector< map<string, string> >  vv = execsql(sql);
			if (vv[0]["affected_rows"] != "0") {
				//���³ɹ�
				msg1 += "[CQ:emoji,id=128180]ȡ��ɹ�![CQ:emoji,id=128180]\n�㿴������ĳ�Ʊ,�ᶨ��׬Ǯ������,ĬĬ���:\nС��[CQ:emoji,id=128111]�����,��׬��Ǯ�ͻ��������ţ....";

			}
			else {
				msg1 += "[CQ:emoji,id=128557]ȡ��ʧ��![CQ:emoji,id=128557]\n�㷢���Ƶĳ��ATM��,���뿨������ȡ��" + to_string(ismoney) + "Ԫ,ATM��ͻȻ�����ų�:\n\"���!!!������,ȡ��ƨ��!!!',������ͨ���ȡ���������˳�ȥ....";
			}
			mysendDataByuni(from_uin, msg1, saytype);
		}

	}
}



/*
* Type=2 Ⱥ��Ϣ
param:
int32_t subType				1.�����ͣ�ʵ��ȫ��1
int32_t sendTime			2.ʱ���
int64_t fromGroup			3.�ĸ�Ⱥ��������Ϣ
int64_t fromQQ				4.�ĸ�QQ��������
const char *fromAnonymous	5.�����ߵı�ʶ
const char *msg				6.Ⱥ��Ϣ����
int32_t font				7.����

*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	gamer(fromGroup, fromQQ, msg, 0);
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}






/*
* Type=4 ��������Ϣ
subType ������
sendTime ʱ���
fromDiscuss �ĸ������鷢������Ϣ
fromQQ �ĸ�QQ��������
msg ��Ϣ����
font ����
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	std::string tmpstr = std::string(msg);
	//�յ��˵�������ʱ��
	gamer(fromDiscuss, fromQQ, tmpstr, 1);

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
* fromGroup Ⱥ��
* beingOperateQQ ��������QQ��
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* fromGroup Ⱥ��
* fromQQ ������QQ�ţ�����Ա����subTypeΪ2��3ʱ���ڣ�
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* sendTime ʱ���
* fromGroup Ⱥ��
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
ʵ��Ч��
* 1/��ȡ������Ա 2/�����ù���Ա 1/ȺԱ�뿪 2/ȺԱ���� 1/����Ա��ͬ�� 2/����Ա���� �������ܵ�subtype��֤������**3/�Լ�(����¼��)����**û�г��֣�����ʱ����Ϊ**2/ȺԱ����**��
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
* subType �����ͣ�
* sendTime ʱ���
* fromGroup Ⱥ��
* fromQQ ������QQ
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* subType �����ͣ�
* sendTime ʱ���
* fromQQ ������QQ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* sendTime ʱ���
* fromGroup Ⱥ��
* fromQQ ������QQ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {


	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	//MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);


	return 0;
}

CQEVENT(int32_t, __menuC, 0)() {
	//MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);

	return 0;
}

CQEVENT(int32_t, __menuD, 0)() {

	return 0;
}
