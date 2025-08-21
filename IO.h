#ifndef IO_H
#define IO_H

#include <iostream>


//這個函式就是一個 簡化版的 std::endl，

//可以正確處理 char 和 wchar_t 等不同字元類型的串流，

//但不會自動 flush 緩衝區（這也是常見原因：std::endl flush 太頻繁會影響效能）。

namespace IO
{

    template<typename T, typename CharT = std::char_traits<T>>
    std::basic_ostream<T,CharT>&
        endl(std::basic_ostream<T,CharT>& os){
            return os<<os.widen('\n');
        }
}


#endif