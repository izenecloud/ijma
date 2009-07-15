// Include the RLP interfaces
#include <bt_rlp.h>
#include <bt_language_names.h>
#include <bt_rlp_ne_iterator.h>

// Basis Tech string class, for UTF16 -> UTF-8 handling
#include <bt_xstring.h>

// C++ includes
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

//prototypes
static void handleResults(BT_RLP_Context* context, ofstream& out);
static void log_callback(void* info_p, int channel, const char* message);

//Use an XML string or file to define a context. The context is an
//ordered sequence of processors and related settings that specify
//the processing that RLP performs on the input. This app defines
//the context in the string below.

//This is a general-purpose context configuration.
// - The Unicode Converter converts the input file from UTF-8 (or any other
//   Unicode encoding) to UTF-16 for internal processing.
// - Depending on the language of the input, the appropriate language analyzer
//   (BL1, JLA, CLA, KLA, or ARBL) performs a variety of tasks appropriate for
//   that language. For example, JLA tokenizes Japanese text, assigns part of
//   speech tags, and determines alphabetic (Hiragana) readings for native
//   Japanese words in Kanji.
// - BaseNounPhrase detects noun phrases.
// - SentenceBoundaryDetector delimits the sentences in the input.
// - Stopwords uses the language-specific stopwords dictionary to tag tokens
//   as stopwords.
// - NamedEntityExtractor, Gazeteer, RegExpLP, and NamedEntityRedactor locate
//   named entities.
static const char* CONTEXT =
  "<?xml version='1.0'?>"
  "<contextconfig>"
  "<languageprocessors>"
  "<languageprocessor>Unicode Converter</languageprocessor>"
  "<languageprocessor>BL1</languageprocessor>"
  "<languageprocessor>JLA</languageprocessor>"
  "<languageprocessor>CLA</languageprocessor>"
  "<languageprocessor>KLA</languageprocessor>"
  "<languageprocessor>Tokenizer</languageprocessor>"
  "<languageprocessor>SentenceBoundaryDetector</languageprocessor>"
  "<languageprocessor>ARBL</languageprocessor>"
  "<languageprocessor>FABL</languageprocessor>"
  "<languageprocessor>URBL</languageprocessor>"
  "<languageprocessor>Stopwords</languageprocessor>"
  "<languageprocessor>BaseNounPhrase</languageprocessor>"
  "<languageprocessor>NamedEntityExtractor</languageprocessor>"
  "<languageprocessor>Gazetteer</languageprocessor>"
  "<languageprocessor>RegExpLP</languageprocessor>"
  "<languageprocessor>NERedactLP</languageprocessor>"
  "</languageprocessors>"
  "</contextconfig>";

/**
* 1. Process input parameters.
* 2. Set up RLP environment.
* 3. Set up RLP context for processing input.
* 4. Process input.
* 5. Work with the results.
* 6. Clean up.
*/
int main(int argc, const char* argv[])
{
  if (!BT_RLP_Library::VersionIsCompatible()) {
    fprintf(stderr, "RLP library mismatch: have %s expect %s\n",
      BT_RLP_Library::VersionString(),
      BT_RLP_LIBRARY_VERSION_STRING);
    return 1;
  }

  //1. Process input parameters. This application gets the following
  //   from the command line:
  //      - Basis root directory
  //      - language ISO639 code
  //      - pathname of the environment config file
  //      - pathname of the input file
  //      - pathname of the output file (encoded in utf-8)
  if (argc != 6) {
    cerr << "Usage: " << argv[0]
    << " BT_ROOT LANGUAGE ENV_CONFIG_FILE INPUT_FILE OUTPUT_FILE" << endl;
    return 1;
  }

  // time evaluation
  clock_t stime = clock();
  
  const char* btRoot   = argv[1];
  // Get BT language ID (defined in bt_language_names.h)
  // from ISO639 code.
  const BT_LanguageID langID = BT_LanguageIDFromISO639(argv[2]) ;
  if (langID ==BT_LANGUAGE_UNKNOWN) {
    cerr << "Warning: Unknown ISO639 language code: " << argv[2] << endl;
    return 1;
  }
  const char* envConfig = argv[3];
  const char* inputFile = argv[4];
  const char* outputFile = argv[5];
  ofstream out(outputFile);
  if (!out) {
    cerr << "Couldn't open output file: " << outputFile << endl;
    return 1;
  }

  //2. Set up the environment.

  //2.1 Use BT_RLP_Environment static methods to set the Basis root
  //    directory, to designate a log callback function, and to set
  //    log level.
  BT_RLP_Environment::SetBTRootDirectory(btRoot);
  BT_RLP_Environment::SetLogCallbackFunction((void*) stderr,
    log_callback);
  //Log level is some combination of "warning,error,info" or "all".
  BT_RLP_Environment::SetLogLevel("error");

  //2.2 Create a new (empty) RLP environment.
  BT_RLP_Environment* rlp = BT_RLP_Environment::Create();
  if (rlp == 0) {
    cerr << "Unable to create the RLP environment." << endl;
    return 1;
  }
  //2.3 Initialize the empty environment with the global environment
  //    configuration file.
  BT_Result rc = rlp->InitializeFromFile(envConfig);
  if (rc != BT_OK) {
    cerr << "Unable to initialize the environment." << endl;
    delete rlp;
    return 1;
  }

  //3. Get a context from the environment. In this case the context
  //   configuration is embedded in the app as a string. It could also
  //   be read in from a file.
  BT_RLP_Context* context;
  rc = rlp->GetContextFromBuffer((const unsigned char*) CONTEXT,
    strlen(CONTEXT),
    &context);
  if (rc != BT_OK) {
    cerr << "Unable to create the context." << endl;
    delete rlp;
    return 1;
  }

  // time evaluation
  clock_t etime = clock();
  double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
  cout << "knowledge loading time: " << dif << endl;

  //4. Use the context object to processes the input file. Must include
  //   language id unless using RLI processor to determine language.
  ifstream in(inputFile);
  if(!in) {
    cerr << "Couldn't open input file: " << inputFile << endl;
    return 1;
  }

  string line;
  while(getline(in, line)) {
      //rc = context->ProcessFile(inputFile, langID);
      rc = context->ProcessBuffer((const unsigned char*)line.c_str(), line.size(), langID);
      if (rc != BT_OK) {
          cerr << "Unable to process the input file '"
              << inputFile << "'." << endl;
          rlp->DestroyContext(context);
          delete rlp;
          return 1;
      }

      //5. Gather results of interest produced by processing the input text.
      handleResults(context, out);

      out << endl;
  }

  // time evaluation
  dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
  cout << "stream analysis time: " << dif << endl;
  dif = (double)(clock() - stime) / CLOCKS_PER_SEC;
  cout << "total time: " << dif << endl;

  fprintf(stdout, "\nSee output file: %s\n\n", outputFile);

  //6. Remove any objects still lying around.
  rlp->DestroyContext(context);
  delete rlp;
  return 0;
}

