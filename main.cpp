#include <algorithm> //sort用
#include <execution> //algorithmの並列実行ポリシー
#include <ppl.h> //parallel_for用

#include <iostream>
#include <vector>
#include <string>
#include <tuple>

#include <concrt.h>

//単語を入れたらSuffixのベクターを返す
std::vector<std::tuple<int, std::string>> MakeSuffix(const std::string &str){
    using namespace std;

    vector<tuple<int, string>> v;
    //parallel_forで並列処理
    concurrency::parallel_for(size_t(0), str.length(), [&str, &v](size_t i){
        v.emplace_back(make_tuple(i, str.substr(i, str.length())));
    });

    return v;
}

//Suffixをソート
void DictionaryOrderSort(std::vector<std::tuple<int, std::string>> &Suffix){
    using namespace std;
    sort(execution::par_unseq,
         Suffix.begin(), Suffix.end(),
         [](tuple<int, string> &t1, tuple<int, string> &t2) {
        return (get<1>(t1) <= get<1>(t2)); }
        );
}

//BWT系列を作成
std::tuple<int, std::string>
MakeBWT(const std::string OriginalString, const std::vector<std::tuple<int, std::string>> &SuffixArray){
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

std::string ReconstructionFromBWT(const std::tuple<int, std::string> &BWT){
    using namespace std;
    vector<tuple<char, int>> v;
    string temp = get<1>(BWT);
    for(size_t i = 0; i < temp.length(); i++){
        v.emplace_back(make_tuple(temp[i], i));
        //cout << get<0>(v[i]) << endl;
    }
    //安定ソートで並べ替え、法則を得る なぜか並列処理が有効化できなかった
    stable_sort(v.begin(), v.end());

    int val = get<0>(BWT);
    int j;
    for(int i = (int)temp.length() -1; i >= 0; i--){
        temp[i] = get<1>(BWT)[val];
        j = 0;
        while(val != get<1>(v[j])) j++;
        val = j;
    }

    return temp;
}

int main() {
    using namespace std;

    string str = "internationalization$"; //入力
    vector<tuple<int, string>> v = MakeSuffix(str); //Suffix作成
    cout << "Suffix" << endl;
    for(auto t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << endl;
    }

    DictionaryOrderSort(v); //ソート
    cout << "\nSuffix Array" << endl;
    for(auto t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << endl;
    }

    tuple<int, string> BWT = MakeBWT(str, v); //BWT取得
    cout << "\nGet BWT" << endl;
    cout << get<0>(BWT) << "\t:" << get<1>(BWT) << endl;

    cout << "\nDecode" << endl;
    cout << ReconstructionFromBWT(BWT) << endl; //デコード
    cout << flush << endl;

    concurrency::wait(1000);
    return 0;
}