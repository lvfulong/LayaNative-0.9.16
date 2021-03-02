#include <string>
//#include <boost/filesystem/path.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/ini_parser.hpp>
//#include "../../../../Conch/source/conch/JCSystemConfig.h"
//#include <downloadCache/JCIosFileSource.h>
#ifdef _TEST_
#include "../../../../Conch/source/conch/Test/JCTestManager.h"
#endif

/*
用法：
    conch5.exe [options] url
参数：
    -test
        只执行c的测试用例。这时候不再使用url
    -perf
        执行一个url进行统计
    -perf_count
        update的次数
    -opath    path
        指定效率测试的输出目录
    -debug=""
*/
int main(int argc, char* argv[]){
    bool bRunTest = false;
    char* pRunTestCase = NULL;
    //解析参数
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
        }
        else {
            char* cargv = (char*)argv[i] + 1;
            if (memcmp(cargv, "test", 4) == 0) {
                bRunTest = true;
                if (strlen(cargv) > 4) {
                    pRunTestCase = cargv + 4;
                }
            }
            else if (memcmp(cargv, "perf", 4) == 0) {
            }
            else if (memcmp(cargv, "opath", 5) == 0) {
            }
            else if (memcmp(cargv, "perf_count", 10) == 0) {
            }
            else if (memcmp(cargv, "debug_port", 10) == 0) {
                //Conch::s_nDebugPort = atoi(cargv + 10);
            }
            else {
                printf("Unknown param:%s\n", cargv);
            }
        }
    }

    if (bRunTest) {
#ifdef _TEST_
        printf("start test %s\n",pRunTestCase);
        JCTestMgr* pTest = JCTestMgr::getInstance();
        pTest->run(pRunTestCase);
#endif
        //JSMemorySurvey::DelInstance();
        //svFileCache::delInstance();
        //delete pAssets;
        return 0;
    }

    return 0;
}