//5. Get results of interest and write to file.
static void handleResults(BT_RLP_Context* context, ofstream& out) {
  try {
    //5.1 Use the context object to get single-valued results: language,
    //    encoding, raw text, transcribed text (Arabic).
    //BT_UInt32 lang = context->GetIntegerResult(BT_RLP_DETECTED_LANGUAGE);
    //const char* langName = BT_ISO639FromLanguageID(lang);
    //const BT_Char8* encoding =
      //context->GetStringResult(BT_RLP_DETECTED_ENCODING);
    //out << "Language: " << langName << "(" << lang << ")\t";
    //if (encoding != 0)
      //out << "Encoding: " << encoding << endl;
    //BT_UInt32 len = 0;
    //const BT_Char16* rawText =
      //context->GetUTF16StringResult(BT_RLP_RAW_TEXT, len);
    //bt_xstring rawText_as_utf8(rawText, len);
    //out << "Raw text: " << rawText_as_utf8.c_str() << endl << endl;

    //5.2 Use token iterator to get information related to tokens: tokens,
    //    token offsets, part of speech tags, stems, normalized tokens,
    //    stopwords, compounds, and readings.
    BT_RLP_TokenIteratorFactory* factory =
      BT_RLP_TokenIteratorFactory::Create();

    //Provide access to readings and compounds.
    //factory->SetReturnReadings(true);
    //factory->SetReturnCompoundComponents(true);

    //Create the iterator and destroy the factory.
    BT_RLP_TokenIterator* token_iter = factory->CreateIterator(context);
    factory->Destroy();

    //vector<bt_xwstring> tokens;
    while (token_iter->Next()) {
      //Get the data you want for each token.
      //Get the token (BT_RLP_TOKEN).
      const BT_Char16* token = token_iter->GetToken();
      //tokens.push_back(bt_xwstring(token));

      //Get the token index (not currently used).
      //BT_UInt32 index = token_iter->GetIndex();

      //Convert the token from UTF-16 to UTF-8 and dump it.
      bt_xstring token_as_utf8(token);
      out << token_as_utf8.c_str();

      //Get the part of speech (BT_RLP_PART_OF_SPEECH) for the token.
      //The POS tag is stored as a null-terminated ASCII string,
      //so it is can be writen out directly.

      //const char* pos = token_iter->GetPartOfSpeech();
      //if(pos){
        //out << "/" << pos;
      //}

      out << "  ";
    }

    //Cleanup
    token_iter->Destroy();
  }
  catch (BT_RLP_InvalidResultRequest& e) {
    cerr << "Exception: " << e.what() << endl;
  }
  catch (...) {
    cerr << "Unhandled exception." << endl;
  }
}

/**
* The application registers this function to receive diagnostic log entries.
* RLP Environment LogLevel determines which message channels (error, warning.
* info) are posted to the callback.
*/
static void log_callback(void* info_p, int channel, const char* message)
{
  static const char* szINFO     = "INFO  : ";
  static const char* szERROR    = "ERROR : ";
  static const char* szWARN     = "WARN  : ";
  static const char* szUNKNOWN  = "UNKWN : ";
  const char* szLevel = szUNKNOWN;
  switch(channel) {
    case 0:
      szLevel = szWARN;
      break;
    case 1:
      szLevel = szERROR;
      break;
    case 2:
      szLevel = szINFO;
      break;
  }
  fprintf((FILE*) info_p, "%s%s\n", szLevel, message);
}


/*
Local Variables:
mode: c
tab-width: 2
c-basic-offset: 2
End:
*/
