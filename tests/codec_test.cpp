#include "../hfutzip.h"
#include "../safe_codec.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <iostream>
#include <random>
void check(bool ok,const char *message){if(!ok)throw std::runtime_error(message);}
void write(const QString& p,const QByteArray& b){QFile f(p);check(f.open(QIODevice::WriteOnly),"写测试文件");check(f.write(b)==b.size(),"写测试内容");}
QByteArray read(const QString& p){QFile f(p);check(f.open(QIODevice::ReadOnly),"读测试文件");return f.readAll();}
int main(int argc,char **argv){QCoreApplication app(argc,argv);QTemporaryDir dir;check(dir.isValid(),"临时目录");
 auto input=dir.filePath("input.bin"),archive=dir.filePath("archive.hfut"),output=dir.filePath("output.bin");
 hfutzip codec;int success=0,failed=0;QObject::connect(&codec,&hfutzip::zip_finished,[&](){++success;});QObject::connect(&codec,&hfutzip::zip_failed,[&](const QString&){++failed;});
 std::vector<QByteArray> examples={QByteArray(),QByteArray("A"),QByteArray(128,'A'),QByteArray(256,'x'),QByteArray("ABABABBABA"),QString::fromUtf8("中文压缩往返测试").toUtf8()};
 QByteArray all;for(int i=0;i<256;++i)all.append(char(i));examples.push_back(all.repeated(256));std::mt19937 rng(42);QByteArray random;for(int i=0;i<30000;++i)random.append(char(rng()));examples.push_back(random);
 for(int level=0;level<3;++level)for(const auto& data:examples){write(input,data);codec.path_in=input;codec.path_out=archive;codec.compression_level=CompressionLevel(level);int prior=failed;codec.process_file_makezip();check(failed==prior,"压缩失败");codec.path_in=archive;codec.path_out=output;codec.process_file();check(failed==prior,"解压失败");check(read(output)==data,"往返不一致");}
 for(int n:{0,1,127,128,255,256,16383,16384,65535,65536,0x3fffffff}){QFile f(archive);check(f.open(QIODevice::ReadWrite|QIODevice::Truncate),"打开编码测试");check(codec.writeVariableLength(f,n)>0,"编码失败");f.seek(0);check(codec.readVariableLength(f)==n,"变长编码不一致");}
 {QFile f(archive);f.open(QIODevice::ReadWrite|QIODevice::Truncate);check(codec.writeVariableLength(f,0x40000000)==-1,"未拒绝溢出");check(f.size()==0,"溢出仍写入");}
 auto reject=[&](const QByteArray& bad){write(archive,bad);write(output,"保留现有结果");codec.path_in=archive;codec.path_out=output;auto before=failed;codec.process_file();check(failed==before+1,"畸形输入未拒绝");check(read(output)=="保留现有结果","失败覆盖输出");};
 for(auto bad:{QByteArray(),QByteArray("O"),QByteArray("Ubin\0",5),QByteArray(1024,'\0')})reject(bad);
 // 固定旧格式夹具：频率 A=2、B=1，旧 priority_queue 对应 A=1、B=0。
 for(char format:{char(0),'O','U'}){
  QByteArray h;if(format)h.append(format);h.append("bin",3);h.append('\0');
  if(format==0){for(int i=0;i<256;++i)SafeCodec::put32(h,i=='A'?2:i=='B'?1:0);}
  else if(format=='O'){SafeCodec::put32(h,2);h.append('A');SafeCodec::put32(h,2);h.append('B');SafeCodec::put32(h,1);}
  else {h.append(char(2));h.append("AB",2);SafeCodec::putVar(h,2);SafeCodec::putVar(h,1);}
  h.append(char(6));h.append(char(3));write(archive,h);codec.path_in=archive;codec.path_out=output;auto before=failed;codec.process_file();check(failed==before,"合法旧格式失败");check(read(output)=="AAB","旧格式结果不一致");
  for(int bits:{9,32,255}){auto bad=h;bad[bad.size()-1]=char(bits);reject(bad);}
  for(int cut=0;cut<h.size();++cut)reject(h.left(cut));
  auto bad=h;bad[bad.size()-2]=0;reject(bad);
 }

 for(char format:{'O','U'}){
   QByteArray h;h.append(format);h.append("bin",3);h.append('\0');
   if(format=='O'){SafeCodec::put32(h,2);h.append('B');SafeCodec::put32(h,1);h.append('A');SafeCodec::put32(h,1);}
   else {h.append(char(2));h.append("BA",2);h.append(char(1));h.append(char(1));}
   h.append(char(1));h.append(char(2));write(archive,h);codec.path_in=archive;codec.path_out=output;codec.process_file();check(read(output)=="AB","频率表顺序兼容失败");
 }
 // 空表和单叶树有明确结果，频率总和受限。
 {QByteArray h("Obin\0",5);SafeCodec::put32(h,1);h.append('A');SafeCodec::put32(h,128);h.append(char(0));h.append(char(0));write(archive,h);codec.path_in=archive;codec.path_out=output;codec.process_file();check(read(output)==QByteArray(128,'A'),"旧单字符文件恢复失败");auto bad=h;bad[bad.size()-2]=1;bad[bad.size()-1]=1;reject(bad);}
 {QByteArray h("Obin\0",5);SafeCodec::put32(h,2);for(auto c:{'A','B'}){h.append(c);SafeCodec::put32(h,INT_MAX);}h.append(char(0));h.append(char(0));reject(h);}
 write(input,"输入原文");codec.path_in=input;codec.path_out=input;codec.process_file_makezip();check(read(input)=="输入原文","同路径损坏输入");
 std::cout<<"通过：24 组往返、11 个变长边界、旧格式兼容与 "<<failed<<" 次拒绝/输出保护检查\n";
}
