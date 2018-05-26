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

//SuffixをソートしてSuffix Arrayを作る
void DictionaryOrderSort(std::vector<std::tuple<int, std::string>> &v){
    using namespace std;
    sort(execution::par_unseq,
         v.begin(), v.end(),
         [](tuple<int, string> &t1, tuple<int, string> &t2) { return get<1>(t1) <= get<1>(t2); }
        );
}

//BWT系列を作成
std::string MakeBWT(const std::string str, const std::vector<std::tuple<int, std::string>> &v){
    using namespace std;
    string s = "";
    int i;
    for(auto t : v){
        i = get<0>(t);
        if(i == 0) s+= str[str.length()-1];
        else s+=str[i-1];
    }
    cout << s << endl;
    return s;
}

int main() {
    using namespace std;

    string str = "internationalization"; //入力
    //string str = "cacao";
    vector<tuple<int, string>> v = MakeSuffix(str);
    /*for(auto &t : v){
        cout << get<1>(t) << '\n';
    }
    cout << "\nend of MakeSuffix\n" << endl;*/

    DictionaryOrderSort(v);
    /*for(auto &t : v){
        cout << get<0>(t) << "\t:" << get<1>(t) << '\n';
    }
    cout << endl;*/

    MakeBWT(str, v);

    return 0;
}