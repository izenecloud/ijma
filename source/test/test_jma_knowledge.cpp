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

    // set system dictionary directory
#if defined(_WIN32) && !defined(__CYGWIN__)
    knowledge->setSystemDict("../../db/ipadic/bin");
#else
    knowledge->setSystemDict("../db/ipadic/bin");
#endif

    // add user dictionary files
#if defined(_WIN32) && !defined(__CYGWIN__)
    knowledge->addUserDict("../../db/userdic/eucjp.csv");
#else
    knowledge->addUserDict("../db/userdic/eucjp.csv");
#endif

    // load dictioanry files
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

    delete knowledge;

    return 0;
}
