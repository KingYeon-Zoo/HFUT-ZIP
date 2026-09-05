#ifndef SAFE_CODEC_H
#define SAFE_CODEC_H
#include "node.h"
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <array>
#include <memory>
#include <queue>
#include <stdexcept>
#include <limits>

// 所有格式共同的解析与写入边界。最多处理 100 MiB，拒绝损坏输入后才提交输出。
namespace SafeCodec {
constexpr qint64 limit = 100 * 1024 * 1024;
inline void require(bool ok, const char *message) { if (!ok) throw std::runtime_error(message); }
inline bool compare(node *a, node *b) { return a->count > b->count; }
struct Tree {
    std::vector<std::unique_ptr<node>> owned;
    node *root = nullptr;
    explicit Tree(const std::array<int,256>& freq, std::vector<unsigned> order = {}) {
        std::vector<node*> leaves;
        if(order.empty())for(unsigned i=0;i<256;++i)if(freq[i])order.push_back(i);
        for (auto i:order) if(freq[i]) {
            owned.emplace_back(new node(freq[i], static_cast<unsigned char>(i)));
            leaves.push_back(owned.back().get());
        }
        std::priority_queue<node*,std::vector<node*>,decltype(&compare)> q(leaves.begin(),leaves.end(),&compare);
        while(q.size()>1) {
            auto a=q.top();q.pop();auto b=q.top();q.pop();
            owned.emplace_back(new node(a->count+b->count,0));
            auto parent=owned.back().get();parent->lchild=a;parent->rchild=b;q.push(parent);
        }
        if(!q.empty())root=q.top();
    }
};
inline void codes(node *n, const std::string& prefix, std::array<std::string,256>& out) {
    if(!n)return;
    if(!n->lchild&&!n->rchild){out[n->a]=prefix.empty()?"0":prefix;return;}
    codes(n->lchild,prefix+"0",out);codes(n->rchild,prefix+"1",out);
}
inline bool samePath(const QString& a,const QString& b) {
    QFileInfo x(a),y(b);
    return x.absoluteFilePath()==y.absoluteFilePath() || (y.exists() && x.canonicalFilePath()==y.canonicalFilePath());
}
inline QByteArray readInput(const QString& input,const QString& output) {
    require(!samePath(input,output),"输入和输出不能是同一文件");
    QFile f(input);require(f.open(QIODevice::ReadOnly),"无法读取输入文件");
    require(f.size()<=limit,"文件超过 100 MiB 限制");
    auto data=f.read(limit+1);require(f.error()==QFile::NoError && data.size()<=limit,"输入读取失败或超出限制");return data;
}
inline void save(const QString& output,const QByteArray& data) {
    QSaveFile f(output);f.setDirectWriteFallback(false);
    require(f.open(QIODevice::WriteOnly),"无法创建输出临时文件");
    require(f.write(data)==data.size(),"输出写入失败");require(f.commit(),"输出提交失败");
}
inline void put32(QByteArray& b,int n) {for(int i=0;i<4;++i)b.append(char((quint32(n)>>(8*i))&255));}
inline void putVar(QByteArray& b,int n) {
    require(n>=0 && n<=0x3fffffff,"频率超出变长编码范围");
    if(n<=0x7f)b.append(char(n));
    else if(n<=0x3fff){b.append(char(0x80|(n>>8)));b.append(char(n&255));}
    else {b.append(char(0xc0|(n>>24)));b.append(char((n>>16)&255));b.append(char((n>>8)&255));b.append(char(n&255));}
}
struct Reader {
    const QByteArray& data;qsizetype pos=0;
    unsigned byte(){require(pos<data.size(),"压缩文件已截断");return static_cast<unsigned char>(data[pos++]);}
    int integer(){quint32 v=0;for(int i=0;i<4;++i)v|=quint32(byte())<<(8*i);require(v<=quint32(std::numeric_limits<int>::max()),"频率无效");return int(v);}
    int variable(){auto a=byte();if(!(a&128))return a;if((a&192)==128)return ((a&63)<<8)|byte();int v=(a&63)<<24;v|=int(byte())<<16;v|=int(byte())<<8;return v|byte();}
};
inline void compress(const QString& input,const QString& output,int level) {
    auto data=readInput(input,output);std::array<int,256> freq{};for(auto c:data)++freq[static_cast<unsigned char>(c)];
    int count=0;for(auto v:freq)if(v)++count;
    Tree tree(freq);std::array<std::string,256> table;codes(tree.root,"",table);
    bool ultra=count>0&&count<256;
    int ultraHeader=1+count;
    for(auto n:freq)if(n)ultraHeader+=n<=0x7f?1:n<=0x3fff?2:4;
    const char format=(ultra && (level==2 || (level==1 && ultraHeader<4+5*count)))?'U':'O';
    QByteArray packed;packed.append(format);
    auto suffix=QFileInfo(input).suffix().toUtf8();
    require(suffix.size()<100,"文件后缀过长");
    packed+=suffix;packed.append('\0');
    if(format=='O'){put32(packed,count);for(int i=0;i<256;++i)if(freq[i]){packed.append(char(i));put32(packed,freq[i]);}}
    else {packed.append(char(count));for(int i=0;i<256;++i)if(freq[i])packed.append(char(i));for(auto n:freq)if(n)putVar(packed,n);}
    unsigned value=0,bits=0;
    for(auto c:data)for(auto bit:table[static_cast<unsigned char>(c)]){
        if(bits==8){packed.append(char(value));bits=0;value=0;}
        value=(value<<1)|(bit-'0');++bits;
    }
    packed.append(char(value));packed.append(char(bits));require(packed.size()<=limit,"压缩结果超过 100 MiB 限制");save(output,packed);
}
inline void decompress(const QString& input,const QString& output) {
    auto data=readInput(input,output);Reader r{data};char format=0;
    require(!data.isEmpty(),"压缩文件为空");if(data[0]=='O'||data[0]=='U')format=char(r.byte());
    bool terminated=false;for(int i=0;i<100;++i){auto c=r.byte();if(c==0){terminated=true;break;}}require(terminated,"缺少后缀终止符");
    std::array<int,256> freq{};std::array<bool,256> seen{};std::vector<unsigned> order;
    if(format=='O'||format=='U'){
        int count=format=='O'?r.integer():r.byte();require(count>=0&&count<=256&&(format!='U'||count>0),"字符数量无效");
        std::vector<unsigned> symbols;
        for(int i=0;i<count;++i){auto c=r.byte();require(!seen[c],"频率表包含重复字符");seen[c]=true;order.push_back(c);
            if(format=='O'){freq[c]=r.integer();require(freq[c]>0,"频率必须为正数");}else symbols.push_back(c);}
        for(auto c:symbols){freq[c]=r.variable();require(freq[c]>0,"频率必须为正数");}
    }else for(auto& v:freq)v=r.integer();
    qint64 total=0;for(auto n:freq)total+=n;require(total<=limit,"解压结果超过 100 MiB 限制");
    require(data.size()-r.pos>=2,"缺少压缩数据尾部");const unsigned finalBits=static_cast<unsigned char>(data.back());require(finalBits<=8,"尾部有效位数无效");
    const unsigned last=static_cast<unsigned char>(data[data.size()-2]);require((last>>finalBits)==0,"尾部包含非零填充位");
    const qint64 bitCount=(data.size()-r.pos-2)*8+finalBits;
    Tree tree(freq,order);QByteArray result;result.reserve(total);
    if(!tree.root){require(total==0&&bitCount==0,"空频率表包含数据");}
    else if(!tree.root->lchild&&!tree.root->rchild){
        // 旧版单字符文件使用空码字；保留其合法形式，输出仍受统一限额约束。
        require(bitCount==0||bitCount==total,"单字符编码长度不符");
        for(auto i=r.pos;i<data.size()-1;++i)require(data[i]==0,"单字符编码必须为零");
        result=QByteArray(total,char(tree.root->a));
    }else{
        auto remaining=freq;node *current=tree.root;
        auto consume=[&](unsigned bit){current=bit?current->rchild:current->lchild;require(current!=nullptr,"无效编码路径");
            if(!current->lchild&&!current->rchild){require(remaining[current->a]>0,"数据超出声明频率");--remaining[current->a];result.append(char(current->a));current=tree.root;}};
        for(auto i=r.pos;i<data.size()-2;++i){auto value=static_cast<unsigned char>(data[i]);for(int bit=7;bit>=0;--bit)consume((value>>bit)&1);}
        for(int bit=int(finalBits)-1;bit>=0;--bit)consume((last>>bit)&1);
        require(current==tree.root&&result.size()==total,"编码截断或解压长度不符");for(auto n:remaining)require(n==0,"解压频率不符");
    }
    save(output,result);
}
}
#endif
