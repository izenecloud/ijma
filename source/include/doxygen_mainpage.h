/** \file doxygen_mainpage.h
 * Document for doxygen mainpage.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 28, 2009
 */

/*! \mainpage iJMA Library
<b>iJMA</b> (iZENEsoft Japanese Morphological Analyzer) is a platform-independent C++ library for Japanese word segmentation and POS tagging. The approach used in iJMA is based on the CRF (Conditional Random Field) statistical model.

<H2>Table of guidance</H2>
    <ul>
      <li><a href="#interface">How to use the interface</a></li>
      <li><a href="#build">How to build and link with the library</a></li>
        <ul>
          <li><a href="#build_linux">Under Linux platform</a></li>
          <li><a href="#build_win32">Under Win32 platform</a></li>
        </ul>
      <li><a href="#demo">How to run the demo</a></li>
      <li><a href="#dictionary">How to use the dictionary</a> </li>
        <ul>
          <li><a href="#sysdict">System dictionary</a></li>
          <li><a href="#userdict">User dictionary</a></li>
          <li><a href="#load">Run time loading</a></li>
        </ul>
    </ul>

<H2><a name="interface">How to use the interface</a></H2>
<i>Step 1: Include the header files in directory <CODE>include</CODE></i>
\code
#include "jma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"
\endcode

<i>Step 2: Use the library name space</i>
\code
using namespace jma;
\endcode

<i>Step 3: Call the interface and handle the result</i>

In the example below, the return value of some functions are not handled for simplicity. In your using, please properly handle those return values in case of failure.

The example code could be available at: <a href="test__jma__run_8cpp-source.html">test_jma_run.cpp</a>.

\code
// create instances
JMA_Factory* factory = JMA_Factory::instance();
Analyzer* analyzer = factory->createAnalyzer();
Knowledge* knowledge = factory->createKnowledge();

// (optional) set character encode type, the default encode type is EUCJP,
// you could set it as SJIS or UTF8
knowledge->setEncodeType(Knowledge::ENCODE_TYPE_SJIS);
 
// set the system dictionary path
knowledge->setSystemDict("db/jumandic/bin_eucjp");
// (optional) add the paths of multiple user dictionaries
knowledge->addUserDict("db/userdic/juman_eucjp.csv");
knowledge->addUserDict("db/userdic/juman_eucjp.txt");
// load system and user dictionary files
knowledge->loadDict();

// (optional) load stop word dictionary
knowledge->loadStopWordDict("db/stopworddic/test-eucjp.txt");

// (optioinal) if Analyzer::splitSentence() would be called,
// load the sentence separator configuration file in specific encode type
knowledge->loadSentenceSeparatorConfig("db/config/sen-eucjp.config");

// (optional) if POS tagging is not necessary,
// disable the output of POS result when Analyzer::runWith***() is called
analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);

// (optional) output POS result in alphabet format such like "NP-S", instead of in Japanese format
analyzer->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);

// (optional) output POS result in full category of Japanese format,
// some POS results might include asterisk symbol
analyzer->setOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY, 1);

// (optional) get 5-best results when Analyzer::runWithSentence() is called
analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 5);

// set knowledge
analyzer->setKnowledge(knowledge);

// 0. split paragraphs into sentences
string line;
vector<Sentence> sentVec;
while(getline(cin, line)) // get paragraph string from standard input
{
    sentVec.clear(); // remove previous sentences
    analyzer->splitSentence(line.c_str(), sentVec);
    for(size_t i=0; i<sentVec.size(); ++i)
    {
        analyzer->runWithSentence(sentVec[i]); // analyze each sentence
        ...
    }
}

// 1. analyze a sentence
Sentence s;
s.setString("...");
analyzer->runWithSentence(s);

// get one-best result
int i= s.getOneBestIndex();
...

// get n-best results
for(int i=0; i<s.getListSize(); ++i)
{
    for(int j=0; j<s.getCount(i); ++j)
    {
        const char* pLexicon = s.getLexicon(i, j);
        const char* strPOS = s.getStrPOS(i, j);
        // get the base form of the specific Japanese word
        const char* baseForm = s.getBaseForm(i, j);
        ...
    }
    double score = s.getScore(i);
    ...
}

// 2. analyze a paragraph
const char* result = analyzer->runWithString("...");
...

// 3. analyze a file
analyzer->runWithStream("...", "...");

// destroy instances
delete knowledge;
delete analyzer;
\endcode

<H2><a name="build">How to build and link with the library</a></H2>

<a href="http://www.cmake.org">CMake</a> is used as the build system.

<H4><a name="build_linux">Under Linux platform</a></H4>
Use script <CODE>build.sh</CODE> in directory <CODE>build</CODE> like below.
\code
$ cd build
$ ./build.sh
\endcode

After the project is built, the library targets <CODE>libjma.a</CODE> and <CODE>libmecab.so</CODE> are created in directory <CODE>lib</CODE>, and the executables in directory <CODE>bin</CODE> are created for demo and test.

To link with the library, the user application needs to include header files in directory <CODE>include</CODE>, and link the library files <CODE>libjma.a</CODE>, <CODE>libmecab.so</CODE> in directory <CODE>lib</CODE>.

An example of compiling user application <CODE>test.cpp</CODE> looks like:
\code
$ export JMA_PATH=path_of_jma_project
$ g++ -I$JMA_PATH/include -o test test.cpp $JMA_PATH/lib/libjma.a $JMA_PATH/lib/libmecab.so -Wl,-rpath,$JMA_PATH/lib
\endcode

<H4><a name="build_win32">Under Win32 platform</a></H4>
Use script <CODE>build.sh</CODE> in directory <CODE>build</CODE> like below.
\code
$ cd build
$ ./build.sh release win32
\endcode

After the project is built, the library targets <CODE>jma.lib</CODE>, <CODE>mecab.lib</CODE> and <CODE>mecab.dll</CODE> are created in directory <CODE>lib/Release</CODE>, and the executables in directory <CODE>bin/Release</CODE> are created for demo and test.

To link with the library, the user application needs to include header files in directory <CODE>include</CODE>, and link the library files <CODE>jma.lib</CODE>, <CODE>mecab.lib</CODE> in directory <CODE>lib</CODE>.

To run the user application, it is also necessary to put <CODE>mecab.dll</CODE> in the search path used by Windows to locate it.

<H2><a name="demo">How to run the demo</a></H2>

To run the demo <CODE>bin/jma_run</CODE>, please make sure the project is built (see <a href="#build">how to build</a>), and also the compiled system dictionary exists on your disk, such like <CODE>db/jumandic/bin_eucjp</CODE>.

Below is the demo usage:
\code
$ cd bin
$ ./jma_run --sentence N-best [--dict DICT_PATH]
$ ./jma_run --string [--dict DICT_PATH]
$ ./jma_run --stream INPUT OUTPUT [--dict DICT_PATH]
\endcode

The <CODE>DICT_PATH</CODE> is the path of compiled system dictionary, such like <CODE>db/jumandic/bin_eucjp</CODE>, which is the default system dictionary used in demo. 

Demo of analyzing 5-best results of a sentence string from standard input.
\code
$ ./jma_run --sentence 5 
\endcode

Demo of analyzing a paragraph string from standard input.
\code
$ ./jma_run --string
\endcode

To exit the loop in the above demos, please press <CODE>CTRL-C</CODE>.

Demo of analyzing file from <CODE>INPUT</CODE> to <CODE>OUTPUT</CODE>. The example of <CODE>INPUT</CODE> file could be available as <CODE>../db/test/asahi_test_raw_eucjp.txt</CODE>.
\code
$ ./jma_run --stream INPUT OUTPUT
\endcode

The demo source code could be available at: <a href="test__jma__run_8cpp-source.html">test_jma_run.cpp</a>. \n

<H2><a name="dictionary">How to use the dictionary</H2>
<H4><a name="sysdict">System dictionary</a></H4>

The CSV files (such like <CODE>db/jumandic/src/Noun.koyuu.csv</CODE>) are source files of system dictionary. Whenever the words are revised or added in source files, you need to compile them into binary files like below.

\code
$ cd bin
$ mkdir ../db/jumandic/bin_eucjp
$ ./jma_encode_sysdict --encode eucjp ../db/jumandic/src ../db/jumandic/bin_eucjp
\endcode

In the above example, the character encoding of binary files is set as <CODE>EUC-JP</CODE>. For encoding <CODE>SHIFT-JIS</CODE> and <CODE>UTF-8</CODE>, please use <CODE>sjis</CODE> and <CODE>utf8</CODE> instead of <CODE>eucjp</CODE>.

<H4><a name="userdict">User dictionary</a></H4>

The user dictionary (such like <CODE>db/userdic/juman_eucjp.csv</CODE> or <CODE>db/userdic/juman_eucjp.txt</CODE>) could be loaded at run time without compilation beforehand.

Please note that the character encoding of user dictionary could be configured by the entry <CODE>dictionary-charset</CODE> in <CODE>dicrc</CODE> file, which is under the directory of compiled system dictionary (such like <CODE>db/jumandic/bin_eucjp/dicrc</CODE>).

<H4><a name="load">Run time loading</a></H4>

In the example below, call the APIs to load dictionary files at run time.

\code
// set character encode type, for which EUCJP, SJIS or UTF8 are available
knowledge->setEncodeType(Knowledge::ENCODE_TYPE_EUCJP);
 
// set the system dictionary path
knowledge->setSystemDict("db/jumandic/bin_eucjp");

// add the paths of multiple user dictionaries
knowledge->addUserDict("db/userdic/juman_eucjp.csv");
knowledge->addUserDict("db/userdic/juman_eucjp.txt");

// load system and user dictionary files
if(knowledge->loadDict() == 0)
{
    cerr << "fail to load dictionary files" << endl;
    exit(1);
}
\endcode

\n
Copyright (C) 2009, iZENEsoft. All rights reserved. \n
Author: Jun <jun.zju@gmail.com>, Vernkin <vernkin@gmail.com> \n
Version: 1.0.0 \n
Date: 2009-09-30 \n
*/
