#include <algorithm> //sort用
#include <execution> //algorithmの並列実行ポリシー
#include <ppl.h> //parallel_for用

#include <iostream>
#include <vector>
#include <string>
#include <tuple>

//単語を入れたらSuffixのベクターを返す
std::vector<std::tuple<int, std::string>>
MakeSuffix(const std::string &str
){
    using namespace std;
    vector<tuple<int, string >> v;
    v.reserve(str.length()); //容量は先に確保しておく
    size_t lim = (str.length() / thread::hardware_concurrency()) + 1; //スレッド並列数計算
    //parallel_forで並列処理
    concurrency::parallel_for(size_t(0), lim, [&](size_t i){
        size_t t = min(str.length(), lim * (i + 1));
        for(size_t j = lim * i; j < t; j++) v.emplace_back(j, str.substr(j, str.length())); //emplace_backの中に書いた要素でtuple<int, std::string>のデフォルトコンストラクタが呼ばれてるそうな
    });

    return v;
}

//Suffixをソート
void
DictionaryOrderSort(std::vector<std::tuple<int, std::string>> &Suffix
){
    using namespace std;
    sort(execution::par_unseq,
         Suffix.begin(), Suffix.end(),
         [](tuple<int, string> &t1, tuple<int, string> &t2) {
        return (get<1>(t1) <= get<1>(t2)); }
        );
}

//BWT系列を作成
std::tuple<int, std::string>
MakeBWT(const std::string OriginalString,
        const std::vector<std::tuple<int, std::string>> &SuffixArray
){
    using namespace std;

    string s(OriginalString.length(), '0'); //容量固定のstringを配列っぽく使用
    int OriginalPoint;
    concurrency::parallel_for(size_t(0), OriginalString.length(), [&](size_t i){
        if(get<0>(SuffixArray[i]) == 0){
            s[i] = OriginalString[OriginalString.length()-1];
            OriginalPoint = i;
        }
        else s[i] = OriginalString[get<0>(SuffixArray[i])-1];
    });

    return make_tuple(OriginalPoint, s);
}

std::string
ReconstructionFromBWT(const std::tuple<int, std::string> &BWT,
                      const unsigned int limit = INT_MAX // 何文字目まで再生するかの指定
){
    using namespace std;
    //タグ付け
    vector<tuple<char, int>> v;
    string temp = get<1>(BWT);
    for(size_t i = 0; i < temp.length(); i++){
        v.emplace_back(make_tuple(temp[i], i));
    }
    //安定ソートで並べ替え、法則を得る
    stable_sort(execution::par_unseq, v.begin(), v.end());

    //リミットまで復元
    const int lim = max((int)temp.length()-(int)limit-1, 0);
    int val = get<0>(BWT);
    int j;
    for(int i = (int)temp.length() -1; i >= lim; i--){
        temp[i] = get<1>(BWT)[val];
        j = 0;
        while(val != get<1>(v[j])) j++;
        val = j;
    }

    return temp.substr(max(lim, 0), temp.length());
}

int main() {
    using namespace std;

    //string str = "internationalization$"; //入力
    string str = "efeasreaygdasfagfdsfsdfdfddffesfesefsfegfdgadsfgaagragsdhtgvhbdvsfgawdawddfthffdsrrttjhilkjpo;dftjuklidfthgsdfasarawarffjsrtjdztyjkdtydtyjtydtsgsgssdfbdvcfgjrjftjrxnbdgrda$";
    vector<tuple<int, string>> v = MakeSuffix(str); //Suffix作成
    cout << "Suffix\n";
    for(auto t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << '\n';
    }

    DictionaryOrderSort(v); //ソート
    cout << "\nSuffix Array\n";
    for(auto t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << '\n';
    }

    tuple<int, string> BWT = MakeBWT(str, v); //BWT取得
    cout << "\nGet BWT" << '\n';
    cout << get<0>(BWT) << "\t:" << get<1>(BWT) << '\n';

    cout << "\nDecode" << '\n';
    //cout << ReconstructionFromBWT(BWT, 5) << '\n'; //デコード
    cout << ReconstructionFromBWT(BWT) << '\n'; //デコード */

    cout << flush;

    return 0;
}