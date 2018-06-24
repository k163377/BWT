#include <algorithm> //sort用
#include <execution> //algorithmの並列実行ポリシー
#include <ppl.h> //parallel_for用
#include <concurrent_vector.h>

#include <iostream>
#include <string>
#include <tuple>

//単語を入れたらSuffixのベクターを返す
concurrency::concurrent_vector<std::tuple<int, std::string>>
MakeSuffix(const std::string &str
){
    using namespace std;

    concurrency::concurrent_vector<std::tuple<int, std::string>> v;
    //parallel_forで並列処理
    concurrency::parallel_for(size_t(0), str.length(), [&str, &v](size_t i){
        v.push_back(make_tuple(i, str.substr(i, str.length())));
    });

    return v;
}

//Suffixをソート
void
DictionaryOrderSort(concurrency::concurrent_vector<std::tuple<int, std::string>> &Suffix
){
    using namespace std;
    sort(execution::par_unseq,
         Suffix.begin(), Suffix.end(),
         [](tuple<int, string> &t1, tuple<int, string> &t2) {
        return (get<1>(t1) <= get<1>(t2)); }
        );
}

//BWT系列を作成
std::string
MakeBWT(const std::string OriginalString,
        const concurrency::concurrent_vector<std::tuple<int, std::string>> &SuffixArray
){
    using namespace std;

    string s(OriginalString.length(), '0'); //容量固定のstringを配列っぽく使用
    concurrency::parallel_for(size_t(0), OriginalString.length(), [&](size_t i){
        if(get<0>(SuffixArray[i]) == 0) s[i] = OriginalString[OriginalString.length()-1];
        else s[i] = OriginalString[get<0>(SuffixArray[i])-1];
    });

    return s;
}

std::string
ReconstructionFromBWT(const std::string &BWT,
                      const unsigned int limit = INT_MAX // 何文字目まで再生するかの指定
){
    using namespace std;
    //タグ付け
    vector<tuple<char, int>> v;
    string temp = BWT;
    size_t val;
    for(size_t i = 0; i < temp.length(); i++){
        v.emplace_back(temp[i], i); //emplace_backの中に書いた要素でデフォルトコンストラクタが呼ばれてるそうな
        if(temp[i] == '$') val = i;
    }
    //安定ソートで並べ替え、法則を得る
    stable_sort(execution::par_unseq, v.begin(), v.end());

    //リミットまで復元
    const int lim = max((int)temp.length()-(int)limit-1, 0);
    int j;
    for(int i = (int)temp.length() -1; i >= lim; i--){
        temp[i] = BWT[val];
        j = 0;
        while(val != get<1>(v[j])) j++;
        val = j;
    }

    return temp.substr(max(lim, 0), temp.length());
}

int main() {
    using namespace std;

    //string str = "abca$";
    string str = "internationalization$"; //入力
    //string str = "efeasreaygdasfagfdsfsdfdfddffesfesefsfegfdgadsfgaghfghfghsddfsdfcvncdrhgrgdrgagragsdhtgvhbdvsfgawdawddfthffdsrrtdfhsfgjghjdsetgrghzdfhjhghgjkdghjygukikfgu,fguhkf]jkhkjkhlkljkljk;lk;ktjhilkjpo;dftjuklidfthgsdfarhdsdhrawrfafhfdfhsrawsawdawdsgsdgsarawarffjsrtjdztyjkdtydtyjtydtsgsgssdfbdvcfgjrjftjrxnbdgrda$";
    concurrency::concurrent_vector<std::tuple<int, std::string>> v = MakeSuffix(str); //Suffix作成
    cout << "Suffix\n";
    for(auto t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << '\n';
    }

    DictionaryOrderSort(v); //ソート
    cout << "\nSuffix Array\n";
    for(auto t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << '\n';
    }

    string BWT = MakeBWT(str, v); //BWT取得
    cout << "\nGet BWT" << '\n';
    cout << BWT << '\n';

    cout << "\nDecode" << '\n';
    if(str == ReconstructionFromBWT(BWT)) cout << "success!";
    else cout << "failed";


    cout << flush;

    return 0;
}