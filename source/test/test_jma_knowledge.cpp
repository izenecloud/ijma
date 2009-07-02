/** \file test_jma_knowledge.cpp
 * Test to create an instance of JMA_Knowledge.
 * Below is the usage examples:
 * \code
 * Test to create and use JMA_Knowledge.
 * $./test_jma_knowledge
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 18, 2009
 */

#include "jma_knowledge.h"
#include "mecab.h"

#include <iostream>
#include <cassert>
#include <stdlib.h>

using namespace std;
using namespace jma;

/**
 * Main function.
 */
int main()
{
    // create knowledge
    JMA_Knowledge* knowledge = new JMA_Knowledge;

    cout<<"\n#Test the System and User Dictionaries. "<<endl;
    // set dictionary files
    const char* sysdict;
    const char* userdict;
#if defined(_WIN32) && !defined(__CYGWIN__)
    sysdict = "../../db/ipadic/bin_eucjp";
    userdict = "../../db/userdic/eucjp.csv";
#else
    sysdict = "../db/ipadic/bin_eucjp";
    userdict = "../db/userdic/eucjp.csv";
#endif

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    knowledge->addUserDict(userdict);
    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }

    // get tagger
    MeCab::Tagger* tagger = knowledge->getTagger();
    assert(tagger && "MeCab::Tagger is accessible after dictionary is loaded");

    // just for test: get dictionary info
    cout << endl << "dictionary infos:" << endl;
    for (const MeCab::DictionaryInfo *d = tagger->dictionary_info(); d; d = d->next) {
        cout << "filename:\t" << d->filename << endl;
        cout << "version:\t" << d->version << endl;
        cout << "charset:\t" << d->charset << endl;
        cout << "type:\t" << d->type   << endl;
        cout << "size:\t" << d->size << endl;
        cout << "left size:\t" << d->lsize << endl;
        cout << "right size:\t" << d->rsize << endl;
        cout << endl;
    }

    cout<<"\n#Test the Sentence Separator "<<endl;
    const char* senConfig;
#if defined(_WIN32) && !defined(__CYGWIN__)
    senConfig = "../../db/config/sen-eucjp.config";
#else
    senConfig = "../db/config/sen-eucjp.config";
#endif
    knowledge->loadSentenceSeparatorConfig(senConfig);
    assert( knowledge->isSentenceSeparator(".") );
    assert( knowledge->isSentenceSeparator("!") );
    assert( !knowledge->isSentenceSeparator("=") );
    assert( knowledge->isSentenceSeparator("。") );
    assert( !knowledge->isSentenceSeparator("、") );
    assert( !knowledge->isSentenceSeparator("の") );


    cout<<"\n#Test the Stop Words Dictionary "<<endl;
    const char* stopWordDic;
#if defined(_WIN32) && !defined(__CYGWIN__)
    stopWordDic = "../../db/stopworddic/test-eucjp.txt";
#else
    stopWordDic = "../db/stopworddic/test-eucjp.txt";
#endif
    knowledge->loadStopWordDict(stopWordDic);
    assert( knowledge->isStopWord("教師") );
    assert( !knowledge->isStopWord("胸元") );
    assert( knowledge->isStopWord("年生") );
    assert( !knowledge->isStopWord("年1生") );
    assert( knowledge->isStopWord("し") );
    assert( knowledge->isStopWord("は") );
    assert( !knowledge->isStopWord("はし") );

    delete knowledge;

    cout<<"\n#All tests are done!"<<endl;
    return 0;
}
