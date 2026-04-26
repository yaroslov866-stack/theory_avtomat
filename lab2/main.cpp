#include "dfa/dfa.h"
#include "nfa/nfa.h"
#include "pattern/pattern.h"




int main() {
    Pattern pattern("a(b|c)+(1:q)");
    std::string text = "addf abcqds";
    auto result = pattern.search(text);
    std::cout<<result.find<<std::endl;
    text = "abcq";
    auto result1 = pattern.match(text);
    if(result1.success){
        std::cout << "Найдено: " << result1.full_match << std::endl;
        std::cout << "Группа 1 (протокол): " << result1.group(1) << std::endl;
    }

    return 0;
}