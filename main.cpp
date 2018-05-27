#include <algorithm> //sort用
#include <execution> //algorithmの並列実行ポリシー
#include <ppl.h> //parallel_for用

#include <iostream>
#include <vector>
#include <string>
#include <tuple>

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
void DictionaryOrderSort(std::vector<std::tuple<int, std::string>> &v){
    using namespace std;
    sort(execution::par_unseq,
         v.begin(), v.end(),
         [](tuple<int, string> &t1, tuple<int, string> &t2) { return get<1>(t1) <= get<1>(t2); }
        );
}

//BWT系列を作成
std::tuple<int, std::string> MakeBWT(const std::string str, const std::vector<std::tuple<int, std::string>> &v){
    using namespace std;

    string s(str.length(), 0); //容量固定のstringを配列っぽく使用
    int OriginalPoint;

    concurrency::parallel_for(size_t(0), str.length(), [&](size_t i){
        if(get<0>(v[i]) == 0){
            s[i] = str[str.length()-1];
            OriginalPoint = i;
        }
        else s[i] = str[get<0>(v[i])-1];
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
    /*stable_sort(v.begin(), v.end(),
                [](const tuple<char, int> &t1, const tuple<char, int> &t2) { return get<1>(t1) <= get<1>(t2); }
    );*/
    stable_sort(v.begin(), v.end());
    /*for(auto t : v){
        cout << get<0>(t) << get<1>(t) << endl;
    }*/
    //DictionaryOrderSort(v);

    temp = "";
    string s =
    int val = get<0>(BWT);
    for(int i = (int)temp.length() -1; i >= 0; i--){
        //temp[i] = get<1>(BWT)[val];
        string s(get<1>(BWT)[val], 1);
        cout << s << endl;
        temp = s + temp;
        for(size_t j = 0; j < temp.length(); j++){
            if(get<1>(v[j]) == val) {
                //cout << val << endl;
                val = j;
                j = temp.length();
            }
        }
    }

    return temp;
}

int main() {
    using namespace std;

    string str = "internationalization"; //入力
    //string str = "cacao";
    vector<tuple<int, string>> v = MakeSuffix(str); //Suffix作成
    DictionaryOrderSort(v); //ソート
    tuple<int, string> BWT = MakeBWT(str, v); //BWT取得
    cout << get<1>(BWT) << endl;
    cout << ReconstructionFromBWT(BWT) << endl;

    return 0;
}