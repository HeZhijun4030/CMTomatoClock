#ifndef CMS_UTILS_HPP
#define CMS_UTILS_HPP

#include <iostream>
#include <limits>
#include <string>

namespace cms {
/*
 清屏void
*/
inline void ClearScreen(){
#if defined(_WIN32) || defined(_WIN64)
	system("cls");
	#else
	system("clear");
        #endif
}


/*
用来处理输入异常清空
*/
inline void ClearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
}//ClearInput

/*
参数:
std::string 提示信息 -> T
*/
template<typename T>
T SafeInput(const std::string &Prompt){
    T Resault;
    while(true){
        if(std::cin >> Resault){
            if (!(std::cin.peek() == '\n')) {
                std::cout << "More unknow chars , plz try again" << std::endl;
                cms::ClearInput();
                continue;
            }else{
                return Resault;
            }

        }else {
            std::cout << "Input Error , plz try again" << std::endl;
            cms::ClearInput();
            continue;
        }
    }
}//SafeInput

} // namespace cms

#endif // CMS_UTILS_HPP
